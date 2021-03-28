#include "stdafx.h"

#include "TryGetHarderHook.h"

extern BOOL g_bInitialized;
extern CRITICAL_SECTION _tryHarderLockList;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		if (g_bInitialized) {
			MH_Initialize();
			InitializeCriticalSection(&_tryHarderLockList);
			g_bInitialized = TRUE;
		}

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		CleanUpTryHarderHook(NULL);

		break;	
	}
	return TRUE;
}

