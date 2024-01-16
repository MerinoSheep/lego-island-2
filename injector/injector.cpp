// injector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <Windows.h>
#include <TlHelp32.h>

char DLL_PATH[] = "C:\\Users\\bizom\\source\\repos\\lego-hacks\\Debug\\lego-hacks.dll";
BOOL InjectDLL(DWORD pid)
{

	HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!procHandle) {
		std::cerr << "Couldn't open process!" << std::endl;
		return 0;
	}
	LPVOID baseAdress = VirtualAllocEx(procHandle, NULL, strlen(DLL_PATH) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!baseAdress) {
		std::cerr << "VirtualALlocEx failed" << std::endl;
		return 0;

	}
	LPVOID loadLibrary = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
	if (!loadLibrary) {
		std::cerr << "GetProcAddress failed" << std::endl;
		return 0;

	}
	WriteProcessMemory(procHandle, baseAdress, DLL_PATH, strlen(DLL_PATH) + 1, NULL);
	HANDLE remoteThread = CreateRemoteThread(procHandle, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibrary, baseAdress, 0, NULL);
	if (!remoteThread) {
		std::cerr << "CreateRemoteThread failed" << std::endl;
		return 0;

	}
	//CloseHandle(procHandle);
	return 1;
}

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
	HMODULE hDll = LoadLibrary(L"C:\\Users\\bizom\\source\\repos\\injector\\Debug\\lego-hacks.dll");
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

