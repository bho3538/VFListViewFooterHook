#include "stdafx.h"
#include <shlwapi.h>
#include "TryGetHarderHook.h"
#include "Utils.h"

#include "XTryHarderItem.h"


//IListControlHost ca9a60fb-ec04-4b21-ae64-e8271f479bec
typedef HRESULT(__stdcall *TOnGetTryHarderArray)(PVOID __pDefView, REFIID riid, void **ppvOut);
const IID IID_IListControlHost = { 0xca9a60fb,0xec04,0x4b21,{0xae,0x64,0xe8,0x27,0x1f,0x47,0x9b,0xec} };

typedef struct _VFTRYGETHARDERHOOKINT {
	DWORD dwSize;
	DWORD dwRefCount;
	DWORD buttonCnt;
	DWORD dwFlags;
	HWND hwnd;
	LPWSTR introText;
	CTRYHARDERINVOKECALLBACK callback;
	PVOID userData;
	PXTRYHARDERBUTTONINFO* buttons;
} VFTRYGETHARDERHOOKINT, *PVFTRYGETHARDERHOOKINT;

typedef struct _VFTRYGETHARDERHOOKINFOLISTITEM {
	HWND id;
	PVFTRYGETHARDERHOOKINT item;
	PVOID next;
} VFTRYGETHARDERHOOKINFOLISTITEM, *PVFTRYGETHARDERHOOKINFOLISTITEM;

PVFTRYGETHARDERHOOKINFOLISTITEM _tryHarderItemList = NULL;
CRITICAL_SECTION _tryHarderLockList = { 0, };
BOOL g_bInitialized = FALSE;

void _VFInsertAtTryHarderInfoList(HWND id, PVFTRYGETHARDERHOOKINT item);
void _VFDeleteAtTryHarderInfoList(HWND id);
PVFTRYGETHARDERHOOKINT _VFGetItemFromTryHarderInfoList(HWND id);

TOnGetTryHarderArray OriginalTryGetHarder = NULL;
TOnGetTryHarderArray NextTryGetHarder = NULL;

HRESULT __stdcall _HookedGetTryHarder(PVOID __this, REFIID riid, void** ppvOut);

void _FinalReleaseTryHarderInfoItem(PVFTRYGETHARDERHOOKINT item);


__declspec(dllexport) PVOID __cdecl VFAddRefTryHarderHook(IUnknown* pDefView,CTRYHARDERINVOKECALLBACK invokeCB,PVOID invokeCBUserData, PBOOL pReqInitBtn) {
	PVFTRYGETHARDERHOOKINT tryHarderInfo = NULL;
	HWND hwnd = 0;
	IUnknown* pListControlHost = NULL;
	MH_STATUS hookStatus;

	if (!g_bInitialized) {
		MH_Initialize();
		InitializeCriticalSection(&_tryHarderLockList);
		g_bInitialized = TRUE;
	}

	*pReqInitBtn = FALSE;

	if (!pDefView) {
		goto escapeArea;
	}

	IUnknown_GetWindow(pDefView, &hwnd);

	hwnd = GetAncestor(hwnd, GA_ROOT);

	if (!hwnd) {
		goto escapeArea;
	}

	//hook CDefView::OnGetTryHarder
	if (!OriginalTryGetHarder) {
		pDefView->lpVtbl->QueryInterface(pDefView, &IID_IListControlHost, &pListControlHost);

		if (!pListControlHost) {
			goto escapeArea;
		}

		OriginalTryGetHarder = VFGetRawMethodFromInterface(pListControlHost, 18);

		hookStatus = MH_CreateHook(OriginalTryGetHarder, &_HookedGetTryHarder, (LPVOID*)&NextTryGetHarder);
		if (hookStatus == MH_OK)
		{
			if (MH_EnableHook(OriginalTryGetHarder) != MH_OK)
			{
				goto escapeArea;
			}
		}
	}

	//insert information (current window)
	tryHarderInfo = _VFGetItemFromTryHarderInfoList(hwnd);
	if (tryHarderInfo) {
		InterlockedIncrement(&tryHarderInfo->dwRefCount);
	}
	else {
		//create info and insert
		tryHarderInfo = (PVFTRYGETHARDERHOOKINT)malloc(sizeof(VFTRYGETHARDERHOOKINT));
		if (!tryHarderInfo) {
			goto escapeArea;
		}
		ZeroMemory(tryHarderInfo, sizeof(VFTRYGETHARDERHOOKINT));

		tryHarderInfo->dwSize = sizeof(VFTRYGETHARDERHOOKINT);
		tryHarderInfo->hwnd = hwnd;
		tryHarderInfo->dwRefCount = 1;

		_VFInsertAtTryHarderInfoList(hwnd, tryHarderInfo);

		*pReqInitBtn = TRUE;
	}

	tryHarderInfo->callback = invokeCB;
	tryHarderInfo->userData = invokeCBUserData;

escapeArea:

	if (pListControlHost) {
		pListControlHost->lpVtbl->Release(pListControlHost);
	}

	return tryHarderInfo;
}

