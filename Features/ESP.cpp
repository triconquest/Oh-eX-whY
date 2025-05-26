#include "ESP.h"
#include "Menu.h"
#include "../Globals/Globals.h"
#include "../Utils/Render/Render.h"
#include "../Utils/DrawManager/DrawManager.h"
#include "../SDK/Interfaces/Entities/IClientEntity.h"
#include "../SDK/Interfaces/Entities/C_BasePlayer.h"
#include "../Utils/vector.h"
#include "../Utils/math.h"

#include <iostream>
#include <cmath>
#include <vector>

#pragma warning(disable:4996)

void DrawESP::Run()
{
	if (!G::bESP || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	// dope
	if (!G::bESPAlwaysOn && (GetAsyncKeyState(0x45) & 1))
		G::bTESP = !G::bTESP;

	DrawPlayers();
}

void DrawESP::DrawPlayers()
{
	if (!G::bTESP)
		return;

	auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (!pLocal)
		return;

	for (int i = 1; i < 64; i++)
	{
		IClientEntity* currentEntity = I::ClientEntityList->GetClientEntity(i);

		if (!currentEntity || currentEntity->IsDormant())
			continue;

		if (currentEntity == pLocal)
			continue;

		if (!G::bShowTeam && currentEntity->GetTeam() == pLocal->GetTeam())
			continue;

		int iEntHealth = currentEntity->GetHealth();
		VMatrix vMatrix = I::EngineClient->WorldToScreenMatrix();

		if (iEntHealth <= 1)
			continue;

		D3DCOLOR color;

		if (currentEntity->GetTeam() == pLocal->GetTeam()) {
			color = D3DCOLOR_ARGB(255, 0, 255, 0);
		}
		else
		{
			if (i == G::iAimTarget && G::bAimLocked)
				color = Draw.FloatToD3D(G::fTargetColor);
			else
				color = Draw.FloatToD3D(G::fColor);
		}

		C_BasePlayer* pEntityBase = reinterpret_cast<C_BasePlayer*>(currentEntity);

		if (!pEntityBase)
			continue;

		Vector3 vEnemyPos = currentEntity->GetAbsOrigin();
		Vector3 vHeadPos = currentEntity->GetBonePosition(49);

		Vector3 vEnemyPosView = currentEntity->GetAbsOrigin() + pEntityBase->m_vecViewOffset();

		Vector2 vScreenFeet = { 0 };
		Vector2 vScreenHead = { 0 };

		Vector3 vEnemyHead = G::bUseBones ? (G::BoneValid(vHeadPos, vEnemyPos) ? vHeadPos + 5.f : vEnemyPosView + 5.f) : vEnemyPosView + 5.f;

		if (Render::WorldToScreen(vEnemyPos, vScreenFeet, vMatrix, I::iScreenWidth, I::iScreenHeight) &&
			Render::WorldToScreen(vEnemyHead, vScreenHead, vMatrix, I::iScreenWidth, I::iScreenHeight))
		{

			if(G::bCornerESP)
				Draw.DrawCornerBox(vScreenFeet, vScreenHead, 1, color);
			else Draw.DrawBox(vScreenFeet, vScreenHead, 1, color);

			DrawHealth(currentEntity, vScreenFeet, vScreenHead);

			player_info_t playerinfo;
			if (I::EngineClient->GetPlayerInfo(i, &playerinfo))
			{
				if (strlen(playerinfo.name) > 0)
				{
					Draw.DrawScreenText(playerinfo.name, vScreenFeet.x - 5.f, vScreenHead.y - 10.f, FONT_SMALL, D3DCOLOR_ARGB(255, 255, 255, 255));

					if (G::bHeadBone)
						Draw.DrawScreenText("Head", vScreenHead.x, vScreenHead.y, FONT_SMALL, D3DCOLOR_ARGB(255, 0, 255, 0));
				}
			}

			if (G::bEnemyLines)
				Draw.DrawLine(I::iScreenWidth / 2, I::iScreenHeight, vScreenFeet.x, vScreenFeet.y, 1.5f, color);

			if (G::bBoneESP)
				DrawBones(currentEntity, vMatrix);

			if (G::bEnemyScopeLines && G::bIsEnemyScoped)
				Draw.DrawScreenText("Scoped", vScreenHead.x, vScreenHead.y, FONT_SMALL, color);

			if (G::bRebelling)
				Draw.DrawOutlineScreenText("Rebelling", vScreenHead.x, vScreenHead.y - 5.f, FONT_SMALL, D3DCOLOR_ARGB(255, 255, 0, 0));
		}
	}
}

void DrawESP::DrawHealth(IClientEntity* entity, Vector2 feetPos, Vector2 headPos)
{
	int height = ABS(feetPos.y - headPos.y);

	//float healthPerc = entity->GetHealth() / 100.f;
	float health = entity->GetHealth();
	float healthPerc2 = std::clamp(health / 100.f, 0.f, 1.f);

	D3DCOLOR color;
	if (healthPerc2 < 0.45f)
		color = D3DCOLOR_ARGB(255, 255, 0, 0);
	else if (healthPerc2 < 0.7f)
		color = D3DCOLOR_ARGB(255, 255, 255, 0);
	else color = D3DCOLOR_ARGB(255, 0, 255, 0);

	Vector2 healthBottom, healthTop;

	int healthHeight = height * healthPerc2;

	healthBottom.x = feetPos.x - (height / 4) - 2;
	healthBottom.y = feetPos.y;

	healthTop.x = healthBottom.x;
	healthTop.y = headPos.y + height - healthHeight;

	Draw.DrawLine(healthBottom, healthTop, 2, color);

	// health text
	int healthText = health;
	char buffer[32];

	sprintf(buffer, "%d", healthText);

	const char* result = buffer;

	Draw.DrawScreenText(result, healthTop.x + 10.f, headPos.y - 20.f, FONT_SMALL, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void DrawESP::DrawBones(IClientEntity* entity, VMatrix vMatrix)
{
	if (!entity)
		return;

	matrix3x4_t bones[128];

	std::vector<std::pair<int, int>> bonePairs =
	{
		{49, 12}, // neck to chest
		{12, 0}, // chest to pelvis
		{12, 29}, // chest to upper right arm
		{29, 30}, // upper right to lower right
		{30, 31}, // lower right to right hand
		{12, 16}, // chest to upper left
		{16, 17}, // upper left to lower left
		{17, 18}, // lower left to left hand
		{0, 5}, // pelvis to right thigh
		{5, 6}, // right thigh to right knee
		{6, 7}, // right knee to right foot
		{0, 1}, // pelvis to left thigh
		{1, 2}, // left thigh to left knee
		{2, 3} // left knee to left foot
	};

	if (entity->SetupBones(bones, 128, BONE_USED_BY_ANYTHING, I::Global->curtime))
	{
		for (const auto& [bone1, bone2] : bonePairs)
		{
			Vector3 pos1, pos2;
			Math::MatrixPosition(bones[bone1], pos1);
			Math::MatrixPosition(bones[bone2], pos2);

			Vector2 screen1, screen2;
			if (Render::WorldToScreen(pos1, screen1, vMatrix, I::iScreenWidth, I::iScreenHeight) &&
				Render::WorldToScreen(pos2, screen2, vMatrix, I::iScreenWidth, I::iScreenHeight))
			{
				Draw.DrawLine(screen1.x, screen1.y, screen2.x, screen2.y, 1.f, D3DCOLOR_ARGB(255, 255, 255, 255));
			}
		}
	}

}

inline int GetBoneCount(IClientEntity* entity)
{
	if (!entity)
		return 0;

	const model_t* model = entity->GetModel();

	if (!model)
		return 0;

	studiohdr_t* hdr = I::ModelInfo->GetStudiomodel(model);

	if (!hdr)
		return 0;

	return hdr->numbones;
}

//	/*if (G::bBoneESP)
		//	{
		//		for (int bone = 0; bone < 128; bone++)
		//		{
		//			Vector3 bonePos = currentEntity->GetBonePosition(bone);

		//			Vector2 screenBone = { 0 };
		//			if (Render::WorldToScreen(bonePos, screenBone, vMatrix, I::iScreenWidth, I::iScreenHeight))
		//			{
		//				char boneLabel[8];
		//				sprintf_s(boneLabel, "%d", bone);
		//				Draw.DrawScreenText(boneLabel, screenBone.x, screenBone.y, FONT_SMALL, D3DCOLOR_ARGB(255, 0, 0, 255));
		//			}
		//		}
		//	}*/