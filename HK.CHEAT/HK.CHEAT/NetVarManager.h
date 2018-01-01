#pragma once

#include "Misc.h"

struct RecvTable_T
{
	RecvProp_T *	m_pProps;
	int				m_nProps;
	void *			m_pDecoder;
	char *			m_pNetTableName;
	bool			m_bInitialized;
	bool			m_bInMainList;
};

struct RecvProp_T
{
	char *			m_pVarName;
	int				m_RecvType;
	int				m_Flags;
	int				m_StringBufferSize;
	bool			m_bInsideArray;
	const void *	m_pExtraData;
	RecvProp_T *	m_pArrayProp;
	void *			m_ArrayLengthProxy;
	void *			m_ProxyFn;
	void *			m_DataTableProxyFn;
	RecvTable_T *	m_pDataTable;
	int				m_Offset;
	int				m_ElementStride;
	int				m_nElements;
	const char *	m_pParentArrayPropName;
};

class ClientClass
{
public:

	CreateClientClassFn m_pCreateFn;
	CreateEventFn * m_pCreateEventFn;
	PCHAR m_pNetworkName;
	RecvTable_T * m_pRecvTable;
	ClientClass * m_pNext;
	INT m_ClassID;
};

class CNetVarMan
{
public:

	VOID Run(VOID);
	INT GetOffset(CONST CHAR *, CONST CHAR *);
	INT Get_Prop(CONST CHAR *, CONST CHAR *, RecvProp_T **);
	INT Get_Prop(RecvTable_T *, CONST CHAR *, RecvProp_T **);
	RecvTable_T * GetTable(CONST CHAR *);
	std::vector <RecvTable_T *> m_vecTables;
};
