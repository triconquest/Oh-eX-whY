#include "Visuals.h"
#include "../Globals/Globals.h"
#include "../SDK/Interfaces/Entities/C_BasePlayer.h"
#include "../SDK/Interfaces/IEngineTrace.h"
#include "../SDK/Interfaces/Entities/C_CSPlayer.h"
#include "../Utils/DrawManager/DrawManager.h"
#include "../Utils/Render/Render.h"
#include "../Utils/MiscUtils/MiscUtils.h"

#include <unordered_map>
#include <vector>

void CVisuals::Run()
{
	if (!I::EngineClient->IsInGame())
		return;

	SpectatorList();
	SniperLines();
	AimingAtLocal();
	RemoveFlash();
	EntityDistance();
	EnemiesBehind();
	EnemyWeapons();
	ShowKillerByIterating();
}

void CVisuals::SpectatorList()
{
	if (!G::bSpectatorList)
		return;

	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal)
		return;

	C_BasePlayer* pBase = reinterpret_cast<C_BasePlayer*>(pLocal);

	if (!pBase || pBase->deadflag())
		return;

	C_BaseEntity* pLocalEntity = reinterpret_cast<C_BaseEntity*>(pLocal);

	if (!pLocalEntity)
		return;

	printf("[LOCAL IS INDEX %d]\n", pLocal->entindex());

	for (int i = 1; i < 64; i++)
	{
		IClientEntity* entity = I::ClientEntityList->GetClientEntity(i);

		if (!entity || entity == pLocal)
			continue;

		C_BasePlayer* entityBase = reinterpret_cast<C_BasePlayer*>(entity);

		if (!entityBase)
			continue;

		if (!entityBase->deadflag() && entityBase->m_lifeState() == LIFE_ALIVE) // we only want dead players
			continue;

		player_info_t playerInfo = {};
		if (I::EngineClient->GetPlayerInfo(i, &playerInfo))
		{
			printf("%s is a DEAD entity\n", playerInfo.name);
		}

		C_BaseEntity* observerTarget = entityBase->m_hObserverTarget().Get();

		if (!observerTarget)
			continue;

		printf("%d entity is observing N/A (LOCAL: %d)\n", i, pLocal->entindex());

		if (observerTarget != pLocalEntity)
			continue;

		int observerMode = entityBase->m_iObserverMode();

		if (observerMode != OBS_MODE_FIRSTPERSON && observerMode != OBS_MODE_THIRDPERSON)
			continue;

		switch (observerMode)
		{
		case OBS_MODE_FIRSTPERSON:
			printf("observer %d in FIRSTPERSON\n", i);
			break;
		case OBS_MODE_THIRDPERSON:
			printf("observer %d in THIRDPERSON\n", i);
			break;
		default:
			printf("bullshit\n");
		}
	}
}

void CVisuals::SniperLines()
{
	if (!G::bEnemyScopeLines || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal)
		return;

	for (int i = 0; i < 64; i++)
	{
		IClientEntity* pEnemyEntity = I::ClientEntityList->GetClientEntity(i);

		if (!pEnemyEntity || pEnemyEntity->GetHealth() <= 1 || pEnemyEntity->IsDormant())
			continue;

		C_BasePlayer* pEnemyBase = reinterpret_cast<C_BasePlayer*>(pEnemyEntity);

		if (!pEnemyBase)
			continue;

		int fov = pEnemyBase->m_iFOVStart();

		if (fov <= 40)
		{
			G::bIsEnemyScoped = true;
		}
		else G::bIsEnemyScoped = false;

		if (G::bIsEnemyScoped)
		{
			Vector3 enemyEyePos = pEnemyEntity->GetAbsOrigin() + pEnemyBase->m_vecViewOffset();

			C_CSPlayer* CSEnemy = static_cast<C_CSPlayer*>(pEnemyBase);

			if (!CSEnemy)
				continue;

			float enemyEyeAngleX = CSEnemy->m_angEyeAnglesX();
			float enemyEyeAngleY = CSEnemy->m_angEyeAnglesY();

			Vector3 angles = Vector3(enemyEyeAngleX, enemyEyeAngleY, 0.0f);

			Vector3 direction;
			Math::AngleVectors(angles, &direction, nullptr, nullptr);
			direction.normalize();

			Vector3 dst = enemyEyePos + direction * 8192.f;

			Draw.DrawLine(enemyEyePos, dst, 2.f, D3DCOLOR_ARGB(255, 0, 255, 0));
		}
	}
}

