#pragma once
#include "../SDK/Interfaces/CreateInterface.h"
#include "../SDK/Interfaces/IBaseClientDLL.h"
#include "../SDK/Interfaces/IClientEntityList.h"
#include "../SDK/Interfaces/IEngineVGui.h"
#include "../SDK/Interfaces/IVEngineClient.h"
#include "../SDK/Interfaces/IVModelInfo.h"
#include "../SDK/Interfaces/IInputSystem.h"
#include "../SDK/Interfaces/IMatSystemSurface.h"
#include "../SDK/Interfaces/IInput.h"
#include "../SDK/Interfaces/ICvar.h"
#include "../SDK/Interfaces/IMaterialSystem.h"
#include "../SDK/Includes/igameevents.h"
#include "../Utils/Memory/Memory.h"

#include <d3d9.h>

class C_BasePlayer;

namespace G
{
	// features
	extern bool bESP;
	extern bool bESPAlwaysOn;
	extern bool bTESP;
	extern bool bAimbot;
	extern bool bTriggerbot;
	extern bool bShowTeam;
	extern bool bCrosshair;
	extern bool bRecoilCrosshair;
	extern bool bFOVCircle;
	extern bool bBoneESP;
	extern bool bEnemyLines;
	extern bool bAutoJoin;
	extern bool bSpectatorList;
	extern bool bHitmarker;
	extern bool bHitMarkerDraw;
	extern bool bEnemyKilled;
	extern bool bThirdPerson;
	extern bool bSilentActive;
	extern bool bBhop;
	extern bool bHitSound;
	extern bool bKillSound;
	extern bool bResetBones;
	extern bool bSetupBones;
	extern bool bUseBones;
	extern bool bHeadBone;
	extern bool bFastStop;
	extern bool bAimbotUseBones;
	extern bool bNoRecoil;
	extern int iAimTarget;
	extern int iFOV;
	extern int iAimBone;
	extern int iAimHold;
	extern int iTriggerBotKey;
	extern bool bAimLocked;
	extern bool bBulletShots;
	extern bool bAutoShoot;
	extern bool bUseTraceRay;
	extern bool bPrintDamage;
	extern bool bEnemyScopeLines;
	extern bool bIsEnemyScoped;
	extern bool bVisualEnemyAimingAtUs;
	extern bool bNoFlash;
	extern bool bTriggerBotHoldMode;
	extern bool bDistanceToEnemy;
	extern bool bRemoveScope;
	extern float fTriggerBotDelay;
	extern bool bKnifeBot;
	extern bool bKnifeBotBackStabOnly;
	extern bool bCornerESP;
	extern bool bEnemiesBehind;
	extern bool bShowEnemyWeapons;
	extern float fTriggerBotRndDelayMin;
	extern float fTriggerBotRndDelayMax;

	extern bool bDisableListeners; // to avoid listener detection

	// triggerbot hitgroups
	extern bool bTriggerCustomHitBoxes;
	extern bool bTriggerHead;
	extern bool bTriggerChest;
	extern bool bTriggerStomach;

	// jailbreak globals
	extern bool bRebelling;
	extern int iRebellerIndex;
	extern bool bEnableCustomModeInfo;

	// inputs
	extern float fMaxDistance;
	extern float fAimFOV;
	extern float fServerTick;
	extern float fColor[3];
	extern float fTargetColor[3];
	extern float fRecoilCrosshairColor[3];
	extern char g_AutoJoinIP[64];
	extern float fCrosshairColor[3];
	extern float fCrosshairSize;

	// global misc
	extern float fEnemyHeight;
	extern ULONGLONG fHitMarkerTime;
	extern Vector3 fSilentAngles;
	extern IClientEntity* pLastTarget;
	extern bool BoneValid(IClientEntity* target, int boneindex);
	extern bool BoneValid(Vector3 bonePos, Vector3 origin);
}

// for entity cacher
namespace E
{
	extern IClientEntity* Local;
	extern C_BasePlayer* LocalBase;
	extern IClientEntity* EnemyClient;
	extern C_BasePlayer* EnemyBasePlayer;
}

namespace Debug
{
#ifdef _DEBUG
	extern Vector3 vDebugHeadPos;
	extern Vector3 vCurrentHeadPos;
	extern Vector3 vOldHeadPos;
	extern Vector3 vDebugHeadBone;
	extern bool bResetPositions;
	extern int iX;
	extern int iY;
#endif
}

class IEngineTrace; // forward declare because the whole project fucks up if we include the header.
class IVModelRender; // another shitty interface

