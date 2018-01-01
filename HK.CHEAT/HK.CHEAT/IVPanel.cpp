
#include "Misc.h"

CONST CHAR * IVPanel::GetName(INT iIndex)
{
	typedef CONST CHAR * (__thiscall * __GetNameFn) (PVOID, INT);
	return GetVFunc<__GetNameFn>(this, 36) (this, iIndex);
};
