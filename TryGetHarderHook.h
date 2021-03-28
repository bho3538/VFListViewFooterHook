#pragma once

#include <ShlObj_core.h>
#include <shobjidl_core.h>

#if __cplusplus
extern "C" {
#endif

	typedef BOOL(__stdcall *CTRYHARDERINVOKECALLBACK)(DWORD, PVOID);

	typedef struct _XTRYHARDERBUTTONINFO {
		DWORD menuIdx;
		DWORD iconIndex;
		LPWSTR pDisplayName;
		LPWSTR pTooltipName;
	} XTRYHARDERBUTTONINFO, *PXTRYHARDERBUTTONINFO;

	__declspec(dllexport) PVOID __cdecl VFAddRefTryHarderHook(IUnknown* pDefView, CTRYHARDERINVOKECALLBACK invokeCB, PVOID invokeCBUserData, PBOOL pReqInitBtn);
	__declspec(dllexport) void __cdecl VFReleaseTryHarderHook(PVOID pInfo);

	__declspec(dllexport) void __cdecl VFSetTryHarderButtons(PVOID pInfo, PXTRYHARDERBUTTONINFO* buttons, DWORD buttonCnt, LPCWSTR introText);

	void CleanUpTryHarderHook(PVOID args);

#if __cplusplus
}
#endif