#include "Misc.h"
#include "../Globals/Globals.h"
#include "../Utils/DrawManager/DrawManager.h"
#include "../Utils/Render/Render.h"
#include "../Utils/MiscUtils/MiscUtils.h"
#include "../SDK/Interfaces/Entities/C_BasePlayer.h"
#include "../SDK/Includes/ehandle.h"
#include "../SDK/Includes/const.h"
#include "../SDK/Includes/client_class.h"
#include "../SDK/Interfaces/Entities/C_CSPlayer.h"

#include <iostream>

#pragma warning(disable:4996)

struct Ray_t;
std::unordered_map<int, KillerInfo> killerMap;

void CMisc::Run()
{
	//if (/*!I::EngineClient->IsInGame() || */I::EngineVGui->IsGameUIVisible())
	//	return;

	DrawCrosshair();
	DrawFOVCircle();
	AutoJoin();
	HitMarker();
	DrawBullets();
	FindAttacker();
	//FlagRebels();
}

void CMisc::DrawCrosshair()
{
	if (!G::bCrosshair || I::EngineVGui->IsGameUIVisible() || !I::EngineClient->IsInGame())
		return;

	float centerX = I::iScreenWidth / 2.0f;
	float centerY = I::iScreenHeight / 2.0f;
	D3DCOLOR mainColor = Draw.FloatToD3D(G::fCrosshairColor);
	D3DCOLOR outlineColor = D3DCOLOR_ARGB(255, 0, 0, 0);

	float lineWidth = 1.0f;

	float outline = 1.0f;
	Draw.DrawFilledRect(centerX - G::fCrosshairSize - outline, centerY - (lineWidth / 2) - outline,
		(G::fCrosshairSize * 2) + (outline * 2), lineWidth + (outline * 2), outlineColor);
	Draw.DrawFilledRect(centerX + G::fCrosshairSize - outline, centerY - (lineWidth / 2) - outline,
		outline * 2, lineWidth + (outline * 2), outlineColor);

	Draw.DrawFilledRect(centerX - (lineWidth / 2) - outline, centerY - G::fCrosshairSize - outline,
		lineWidth + (outline * 2), G::fCrosshairSize * 2 + (outline * 2), outlineColor);
	Draw.DrawFilledRect(centerX - (lineWidth / 2) - outline, centerY + G::fCrosshairSize - outline,
		lineWidth + (outline * 2), outline * 2, outlineColor);

	Draw.DrawFilledRect(centerX - G::fCrosshairSize, centerY - (lineWidth / 2),
		G::fCrosshairSize * 2, lineWidth, mainColor);

	Draw.DrawFilledRect(centerX - (lineWidth / 2), centerY - G::fCrosshairSize,
		lineWidth, G::fCrosshairSize * 2, mainColor);
}

void CMisc::DrawFOVCircle()
{
	if (!G::bFOVCircle || I::EngineVGui->IsGameUIVisible() || !I::EngineClient->IsInGame())
		return;

	float radius = Draw.FOVToScreenRadius(G::fAimFOV, I::iScreenHeight, 90.f);
	Draw.DrawCircle(I::iScreenWidth / 2, I::iScreenHeight / 2, radius + 10.f, D3DCOLOR_ARGB(255, 255, 255, 255), 100);
}

void CMisc::AutoJoin()
{
	if (I::EngineClient->IsInGame() || !G::bAutoJoin)
		return;

	if (!strcmp(G::g_AutoJoinIP, ""))
	{
		I::EngineClient->ClientCmd_Unrestricted("echo [AutoJoin] IP is invalid!");
		return;
	}

	static ULONGLONG lastAttemptTime = 0;
	ULONGLONG currentTime = GetTickCount64();

	if (currentTime - lastAttemptTime >= 7000)
	{
		char cmd[64];
		sprintf(cmd, "connect %s", G::g_AutoJoinIP);


		I::EngineClient->ClientCmd_Unrestricted(cmd);

		lastAttemptTime = currentTime;
	}
}

