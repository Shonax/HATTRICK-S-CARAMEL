#pragma once

#include "Misc.h"

struct Ray_T
{
	Vector m_Start;
	FLOAT fUnusedA;

	Vector m_Delta;
	FLOAT fUnusedB;

	Vector m_StartOffset;
	FLOAT fUnusedC;

	Vector m_Extents;
	FLOAT fUnusedD;

	CONST Matrix34_T * m_pWorldAxisTransform;

	bool m_bIsRay;
	bool m_bIsSwept;

	VOID Run(Vector S, Vector D)
	{
		m_Delta = D - S, m_bIsSwept = m_Delta.LengthSqr() != 0.f, \
			m_Extents.Zero(), m_pWorldAxisTransform = NULL, m_bIsRay = true, \
			m_StartOffset.Zero(), m_Start = S;
	};
};

struct Plane_T
{
	Vector normal;
	FLOAT dist;
	BYTE type;
	BYTE signbits;
	BYTE __UNUSED__[2];
};

class ITraceFilter
{
public:

	virtual bool ShouldHitEntity(Entity_T * pEntityHandle, INT)
	{
		return !(pEntityHandle == pSkip);
	};

	virtual INT	GetTraceType(VOID)
	{
		return NULL;
	};

	PVOID pSkip;
};

class TraceFilter_T : public ITraceFilter
{
public:

	virtual bool ShouldHitEntity(Entity_T * pEntityHandle, INT)
	{
		return !(pEntityHandle == pSkip);
	};

	virtual INT	GetTraceType(VOID) CONST
	{
		return NULL;
	};

	PVOID pSkip;
};

class BaseTrace_T
{
public:

	Vector startpos;
	Vector endpos;
	Plane_T plane;
	FLOAT fraction;
	INT contents;
	USHORT dispFlags;
	bool allsolid;
	bool startsolid;
};

struct Surface_T
{
	CONST CHAR * name;
	SHORT surfaceProps;
	USHORT flags;
};

class GameTrace_T : public BaseTrace_T
{
public:

	FLOAT fractionleftsolid;
	Surface_T surface;
	INT hitgroup;
	SHORT physicsbone;
	USHORT worldSurfaceIndex;
	Entity_T * ent;
	INT hitbox;
};
