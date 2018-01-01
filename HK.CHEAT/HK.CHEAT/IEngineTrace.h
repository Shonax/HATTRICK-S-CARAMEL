#pragma once

#include "Misc.h"

class IEngineTrace
{
public:

	virtual INT GetPointContents(CONST Vector &, INT, VOID **) = NULL;
	virtual INT GetPointContents_WorldOnly(CONST Vector &, INT) = NULL;
	virtual INT GetPointContents_Collideable(VOID *, CONST Vector &) = NULL;
	virtual VOID ClipRayToEntity(CONST Ray_T &, UINT, VOID *, GameTrace_T *) = NULL;
	virtual VOID ClipRayToCollideable(CONST Ray_T &, UINT, VOID *, GameTrace_T *) = NULL;
	virtual VOID TraceRay(CONST Ray_T &, UINT, ITraceFilter *, GameTrace_T *) = NULL;
	virtual VOID SetupLeafAndEntityListRay(CONST Ray_T &, VOID *) = NULL;
	virtual VOID SetupLeafAndEntityListBox(CONST Vector &, CONST Vector &, VOID *) = NULL;
	virtual VOID TraceRayAgainstLeafAndEntityList(CONST Ray_T &, VOID *, UINT, ITraceFilter *, GameTrace_T *) = NULL;
	virtual VOID SweepCollideable(VOID *, CONST Vector &, CONST Vector &, CONST QAngle &, UINT, ITraceFilter *, GameTrace_T *) = NULL;
	virtual VOID EnumerateEntities(CONST Ray_T &, bool, VOID *) = NULL;
	virtual VOID EnumerateEntities(CONST Vector &, CONST Vector &, VOID *) = NULL;
	virtual VOID * GetCollideable(VOID *) = NULL;
	virtual INT GetStatByIndex(INT, bool) = NULL;
	virtual VOID lolignorethisaswellrifk(VOID) = NULL;
	virtual VOID GetCollidableFromDisplacementsInAABB(VOID) = NULL;
	virtual INT GetNumDisplacements(VOID) = NULL;
	virtual VOID GetDisplacementMesh(INT, PVOID) = NULL;
	virtual bool lolignorethis(VOID) = NULL;
	virtual bool PointOutsideWorld(CONST Vector &) = NULL;
	virtual INT GetLeafContainingPoint(CONST Vector &) = NULL;
	virtual VOID *AllocTraceListData(VOID) = NULL;
	virtual VOID FreeTraceListData(VOID *) = NULL;
	virtual INT GetSetDebugTraceCounter(INT, INT) = NULL;
};