void CMisc::HitMarker()
{
	if (!I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	if (!G::bHitMarkerDraw || G::bDisableListeners)
		return;

	DWORD now = GetTickCount64();
	DWORD delta = now - G::fHitMarkerTime;

	if (delta > 150)
	{
		G::bHitMarkerDraw = false;
		return;
	}

	int centerX = I::iScreenWidth / 2;
	int centerY = I::iScreenHeight / 2;

	D3DCOLOR color = G::bEnemyKilled ? D3DCOLOR_ARGB(255, 255, 0, 0) : D3DCOLOR_ARGB(255, 255, 255, 255);

	Draw.DrawLine(centerX - 8, centerY - 8, centerX - 2, centerY - 2, 1.5f, color);
	Draw.DrawLine(centerX + 8, centerY - 8, centerX + 2, centerY - 2, 1.5f, color);
	Draw.DrawLine(centerX - 8, centerY + 8, centerX - 2, centerY + 2, 1.5f, color);
	Draw.DrawLine(centerX + 8, centerY + 8, centerX + 2, centerY + 2, 1.5f, color);
}

void CMisc::Bhop(CUserCmd* cmd)
{
	if (!G::bBhop || !cmd)
		return;

	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal || pLocal->GetHealth() <= 1)
		return;

	if (!(cmd->buttons & IN_JUMP))
		return;

	static int spam_jumps_left = 0;
	static int spam_cooldown_ticks = 0;
	static int delay_ticks = 0;
	static int landed_tick = 0;

	bool onGround = (pLocal->GetFlags() & FL_ONGROUND);
	int current_tick = I::Global->tickcount;

	if (onGround)
	{
		bool delay_jump = MiscUtils.RandomFloat(0.f, 1.f) < 0.2f;

		if (delay_jump)
		{
			delay_ticks = MiscUtils.RandomInt(1, 2); // delay 1-2 ticks
			landed_tick = current_tick;
			cmd->buttons &= ~IN_JUMP;
			return;
		}
		else
		{
			delay_ticks = 0;
		}
	}

	if (delay_ticks > 0)
	{
		if ((current_tick - landed_tick) < delay_ticks)
		{
			cmd->buttons &= ~IN_JUMP;
			return;
		}
	}

	if (onGround)
	{
		cmd->buttons |= IN_JUMP;

		spam_jumps_left = MiscUtils.RandomInt(12, 16); // random spam jump, avoids detection
		spam_cooldown_ticks = 0;
	}
	else
	{
		if (spam_jumps_left > 0)
		{
			if (spam_cooldown_ticks == 0)
			{
				cmd->buttons |= IN_JUMP;
				spam_jumps_left--;
				spam_cooldown_ticks = MiscUtils.RandomInt(0, 1);
			}
			else
			{
				cmd->buttons &= ~IN_JUMP;
				spam_cooldown_ticks--;
			}
		}
		else
		{
			cmd->buttons &= ~IN_JUMP;
		}
	}
}

void CMisc::DrawBullets()
{
	if (!G::bBulletShots || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	ULONGLONG currentTime = GetTickCount64();

	if (size_t i = 0; i < impacts.size())
	{
		if (currentTime - impacts[i].time > 500)
		{
			impacts.erase(impacts.begin() + i);
		}
		else
		{
			Vector2 screen;
			VMatrix vMatrix = I::EngineClient->WorldToScreenMatrix();

			if (Render::WorldToScreen(impacts[i].position, screen, vMatrix, I::iScreenWidth, I::iScreenHeight))
			{
				Draw.DrawFilledRect(screen.x, screen.y, 5, 5, D3DCOLOR_ARGB(255, 255, 0, 255));
				Draw.DrawLine(I::iScreenWidth / 2, I::iScreenHeight / 2, screen.x, screen.y, 1.5f, D3DCOLOR_ARGB(255, 255, 0, 255));
			}
		}
		++i;
	}
}

IHandleEntity* CBaseHandle::Get() const
{
	if (!IsValid())
		return nullptr;

	return I::ClientEntityList->GetClientEntityFromHandle(*this);
}

ConVar* GrabVarUnprotected(const char* varname)
{
	ConVar* var = I::Cvar->FindVar(varname);

	if (!var)
		return nullptr;

	var->m_nFlags &= ~(FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN | FCVAR_NOT_CONNECTED);

	return var;
}

void CMisc::CustomFOV(CViewSetup* view)
{
	if (G::iFOV == 90 || !I::EngineClient->IsInGame())
		return;

	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal)
		return;
	
	// change back to 90 if we're scoped so we can use the game's scope fov
	if (view->fov > 90 && MiscUtils.Scoped(pLocal))
		view->fov = 90.f;

	if (view && !MiscUtils.Scoped(pLocal))
		view->fov = G::iFOV;
}