// interfaces
namespace I
{
	inline IBaseClientDLL* BaseClientDLL = CInterface::GetInterface<IBaseClientDLL>("VClient017", "client.dll");
	inline IClientEntityList* ClientEntityList = CInterface::GetInterface<IClientEntityList>("VClientEntityList003", "client.dll");
	inline IEngineVGui* EngineVGui = CInterface::GetInterface<IEngineVGui>("VEngineVGui001", "engine.dll");
	inline IVEngineClient013* EngineClient = CInterface::GetInterface<IVEngineClient013>("VEngineClient014", "engine.dll");
	inline IVModelInfoClient* ModelInfo = CInterface::GetInterface<IVModelInfoClient>("VModelInfoClient006", "engine.dll");
	inline IInputSystem* NInputSystem = CInterface::GetInterface<IInputSystem>("InputSystemVersion001", "inputsystem.dll");
	inline IMatSystemSurface* Surface = CInterface::GetInterface<IMatSystemSurface>("VGUI_Surface030", "vguimatsurface.dll");
	inline IGameEventManager2* GameEvent = CInterface::GetInterface<IGameEventManager2>("GAMEEVENTSMANAGER002", "engine.dll");
	inline ICvar* Cvar = CInterface::GetInterface<ICvar>("VEngineCvar004", "vstdlib.dll");
	//inline IMaterialSystem* MaterialSystem = CInterface::GetInterface<IMaterialSystem>("VMaterialSystem080", "MaterialSystem.dll");
	inline IVModelRender* ModelRender = CInterface::GetInterface<IVModelRender>("VEngineModel016", "engine.dll");
	inline IEngineTrace* EngineTrace = CInterface::GetInterface<IEngineTrace>("EngineTraceClient003", "engine.dll");
	extern CInput* Input;
	extern CGlobalVarsBase* Global;
	//extern IClientMode* ClientMode;
	//extern CClientModeShared* ClientModeShared;
	inline int iScreenHeight = BaseClientDLL->GetScreenHeight();
	inline int iScreenWidth = BaseClientDLL->GetScreenWidth();
}

/*I::BaseClientDLL = U::Interface.Get<IBaseClientDLL*>("client.dll", "VClient017");
		I::Input = **reinterpret_cast<CInput***>((*reinterpret_cast<uintptr_t**>(I::BaseClientDLL))[15] + 0x2);
		I::ClientEntityList = U::Interface.Get<IClientEntityList*>("client.dll", "VClientEntityList003");
		I::GameMovement = U::Interface.Get<IGameMovement*>("client.dll", "GameMovement001");
		I::ClientPrediction = U::Interface.Get<CPrediction*>("client.dll", "VClientPrediction001");
		I::EngineClient = U::Interface.Get<IVEngineClient*>("engine.dll", "VEngineClient014");
		I::ModelInfoClient = U::Interface.Get<IVModelInfoClient*>("engine.dll", "VModelInfoClient006");
		I::EngineVGui = U::Interface.Get<IEngineVGui*>("engine.dll", "VEngineVGui001");
		I::InputSystem = U::Interface.Get<IInputSystem*>("inputsystem.dll", "InputSystemVersion001");
		I::EngineTrace = U::Interface.Get<IEngineTrace*>("engine.dll", "EngineTraceClient003");
		I::RenderView = U::Interface.Get<IVRenderView*>("engine.dll", "VEngineRenderView014");
		I::ModelRender = U::Interface.Get<IVModelRender*>("engine.dll", "VEngineModel016");
		I::GameEventManager = U::Interface.Get<IGameEventManager2*>("engine.dll", "GAMEEVENTSMANAGER002");
		I::StudioRender = U::Interface.Get<IStudioRender*>("studiorender.dll", "VStudioRender025");
		I::MatSystemSurface = U::Interface.Get<IMatSystemSurface*>("vguimatsurface.dll", "VGUI_Surface030");
		I::MaterialSystem = U::Interface.Get<IMaterialSystem*>("MaterialSystem.dll", "VMaterialSystem080");
		I::Cvar = U::Interface.Get<ICvar*>("vstdlib.dll", "VEngineCvar004");
		I::ClientMode = **(ClientModeShared***)(U::Pattern.Find("client.dll", "8B 0D ? ? ? ? 8B 01 5D FF 60 28 CC") + 0x2);
		I::HudChat = (CHudChat*)(((uintptr_t*)(I::ClientMode))[4]);*/