#pragma once
#include "../Includes/dt_recv.h"

class NetVarManager
{
public:
	int Get(const char* const szClass, const char* const szNetVar);

private:
	int GetOffset(RecvTable* pTable, const char* const szNetVar);
};

inline NetVarManager NetVar;

#define NETVAR(_name, type, table, name) inline type &_name() \
{ \
	static const int nOffset = NetVar.Get(table, name); \
	return *reinterpret_cast<type *>(reinterpret_cast<uintptr_t>(this) + nOffset); \
}

#define NETVAR_OFF(_name, type, table, name, offset) inline type &_name() \
{ \
	static const int nOffset = NetVar.Get(table, name) + offset; \
	return *reinterpret_cast<type *>(reinterpret_cast<uintptr_t>(this) + nOffset); \
}