void CMisc::FastStop(CUserCmd* cmd)
{
	if (!G::bFastStop || !I::EngineClient->IsInGame())
		return;

	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal)
		return;

	C_BasePlayer* pLocalBase = reinterpret_cast<C_BasePlayer*>(pLocal);

	if (!pLocalBase || pLocalBase->deadflag())
		return;

	// fucky shit
	if (GetAsyncKeyState(0x41) || // A key
		GetAsyncKeyState(0x44) || // D key
		GetAsyncKeyState(0x53) || // S key
		GetAsyncKeyState(0x57)) // W key
		return;

	Vector3 velocity = pLocalBase->m_vecVelocity();
	velocity.z = 0.0f;

	if (velocity.Length2() < 1.0f)
		return;

	Vector3 viewAngles = cmd->viewangles;
	Vector3 forward, right;

	Math::AngleVectors(viewAngles, &forward, &right, nullptr);

	forward.z = 0.0f;
	right.z = 0.0f;

	forward.NormalizeInPlace();
	right.NormalizeInPlace();

	float forwardMove = -velocity.dot(forward);
	float sideMove = -velocity.dot(right);

	cmd->forwardmove = forwardMove * 5.0f;
	cmd->sidemove = sideMove * 5.0f;
}

void CMisc::DynamicCrosshair(CUserCmd* cmd)
{
	if (!G::bRecoilCrosshair || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	IClientEntity* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal)
		return;

	const char* netWeapon = MiscUtils.GetNetworkedWeapon(pLocal);

	if (!netWeapon)
		return;

	if (strstr(netWeapon, "CKnife") || MiscUtils.LocalHoldingSniper())
		return;

	static int shotsFired = 0;

	if (cmd->buttons & IN_ATTACK)
		shotsFired++;
	else shotsFired = 0;

	if (shotsFired < 2)
		return;

	C_BasePlayer* pLocalBase = reinterpret_cast<C_BasePlayer*>(pLocal);

	if (!pLocalBase || pLocalBase->deadflag())
		return;

	Vector3 bulletAngle = cmd->viewangles + pLocalBase->m_vecPunchAngle() * 2.0f;

	Vector3 bulletDir;
	Math::AngleVectors(bulletAngle, &bulletDir, nullptr, nullptr);

	Vector3 bulletEnd = (pLocal->GetAbsOrigin() + pLocalBase->m_vecViewOffset()) + bulletDir * 8192.0f;

	Vector2 screenBullet;

	// interpolating the crosshair so it looks better
	static Vector2 prevPos = { 0, 0 };
	static bool hasPrev = false;

	Vector2 newPos;
	if (Render::WorldToScreen(bulletEnd, newPos, I::EngineClient->WorldToScreenMatrix(), I::iScreenWidth, I::iScreenHeight))
	{
		if (!hasPrev)
		{
			prevPos = newPos;
			hasPrev = true;
		}

		float smooth = 0.1f;
		prevPos.x = prevPos.x + (newPos.x - prevPos.x) * smooth;
		prevPos.y = prevPos.y + (newPos.y - prevPos.y) * smooth;

		Draw.DrawCrosshairOnPosition(prevPos.x, prevPos.y);
	}
}

void CMisc::NoRecoil(CUserCmd* cmd)
{
	if (!G::bNoRecoil || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	IClientEntity* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal)
		return;

	C_BasePlayer* pLocalBase = reinterpret_cast<C_BasePlayer*>(pLocal);

	if (!pLocalBase || pLocalBase->deadflag())
		return;

	cmd->viewangles = cmd->viewangles - pLocalBase->m_vecPunchAngle() * 2.f;
}

