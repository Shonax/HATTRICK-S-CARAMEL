#pragma once

#include "Misc.h"

class IEnts
{
public:

	virtual VOID Fn_A(VOID) = NULL;
	virtual VOID Fn_B(VOID) = NULL;
	virtual VOID Fn_C(VOID) = NULL;

	virtual Entity_T * GetClEntity(INT) = NULL;
	virtual Entity_T * GetEntityFromHndl(DWORD) = NULL;

	virtual INT NumberOfEntities(bool) = NULL;
	virtual INT GetHighestEntityIndex(VOID) = NULL;
};
