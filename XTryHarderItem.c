#include "stdafx.h"
#include "XTryHarderItem.h"

const IID IID_ITryHarderItem = { 0x5e8a79a2,0x8188,0x41cd,{0x96,0x2b,0x9c,0x74,0xe3,0xc5,0xd2,0x0c} };

PXTryHarderItem _CreateHarderItemInstance();

HRESULT __stdcall _XTryHarderItemQueryInterface(XTryHarderItem * __this, REFIID riid, void **ppvOut);
ULONG __stdcall _XTryHarderItemAddRef(XTryHarderItem * __this);
ULONG __stdcall _XTryHarderItemRelease(XTryHarderItem * __this);

HRESULT __stdcall _XTryHarderItemQueryInterface2(XTryHarderItem * __this, REFIID riid, void **ppvOut);
ULONG __stdcall _XTryHarderItemAddRef2(XTryHarderItem * __this);
ULONG __stdcall _XTryHarderItemRelease2(XTryHarderItem * __this);

HRESULT __stdcall _XTryHarderItemGetIconIndex(XTryHarderItem * __this,INT* pIndex);
HRESULT __stdcall _XTryHarderItemGetDisplayName(XTryHarderItem * __this, LPWSTR* ppName);
HRESULT __stdcall _XTryHarderItemGetDisplayNameAlt(XTryHarderItem * __this, LPWSTR* ppName);
HRESULT __stdcall _XTryHarderItemGetTooltip(XTryHarderItem * __this, LPWSTR* ppToolTipName);
HRESULT __stdcall _XTryHarderItemGetIDList(XTryHarderItem * __this, LPITEMIDLIST ppidl);
HRESULT __stdcall _XTryHarderItemOnInvoke(XTryHarderItem * __this, IUnknown* pUnk);
HRESULT __stdcall _XTryHarderItemCancel(XTryHarderItem * __this);
HRESULT __stdcall _XTryHarderItemGetScope(XTryHarderItem * __this, PDWORD pFlags);
HRESULT __stdcall _XTryHarderitemGetSite(XTryHarderItem * __this, REFIID riid, void **ppvSite);
HRESULT __stdcall _XTryHarderitemSetSite(XTryHarderItem * __this, IUnknown* punkSite);


PXTryHarderItem CreateTryHarderItem(LPCWSTR pDisplayName, LPCWSTR pTooltipText, DWORD iconIndex, DWORD menuIndex, CTRYHARDERINVOKECALLBACK cb, PVOID userData) {
	PXTryHarderItem item = NULL;
	DWORD tmp = 0;

	item = _CreateHarderItemInstance();

	if (pDisplayName) {
		tmp = (DWORD)wcslen(pDisplayName) + 1;
		item->pDisplayName = (LPWSTR)CoTaskMemAlloc(tmp * sizeof(WCHAR));
		wcscpy_s(item->pDisplayName, tmp, pDisplayName);
	}

	if (pTooltipText) {
		tmp = (DWORD)wcslen(pTooltipText) + 1;
		item->pTooltipText = (LPWSTR)CoTaskMemAlloc(tmp * sizeof(WCHAR));
		wcscpy_s(item->pTooltipText, tmp, pTooltipText);
	}

	item->iconIndex = iconIndex;
	item->idx = menuIndex;
	item->cb = cb;
	item->userData = userData;

	return item;
}


PXTryHarderItem _CreateHarderItemInstance() {
	PXTryHarderItem item = NULL;

	item = (PXTryHarderItem)malloc(sizeof(XTryHarderItem));

	if (!item) {
		goto escapeArea;
	}

	item->lpVtbl = (ITryHarderItemVtbl*)malloc(sizeof(ITryHarderItemVtbl));

	if (!item->lpVtbl) {
		free(item);
		item = NULL;
		goto escapeArea;
	}

	item->lpVtbl->QueryInterface = _XTryHarderItemQueryInterface;
	item->lpVtbl->AddRef = _XTryHarderItemAddRef;
	item->lpVtbl->Release = _XTryHarderItemRelease;
	item->lpVtbl->GetIconIndex = _XTryHarderItemGetIconIndex;
	item->lpVtbl->GetDisplayName = _XTryHarderItemGetDisplayName;
	item->lpVtbl->GetDisplayNameAlt = _XTryHarderItemGetDisplayNameAlt;
	item->lpVtbl->GetTooltip = _XTryHarderItemGetTooltip;
	item->lpVtbl->GetIDList = _XTryHarderItemGetIDList;
	item->lpVtbl->OnInvoke = _XTryHarderItemOnInvoke;
	item->lpVtbl->Cancel = _XTryHarderItemCancel;
	item->lpVtbl->GetScope = _XTryHarderItemGetScope;

	item->siteVtbl = (IObjectWithSiteVtbl*)malloc(sizeof(IObjectWithSiteVtbl));
	item->siteVtbl->QueryInterface = _XTryHarderItemQueryInterface2;
	item->siteVtbl->AddRef = _XTryHarderItemAddRef2;
	item->siteVtbl->Release = _XTryHarderItemAddRef2;
	item->siteVtbl->SetSite = _XTryHarderitemSetSite;
	item->siteVtbl->GetSite = _XTryHarderitemGetSite;


	escapeArea:

	return item;
}

