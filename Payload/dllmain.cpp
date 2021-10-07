// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "detours.h"
#include <windows.h>
#include <wchar.h>


typedef BOOL(WINAPI* realCreateProcessW)(
	LPCWSTR a,
	LPWSTR b,
	LPSECURITY_ATTRIBUTES c,
	LPSECURITY_ATTRIBUTES d,
	BOOL e,
	DWORD f,
	LPVOID g,
	LPCWSTR h,
	LPSTARTUPINFO i,
	LPPROCESS_INFORMATION j);


realCreateProcessW originalRealCreateProcessW = (realCreateProcessW)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "CreateProcessW");

BOOL WINAPI _CreateProcessW(LPCWSTR a,
	LPWSTR b,
	LPSECURITY_ATTRIBUTES c,
	LPSECURITY_ATTRIBUTES d,
	BOOL e,
	DWORD f,
	LPVOID g,
	LPCWSTR h,
	LPSTARTUPINFO i,
	LPPROCESS_INFORMATION j) {

	OutputDebugString(b);

	if (wcscmp(b, L"notepad") == 0) {
		return FALSE;
	}
	else {
		return originalRealCreateProcessW(a, b, c, d, e, f, g, h, i, j);
	}

}

void attachDetour() {

	DetourRestoreAfterWith();
	DetourTransactionBegin();

	DetourUpdateThread(GetCurrentThread());

	DetourAttach((PVOID*)&originalRealCreateProcessW, _CreateProcessW);

	DetourTransactionCommit();
}

void deAttachDetour() {

	DetourTransactionBegin();

	DetourUpdateThread(GetCurrentThread());

	DetourDetach((PVOID*)&originalRealCreateProcessW, _CreateProcessW);

	DetourTransactionCommit();
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		attachDetour();
		break;
	case DLL_PROCESS_DETACH:
		deAttachDetour();
		break;
	}
	return TRUE;
}


