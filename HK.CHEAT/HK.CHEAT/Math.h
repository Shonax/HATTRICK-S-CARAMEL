#pragma once

#include "Misc.h"

namespace M
{
	extern VOID SinCos(FLOAT, PFLOAT, PFLOAT);
	extern VOID VecAng(Vector &, QAngle &);
	extern VOID AngVec(QAngle &, Vector *);
	extern QAngle CalcAng(Vector, Vector);
	extern FLOAT FOV(QAngle &, QAngle &);
	extern VOID VecTransf(Vector &, Matrix34_T &, Vector &);
	extern Vector ExtrapolateTick(Vector, Vector);
};
