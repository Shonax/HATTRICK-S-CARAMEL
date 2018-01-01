
#include "Misc.h"

ClientClass * IBaseClientDll::GetAllClasses(VOID)
{
	typedef ClientClass * (__thiscall * OriginalFn)(PVOID);
	return GetVFunc<OriginalFn>(this, 8)(this);
};
