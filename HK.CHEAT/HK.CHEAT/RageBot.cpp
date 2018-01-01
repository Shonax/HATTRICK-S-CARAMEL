
#include "Misc.H"

#if defined(ENABLE_RAGE_BOT) && ENABLE_RAGE_BOT == 1

VOID CRBOT::Run(VOID)
{
	FindTarget();

	if (g_pTopEnemy && (Vars.Ragebot.AutoFire || G::Keys[VK_LBUTTON] || G::pCmd->buttons & IN_ATTACK))
		GoToTarget();
};

VOID CRBOT::FindTarget(VOID)
{
	static INT iIter = 0, iTeam = 0;
	static Entity_T * pEntity = NULL;
	static FLOAT fDamage = 0.f, fBestDamage = 0.f;
	static QAngle angPunch;
	static Vector vecSelfEyePos;

	g_pTopEnemy = NULL, fBestDamage = 0.f, vecSelfEyePos = G::pSelf->GetEyePosition(), angPunch = G::pSelf->GetPunch() * 2.f;

	for (iIter = 0; iIter <= I::Globals->m_iMaxClients; iIter++)
	{
		if (!(pEntity = I::Ents->GetClEntity(iIter)) || pEntity == G::pSelf || (iTeam = pEntity->GetTeam()) < 1 || \
			iTeam == G::iTeam || pEntity->GetDormant() || pEntity->GetImmune() || pEntity->GetHealth() < 1 || \
			(!Vars.Ragebot.bAutoWall && !pEntity->IsVisible()) || (fDamage = BestAimPointAll(pEntity, g_vecTopHBox)) <= 0.f ||
			((Vars.Ragebot.FOV < 180.f) && (M::FOV(G::pCmd->viewang + angPunch, M::CalcAng(vecSelfEyePos, g_vecTopHBox)) > Vars.Ragebot.FOV)))
			continue;

		if (fDamage > fBestDamage)
			g_pTopEnemy = pEntity, fBestDamage = fDamage;
	};
};

VOID CRBOT::GoToTarget(VOID)
{
	static FLOAT fSvTime = 0.f, fNextShot = 0.f;

	fSvTime = G::pSelf->GetTickBase() * I::Globals->m_fIntervPerTick, fNextShot = G::pWeap->GetNextPrimaryAttack() - fSvTime;

	if (fNextShot <= 0.f)
	{
		switch (Vars.Ragebot.extrapolationMethod)
		{
		case NULL: g_vecTopHBox = g_pTopEnemy->GetPredicted(g_vecTopHBox); break;
		case 2: g_pTopEnemy->GetPredicted2Way_Smoothed(g_vecTopHBox); break;
		case 3: g_pTopEnemy->GetPredicted2Way_NS(g_vecTopHBox); break;
		};

		g_FM.Start(), G::pCmd->viewang = M::CalcAng(G::pSelf->GetEyePosition(), g_vecTopHBox) - G::pSelf->GetPunch() * 2.f, g_FM.End();

		if (Vars.Ragebot.AutoScope && IS_SNIPER(G::iWeapId) && !G::pSelf->IsScoped())
			G::pCmd->buttons |= IN_ATTACK2, g_zoomTime = clock();

		else
		{
			if (G::iWeapId != WEAPON_AWP && G::iWeapId != WEAPON_SSG08)
				G::pCmd->buttons |= IN_ATTACK;

			else if (FLOAT(((DOUBLE)clock() - g_zoomTime) / CLOCKS_PER_SEC) > FLOAT(.333333f) && G::pSelf->GetVelocity().Length() < 2.f && G::pSelf->IsScoped())
				G::pCmd->buttons |= IN_ATTACK;
		};
	};
};

