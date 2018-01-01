
#include "Misc.h"

VOID M::SinCos(FLOAT radians, PFLOAT sine, PFLOAT cosine)
{
	*sine = sinf(radians), *cosine = cosf(radians);
};

VOID M::VecAng(Vector & vecFwd, QAngle & angAng)
{
	if (vecFwd.y == 0.f && vecFwd.x == 0.f) angAng.x = (vecFwd.z > 0.f) ? 270.f : 90.f, angAng.y = 0.f;
	else
	{
		angAng.x = atan2f(-vecFwd.z, vecFwd.Length2D()) * -180.f / M_PI_F, angAng.y = atan2f(vecFwd.y, vecFwd.x) * 180.f / M_PI_F;
		if (angAng.y > 90.f) angAng.y -= 180.f;
		else if (angAng.y < 90.f) angAng.y += 180.f;
		else if (angAng.y == 90.f) angAng.y = 0.f;
	};
	angAng.z = 0.f;
};

VOID M::AngVec(QAngle & angAng, Vector * vecFwd)
{
	static FLOAT sp, sy, cp, cy;
	SinCos(DEG2RAD(angAng.y), &sy, &cy), SinCos(DEG2RAD(angAng.x), &sp, &cp);
	vecFwd->x = cp * cy, vecFwd->y = cp * sy, vecFwd->z = -sp;
};

FLOAT M::FOV(QAngle & angViewAng, QAngle & angAimAng)
{
	static Vector vecAng, vecAim;
	AngVec(angViewAng, &vecAim), AngVec(angAimAng, &vecAng);
	return RAD2DEG(acosf(vecAim.Dot(vecAng) / vecAim.LengthSqr()));
};

QAngle M::CalcAng(Vector Src, Vector Dst)
{
	static QAngle angRes;
	VecAng(Src - Dst, angRes);
	return angRes;
};

VOID M::VecTransf(Vector & A, Matrix34_T & B, Vector & Output)
{
	Output.x = A.Dot(B.m_pArray[0]) + B.m_pArray[0][3], Output.y = A.Dot(B.m_pArray[1]) + B.m_pArray[1][3], Output.z = A.Dot(B.m_pArray[2]) + B.m_pArray[2][3];
};

Vector M::ExtrapolateTick(Vector Origin, Vector Velocity)
{
	return Origin + (Velocity * I::Globals->m_fIntervPerTick);
};
