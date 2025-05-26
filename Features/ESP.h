#pragma once

#include "../SDK/Interfaces/Entities/IClientEntity.h"



class DrawESP
{
public:
	void Run();
	void DrawPlayers();
	void DrawHealth(IClientEntity* entity, Vector2 feetPos, Vector2 headPos);
	void DrawBones(IClientEntity* entity, VMatrix vMatrix);
};

inline int GetBoneCount(IClientEntity* entity);

struct BoneCache
{
	matrix3x4_t bones[128];
	bool valid = false;
};

inline BoneCache boneCache;
inline DrawESP ESP;