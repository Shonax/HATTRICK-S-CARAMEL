#pragma once

#include "Misc.h"

#if defined(ENABLE_RAGE_BOT) && ENABLE_RAGE_BOT == 1

struct CRBHBox
{
	CRBHBox(VOID)
	{
		m_iHBox = -1;
	};

	CRBHBox(INT iNewHBox)
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

struct CRBBestPoint
{
	CRBBestPoint(VOID)
	{
		Clear();
	};

	CRBBestPoint(INT iNewHBox)
	{
		m_iHBox = iNewHBox;
		m_vecPoint.Zero();
		m_fDmg = 0.f;
	};

	VOID Clear(VOID)
	{
		m_iHBox = -1;
		m_vecPoint.Zero();
		m_fDmg = 0.f;
	};

	Vector m_vecPoint;
	INT m_iHBox;
	FLOAT m_fDmg;
};

class CRBOT
{
public:

	VOID Run(VOID);
	VOID FindTarget(VOID);
	VOID GoToTarget(VOID);
	bool GetHBox(Entity_T *, CRBHBox *);
	bool GetBestPoint(Entity_T *, CRBHBox *, CRBBestPoint *);
	FLOAT BestAimPointAll(Entity_T *, Vector &);
};

class CFM
{
public:

	VOID Start(VOID);
	VOID End(VOID);

	FLOAT m_fOldFwd, m_fOldSide;
	QAngle m_angOld;
};

#endif