__declspec(dllexport) void __cdecl VFReleaseTryHarderHook(PVOID pInfo) {
	PVFTRYGETHARDERHOOKINT info = NULL;
	DWORD refCount = 0;

	if (!pInfo) {
		return;
	}

	info = (PVFTRYGETHARDERHOOKINT)pInfo;

	refCount = InterlockedDecrement(&info->dwRefCount);

	if (refCount == 0) {
		_VFDeleteAtTryHarderInfoList(info->hwnd);
		_FinalReleaseTryHarderInfoItem(info);
	}
}

void CleanUpTryHarderHook(PVOID args) {
	if (NextTryGetHarder) {
		MH_DisableHook(OriginalTryGetHarder);
		MH_RemoveHook(OriginalTryGetHarder);

		OriginalTryGetHarder = NULL;
		NextTryGetHarder = NULL;
	}

	if (g_bInitialized) {
		DeleteCriticalSection(&_tryHarderLockList);
		MH_Uninitialize();
		g_bInitialized = FALSE;
	}
}

void _FinalReleaseTryHarderInfoItem(PVFTRYGETHARDERHOOKINT item) {
	DWORD i = 0;
	PXTRYHARDERBUTTONINFO buttonInfo = NULL;

	if (item->buttons) {
		for (i = 0; i < item->buttonCnt; i++) {
			buttonInfo = (item->buttons)[i];
			CoTaskMemFree(buttonInfo->pDisplayName);
			CoTaskMemFree(buttonInfo->pTooltipName);
			CoTaskMemFree(buttonInfo);
		}
		CoTaskMemFree(item->buttons);
	}

	free(item->introText);
	free(item);
}

HRESULT __stdcall _HookedGetTryHarder(PVOID __this, REFIID riid, void** ppvOut) {
	HRESULT hr = E_FAIL;
	DWORD tmp = 0;
	IObjectCollection* tryHarderList = NULL;
	PXTryHarderItem tryHarderItem = NULL;
	PXTRYHARDERBUTTONINFO buttonInfo = NULL;
	PVFTRYGETHARDERHOOKINT info = NULL;
	HWND hwnd = 0;

	IUnknown_GetWindow(__this, &hwnd);

	hwnd = GetAncestor(hwnd, GA_ROOT);

	info = _VFGetItemFromTryHarderInfoList(hwnd);
	if (!info) {
		goto escapeArea;
	}

	hr = CoCreateInstance(&CLSID_EnumerableObjectCollection, NULL, CLSCTX_INPROC,&IID_IObjectCollection,(LPVOID*)&tryHarderList);

	if (hr != S_OK) {
		goto escapeArea;
	}

	if (info->buttons) {
		for (tmp = 0; tmp < info->buttonCnt; tmp++) {
			buttonInfo = info->buttons[tmp];
			tryHarderItem = CreateTryHarderItem(buttonInfo->pDisplayName, buttonInfo->pTooltipName, buttonInfo->iconIndex, buttonInfo->menuIdx,info->callback,info->userData);

			if (tryHarderItem) {
				tryHarderList->lpVtbl->AddObject(tryHarderList, (IUnknown*)tryHarderItem);
				tryHarderItem->lpVtbl->Release(tryHarderItem);
			}
		}
	}

	*ppvOut = tryHarderList;

	hr = S_OK;

	escapeArea:

	return hr;
}