void CMisc::KnifeBot(CUserCmd* cmd)
{
	if (!G::bKnifeBot || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal || pLocal->GetHealth() <= 1)
		return;

	const char* netWeapon = MiscUtils.GetNetworkedWeapon(pLocal);

	if (!strstr(netWeapon, "CKnife"))
		return;

	for (int i = 1; i < 64; i++)
	{
		IClientEntity* pEntity = I::ClientEntityList->GetClientEntity(i);

		if (!pEntity || pEntity->GetHealth() <= 1 || pEntity->IsDormant())
			continue;

		if (pEntity->GetTeam() == pLocal->GetTeam())
			continue;

		if (!G::bKnifeBotBackStabOnly)
		{
			float distance = Math::GetDistance(pLocal->GetAbsOrigin(), pEntity->GetAbsOrigin());
			distance /= 100.f;

			if (distance <= 0.630f)
				cmd->buttons |= IN_ATTACK2;
		}
		else
		{
			C_CSPlayer* pCSEntity = reinterpret_cast<C_CSPlayer*>(pEntity);

			if (!pCSEntity)
				continue;

			Vector3 enemyToLocal = pEntity->GetAbsOrigin() - pLocal->GetAbsOrigin();
			enemyToLocal.NormalizeInPlace();

			float angleX = pCSEntity->m_angEyeAnglesX();
			float angleY = pCSEntity->m_angEyeAnglesY();

			Vector3 angles = { angleX, angleY, 0.0f };

			Vector3 forward;
			Math::AngleVectors(angles, &forward, nullptr, nullptr);

			float dot = forward.dot(enemyToLocal);

			float distance = Math::GetDistance(pLocal->GetAbsOrigin(), pEntity->GetAbsOrigin());
			distance /= 100.f;

			if (dot > 0.82f && distance <= 0.630f)
				cmd->buttons |= IN_ATTACK2;
		}
	}
}

void CMisc::FindAttacker()
{
	if (!G::bEnableCustomModeInfo || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	int offsetY = 400;

	Draw.DrawOutlineScreenText("Killers", 300, 385, FONT_SMALL, D3DCOLOR_ARGB(255, 0, 255, 255));

	for (const auto& entry : killerMap)
	{
		auto& killer = entry.second;

		auto pKillerEntity = I::ClientEntityList->GetClientEntity(killer.index);

		if (!pKillerEntity || pKillerEntity->GetHealth() <= 1)
			continue;

		const char* netWeapon = MiscUtils.GetNetworkedWeapon(pKillerEntity);
		const char* weapon = MiscUtils.GetCleanWeaponName(netWeapon);

		char buffer[256];
		sprintf_s(buffer, sizeof(buffer), "%s, %s, %d kills", killer.name.c_str(), weapon, killer.victimCount);

		Draw.DrawFilledBackground(buffer, 300, offsetY, D3DCOLOR_ARGB(255, 0, 0, 0));
		Draw.DrawScreenText(buffer, 300, offsetY, FONT_MEDIUM, D3DCOLOR_ARGB(255, 255, 0, 0));
		offsetY += 20;
	}
}

// fuckass function crashes randomly needs 200 checks to work
//void CMisc::FlagRebels()
//{
//	if (!I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
//		return;
//
//	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());
//
//	if (!pLocal)
//		return;
//
//	for (int i = 1; i < 64; i++)
//	{
//		IClientEntity* pEnemyEntity = I::ClientEntityList->GetClientEntity(i);
//
//		if (!pEnemyEntity)
//			continue;
//
//		if (pEnemyEntity->GetTeam() == pLocal->GetTeam())
//			continue;
//
//		C_BasePlayer* pEnemyBase = reinterpret_cast<C_BasePlayer*>(pEnemyEntity);
//
//		if (!pEnemyBase)
//			continue;
//
//		if (pEnemyBase->deadflag())
//			continue;
//
//		if (!MiscUtils.HoldingMelee(pEnemyEntity))
//			G::bRebelling = true;
//	}
//}
