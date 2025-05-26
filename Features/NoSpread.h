#pragma once

class CUserCmd;
class C_CSPlayer;
class C_WeaponCSBase;

class CNoSpread
{
public:
	void Run(CUserCmd* cmd);
	void NoSpread(CUserCmd* cmd, C_WeaponCSBase* pWeapon);
	bool ShouldRun(C_CSPlayer* pLocal, C_WeaponCSBase* pWeapon, CUserCmd* cmd);
};

extern CNoSpread NoSpread;