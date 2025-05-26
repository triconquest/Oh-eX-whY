#pragma once
#include "../Utils/vector.h"

#include <string>

class IMaterial;

class CVisuals
{
public:
	void Run();
	void SpectatorList();
	void SniperLines();
	void AimingAtLocal();
	void RemoveFlash();
	void EntityDistance();
	void RemoveScope();
	void EnemiesBehind();
	void EnemyWeapons();
	void ScopedEnemyLines();
	void ShowKillerByIterating();

private:
	bool m_bWorldModulated = false;
};

struct AimEntities
{
	const char* name;
	float dot;
};

struct ShotInfo
{
	float lastShotTime = 0.f;
	Vector3 lastShootPos;
	Vector3 lastViewAngle;
};

inline CVisuals Visuals;