#pragma once

#include "Entities/IClientEntity.h"
#include "../Includes/basehandle.h"

class IClientEntityList
{
public:
	virtual void* GetClientNetworkable(int entnum) = 0;
	virtual void* GetClientNetworkableFromHandle(unsigned long hEnt) = 0;
	virtual void* GetClientUnknownFromHandle(unsigned long hEnt) = 0;
	virtual IClientEntity* GetClientEntity(int entnum) = 0; // change this to CEntity*
	virtual IClientEntity* GetClientEntityFromHandle(CBaseHandle hEnt) = 0; // change this to CEntity*
	virtual int					NumberOfEntities(bool bIncludeNonNetworkable) = 0;
	virtual int					GetHighestEntityIndex(void) = 0;
	virtual void				SetMaxEntities(int maxents) = 0;
	virtual int					GetMaxEntities() = 0;
};