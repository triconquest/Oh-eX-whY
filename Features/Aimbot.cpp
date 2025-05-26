#include "Aimbot.h"
#include "../Globals/Globals.h"
#include "../Utils/MiscUtils/MiscUtils.h"
#include "../SDK/Includes/ihandleentity.h"
#include "../SDK/Includes/studio.h"
#include "../SDK/Interfaces/Entities/C_BasePlayer.h"
#include "../SDK/Interfaces/IEngineTrace.h"

void CAimbot::Run()
{
	if (!G::bAimbot || I::EngineVGui->IsGameUIVisible() || !I::EngineClient->IsInGame())
		return;

	if (!GetAsyncKeyState(0x45))
		return;

	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal || pLocal->GetHealth() <= 0)
		return;

	IClientEntity* pTarget = GetTarget(pLocal);
	if (!pTarget)
		return;

	C_BasePlayer* pLocalPlayer = reinterpret_cast<C_BasePlayer*>(pLocal);

	if (!pLocalPlayer || pLocalPlayer->deadflag())
		return;

	C_BasePlayer* pTargetBase = reinterpret_cast<C_BasePlayer*>(pTarget);

	if (!pTargetBase)
		return;

	Vector3 headWithoutBone = pTarget->GetAbsOrigin() + pTargetBase->m_vecViewOffset();
	headWithoutBone.x += 10.f;

	Vector3 headBone = pTarget->GetBonePosition(G::iAimBone);
	Vector3 enemyHead = G::bAimbotUseBones ? headBone : headWithoutBone; //  //G::BoneValid(pTarget, G::iAimBone)

	Vector3 pLocalPos = pLocal->GetAbsOrigin() + pLocalPlayer->m_vecViewOffset();

	Vector3 angles = Math::CalcAngle(pLocalPos, enemyHead);

	if (TraceHit(pLocalPos, enemyHead, pTarget))
	{
		Math::NormalizeAngles(angles);
		SetViewAngles(angles);
	}
}

void CAimbot::RunSilent(CUserCmd* cmd)
{
	if (!G::bSilentActive || I::EngineVGui->IsGameUIVisible() || !I::EngineClient->IsInGame())
		return;

	G::bAimLocked = false;

	if (G::iAimHold != 0 && !GetAsyncKeyState(G::iAimHold))
		return;

	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal || pLocal->GetHealth() <= 0)
		return;

	IClientEntity* pTarget = GetTarget(pLocal);

	if (!pTarget)
		return;

	C_BasePlayer* pLocalPlayer = reinterpret_cast<C_BasePlayer*>(pLocal);

	if (!pLocalPlayer || pLocalPlayer->deadflag())
		return;

	C_BasePlayer* pTargetBase = reinterpret_cast<C_BasePlayer*>(pTarget);

	if (!pTargetBase)
		return;

	Vector3 headWithoutBone = pTarget->GetAbsOrigin() + pTargetBase->m_vecViewOffset();
	headWithoutBone.x += 10.f; // aim close to the head, not at the origin

	Vector3 headBone = pTarget->GetBonePosition(G::iAimBone);
	Vector3 enemyHead = G::BoneValid(pTarget, G::iAimBone) ? headBone : headWithoutBone; // G::bAimbotUseBones

	Vector3 pLocalPos = pLocal->GetAbsOrigin() + pLocalPlayer->m_vecViewOffset();

	Vector3 anglesToTarget = Math::CalcAngle(pLocalPos, enemyHead);

	Vector3 oldAngles = cmd->viewangles;

	if (TraceHit(pLocalPos, enemyHead, pTarget) && G::bUseTraceRay)
	{
		if (cmd->buttons & IN_ATTACK)
			cmd->viewangles = anglesToTarget;

		G::bAimLocked = true;

		if (G::bAutoShoot)
		{
			cmd->viewangles = anglesToTarget;
			AutoShoot(pTarget, cmd);
		}
	}

	if (!G::bUseTraceRay)
	{
		if (cmd->buttons & IN_ATTACK)
			cmd->viewangles = anglesToTarget;

		G::bAimLocked = true;
	}
}

//IClientEntity* CAimbot::GetSilentTarget(IClientEntity* pLocal, Vector3 angles)
//{
//	IClientEntity* pBestTarget = nullptr;
//	float fBestFov = FLT_MAX;
//
//	G::iAimTarget = -1;
//
//	for (int i = 1; i < 64; i++)
//	{
//		IClientEntity* pCurEntity = I::ClientEntityList->GetClientEntity(i);
//
//		if (!pCurEntity || pCurEntity->IsDormant())
//			continue;
//
//		if (pCurEntity == pLocal || (pCurEntity->GetTeam() == pLocal->GetTeam()))
//			continue;
//
//		int iEntHealth = pCurEntity->GetHealth();
//		if (iEntHealth <= 1)
//			continue;
//
//		Vector3 CurEntityPos = pCurEntity->GetAbsOrigin();
//		Vector3 aimAngleToEntity = Math::CalcAngle(pLocal->GetAbsOrigin(), CurEntityPos);
//
//		Vector3 viewAngles;
//		I::EngineClient->GetViewAngles(viewAngles);
//		float fov = Math::GetFOV(viewAngles, aimAngleToEntity);
//
//		if (fov < fBestFov && fov <= G::fAimFOV)
//		{
//			fBestFov = fov;
//			pBestTarget = pCurEntity;
//			G::iAimTarget = i;
//		}
//	}
//
//	return pBestTarget;
//}

