
#include "Misc.h"

VOID CNetVarMan::Run(VOID)
{
	m_vecTables.clear();
	ClientClass * pClass = I::Client->GetAllClasses();
	while (pClass)
	{
		RecvTable_T * pTable = pClass->m_pRecvTable;
		m_vecTables.push_back(pTable), pClass = pClass->m_pNext;
	};
};

INT CNetVarMan::GetOffset(CONST CHAR * pszTable, CONST CHAR * pszProp)
{
	return Get_Prop(pszTable, pszProp, NULL);
};

INT CNetVarMan::Get_Prop(CONST CHAR * pszTable, CONST CHAR * pszProp, RecvProp_T ** ppProp)
{
	RecvTable_T * pTable = GetTable(pszTable);
	if (!pTable) return 0;
	return Get_Prop(pTable, pszProp, ppProp);
};

INT CNetVarMan::Get_Prop(RecvTable_T * pTable, CONST CHAR * pszProp, RecvProp_T ** ppProp)
{
	INT iExtraOffs = 0;
	for (INT iIter = 0; iIter < pTable->m_nProps; iIter++)
	{
		RecvProp_T * pProp = &pTable->m_pProps[iIter];
		RecvTable_T * pChildTable = pProp->m_pDataTable;
		if (pChildTable && pChildTable->m_nProps > 0)
		{
			INT iTemp = Get_Prop(pChildTable, pszProp, ppProp);
			if (iTemp) iExtraOffs += (pProp->m_Offset + iTemp);
		};
		if (_stricmp(pProp->m_pVarName, pszProp)) continue;
		if (ppProp) *ppProp = pProp;
		return pProp->m_Offset + iExtraOffs;
	};
	return iExtraOffs;
};

RecvTable_T * CNetVarMan::GetTable(CONST CHAR * pszTable)
{
	if (m_vecTables.empty()) return 0;
	for each (RecvTable_T * pTable in m_vecTables)
	{
		if (pTable && _stricmp(pTable->m_pNetTableName, pszTable) == 0) return pTable;
	};
	return 0;
};
