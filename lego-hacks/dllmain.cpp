// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "dllmain.h"
#include <iostream>
#include <TlHelp32.h>
HHOOK hook;
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0) {
		return CallNextHookEx(hook, nCode, wParam, lParam);
	}
	switch (nCode) {
	case HCBT_SETFOCUS:
		HWND hwnd = (HWND)wParam;
		RECT rect;
		GetWindowRect(hwnd,&rect);
		ClipCursor(&std::as_const(rect));
		ShowCursor(false);
		break;
	}
	return CallNextHookEx(hook, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) LRESULT CALLBACK WindowProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0) {
		return CallNextHookEx(hook, nCode, wParam, lParam);
	}
	CWPSTRUCT* cwpstruct = (CWPSTRUCT*)lParam;
	switch (cwpstruct->message)
	{
	case WM_EXITSIZEMOVE: {
		UpdateWindow(cwpstruct->hwnd);
		break;
	}
	case WM_SETFOCUS:
		//MessageBox(NULL, L"WM_SETFOCUS", L"WM_SETFOCUS", MB_ICONINFORMATION);
		break;
	}
	return CallNextHookEx(hook, nCode, wParam, lParam);
}