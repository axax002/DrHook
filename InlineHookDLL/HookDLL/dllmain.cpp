
#include "stdafx.h"
#include "stdio.h"
#include <windows.h>

void hook(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

void* dllFuncAddress = GetProcAddress(GetModuleHandleA("user32.dll"), "MessageBoxA");

DWORD oldAddress;
BYTE source[5];
INT32 hookAddress = (INT32)hook;
INT32 offset;

void hook(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType) {


	lpText = (LPCTSTR)"Hooked";

	int retJmpAddress = (int)dllFuncAddress + 5;

	_asm {
		mov edi, edi
		push ebp
		mov esp, ebp
		jmp $+retJmpAddress
	}


}

// DllMain Defines the entry point for the DLL application.
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		
		offset = ((INT32)dllFuncAddress - hookAddress);
		offset = (hookAddress > (INT32)dllFuncAddress) ? offset : offset * -1;

		source[0] = 0xe9;
		source[1] = (offset - 5)& 0xff;
		source[2] = (offset >> (8 * 1)) & 0xff;
		source[3] = (offset >> (8 * 2)) & 0xff;
		source[4] = (offset >> (8 * 3)) & 0xff;
		

		VirtualProtect(dllFuncAddress, 5, PAGE_EXECUTE_READWRITE, &oldAddress);
		memcpy(dllFuncAddress, source, 5);
		VirtualProtect(dllFuncAddress, 5, oldAddress, &oldAddress);

		break;
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

