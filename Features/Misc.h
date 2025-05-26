#pragma once
#include "../Globals/Globals.h"
#include "../SDK/Includes/usercmd.h"
#include "../SDK/Interfaces/Entities/C_WeaponCSBase.h"
#include "../Utils/Render/Render.h"
#include "../Utils/DrawManager/DrawManager.h"

#include <iostream>
#include <vector>
#include <unordered_map>

class CMisc
{
public:
	void Run();
	void DrawCrosshair();
	void DrawFOVCircle();
	void AutoJoin();
	void HitMarker();
	void Bhop(CUserCmd* cmd);
	void DrawBullets();
	void CustomFOV(CViewSetup* view);
	void ThirdPerson(CViewSetup* view);
	void FastStop(CUserCmd* cmd);
	void DynamicCrosshair(CUserCmd* cmd);
	void NoRecoil(CUserCmd* cmd);
	void KnifeBot(CUserCmd* cmd);

	// other fun features
	void FindAttacker();
};

struct BulletImpact
{
	Vector3 position;
	ULONGLONG time;
};

struct KillerInfo
{
	std::string name = "Unknown";
	int index = -1;
	int victim = -1;
	int victimCount = 0;
	std::string weapon = "NULL";
};

static std::vector<BulletImpact> impacts;
extern std::unordered_map<int, KillerInfo> killerMap;

class CHitMarkerListener : public IGameEventListener2
{
public:
	void FireGameEvent(IGameEvent* ievent) override {
		if (!ievent || G::bDisableListeners)
			return;

		const char* eventName = ievent->GetName();

		if (!strcmp(eventName, "player_hurt"))
		{
			int attackerUserID = ievent->GetInt("attacker");
			int attacker = I::EngineClient->GetPlayerForUserID(attackerUserID);
			int local = I::EngineClient->GetLocalPlayer();

			if (attacker == local && local != -1)
			{
				G::bEnemyKilled = false;
				G::bHitMarkerDraw = true;
				G::fHitMarkerTime = GetTickCount64();
			}
		}

		else if (!strcmp(eventName, "player_death"))
		{
			int attackerUserID = ievent->GetInt("attacker");
			int attacker = I::EngineClient->GetPlayerForUserID(attackerUserID);
			int local = I::EngineClient->GetLocalPlayer();

			if (G::bEnableCustomModeInfo)
			{
				int victimID = ievent->GetInt("userid");
				int victim = I::EngineClient->GetPlayerForUserID(victimID);

				if (attacker != local)
				{

					player_info_t killerInfo = {}, victimInfo = {};
					if (I::EngineClient->GetPlayerInfo(attacker, &killerInfo) &&
						I::EngineClient->GetPlayerInfo(victim, &victimInfo))
					{
						char buffer[256];
						sprintf_s(buffer, sizeof(buffer), "[EVENTS] %s killed %s\n", killerInfo.name, victimInfo.name);
						I::Cvar->ConsoleColorPrintf({ 0, 255, 255, 255 }, buffer);

						auto it = killerMap.find(attacker);
						if (it != killerMap.end())
						{
							it->second.victimCount++;
							it->second.victim = victim;
						}

						else
						{
							KillerInfo s_Killer = {};

							s_Killer.name = std::string(killerInfo.name);
							s_Killer.index = attacker;
							s_Killer.victim = victim;
							s_Killer.weapon = "Bullshit";
							s_Killer.victimCount = 1;

							killerMap.insert({ attacker, s_Killer });
						}
					}
				}
			}

			if (attacker == local && local != -1)
			{
				G::bEnemyKilled = true;
				G::bHitMarkerDraw = true;
				G::fHitMarkerTime = GetTickCount64();
				I::Surface->PlaySound("/player/headshot1.wav");
			}
		}

		else if (!strcmp(eventName, "bullet_impact"))
		{
			int shooterID = ievent->GetInt("userid");
			int shooter = I::EngineClient->GetPlayerForUserID(shooterID);
			int local = I::EngineClient->GetLocalPlayer();

			if (shooter == local && local != -1)
			{
				float x = ievent->GetFloat("x");
				float y = ievent->GetFloat("y");
				float z = ievent->GetFloat("z");

				BulletImpact impactInfo;
				impactInfo.position = Vector3(x, y, z);
				impactInfo.time = GetTickCount64();
				impacts.push_back(impactInfo);
			}
		}

		else if (!strcmp(eventName, "round_start"))
		{
			G::bResetBones = true;
			killerMap.clear();
		}
	}

	void Register()
	{
		if (G::bDisableListeners)
			return;

		if (I::GameEvent)
		{
			I::GameEvent->AddListener(this, "player_hurt", false);
			I::GameEvent->AddListener(this, "player_death", false);
			I::GameEvent->AddListener(this, "bullet_impact", false);
			I::GameEvent->AddListener(this, "round_start", false);
		}
	}

	void Unregister()
	{
		if (G::bDisableListeners)
			return;

		if (I::GameEvent)
			I::GameEvent->RemoveListener(this);
	}
};

inline CHitMarkerListener g_HitMarkerListener;
inline CMisc Misc;