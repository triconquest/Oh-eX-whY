#include "hooks.h"
#include "MinHook/MinHook.h"
#include "../Features/Menu.h"
#include "../Features/Aimbot.h"
#include "../Features/ESP.h"
#include "../Features/Misc.h"
#include "../Features/NoSpread.h"
#include "../Features/Visuals.h"
#include "IDirect3DDevice9.h"
#include "../Globals/globals.h"
#include "../SDK/Includes/usercmd.h"
#include "../Utils/DrawManager/DrawManager.h"
#include "../Utils/Memory/Memory.h"
#include "../SDK/Interfaces/Entities/C_BasePlayer.h"
#include "../SDK/Interfaces/IVModelRender.h"
#include "../SDK/Includes/igameevents.h"

#include <Windows.h>
#include <d3d9.h>
#include <iostream>

typedef HRESULT(__stdcall* tEndScene)(LPDIRECT3DDEVICE9 pDevice);
tEndScene oEndScene;

static HWND hwnd = FindWindowA(NULL, "Counter-Strike Source - Direct3D 9 - 64 bit");

HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	static bool initialized = false;
	if (!initialized)
	{
		Menu::Init(pDevice, hwnd);
		Draw.GetDevice(pDevice);
		Draw.InitFonts();
		g_HitMarkerListener.Register();
		I::Cvar->ConsoleColorPrintf({ 0, 255, 0, 255 }, "[Oh eX whY] Loaded!\n");
		initialized = true;
	}

	Menu::ShouldRun();
	ESP.Run();
	Aimbot.Run();
	Misc.Run();
	Visuals.Run();
	Draw.DrawScreenText("smokin nerdz & doin math", 0, 0, FONT_LARGE, D3DCOLOR_ARGB(255, 255, 255, 255));

	return oEndScene(pDevice);
}

typedef bool(__fastcall* CreateMoveFn)(void*, float, CUserCmd*);
inline CreateMoveFn oCreateMove = nullptr;

bool __fastcall hkCreateMove(void* thisptr, float frameTime, CUserCmd* cmd)
{
	if (!cmd || !cmd->command_number)
		return oCreateMove(thisptr, frameTime, cmd);

	Vector3 original = cmd->viewangles;
	Aimbot.RunSilent(cmd);
	Math::NormalizeAngles(original);
	I::EngineClient->SetViewAngles(original);

	Misc.Bhop(cmd);
	Misc.FastStop(cmd);
	Misc.DynamicCrosshair(cmd);
	Misc.NoRecoil(cmd);
	Misc.KnifeBot(cmd);

	Aimbot.TriggerBot2(cmd);

	return false;
}

typedef void(__fastcall* OverrideViewFn)(void*, CViewSetup*);
OverrideViewFn oOverrideView = nullptr;

void __fastcall hkOverrideView(void* ecx, CViewSetup* pSetup)
{
	Misc.CustomFOV(pSetup);
	//Misc.ThirdPerson(pSetup);

	oOverrideView(ecx, pSetup);
}

void HookTHAT()
{
	auto pattern = Memory.FindSignature("client.dll", "48 8D 1D ? ? ? ? 48 8B CB 89 05 ? ? ? ? E8 ? ? ? ? 48 8B 0D");

	if (!pattern)
	{
		printf("pattern invalid\n");
		return;
	}

	uintptr_t relative = *(int32_t*)(pattern + 3);
	void* pClientMode = (void*)(pattern + 7 + relative);
	void** vtable = *(void***)pClientMode;

	void* pCreateMove = vtable[21];
	void* pOverrideMove = vtable[16];

	MH_CreateHook(pCreateMove, &hkCreateMove, (void**)&oCreateMove);
	MH_EnableHook(pCreateMove);

	MH_CreateHook(pOverrideMove, &hkOverrideView, (void**)&oOverrideView);
	MH_EnableHook(pOverrideMove);
}

//typedef void(__fastcall* OverrideViewFn)(void*, CViewSetup*);
//OverrideViewFn oOverrideView = nullptr;

//void __fastcall hkOverrideView(void* ecx, CViewSetup* pSetup)
//{
//	Misc.CustomFOV(pSetup);
//	//Misc.ThirdPerson(pSetup);
//
//	oOverrideView(ecx, pSetup);
//}

//void HookOverrideMove()
//{
//	auto pattern = Memory.FindSignature("client.dll", "48 8D 1D ? ? ? ? 48 8B CB 89 05 ? ? ? ? E8 ? ? ? ? 48 8B 0D");
//
//	uintptr_t relative = *(int32_t*)(pattern + 3);
//	void* pClientMode = (void*)(pattern + 7 + relative);
//	void** vtable = *(void***)pClientMode;
//
//	void* pOverrideMove = vtable[16];
//
//	if (MH_CreateHook(pOverrideMove, &hkOverrideView, (void**)&oOverrideView) != MH_OK)
//	{
//		printf("couldn't create override hook\n");
//		return;
//	}
//
//	if (MH_EnableHook(pOverrideMove) != MH_OK)
//	{
//		printf("couldn't enable override\n");
//		return;
//	}
//}

