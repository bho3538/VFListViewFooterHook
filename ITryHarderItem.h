#pragma once

#include <ShlObj_core.h>


typedef struct ITryHarderItemVtbl
{
	BEGIN_INTERFACE

	HRESULT(STDMETHODCALLTYPE *QueryInterface)(PVOID __this,REFIID riid,void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(PVOID __this);
	ULONG(STDMETHODCALLTYPE *Release)(PVOID __this);

	HRESULT(STDMETHODCALLTYPE *GetIconIndex)(PVOID __this, INT* pIndex);
	HRESULT(STDMETHODCALLTYPE *GetDisplayName)(PVOID __this,LPWSTR* ppName);
	HRESULT(STDMETHODCALLTYPE *GetDisplayNameAlt)(PVOID __this,LPWSTR* ppName);
	HRESULT(STDMETHODCALLTYPE *GetTooltip)(PVOID __this, LPWSTR* ppToolTipName);
	HRESULT(STDMETHODCALLTYPE *GetIDList)(PVOID __this,LPITEMIDLIST ppidl);
	HRESULT(STDMETHODCALLTYPE *OnInvoke)(PVOID __this,IUnknown* pUnk);
	HRESULT(STDMETHODCALLTYPE *Cancel)(PVOID __this);
	HRESULT(STDMETHODCALLTYPE *GetScope)(PVOID __this,PDWORD pFlags);

	END_INTERFACE

} ITryHarderItemVtbl;