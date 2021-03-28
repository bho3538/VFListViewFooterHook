#pragma once

#include "TryGetHarderHook.h"
#include "ITryHarderItem.h"

typedef struct _XTryHarderItem {
	ITryHarderItemVtbl* lpVtbl;
	IObjectWithSiteVtbl* siteVtbl;
	ULONG refCnt;
	//member
	DWORD idx;
	DWORD iconIndex;
	LPWSTR pDisplayName;
	LPWSTR pTooltipText;
	CTRYHARDERINVOKECALLBACK cb;
	PVOID userData;
} XTryHarderItem,*PXTryHarderItem;

PXTryHarderItem CreateTryHarderItem(LPCWSTR pDisplayName,LPCWSTR pTooltipText,DWORD iconIndex,DWORD menuIndex, CTRYHARDERINVOKECALLBACK cb,PVOID userData);