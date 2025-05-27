#include <Windows.h>
#include <d3d9.h>
#include <iostream>

#include "Hooks/Hooks.h"
#include <d3dx9.h>

DWORD WINAPI Loop(LPVOID lParam)
{
#ifdef _DEBUG
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "loop console attached";
	MessageBox(0, "fuck this", "info", MB_OK | MB_ICONINFORMATION);
#endif
	Hooks::Run();


	while (!GetAsyncKeyState(VK_F11))
		Sleep(100);

#ifdef _DEBUG
	std::cout << "unloading..." << std::endl;
	FreeConsole();
#endif
	Hooks::Destroy();
	FreeLibraryAndExitThread((HMODULE)lParam, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReseved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		if (const HANDLE MainThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop, LPVOID(hModule), 0, NULL))
		{
			CloseHandle(MainThread);
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}