void HookCInput()
{
	auto pattern = Memory.FindSignature("client.dll", "48 8B 0D ? ? ? ? 48 8B 01 FF 90 ? ? ? ? 85 C0 0F 84 E2 00 00 00");

	if (!pattern)
		return;

	uintptr_t relative = *(int32_t*)(pattern + 3);
	CInput* Input = *(CInput**)(pattern + 7 + relative);

	if (!Input)
	{
		printf("Input is NULL\n");
		return;
	}

	I::Input = Input;
}

void HookGlobalVars()
{
	auto pattern = Memory.FindSignature("client.dll", "48 89 1D ? ? ? ? 48 8D 4D 18");

	if (!pattern)
		return;

	uintptr_t relative = *(int32_t*)(pattern + 3);
	CGlobalVarsBase* GlobalVars = *(CGlobalVarsBase**)(pattern + 7 + relative);

	if (!GlobalVars)
	{
		printf("GlobalVars is NULL!\n");
		return;
	}

	I::Global = GlobalVars;
}

// == hook works, but not ready ==

//typedef void(__fastcall* FrameStageNotifyFn)(void*, ClientFrameStage_t);
//FrameStageNotifyFn oFrameStageNotify = nullptr;
//
//void __fastcall hkFrameStageNotify(void* ecx, ClientFrameStage_t stage)
//{
//	if (!stage)
//		return;
//
//	if (!I::EngineClient->IsInGame() || !I::EngineClient->IsConnected())
//		return;
//
//	switch (stage)
//	{
//	case FRAME_RENDER_START:
//		for (int i = 1; i < 64; i++)
//		{
//			IClientEntity* currentEntity = I::ClientEntityList->GetClientEntity(i);
//
//			if (!currentEntity)
//				continue;
//
//			C_BasePlayer* p = reinterpret_cast<C_BasePlayer*>(currentEntity);
//
//			if (!p)
//			{
//				printf("p is NULL\n");
//				return;
//			}
//
//			if (!p || p->IsDormant() || p->GetHealth() <= 1)
//				continue;
//
//			p->UpdateClientSideAnimation();
//			p->SetupBones(boneCache.bones, 128, BONE_USED_BY_HITBOX, I::Global->curtime);
//		}
//		break;
//	}
//
//	oFrameStageNotify(ecx, stage);
//}
//
//void HookFrameStageNotify()
//{
//	void** vtable = *(void***)(I::BaseClientDLL);
//
//	if (!vtable)
//	{
//		printf("vtable is null\n");
//		return;
//	}
//
//	void* frameStageNotify = vtable[35];
//
//	if (!frameStageNotify)
//	{
//		printf("framestage is NULL\n");
//		return;
//	}
//
//	if (MH_CreateHook(frameStageNotify, &hkFrameStageNotify, (void**)&oFrameStageNotify) != MH_OK)
//	{
//		printf("couldnt create framestatenotify hook\n");
//		return;
//	}
//
//	MH_EnableHook(frameStageNotify);
//}

// == not ready ==
//typedef void(__fastcall* DrawModelExecuteFn)(void*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
//DrawModelExecuteFn oDrawModelExecute = nullptr;
//
//void __fastcall hkDrawModelExecute(void* ecx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* matrix)
//{
//	
//
//	oDrawModelExecute(ecx, state, pInfo, matrix);
//}
//
//void HookDrawModelExecute()
//{
//	void** vtable = *(void***)(I::ModelRender);
//
//	if (!vtable)
//	{
//		printf("vtable is NULL!\n");
//		return;
//	}
//
//	void* DrawModelExecute = vtable[19];
//
//	if (!DrawModelExecute)
//	{
//		printf("drawmodelexec is NULL\n");
//		return;
//	}
//
//	if (MH_CreateHook(DrawModelExecute, &hkDrawModelExecute, (void**)&oDrawModelExecute) != MH_OK)
//	{
//		printf("couldnt create hook\n");
//		return;
//	}
//	
//	MH_EnableHook(DrawModelExecute);
//}

void Hooks::Init()
{
	if (MH_Initialize() != MH_OK)
	{
		MessageBox(0, "MinHook couldn't initalize!", "Error", MB_OK | MB_ICONERROR);
		return;
	}
}

void Hooks::HookDirectX()
{
	void* vEndSceneAddr = GetIDirect3DDevice9();
	if (hwnd)
	{
		originalWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)hkWndProc);
		std::cout << "window procedure hooked successfully" << std::endl;
	}
	else {
		std::cout << "failed to hook window procedure" << std::endl;
	}

	// Now hook it
	MH_CreateHook(vEndSceneAddr, (LPVOID)hkEndScene, (LPVOID*)&oEndScene);
	MH_EnableHook(vEndSceneAddr);
	std::cout << "DirectX hook initialized successfully" << std::endl;
}

void Hooks::Run()
{
	Hooks::Init();
	HookDirectX();
	HookTHAT();
	//HookOverrideMove();
	HookCInput();
	HookGlobalVars();
}

void Hooks::Destroy()
{
	SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)originalWndProc);

	Menu::Destroy();
	I::Cvar->ConsoleColorPrintf({ 0, 255, 0, 255 }, "[Oh eX whY] Unloaded!\n");
	Draw.Destroy();
	g_HitMarkerListener.Unregister();
	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}
