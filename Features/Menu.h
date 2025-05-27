#pragma once
#include <Windows.h>
#include <d3d9.h>

#include "../Utils/vector.h"
#include "../Utils/ImGui/imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern WNDPROC originalWndProc;

extern LRESULT CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

namespace Menu
{
	static bool bShow = false;
	static bool bColorText = false; // deprecated, used in the old menu for rainbow colors
	static int iCurrentTab = 0;

	void Init(IDirect3DDevice9* pDevice, HWND hwnd);
	void Render();
	void ShouldRun();
	void Destroy();
}
