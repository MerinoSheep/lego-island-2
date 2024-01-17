// injector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <Windows.h>
#include <TlHelp32.h>

DWORD FindUIThread(DWORD pid) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	THREADENTRY32 te;
	te.dwSize = sizeof(te);
	Thread32First(hSnapshot, &te);
	DWORD tid = 0;
	while (Thread32Next(hSnapshot, &te)) {
		if (te.th32OwnerProcessID == pid) {
			GUITHREADINFO info{ sizeof(info) };
			if (GetGUIThreadInfo(te.th32ThreadID, &info)) {
				tid = te.th32ThreadID;
				break;
			}
		}
	}
	CloseHandle(hSnapshot);
	return tid;
}
 DWORD getPid() {
	DWORD pid;
	std::cout << "PID:";
	std::cin >> pid;
	return pid;
}

int main(int argc, char* argv[]) {
	DWORD pid = getPid();
	std::cout << "Injecting DLL" << std::endl;
	HMODULE hDll = LoadLibrary(L"lego-hacks.dll");
	if (!hDll) {
		std::cout << "Loading Library Failed" << std::endl;
		return 1;
	}
	HOOKPROC hbtproc = (HOOKPROC)GetProcAddress(hDll, (PCSTR)MAKEINTRESOURCE(1));
	if (!hbtproc) {
		std::cout << "ProcAddress failed" << std::endl;
	}

	HOOKPROC wndproc = (HOOKPROC)GetProcAddress(hDll, (PCSTR)MAKEINTRESOURCE(2));
	if (!wndproc) {
		std::cout << "ProcAddress failed" << std::endl;
	}
	HHOOK hbtHook = SetWindowsHookEx(WH_CBT, hbtproc, hDll, FindUIThread(pid));
	if (!hbtHook) {
		std::cout << "ProcAddress wndproc failed" << std::endl;
		FreeLibrary(hDll);
		return 1;
	}
	HHOOK winHook = SetWindowsHookEx(WH_CALLWNDPROC, wndproc, hDll, FindUIThread(pid));
	if (!winHook) {
		std::cout << "ProcAddress wndproc failed" << std::endl;
		FreeLibrary(hDll);
		return 1;
	}

	std::cout << "Sucessfully Hooked!" << std::endl;
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {}
	UnhookWindowsHookEx(winHook);
	UnhookWindowsHookEx(hbtHook);
	return 0;
}