void _VFInsertAtTryHarderInfoList(HWND id, PVFTRYGETHARDERHOOKINT insertItem) {
	PVFTRYGETHARDERHOOKINFOLISTITEM newItem = NULL;
	PVFTRYGETHARDERHOOKINFOLISTITEM item = NULL;
	if (!insertItem && !_tryHarderItemList) {
		return;
	}

	newItem = (PVFTRYGETHARDERHOOKINFOLISTITEM)malloc(sizeof(VFTRYGETHARDERHOOKINFOLISTITEM));

	if (!newItem) {
		return;
	}
	newItem->id = id;
	newItem->item = insertItem;
	newItem->next = 0;

	EnterCriticalSection(&_tryHarderLockList);

	if (_tryHarderItemList) {
		item = _tryHarderItemList;
		for (;;) {
			if (!item->next) {
				break;
			}
			item = item->next;
		}
		item->next = newItem;
	}
	else {
		_tryHarderItemList = newItem;
	}

	LeaveCriticalSection(&_tryHarderLockList);


}
void _VFDeleteAtTryHarderInfoList(HWND id) {
	PVFTRYGETHARDERHOOKINFOLISTITEM item = NULL;
	PVFTRYGETHARDERHOOKINFOLISTITEM beforeItem = NULL;

	EnterCriticalSection(&_tryHarderLockList);

	if (_tryHarderItemList) {
		item = _tryHarderItemList;
		for (;;) {
			if (item->id == id) {
				if (beforeItem) {
					beforeItem->next = item->next;
				}
				else {
					_tryHarderItemList = NULL;
				}

				free(item);
				break;
			}

			if (!item->next) {
				break;
			}
			beforeItem = item;
			item = item->next;
		}
	}

	LeaveCriticalSection(&_tryHarderLockList);
}

PVFTRYGETHARDERHOOKINT _VFGetItemFromTryHarderInfoList(HWND id) {
	PVFTRYGETHARDERHOOKINFOLISTITEM item = NULL;
	BOOL bFound = FALSE;

	EnterCriticalSection(&_tryHarderLockList);

	if (_tryHarderItemList) {
		item = _tryHarderItemList;
		for (;;) {
			if (!item) {
				break;
			}
			if (item->id == id) {
				bFound = TRUE;
				break;
			}
			item = item->next;
		}
	}

	LeaveCriticalSection(&_tryHarderLockList);

	if (bFound && item) {
		return item->item;
	}
	else {
		return NULL;
	}
}

__declspec(dllexport) void __cdecl VFSetTryHarderButtons(PVOID pInfo, PXTRYHARDERBUTTONINFO* buttons, DWORD buttonCnt, LPCWSTR introText) {
	PVFTRYGETHARDERHOOKINT tryHarderInfo = NULL;
	DWORD tmp = 0;

	if (!pInfo || !buttons) {
		return;
	}

	tryHarderInfo = (PVFTRYGETHARDERHOOKINT)pInfo;

	tryHarderInfo->buttons = buttons;
	tryHarderInfo->buttonCnt = buttonCnt;
	
	if (introText) {
		tmp = (DWORD)wcslen(introText) + 1;
		tryHarderInfo->introText = (LPWSTR)malloc(sizeof(WCHAR) * tmp);
		wcscpy_s(tryHarderInfo->introText, tmp, introText);
	}
}