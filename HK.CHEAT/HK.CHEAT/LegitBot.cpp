
#include "Misc.h"

VOID CLBOT::Run(VOID)
{
	static FLOAT fOldSvTime = G::pSelf->GetTickBase() * I::Globals->m_fIntervPerTick, fSvTime = 0.f;

	g_fFoV = IS_PISTOL(G::iWeapId) ? Vars.Legitbot.Aimbot.FOV_Pistols : Vars.Legitbot.Aimbot.FOV;

	if (g_iTopLBOTEnemy != -1)
	{
		if (!(g_pTopLBOTEnemy = I::Ents->GetClEntity(g_iTopLBOTEnemy)) || g_pTopLBOTEnemy == G::pSelf || \
			g_pTopLBOTEnemy->GetHealth() < 1 || g_pTopLBOTEnemy->GetImmune() || g_pTopLBOTEnemy->GetDormant() || \
			(g_iEnemyTeam = g_pTopLBOTEnemy->GetTeam()) < 1 || g_iEnemyTeam == G::iTeam || !BestAimPointAll(g_pTopLBOTEnemy, g_vecLBOTHBox) || \
			!g_pTopLBOTEnemy->IsVisibleAim(g_vecLBOTHBox) || M::FOV(G::pCmd->viewang + G::pSelf->GetPunch() * 2.f, M::CalcAng(G::pSelf->GetEyePosition(), g_vecLBOTHBox)) > g_fFoV)
				g_iTopLBOTEnemy = -1, g_fCurAimTime = 0.f;
	};

	fSvTime = G::pSelf->GetTickBase() * I::Globals->m_fIntervPerTick, g_fDeltaTime = fSvTime - fOldSvTime, fOldSvTime = fSvTime;

	if (Vars.Legitbot.Aimbot.Enabled && (!(G::pCmd->buttons & IN_ATTACK) || (g_iTopLBOTEnemy == -1)))
		FindTarget();

	if (g_iTopLBOTEnemy != -1 && ((G::Keys[VK_LBUTTON]) || (G::pCmd->buttons & IN_ATTACK)))
		GoToTarget();

	if ((G::Keys[Vars.Legitbot.Triggerbot.Key] || Vars.Legitbot.Triggerbot.AutoFire) && Vars.Legitbot.Triggerbot.Enabled)
		TriggerBOT();
};

VOID CLBOT::FindTarget(VOID)
{
	static INT iIter = 0, iTeam = 0;
	static Entity_T * pEntity = NULL;
	static FLOAT fFOV = 0.f;
	static Vector vecEyePos;
	static QAngle angPunch;

	g_fBestFOV = g_fFoV, angPunch = G::pSelf->GetPunch() * 2.f, vecEyePos = G::pSelf->GetEyePosition();

	for (iIter = 0; iIter <= I::Globals->m_iMaxClients; iIter++)
	{
		if (!(pEntity = I::Ents->GetClEntity(iIter)) || pEntity == G::pSelf || pEntity->GetHealth() < 1 || \
			pEntity->GetImmune() || pEntity->GetDormant() || (iTeam = pEntity->GetTeam()) < 1 || iTeam == G::iTeam || \
			!BestAimPointAll(pEntity, g_vecLBOTHBox) || !pEntity->IsVisibleAim(g_vecLBOTHBox))
			continue;

		fFOV = M::FOV(G::pCmd->viewang + angPunch, M::CalcAng(vecEyePos, g_vecLBOTHBox));

		if (fFOV < g_fBestFOV)
			g_fBestFOV = fFOV, g_iTopLBOTEnemy = iIter, g_pTopLBOTEnemy = pEntity;
	};
};

