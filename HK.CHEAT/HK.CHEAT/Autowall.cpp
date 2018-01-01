
#include "Misc.h"

bool TraceToExit(Vector & vecEnd, GameTrace_T & xEnterTrace, FLOAT fX, FLOAT fY, FLOAT fZ, FLOAT fDirX, FLOAT fDirY, FLOAT fDirZ, GameTrace_T * pExitTrace)
{
	typedef bool(__fastcall * TTE) (Vector &, GameTrace_T &, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, GameTrace_T *);

	static TTE pFN = TTE(FindPattern(XorString("client.dll"), XorString("55 8B EC 83 EC 30 F3 0F 10 75")));

	if (!pFN)
		return false;

	_asm
	{
		push pExitTrace
		push fDirZ
		push fDirY
		push fDirX
		push fZ
		push fY
		push fX
		mov edx, xEnterTrace
		mov ecx, vecEnd
		call pFN
		add esp, 0x0000001C
	};
};

FLOAT GetDamage(Vector & vecPoint, INT iHBoxID, Entity_T * pEntity)
{
	static Vector vecStart, vecDir, vecDummy;
	static INT iTeam = 0;
	static GameTrace_T xTrace, xExitTrace;
	static FLOAT fWallLen = 0.f, fDamage[128] = { 0.f };
	static clock_t tArray[128][8], tNow = 0;

	vecStart = G::pSelf->GetEyePosition(), M::AngVec(M::CalcAng(vecStart, vecPoint), &vecDir), vecDir *= 8192.f;

	TraceLine(vecStart, vecStart + vecDir, MASK_SHOT, G::pSelf, &xTrace);

	if (xTrace.hitgroup > HITGROUP_NO)
	{
		if (xTrace.ent && ((iTeam = xTrace.ent->GetTeam()) < 2 || iTeam == G::iTeam || iTeam > 3))
			return (fDamage[pEntity->m_iId] = 0.f);

		switch (xTrace.hitgroup)
		{
		case HITGROUP_HEAD: fDamage[pEntity->m_iId] = 40.f;
		case HITGROUP_STOMACH: fDamage[pEntity->m_iId] = 30.f;
		case HITGROUP_CHEST: case HITGROUP_LARM: case HITGROUP_RARM: fDamage[pEntity->m_iId] = 20.f;
		case HITGROUP_LLEG: case HITGROUP_RLEG: fDamage[pEntity->m_iId] = 10.f;
		};

		return fDamage[pEntity->m_iId];
	};

	if (iHBoxID != HITBOX_HEAD && iHBoxID != HITBOX_CHEST)
		return (fDamage[pEntity->m_iId] = 0.f);

	if (G::iWeapId != WEAPON_DEAGLE && G::iWeapId != WEAPON_SCAR20 && G::iWeapId != WEAPON_G3SG1 && \
		G::iWeapId != WEAPON_SSG08 && G::iWeapId != WEAPON_AWP && G::iWeapId != WEAPON_AUG && \
		G::iWeapId != WEAPON_SG556 && G::iWeapId != WEAPON_M4A1_SILENCER && G::iWeapId != WEAPON_M4A1 && \
		G::iWeapId != WEAPON_AK47 && G::iWeapId != WEAPON_M249)
		return (fDamage[pEntity->m_iId] = 0.f);

	tNow = clock();

	if (FLOAT(((DOUBLE)tNow - tArray[pEntity->m_iId][iHBoxID]) / CLOCKS_PER_SEC) > FLOAT(.1f))
	{
		tArray[pEntity->m_iId][iHBoxID] = tNow, vecDummy.Zero();

		if (!TraceToExit(vecDummy, xTrace, xTrace.endpos.x, xTrace.endpos.y, xTrace.endpos.z, vecDir.x, vecDir.y, vecDir.z, &xExitTrace))
			return (fDamage[pEntity->m_iId] = 0.f);

		fWallLen = (xExitTrace.endpos - xTrace.endpos).Length();

		if (g_fMaxWallLen == 0.f)
		{
			g_fMaxWallLen = fWallLen;
			return (fDamage[pEntity->m_iId] = 0.f);
		};

		if (fWallLen > g_fMaxWallLen)
			g_fMaxWallLen = fWallLen;

		if (fWallLen < g_fMaxWallLen / 2.f)
		{
			TraceLine(xExitTrace.endpos, xExitTrace.endpos + vecDir, MASK_SHOT, G::pSelf, &xTrace);

			if (xTrace.hitgroup > HITGROUP_NO)
				return (fDamage[pEntity->m_iId] = ((xTrace.hitgroup == HITGROUP_HEAD) ? 5.f : 1.f));

			return (fDamage[pEntity->m_iId] = 0.f);
		};

		return (fDamage[pEntity->m_iId] = 0.f);
	};

	return fDamage[pEntity->m_iId];
};
