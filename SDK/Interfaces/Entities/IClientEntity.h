//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef ICLIENTENTITY_H
#define ICLIENTENTITY_H
#ifdef _WIN32
#pragma once
#endif


#include "iclientrenderable.h"
#include "iclientnetworkable.h"
#include "iclientthinkable.h"
#include "../../NetvarManager/NetvarManager.h"

struct Ray_t;
class CGameTrace;
typedef CGameTrace trace_t;
class CMouthInfo;
class IClientEntityInternal;
struct SpatializationInfo_t;

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual void			Release(void) = 0;
	virtual const Vector&	GetAbsOrigin(void) const = 0;
	virtual const Vector&	GetAbsAngles(void) const = 0;
	virtual CMouthInfo*		GetMouth(void) = 0;
	virtual bool			GetSoundSpatialization(SpatializationInfo_t& info) = 0;

	int GetHealth() { return *reinterpret_cast<int*>(uintptr_t(this) + 0xD0); }
	int GetTeam() { return *reinterpret_cast<int*>(uintptr_t(this) + 0xD8); }
	int GetFlags() { return *reinterpret_cast<int*>(uintptr_t(this) + 0x440); }

	Vector3 GetBonePosition(int bone)
	{
		uintptr_t boneMatrixBase = *(uintptr_t*)((uintptr_t)this + 0x810);

		if (!boneMatrixBase)
			return Vector3(0, 0, 0);

		matrix3x4_t boneMatrix = *(matrix3x4_t*)(boneMatrixBase + (bone * sizeof(matrix3x4_t)));

		return Vector3(
			boneMatrix[0][3],
			boneMatrix[1][3],
			boneMatrix[2][3]
		);
	}

	template <typename T> inline T* As() { return static_cast<T*>(this); }

	//NETVAR(m_vecViewOffset, Vector3, "DT_BasePlayer", "m_vecViewOffset[0]");
};

//namespace Util
//{
//	inline IClientEntity* EntityByIndex(const int entindex)
//	{
//		return I::ClientEntityList->GetClientEntity(entindex);
//	}
//}

#endif // ICLIENTENTITY_H