bool CRBOT::GetHBox(Entity_T * pVictim, CRBHBox * pHBox)
{
	static INT iID = 0, iHBoxSet = 0;
	static Matrix34_T mMatrix[MAXSTUDIOBONES];
	static Vector vecMin, vecMax, vecPts[9];
	static StdHdr_T * pHdr = NULL;
	static StdioBBox_T * pUntransfBox = NULL;
	static Model_T * pModel = NULL;
	static StdHBoxSet_T * pHBoxSet = NULL;

	if (!pVictim->SetupBones(mMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, I::Globals->m_fCurTime) || \
		!(pModel = pVictim->GetModel()) || !(pHdr = I::MDLInfo->GetStudioModel(pModel)) || \
		(iHBoxSet = pVictim->GetHitboxSet()) < 0 || !(pHBoxSet = pHdr->pHBoxSet(iHBoxSet)) || \
		!(pUntransfBox = pHBoxSet->pHBox(pHBox->m_iHBox)))
		return false;

	vecMin = pUntransfBox->m_vecMin, vecMax = pUntransfBox->m_vecMax;

	if (pUntransfBox->m_fRadius != -1.f)
		vecMin -= Vector(pUntransfBox->m_fRadius, pUntransfBox->m_fRadius, pUntransfBox->m_fRadius), \
		vecMax += Vector(pUntransfBox->m_fRadius, pUntransfBox->m_fRadius, pUntransfBox->m_fRadius);

	vecPts[0] = ((vecMin + vecMax) * .5f), \
		vecPts[1] = Vector(vecMin.x, vecMin.y, vecMin.z), \
		vecPts[2] = Vector(vecMin.x, vecMax.y, vecMin.z), \
		vecPts[3] = Vector(vecMax.x, vecMax.y, vecMin.z), \
		vecPts[4] = Vector(vecMax.x, vecMin.y, vecMin.z), \
		vecPts[5] = Vector(vecMax.x, vecMax.y, vecMax.z), \
		vecPts[6] = Vector(vecMin.x, vecMax.y, vecMax.z), \
		vecPts[7] = Vector(vecMin.x, vecMin.y, vecMax.z), \
		vecPts[8] = Vector(vecMax.x, vecMin.y, vecMax.z);

	for (iID = 0; iID < ARRAYSIZE(vecPts); iID++)
	{
		if (iID)
			vecPts[iID] = ((((vecPts[iID] + vecPts[0]) * .5f) + vecPts[iID]) * .5f);

		M::VecTransf(vecPts[iID], mMatrix[pUntransfBox->m_iBone], pHBox->m_vecPoints[iID]);
	};

	return true;
};

bool CRBOT::GetBestPoint(Entity_T * pVictim, CRBHBox * pHBox, CRBBestPoint * pPoint)
{
	static CONST Vector vecZ = Vector(0.f, 0.f, 1.f);

	static Vector vecHigh, vecHeight, vecNewHeight;
	static FLOAT fPitch = 0.f, fTmpDmg = 0.f;
	static INT iID = 0;

	if (pHBox->m_iHBox == HITBOX_HEAD)
	{
		vecHigh = ((pHBox->m_vecPoints[3] + pHBox->m_vecPoints[5]) * .5f), fPitch = pVictim->GetEyeAngles().x;

		if ((fPitch > 0.f) && (fPitch <= 89.f))
			vecHeight = (((vecHigh - pHBox->m_vecPoints[0]) / 3.f) * 4.f), vecNewHeight = (pHBox->m_vecPoints[0] + (vecHeight * (fPitch / 89.f))), \
			pHBox->m_vecPoints[0] = vecNewHeight;

		else if ((fPitch < 292.5f) && (fPitch >= 271.f))
			pHBox->m_vecPoints[0] -= vecZ;
	};

	for (iID = 0; iID < ARRAYSIZE(pHBox->m_vecPoints); iID++)
	{
		fTmpDmg = GetDamage(pHBox->m_vecPoints[iID], pHBox->m_iHBox, pVictim);

		if (pPoint->m_fDmg < fTmpDmg)
			pPoint->m_fDmg = fTmpDmg, pPoint->m_vecPoint = pHBox->m_vecPoints[iID];
	};

	return (pPoint->m_fDmg > 0.f);
};

FLOAT CRBOT::BestAimPointAll(Entity_T * pVictim, Vector & vecHBox)
{
	static INT iID = 0;
	static CRBBestPoint ptAim;
	static CRBHBox HBox;

	ptAim.Clear();
	for (iID = 0; iID < ARRAYSIZE(g_nHitBoxes); iID++)
		HBox.Set(g_nHitBoxes[iID]), GetHBox(pVictim, &HBox), GetBestPoint(pVictim, &HBox, &ptAim);

	if (ptAim.m_fDmg > 0.f)
		vecHBox = ptAim.m_vecPoint;

	return ptAim.m_fDmg;
};

VOID CFM::Start(VOID)
{
	m_angOld = G::pCmd->viewang, m_fOldFwd = G::pCmd->fwdmove, m_fOldSide = G::pCmd->sidemove;
};

VOID CFM::End(VOID)
{
	static FLOAT fYawDelta = 0.f, fA = 0.f, fB = 0.f;

	if (m_angOld.y < 0.f) fA = 360.f + m_angOld.y;
	else fA = m_angOld.y;
	if (G::pCmd->viewang.y < 0.f) fB = 360.f + G::pCmd->viewang.y;
	else fB = G::pCmd->viewang.y;
	if (fB < fA) fYawDelta = fabsf(fB - fA);
	else fYawDelta = 360.f - fabsf(fA - fB);
	fYawDelta = 360.f - fYawDelta, G::pCmd->fwdmove = cosf(DEG2RAD(fYawDelta)) * m_fOldFwd + cosf(DEG2RAD(fYawDelta + 90.f)) * m_fOldSide, G::pCmd->sidemove = sinf(DEG2RAD(fYawDelta)) * m_fOldFwd + sinf(DEG2RAD(fYawDelta + 90.f)) * m_fOldSide;
};

#endif
