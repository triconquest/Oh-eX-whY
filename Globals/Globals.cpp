#include "globals.h"

// features
bool G::bESP = true;
bool G::bESPAlwaysOn = true;
bool G::bTESP = true;
bool G::bAimbot = false;
bool G::bTriggerbot = false;
bool G::bShowTeam = false;
bool G::bCrosshair = false;
bool G::bRecoilCrosshair = true;
bool G::bFOVCircle = false;
bool G::bBoneESP = false;
bool G::bEnemyLines = false;
bool G::bAutoJoin = false;
bool G::bSpectatorList = false;
bool G::bHitmarker = true;
bool G::bHitMarkerDraw = false; // should we draw the hitmarker?
bool G::bEnemyKilled = false; // should we draw the killmarker?
bool G::bThirdPerson = false;
bool G::bSilentActive = false;
bool G::bBhop = false;
bool G::bHitSound = true;
bool G::bKillSound = true;
bool G::bResetBones = false;
bool G::bSetupBones = false;
bool G::bUseBones = false;
bool G::bHeadBone = false;
bool G::bFastStop = false;
bool G::bAimbotUseBones = true;
bool G::bNoRecoil = false;
bool G::bUseTraceRay = true;
int G::iAimTarget = -1;
int G::iFOV = 90.f;
int G::iAimBone = 49; // head bone
int G::iAimHold = 0x45; // E key
int G::iTriggerBotKey = 0;
bool G::bAimLocked = false;
bool G::bBulletShots = false;
bool G::bAutoShoot = false;
bool G::bPrintDamage = false;
bool G::bEnemyScopeLines = false;
bool G::bIsEnemyScoped = false;
bool G::bVisualEnemyAimingAtUs = false;
bool G::bNoFlash = true;
bool G::bTriggerBotHoldMode = true;
bool G::bDistanceToEnemy = false;
bool G::bRemoveScope = false;
float G::fTriggerBotDelay = 0.0f;
bool G::bKnifeBot = false;
bool G::bKnifeBotBackStabOnly = false;
bool G::bCornerESP = false;
bool G::bEnemiesBehind = true;
bool G::bShowEnemyWeapons = false;
float G::fTriggerBotRndDelayMin = 0.07f;
float G::fTriggerBotRndDelayMax = 0.15f;

bool G::bDisableListeners = false; // listener killswitch

// triggerbot hitgroups
bool G::bTriggerCustomHitBoxes = false;
bool G::bTriggerHead = true;
bool G::bTriggerChest = true;
bool G::bTriggerStomach = true;

// jailbreak globals
bool G::bRebelling = false;
int G::iRebellerIndex = -1;
bool G::bEnableCustomModeInfo = false;

// inputs
float G::fServerTick = 66.0f;
float G::fColor[3] = { 1.0f, 0.0f, 0.0f };
float G::fTargetColor[3] = { 1.0f, 0.0f, 1.0f };
float G::fCrosshairColor[3] = { 0.0f, 1.0f, 0.0f };
float G::fRecoilCrosshairColor[3] = { 1.0f, 0.0f, 0.0f };
float G::fCrosshairSize = 4.8f;
float G::fAimFOV = 30.f;

// misc
ULONGLONG G::fHitMarkerTime = 0;
char G::g_AutoJoinIP[64] = "";
Vector3 G::fSilentAngles{};
IClientEntity* G::pLastTarget = nullptr;

// entity cacher stuff
IClientEntity* E::Local = nullptr;
C_BasePlayer* E::LocalBase = nullptr;
IClientEntity* E::EnemyClient = nullptr;
C_BasePlayer* E::EnemyBasePlayer = nullptr;

// interface pointer declarations for sigs
CInput* I::Input = nullptr;
CGlobalVarsBase* I::Global = nullptr;
//IClientMode* I::ClientMode = nullptr;
//CClientModeShared* I::ClientModeShared = nullptr;


// debug stuff
#ifdef _DEBUG
Vector3 Debug::vDebugHeadPos{};
Vector3 Debug::vCurrentHeadPos{};
Vector3 Debug::vOldHeadPos{};
Vector3 Debug::vDebugHeadBone{};
bool Debug::bResetPositions = false;
int Debug::iX = 500;
int Debug::iY = 400;
#endif

float G::fEnemyHeight = 0.0f;

bool G::BoneValid(IClientEntity* target, int boneindex)
{
	if (!target)
		return false;

	Vector3 bonePos = target->GetBonePosition(boneindex);

	if (G::fEnemyHeight > 100.f)
		return false;

	if (bonePos == 0)
		return false;

	if (!std::isfinite(bonePos.x) || !std::isfinite(bonePos.y) || !std::isfinite(bonePos.z))
		return false;

	if (std::fabs(bonePos.x) > 10000.f || std::fabs(bonePos.y) > 10000.f || std::fabs(bonePos.z) > 10000.f)
		return false;

	return true;
}

bool G::BoneValid(Vector3 bonePos, Vector3 origin)
{
	if (G::fEnemyHeight > 100.f)
		return false;

	if (bonePos == 0)
		return false;

	if (!std::isfinite(bonePos.x) || !std::isfinite(bonePos.y) || !std::isfinite(bonePos.z))
		return false;

	if (std::fabs(bonePos.x) > 10000.f || std::fabs(bonePos.y) > 10000.f || std::fabs(bonePos.z) > 10000.f)
		return false;

	if ((bonePos - origin).Length() < 20.f)
		return false;

	return true;
}