IClientEntity* CAimbot::GetTarget(IClientEntity* pLocal)
{
	IClientEntity* pBestTarget = nullptr;
	float fBestFov = FLT_MAX;

	Vector3 viewAngles;
	I::EngineClient->GetViewAngles(viewAngles);

	for (int i = 1; i < 64; i++)
	{
		IClientEntity* pCurEntity = I::ClientEntityList->GetClientEntity(i);

		if (!pCurEntity || pCurEntity->IsDormant())
			continue;

		if (pCurEntity == pLocal || (pCurEntity->GetTeam() == pLocal->GetTeam()))
			continue;

		int iEntHealth = pCurEntity->GetHealth();

		if (iEntHealth <= 1)
			continue;

		Vector3 CurEntityPos = pCurEntity->GetAbsOrigin();
		Vector3 aimAngle = Math::CalcAngle(pLocal->GetAbsOrigin(), CurEntityPos);

		float fov = Math::GetFOV(viewAngles, aimAngle);

		if (fov < fBestFov && fov <= G::fAimFOV)
		{
			fBestFov = fov;
			pBestTarget = pCurEntity;
			G::iAimTarget = i;
		}
	}

	return pBestTarget;
}

void CAimbot::SetViewAngles(Vector3& angles)
{
	I::EngineClient->SetViewAngles(angles);
}

void CAimbot::AutoShoot(IClientEntity* target, CUserCmd* cmd)
{
	if (!G::bAutoShoot || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	static bool bShot = false;

	if (!bShot)
	{
		cmd->buttons |= IN_ATTACK;
		bShot = true;
	}
	else
	{
		cmd->buttons &= ~IN_ATTACK;
		bShot = false;
	}
}

void CAimbot::TriggerBot2(CUserCmd* cmd)
{
	if (!G::bTriggerbot || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	if (G::iTriggerBotKey != 0 && !GetAsyncKeyState(G::iTriggerBotKey))
		return;

	IClientEntity* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal)
		return;

	C_BasePlayer* pLocalBase = reinterpret_cast<C_BasePlayer*>(pLocal);

	if (!pLocalBase || pLocalBase->deadflag())
		return;

	Vector3 eyePos = pLocal->GetAbsOrigin() + pLocalBase->m_vecViewOffset();
	Vector3 aimPunch = pLocalBase->m_vecPunchAngle();

	Vector3 viewAngles = cmd->viewangles + aimPunch;
	Vector3 direction;

	Math::AngleVectors(viewAngles, &direction, nullptr, nullptr);
	direction.normalize();

	Vector3 dst = eyePos + direction * 8192.f;

	Ray_t ray;
	ray.Init(eyePos, dst);

	CTraceFilter filter(pLocal);

	trace_t trace;
	I::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);

	C_BaseEntity* hitEnt = reinterpret_cast<C_BaseEntity*>(trace.m_pEnt);

	if (!hitEnt || !hitEnt->IsPlayer())
		return;

	IClientEntity* enemyTeamClient = reinterpret_cast<IClientEntity*>(trace.m_pEnt);

	if (!enemyTeamClient)
		return;

	if (enemyTeamClient->GetTeam() == pLocal->GetTeam())
		return;

	if (pLocalBase->m_vecVelocity().Length2() > 150.f || !(pLocal->GetFlags() & FL_ONGROUND))
		return;

	static bool isTargeting = false;
	static float lastTargetTime = 0.0f;
	static float triggerDelay = 0.0f;

	float currentTime = I::Global->curtime;

	if (!hitEnt || !hitEnt->IsPlayer() || enemyTeamClient->GetTeam() == pLocal->GetTeam())
	{
		isTargeting = false;
		lastTargetTime = 0.0f;
		return;
	}

	bool shouldShoot = false;

	if (!G::bTriggerCustomHitBoxes)
	{
		if (!MiscUtils.LocalHoldingSniper())
		{
			if (trace.hitgroup > HITGROUP_GENERIC)
				shouldShoot = true;
		}
		else
		{
			if (MiscUtils.Scoped(pLocal))
			{
				if (trace.hitgroup > HITGROUP_GENERIC)
					shouldShoot = true;
			}
		}
	}
	else
	{
		switch (trace.hitgroup)
		{
		case HITGROUP_HEAD:
			if (G::bTriggerHead) shouldShoot = true;
			break;
		case HITGROUP_CHEST:
			if (G::bTriggerChest) shouldShoot = true;
			break;
		case HITGROUP_STOMACH:
			if (G::bTriggerStomach) shouldShoot = true;
			break;
		}
	}

	if (shouldShoot)
	{
		if (!isTargeting)
		{
			isTargeting = true;
			lastTargetTime = currentTime;
			triggerDelay = MiscUtils.RandomFloat(G::fTriggerBotRndDelayMin, G::fTriggerBotRndDelayMax);
		}

		if ((currentTime - lastTargetTime) >= triggerDelay)
		{
			cmd->buttons |= IN_ATTACK;
			isTargeting = false;
		}
	}
	else
	{
		isTargeting = false;
		lastTargetTime = 0.0f;
	}
}

bool CAimbot::TraceHit(const Vector3& start, const Vector3& end, IClientEntity* entity)
{
	if (!I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return false;

	IClientEntity* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal || !I::EngineTrace)
		return false;

	Ray_t ray;
	ray.Init(start, end);

	CTraceFilter filter(pLocal);

	trace_t trace;
	I::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);

	return (trace.m_pEnt == entity || trace.fraction > 0.97f);
}