void CVisuals::AimingAtLocal()
{
	if (!G::bVisualEnemyAimingAtUs || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal)
		return;

	C_BaseEntity* pLocalEntity = static_cast<C_BaseEntity*>(pLocal);

	if (!pLocalEntity)
		return;

	C_BasePlayer* pLocalBase = reinterpret_cast<C_BasePlayer*>(pLocal);

	if (!pLocalBase || pLocalBase->deadflag()) // deadflag returns 1?
		return;

	int offsetY = 300;

	for (int i = 1; i < 64; i++)
	{
		IClientEntity* pEnemyEntity = I::ClientEntityList->GetClientEntity(i);

		if (!pEnemyEntity || pEnemyEntity->GetHealth() <= 1 || pEnemyEntity->IsDormant())
			continue;

		if (pEnemyEntity->GetTeam() == pLocal->GetTeam())
			continue;

		if (MiscUtils.HoldingMelee(pEnemyEntity))
			continue;

		C_BasePlayer* pEnemyBase = reinterpret_cast<C_BasePlayer*>(pEnemyEntity);

		if (!pEnemyBase)
			continue;

		C_CSPlayer* CSEnemy = static_cast<C_CSPlayer*>(pEnemyBase);

		if (!CSEnemy)
			continue;

		float eyeAngleX = CSEnemy->m_angEyeAnglesX();
		float eyeAngleY = CSEnemy->m_angEyeAnglesY();

		Vector3 enemyEyeAngles = { eyeAngleX, eyeAngleY, 0.0f };

		Vector3 vecToLocal = (pLocal->GetAbsOrigin() + pLocalBase->m_vecViewOffset()) - (pEnemyEntity->GetAbsOrigin() + pEnemyBase->m_vecViewOffset());
		vecToLocal.NormalizeInPlace();

		Vector3 forward;
		Math::AngleVectors(enemyEyeAngles, &forward, nullptr, nullptr);
		forward.NormalizeInPlace();

		float dot = forward.dot(vecToLocal);

		if (dot > 0.78f)
		{
			char buffer[128];
			player_info_t playerInfo = {};

			int x = 100;

			if (I::EngineClient->GetPlayerInfo(i, &playerInfo))
			{
				if (dot > 0.94f && dot < 0.97f)
				{
					const char* netWeapon = MiscUtils.GetNetworkedWeapon(pEnemyEntity);
					const char* weaponName = MiscUtils.GetCleanWeaponName(netWeapon);
					sprintf_s(buffer, sizeof(buffer), "%s [%s]", playerInfo.name, weaponName);
					Draw.DrawFilledBackground(buffer, x, offsetY, D3DCOLOR_ARGB(160, 0, 0, 0, ));
					Draw.DrawOutlineScreenText(buffer, x, offsetY, FONT_MEDIUM, D3DCOLOR_ARGB(255, 250, 156, 28));
					offsetY += 20;
				}

				if (dot > 0.99f)
				{
					const char* netWeapon = MiscUtils.GetNetworkedWeapon(pEnemyEntity);
					const char* weaponName = MiscUtils.GetCleanWeaponName(netWeapon);
					sprintf_s(buffer, sizeof(buffer), "%s [%s]", playerInfo.name, weaponName);
					Draw.DrawFilledBackground(buffer, x, offsetY, D3DCOLOR_ARGB(160, 0, 0, 0));
					Draw.DrawOutlineScreenText(buffer, x, offsetY, FONT_MEDIUM, D3DCOLOR_ARGB(255, 250, 0, 0));
					offsetY += 20;
				}
			}
		}
	}
}

void CVisuals::RemoveFlash()
{
	if (!G::bNoFlash || !I::EngineClient->IsInGame())
		return;

	IClientEntity* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal || pLocal->GetHealth() <= 0)
		return;

	C_CSPlayer* pCSLocal = reinterpret_cast<C_CSPlayer*>(pLocal);

	if (!pCSLocal)
		return;

	// maybe one of these
	pCSLocal->m_flFlashDuration() = 0;
	pCSLocal->m_flFlashMaxAlpha() = 0;
}