VOID CLBOT::GoToTarget(VOID)
{
	static QAngle angD, angDelta;
	static FLOAT fFinalTime = 0.f;

	if ((g_pTopLBOTEnemy = I::Ents->GetClEntity(g_iTopLBOTEnemy)))
	{
		angD = M::CalcAng(G::pSelf->GetEyePosition(), g_pTopLBOTEnemy->GetPredicted(g_vecLBOTHBox)) - G::pSelf->GetPunch() * 2.f, \
			angDelta = angD - G::pCmd->viewang, angDelta.Clamp();

		if (!angDelta.IsZero())
		{
			fFinalTime = angDelta.Length() / 50.f, g_fCurAimTime += g_fDeltaTime;

			if (g_fCurAimTime > fFinalTime)
				g_fCurAimTime = fFinalTime;

			angDelta *= g_fCurAimTime / fFinalTime, angD = G::pCmd->viewang + angDelta;
		};

		G::pCmd->viewang = angD.Clamp();

		if (Vars.Legitbot.Aimbot.bNonSilent)
			I::Engine->SetViewAngles(G::pCmd->viewang);
	};
};

VOID CLBOT::TriggerBOT(VOID)
{
	static Vector From, fwdVec;
	static bool bDidHit = false, bSniper = false, bIsInMove = false, bPistol = false, bScoped = false;
	static INT enemyTeam = 0;
	static FLOAT fSelfVelLen = 0.f, fSvTime = 0.f;
	static clock_t Old = 0, Now = 0;
	static GameTrace_T _Trace;
	static Ray_T _Ray;
	static TraceFilter_T _Filter;

	M::AngVec(G::pCmd->viewang + G::pSelf->GetPunch() * 2.f, &fwdVec), \
		fwdVec *= 8192.f, _Filter.pSkip = G::pSelf, From = G::pSelf->GetEyePosition(), \
		_Ray.Run(From, From + fwdVec), I::EngTrace->TraceRay(_Ray, MASK_SHOT, &_Filter, &_Trace);

	if (!_Trace.ent || (enemyTeam = _Trace.ent->GetTeam()) < 1 || enemyTeam == G::iTeam)
	{
		if (g_uTraces > 0)
			g_uTraces--;

		return;
	};

	bSniper = IS_SNIPER(G::iWeapId), bPistol = IS_PISTOL(G::iWeapId), bDidHit = false;

	if (bSniper || bPistol)
	{
		if (Vars.Legitbot.Triggerbot.Filter.Head && (_Trace.hitgroup == HITGROUP_HEAD))
			bDidHit = true;

		if (Vars.Legitbot.Triggerbot.Filter.Chest && (_Trace.hitgroup == HITGROUP_CHEST))
			bDidHit = true;

		if (Vars.Legitbot.Triggerbot.Filter.Stomach && (_Trace.hitgroup == HITGROUP_STOMACH))
			bDidHit = true;

		if (Vars.Legitbot.Triggerbot.Filter.Arms && ((_Trace.hitgroup == HITGROUP_LARM) || (_Trace.hitgroup == HITGROUP_RARM)))
			bDidHit = true;

		if (Vars.Legitbot.Triggerbot.Filter.Legs && ((_Trace.hitgroup == HITGROUP_LLEG) || (_Trace.hitgroup == HITGROUP_RLEG)))
			bDidHit = true;
	}

	else
	{
		if (_Trace.hitgroup == HITGROUP_HEAD)
			bDidHit = true;
	};

	if (!bDidHit)
	{
		if (g_uTraces > 0)
			g_uTraces--;

		return;
	};

	if (_Trace.ent->GetHealth() > 0 && !_Trace.ent->GetImmune())
	{
		bIsInMove = ((G::pCmd->buttons & IN_BACK) || (G::pCmd->buttons & IN_FORWARD) || (G::pCmd->buttons & IN_MOVELEFT) || (G::pCmd->buttons & IN_MOVERIGHT) || (G::pCmd->buttons & IN_JUMP)) ? true : false, \
			fSelfVelLen = G::pSelf->GetVelocity().Length(), bScoped = G::pSelf->IsScoped();

		if (!Vars.Legitbot.Triggerbot.Filter.Snipers && bSniper)
		{
			if (g_uTraces > 0)
				g_uTraces--;

			return;
		};

		if (!Vars.Legitbot.Triggerbot.Filter.Weapons && !bSniper)
		{
			if (g_uTraces > 0)
				g_uTraces--;

			return;
		};

		if (bSniper && !bScoped)
		{
			if (g_uTraces > 0)
				g_uTraces--;

			return;
		};

		if (Vars.Legitbot.Triggerbot.Filter.StandSnipers && bSniper && (fSelfVelLen > 0.f || bIsInMove))
		{
			if (g_uTraces > 0)
				g_uTraces--;

			return;
		};

		if (Vars.Legitbot.Triggerbot.Filter.StandWeapons && !bSniper && (fSelfVelLen > 0.f || bIsInMove))
		{
			if (g_uTraces > 0)
				g_uTraces--;

			return;
		};

		if (++g_uTraces >= 2)
		{
			if (bPistol)
			{
				fSvTime = G::pSelf->GetTickBase() * I::Globals->m_fIntervPerTick;

				if (G::pWeap->GetNextPrimaryAttack() <= fSvTime)
				{
					Now = clock();

					if (FLOAT(((DOUBLE)Now - Old) / CLOCKS_PER_SEC) < FLOAT(Vars.Legitbot.Triggerbot.fSecondsDelay))
					{
						if (g_uTraces > 0)
							g_uTraces--;
					}

					else
						G::pCmd->buttons |= IN_ATTACK, Old = clock();

				};
			}

			else
			{
				Now = clock();

				if (FLOAT(((DOUBLE)Now - Old) / CLOCKS_PER_SEC) < FLOAT(Vars.Legitbot.Triggerbot.fSecondsDelay))
				{
					if (g_uTraces > 0)
						g_uTraces--;
				}

				else
					G::pCmd->buttons |= IN_ATTACK, Old = clock();
			};

			g_uTraces = 2 - 1;
		};
	}

	else
	{
		if (g_uTraces > 0)
			g_uTraces--;
	};
};