HRESULT __stdcall _XTryHarderItemQueryInterface(XTryHarderItem * __this, REFIID riid, void **ppvOut) {
	HRESULT hr = E_NOINTERFACE;
	if (IsEqualCLSID(riid, &IID_ITryHarderItem) || IsEqualCLSID(riid, &IID_IUnknown)) {
		__this->lpVtbl->AddRef(__this);
		*ppvOut = __this;
		hr = S_OK;
	}
	else if (IsEqualCLSID(riid, &IID_IObjectWithSite)) {
		__this->lpVtbl->AddRef(__this);
		__this = (unsigned char*)__this + sizeof(PVOID);
		*ppvOut = __this;
		hr = S_OK;
	}

	return hr;
}
ULONG __stdcall _XTryHarderItemAddRef(XTryHarderItem * __this) {
	return InterlockedIncrement(&__this->refCnt);
}
ULONG __stdcall _XTryHarderItemRelease(XTryHarderItem * __this) {
	DWORD re = InterlockedDecrement(&__this->refCnt);
	if (re == 0) {
		free(__this->lpVtbl);
		free(__this);
	}

	return re;
}

//iobjectwithsite
HRESULT __stdcall _XTryHarderItemQueryInterface2(XTryHarderItem * __this, REFIID riid, void **ppvOut) {
	__this = (unsigned char*)__this - sizeof(PVOID);
	return _XTryHarderItemQueryInterface(__this, riid, ppvOut);
}
ULONG __stdcall _XTryHarderItemAddRef2(XTryHarderItem * __this) {
	__this = (unsigned char*)__this - sizeof(PVOID);
	return _XTryHarderItemAddRef(__this);
}
ULONG __stdcall _XTryHarderItemRelease2(XTryHarderItem * __this) {
	__this = (unsigned char*)__this - sizeof(PVOID);
	return _XTryHarderItemRelease(__this);
}

HRESULT __stdcall _XTryHarderitemGetSite(XTryHarderItem * __this, REFIID riid, void **ppvSite) {
	return E_NOTIMPL;
}
HRESULT __stdcall _XTryHarderitemSetSite(XTryHarderItem * __this, IUnknown* punkSite) {
	return S_OK;
}


HRESULT __stdcall _XTryHarderItemGetIconIndex(XTryHarderItem * __this, INT* pIndex) {
	*pIndex = __this->iconIndex;
	return S_OK;
}
HRESULT __stdcall _XTryHarderItemGetDisplayName(XTryHarderItem * __this, LPWSTR* ppName) {
	DWORD len = (DWORD)wcslen(__this->pDisplayName) + 1;

	*ppName = CoTaskMemAlloc(len * sizeof(WCHAR));
	wcscpy_s(*ppName,len, __this->pDisplayName);

	return S_OK;
}
HRESULT __stdcall _XTryHarderItemGetDisplayNameAlt(XTryHarderItem * __this, LPWSTR* ppName) {
	DWORD len = (DWORD)wcslen(__this->pDisplayName) + 1;

	*ppName = CoTaskMemAlloc(len * sizeof(WCHAR));
	wcscpy_s(*ppName, len, __this->pDisplayName);

	return S_OK;
}
HRESULT __stdcall _XTryHarderItemGetTooltip(XTryHarderItem * __this, LPWSTR* ppToolTipName) {
	DWORD len = (DWORD)wcslen(__this->pTooltipText) + 1;

	*ppToolTipName = CoTaskMemAlloc(len * sizeof(WCHAR));
	wcscpy_s(*ppToolTipName, len, __this->pTooltipText);

	return S_OK;
}
HRESULT __stdcall _XTryHarderItemGetIDList(XTryHarderItem * __this, LPITEMIDLIST ppidl) {
	return E_NOTIMPL;
}
HRESULT __stdcall _XTryHarderItemOnInvoke(XTryHarderItem * __this, IUnknown* pUnk) {
	
	if (__this->cb) {
		__this->cb(__this->idx, __this->userData);
	}
	else {
		MessageBoxW(NULL, __this->pDisplayName, L"Tryharder default cb", 0);
	}

	return S_OK;
}
HRESULT __stdcall _XTryHarderItemCancel(XTryHarderItem * __this) {
	return E_NOTIMPL;
}
HRESULT __stdcall _XTryHarderItemGetScope(XTryHarderItem * __this, PDWORD pFlags) {
	*pFlags = 3;
	return 0;
}