void CVisuals::EntityDistance()
{
	if (!G::bDistanceToEnemy || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal)
		return;

	Vector3 pLocalPos = pLocal->GetAbsOrigin();

	for (int i = 1; i < 64; i++)
	{
		IClientEntity* pEntity = I::ClientEntityList->GetClientEntity(i);

		if (!pEntity || pEntity->GetHealth() <= 1)
			continue;

		Vector3 pEntityPos = pEntity->GetAbsOrigin();

		int distance = Math::GetDistance(pEntityPos, pLocalPos);
		distance /= 100;

		char buffer[8];

		sprintf_s(buffer, sizeof(buffer), "%dm", distance);

		VMatrix vmatrix = I::EngineClient->WorldToScreenMatrix();
		Vector2 screen;
		if (Render::WorldToScreen(pEntityPos, screen, vmatrix, I::iScreenWidth, I::iScreenHeight))
			Draw.DrawOutlineScreenText(buffer, screen.x, screen.y + 5.f, FONT_SMALL, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
}

void CVisuals::RemoveScope()
{
	if (!G::bRemoveScope || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal || pLocal->GetHealth() <= 1)
		return;

	static bool bHudRemoved = false;

	if (MiscUtils.Scoped(pLocal) && !bHudRemoved)
	{
		I::EngineClient->ClientCmd_Unrestricted("cl_drawhud 0");
		bHudRemoved = true;
	}
	
	if (!MiscUtils.Scoped(pLocal) && bHudRemoved)
	{
		I::EngineClient->ClientCmd_Unrestricted("cl_drawhud 1");
		bHudRemoved = false;
	}
}

void CVisuals::EnemiesBehind()
{
	if (!G::bEnemiesBehind || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal || pLocal->GetHealth() <= 1)
		return;

	Vector3 localPos = pLocal->GetAbsOrigin();

	C_CSPlayer* pCSLocal = reinterpret_cast<C_CSPlayer*>(pLocal);

	if (!pCSLocal)
		return;

	int enemiesBehind = 0;

	for (int i = 1; i < 64; i++)
	{
		IClientEntity* pEnemyEntity = I::ClientEntityList->GetClientEntity(i);

		if (!pEnemyEntity || pEnemyEntity->IsDormant() || pEnemyEntity->GetHealth() <= 1)
			continue;

		if (pEnemyEntity->GetTeam() == pLocal->GetTeam())
			continue;

		if (pEnemyEntity == pLocal)
			continue;

		Vector3 pEnemyEntityCenter = pEnemyEntity->GetAbsOrigin() + 30.f; // the chest
		Vector3 toLocal = pEnemyEntityCenter - localPos;
		toLocal.NormalizeInPlace();

		float localEyeX = pCSLocal->m_angEyeAnglesX();
		float localEyeY = pCSLocal->m_angEyeAnglesY();

		Vector3 angles = { localEyeX, localEyeY, 0.0f };
		Vector3 direction;
		Math::AngleVectors(angles, &direction, nullptr, nullptr);
		direction.NormalizeInPlace();

		float dot = direction.dot(toLocal);

		if (dot < 0.0f) // if lower than 90 degrees, they're behind us
			enemiesBehind++;
	}

	if (enemiesBehind > 0)
	{
		char buffer[32];
		sprintf_s(buffer, sizeof(buffer), "%d behind", enemiesBehind);

		int x = 400;
		int y = 500;

		Draw.DrawFilledBackground(buffer, x, y, D3DCOLOR_ARGB(255, 0, 0, 0));

		Draw.DrawOutlineScreenText(buffer, x, y, FONT_MEDIUM, D3DCOLOR_ARGB(180, 255, 255, 255));
	}
}

void CVisuals::EnemyWeapons()
{
	if (!G::bShowEnemyWeapons || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	for (int i = 1; i < 64; i++)
	{
		IClientEntity* pEnemyEntity = I::ClientEntityList->GetClientEntity(i);

		if (!pEnemyEntity || pEnemyEntity->GetHealth() <= 1 || pEnemyEntity->IsDormant())
			continue;

		if (pEnemyEntity == pLocal)
			continue;

		const char* pEnemyWeapon = MiscUtils.GetNetworkedWeapon(pEnemyEntity);

		if (!pEnemyWeapon)
			continue;

		const char* pEnemyWeaponName = MiscUtils.GetCleanWeaponName(pEnemyWeapon);

		if (!pEnemyWeaponName)
			continue;

		Vector2 screen;
		VMatrix vmatrix = I::EngineClient->WorldToScreenMatrix();
		if (Render::WorldToScreen(pEnemyEntity->GetAbsOrigin(), screen, vmatrix, I::iScreenWidth, I::iScreenHeight))
		{
			Draw.DrawOutlineScreenText(pEnemyWeaponName, screen.x - 3.f, screen.y + 2.f, FONT_SMALL, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}
}

void CVisuals::ShowKillerByIterating()
{
	if (!G::bEnableCustomModeInfo || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	static std::unordered_map<int, int> prevHealth;
	static std::unordered_map<int, ShotInfo> lastShots;

	float curTime = I::Global->curtime;

	for (int i = 1; i < 64; i++)
	{
		auto pEntity = I::ClientEntityList->GetClientEntity(i);

		if (!pEntity || pEntity->IsDormant() || pEntity->GetHealth() <= 1)
			continue;

		int health = pEntity->GetHealth();
		int index = pEntity->entindex();

		C_BasePlayer* pEntityBase = reinterpret_cast<C_BasePlayer*>(pEntity);

		if (!pEntityBase)
			continue;

		auto hWeapon = pEntityBase->m_hActiveWeapon();
		auto pWeapon = reinterpret_cast<C_BaseCombatWeapon*>(I::ClientEntityList->GetClientEntityFromHandle(hWeapon));

		static std::unordered_map<int, float> lastWeaponTime;

		if (pWeapon)
		{
			float lastTime = lastWeaponTime[index];
			float nextAttack = pWeapon->m_flNextPrimaryAttack();

			if (nextAttack > lastTime)
			{
				ShotInfo info;
				info.lastShotTime = curTime;
				info.lastShootPos = pEntity->GetAbsOrigin();
				info.lastViewAngle = pEntity->GetAbsOrigin() + pEntityBase->m_vecViewOffset();

				lastShots[index] = info;
			}
		}

		auto it = prevHealth.find(index);
		if (it != prevHealth.end() && it->second > 0 && health <= 1)
		{
			IClientEntity* pCandidate = nullptr;
			float bestScore = FLT_MAX;

			for (int j = 1; j < 64; j++)
			{
				if (j == index)
					continue;

				auto pAttacker = I::ClientEntityList->GetClientEntity(j);

				if (!pAttacker || pAttacker->IsDormant() || pAttacker->GetHealth() <= 1)
					continue;

				C_BasePlayer* pAttackerBase = reinterpret_cast<C_BasePlayer*>(pAttacker);

				if (!pAttackerBase)
					continue;

				auto& shot = lastShots[j];
				float delta = curTime - shot.lastShotTime;

				if (delta > 1.0f) continue;

				Vector3 direction;
				Math::AngleVectors(shot.lastViewAngle, &direction, nullptr, nullptr);
				Vector3 pAttackerEyePos = pAttacker->GetAbsOrigin() + pAttackerBase->m_vecViewOffset();
				Vector3 vecToVictim = pEntity->GetAbsOrigin() - pAttackerEyePos;

				float dist = vecToVictim.Length();
				direction.NormalizeInPlace();
				vecToVictim.NormalizeInPlace();

				float dot = direction.dot(vecToVictim);
				float aimError = acos(dot) * (180.f / M_PI);

				float score = dist + aimError * 10.f;

				if (score < bestScore)
				{
					bestScore = score;
					pCandidate = pAttacker;
				}

				if (pCandidate)
				{
					player_info_t info;
					if (I::EngineClient->GetPlayerInfo(pCandidate->entindex(), &info))
					{
						char buffer[256];
						sprintf_s(buffer, sizeof(buffer), "%s likely killed %d\n", info.name, index);
						I::Cvar->ConsoleColorPrintf({ 255, 0, 255, 255 }, buffer);
					}
				}
			}
		}

		prevHealth[index] = health;
	}
}
