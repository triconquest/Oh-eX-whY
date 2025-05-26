#pragma once

#include <Windows.h>

namespace Offsets
{
	static uintptr_t dwLocalPlayer = 0x005F4B68; // client.dll
	static uintptr_t dwEntityList = 0x006098C8; // client.dll 
	static uintptr_t dwViewMatrix = 0x00698F18;  // +0x2D4
	static uintptr_t dwViewAngles = 0x4B84;
	static uintptr_t m_iHealth = 0xD0;
	static uintptr_t m_iArmor = 0x1AA4;
	static uintptr_t m_vecOrigin = 0x320;
	static uintptr_t m_iTeamNum = 0xD8;
	static uintptr_t m_MoveType = 0x1F4;
	static uintptr_t m_bDormant = 0x1FA;
	static uintptr_t m_fHeadheight = 0x144; // doesn't work
	static uintptr_t m_iFlags = 0x440;
	static uintptr_t m_iBoneMatrix = 0x810;
}