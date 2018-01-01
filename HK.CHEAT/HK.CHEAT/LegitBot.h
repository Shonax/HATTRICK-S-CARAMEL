#pragma once

#include "Misc.h"

struct CLBHBox
{
	CLBHBox(VOID)
	{
		m_iHBox = -1;
	};

	CLBHBox(INT iNewHBox)
	{
		Set(iNewHBox);
	};

	VOID Set(INT iNewHBox)
	{
		m_iHBox = iNewHBox;
	};

	INT m_iHBox;
	Vector m_vecPoints[9];
};

struct CLBBestPoint
{
	CLBBestPoint(VOID)
	{
		Clear();
	};

	CLBBestPoint(INT iNewHBox)
	{
		m_iHBox = iNewHBox;
		m_vecPoint.Zero();
		m_fFoV = 8192.f;
	};

	VOID Clear(VOID)
	{
		m_iHBox = -1;
		m_vecPoint.Zero();
		m_fFoV = 8192.f;
	};

	Vector m_vecPoint;
	INT m_iHBox;
	FLOAT m_fFoV;
};

class CLBOT
{
public:

	VOID Run(VOID);
	VOID GoToTarget(VOID);
	VOID FindTarget(VOID);
	VOID TriggerBOT(VOID);
	bool GetHBox(Entity_T *, CLBHBox *);
	bool GetBestPoint(Entity_T *, CLBHBox *, CLBBestPoint *);
	bool BestAimPointAll(Entity_T *, Vector &);
};
