#pragma once

#include "../Utils/vector.h"
#include "../SDK/Interfaces/Entities/IClientEntity.h"
#include "../SDK/Includes/usercmd.h"

#define	HITGROUP_GENERIC 0
#define	HITGROUP_HEAD 1
#define	HITGROUP_CHEST 2
#define	HITGROUP_STOMACH 3
#define HITGROUP_LEFTARM 4	
#define HITGROUP_RIGHTARM 5
#define HITGROUP_LEFTLEG 6
#define HITGROUP_RIGHTLEG 7
#define HITGROUP_GEAR 10

enum AimBones
{
	BONE_HEAD = 49,
	BONE_CHEST = 11,
	BONE_PELVIS = 9
};

enum AimKeys
{
	AIM_NONE = 0,
	AIM_E = 0x45
};

enum TriggerBotKeys
{
	TRIGGERBOT_NONE = 0,
	TRIGGERBOT_E = 0x45
};

class CAimbot
{
public:
	void Run();
	void RunSilent(CUserCmd* cmd);
	IClientEntity* GetTarget(IClientEntity* pLocal);
	void SetViewAngles(Vector3& angles);
	void AutoShoot(IClientEntity* target, CUserCmd* cmd);
	void TriggerBot2(CUserCmd* cmd);
	bool TraceHit(const Vector3& start, const Vector3& end, IClientEntity* entity);
};

inline CAimbot Aimbot;
