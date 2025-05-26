#include "NetvarManager.h"
#include "../../Utils/Hash.h"
#include "../../Globals/Globals.h"
#include "../Includes/client_class.h"

int NetVarManager::Get(const char* const szClass, const char* const szNetVar)
{
	ClientClass* pCC = I::BaseClientDLL->GetAllClasses();

	while (pCC)
	{
		if (FNV::Hash(szClass) == FNV::Hash(pCC->m_pNetworkName))
			return GetOffset(pCC->m_pRecvTable, szNetVar);

		pCC = pCC->m_pNext;
	}

	return 0;
}

int NetVarManager::GetOffset(RecvTable* pTable, const char* const szNetVar)
{
	int n;
	for (n = 0; n < pTable->m_nProps; n++)
	{
		RecvProp Prop = pTable->m_pProps[n];

		if (FNV::Hash(Prop.m_pVarName) == FNV::Hash(szNetVar))
			return Prop.GetOffset();

		RecvTable* pChild = Prop.GetDataTable();

		if (pChild)
		{
			const int nOff = GetOffset(pChild, szNetVar);

			if (nOff)
				return (nOff + Prop.GetOffset());
		}
	}

	return 0;
}