bool CLBOT::GetHBox(Entity_T * pVictim, CLBHBox * pHBox)
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

bool CLBOT::GetBestPoint(Entity_T * pVictim, CLBHBox * pHBox, CLBBestPoint * pPoint)
{
	static CONST Vector vecZ = Vector(0.f, 0.f, 1.f);

	static QAngle angPunch;
	static Vector vecEyePos, vecHigh, vecHeight, vecNewHeight;
	static FLOAT fPitch = 0.f, fTmpFoV = 8192.f;
	static INT iID = 0;

	vecEyePos = G::pSelf->GetEyePosition(), angPunch = G::pSelf->GetPunch() * 2.f;

	if (pHBox->m_iHBox == HITBOX_HEAD)
	{
		vecHigh = ((pHBox->m_vecPoints[3] + pHBox->m_vecPoints[5]) * .5f), \
			fPitch = pVictim->GetEyeAngles().x;

		if (fPitch > 0.f && fPitch <= 89.f)
			vecHeight = (((vecHigh - pHBox->m_vecPoints[0]) / 3.f) * 4.f), \
			vecNewHeight = (pHBox->m_vecPoints[0] + (vecHeight * (fPitch / 89.f))), \
			pHBox->m_vecPoints[0] = vecNewHeight;

		else if (fPitch < 292.5f && fPitch >= 271.f)
			pHBox->m_vecPoints[0] -= vecZ;
	};

	for (iID = 0; iID < ARRAYSIZE(pHBox->m_vecPoints); iID++)
	{
		fTmpFoV = M::FOV(G::pCmd->viewang + angPunch, M::CalcAng(vecEyePos, pHBox->m_vecPoints[iID]));

		if (pPoint->m_fFoV > fTmpFoV)
			pPoint->m_fFoV = fTmpFoV, pPoint->m_vecPoint = pHBox->m_vecPoints[iID];
	};

	return pPoint->m_fFoV <= g_fFoV;
};

bool CLBOT::BestAimPointAll(Entity_T * pVictim, Vector & vecHBox)
{
	static INT iID = 0;
	static CLBBestPoint ptAim;
	static CLBHBox HBox;

	ptAim.Clear();

	for (iID = 0; iID < ARRAYSIZE(g_nHitBoxes); iID++)
		HBox.Set(g_nHitBoxes[iID]), GetHBox(pVictim, &HBox), GetBestPoint(pVictim, &HBox, &ptAim);

	if (ptAim.m_fFoV <= g_fFoV)
	{
		vecHBox = ptAim.m_vecPoint;
		return true;
	};

	return false;
};
