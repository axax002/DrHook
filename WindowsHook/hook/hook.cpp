#include "pch.h"
#include <iostream>
#include <easyhook.h>
#include <windows.h>
#include <string>
#include <tchar.h>

using namespace std;

BOOL CreateProcessHook(
	LPCSTR                lpApplicationName,
	LPSTR                 lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	LPCSTR                lpCurrentDirectory,
	LPSTARTUPINFOA        lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
) 
{	// A beep will act as a notification for when our hook is called
	Beep(800, 1000);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	LPTSTR path = _tcsdup(TEXT("calc.exe"));

	return CreateProcess(NULL, path, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
};


int main()
{
	HOOK_TRACE_INFO hHook = { NULL }; // keep track of our hook
	cout << "\n";
	cout << GetProcAddress(GetModuleHandle(TEXT("kernel32")), "CreateProcessW");
	cout << "\n";

	// Install the hook
	NTSTATUS result = LhInstallHook(
		GetProcAddress(GetModuleHandle(TEXT("kernel32")), "CreateProcessW"),
		CreateProcessHook,
		NULL,
		&hHook);

	if (FAILED(result))
	{
		wstring s(RtlGetLastErrorString());
		wcout << "Failed to install hook: ";
		wcout << s;
		cout << "\n\nPress any key to exit.";
		cin.get();
		return -1;
	}


	cout << "Activating hook for current thread only.\n";
	// If the threadId in the ACL is set to 0, 
	// then internally EasyHook uses GetCurrentThreadId()
	ULONG ACLEntries[1] = { 0 };
	LhSetInclusiveACL(ACLEntries, 1, &hHook);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	LPTSTR path = _tcsdup(TEXT("calc.exe"));
	
	if (!CreateProcess(NULL, path, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) 
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return -1;
	}
	
	cout << "Uninstall hook\n";
	LhUninstallHook(&hHook);
	
	return 0;
}

