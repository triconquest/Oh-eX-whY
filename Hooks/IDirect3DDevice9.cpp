#include "IDirect3DDevice9.h"

#include <Windows.h>
#include <d3d9.h>
#include <iostream>

void* GetIDirect3DDevice9()
{
	char windowTitle[256];
	HWND hwnd = FindWindowA(NULL, "Counter-Strike Source - Direct3D 9 - 64 bit");
	if (!hwnd) {
		std::cout << "Failed to find window" << std::endl;
		return nullptr;
	}
	else {
		GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));
		std::cout << "window title: " << windowTitle << std::endl;
	}

	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!pD3D) {
		MessageBox(0, "Failed to create D3D object", "Error", MB_OK | MB_ICONERROR);
		std::cout << "Failed to create D3D object" << std::endl;
		return nullptr;
	}

	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;

	IDirect3DDevice9* pDevice = nullptr;
	HRESULT result = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice);

	if (FAILED(result) || !pDevice) {
		std::cout << "Failed to create D3D device" << std::endl;
		pD3D->Release();
		return nullptr;
	}

	void** vTable = *reinterpret_cast<void***>(pDevice);
	void* endSceneAddr = vTable[42];

	pDevice->Release();
	pD3D->Release();

	return endSceneAddr;
}