
#include "Misc.h"

VOID CVisuals::Run(VOID)
{
	static CONST Vector vecZ = Vector(0.f, 0.f, 1.f);

	static Color xColor;
	static Entity_T * pEntity = NULL;
	static ClientClass * pClass = NULL;
	static INT iId = 0, iTeam = 0, iIndex = 0, iWidth = 0, iTall = 0;
	static Weapon_T * pWeapon = NULL;
	static Vector vecTop, vecEnemyPos, vecNewPos, vecGrenVel, vecSelfVel, vecThrow, vecStart, vecEnd, vecScreenStart, vecScreenEnd, vecPlaneNormal;
	static FLOAT fTimer = 0.f, fThrowVel = 0.f, fDT = 0.f, fT = 0.f, fDTime = 0.f, fSurfElasticity = 0.f, fElasticity = 0.f, fForce = 0.f, fImpulse = 0.f, fAvgVel = 0.f, fNewVel = 0.f;
	static GameTrace_T xTrace;
	static QAngle angThrow;

	if (!(G::pSelf = GetSelf()))
		return;

	g_pNadeTraj->Work();

	if (G::bAlive)
	{
		if (Vars.Misc.bGrenadePrediction && !G::bEmpty && !G::bGun && IS_GRENADE(G::iWeapId) && ((G::pCmd->buttons & IN_ATTACK) || (G::pCmd->buttons & IN_ATTACK2)))
		{
			iIndex = 0, fDTime = GetDetonationTime(), angThrow = G::pCmd->viewang, \
				angThrow.x = -10.f + angThrow.x + fabsf(angThrow.x) * 10.f / 90.f, \
				M::AngVec(angThrow, &vecThrow), vecThrow.Normalize(), \
				vecStart = G::pSelf->GetEyePosition(), vecStart.x += vecThrow.x * 16.f, \
				vecStart.y += vecThrow.y * 16.f, vecStart.z += vecThrow.z * 16.f, \
				vecStart.z += GetDisp(), vecSelfVel = G::pSelf->GetVelocity(), \
				fThrowVel = 750.f * GetFactor(), vecSelfVel *= 1.25f, \
				vecGrenVel.x = vecThrow.x * fThrowVel + vecSelfVel.x, \
				vecGrenVel.y = vecThrow.y * fThrowVel + vecSelfVel.y, \
				vecGrenVel.z = vecThrow.z * fThrowVel + vecSelfVel.z, fDT = .05f;

			for (fT = 0.f; fT <= fDTime; fT += fDT)
			{
				vecEnd.x = vecStart.x + vecGrenVel.x * fDT, vecEnd.y = vecStart.y + vecGrenVel.y * fDT, \
					fForce = .4f * 800.f, fNewVel = vecGrenVel.z - fForce * fDT, \
					fAvgVel = (vecGrenVel.z + fNewVel) / 2.f, vecEnd.z = vecStart.z + fAvgVel * fDT, \
					vecGrenVel.z = fNewVel, TraceLine(vecStart, vecEnd, MASK_SHOT_HULL, G::pSelf, &xTrace);

				if (xTrace.fraction != 1.f)
				{
					if (xTrace.ent == G::pSelf && fT == 0.f)
					{
						vecStart = vecEnd;
						continue;
					};

					vecEnd = xTrace.endpos, vecPlaneNormal = xTrace.plane.normal, fImpulse = 2.f * vecPlaneNormal.Dot(vecGrenVel), \
						vecGrenVel.x -= fImpulse * vecPlaneNormal.x;

					if (fabsf(vecGrenVel.x) < .1f)
						vecGrenVel.x = 0.f;

					vecGrenVel.y -= fImpulse * vecPlaneNormal.y;

					if (fabsf(vecGrenVel.y) < .1f)
						vecGrenVel.y = 0.f;

					vecGrenVel.z -= fImpulse * vecPlaneNormal.z;

					if (fabsf(vecGrenVel.z) < .1f)
						vecGrenVel.z = 0.f;

					fSurfElasticity = xTrace.ent != NULL ? xTrace.ent->GetElasticity() : 0.f, \
						fElasticity = .45f * fSurfElasticity, \
						vecGrenVel *= fElasticity;

					if (vecPlaneNormal.Dot(vecZ) > .7f && ((G::iWeapId == WEAPON_MOLOTOV) || (G::iWeapId == WEAPON_INCGRENADE)))
						fDTime = 0.f;

					if (++iIndex >= 1 && D::WorldToScreen(vecStart, vecScreenStart) && D::WorldToScreen(vecEnd, vecScreenEnd))
						I::Surf->GetTextSize(g_nStyle, (CONST WCHAR *) XorString("Raw"), iWidth, iTall), \
						D::DrawStr((INT)vecScreenEnd.x, (INT)(vecScreenEnd.y - FLOAT(iTall) - 2.f), g_Red, true, XorString("[ %d ]"), iIndex);
				};

				if (iIndex >= 1 && D::WorldToScreen(vecStart, vecScreenStart) && D::WorldToScreen(vecEnd, vecScreenEnd))
				{
					switch (iIndex)
					{
					case 1: D::DrawLine((INT)vecScreenStart.x, (INT)vecScreenStart.y, (INT)vecScreenEnd.x, (INT)vecScreenEnd.y, g_Blue); break;
					case 2: D::DrawLine((INT)vecScreenStart.x, (INT)vecScreenStart.y, (INT)vecScreenEnd.x, (INT)vecScreenEnd.y, g_Purple); break;
					case 3: D::DrawLine((INT)vecScreenStart.x, (INT)vecScreenStart.y, (INT)vecScreenEnd.x, (INT)vecScreenEnd.y, g_Green); break;
					case 4: D::DrawLine((INT)vecScreenStart.x, (INT)vecScreenStart.y, (INT)vecScreenEnd.x, (INT)vecScreenEnd.y, g_Grey); break;
					case 5: D::DrawLine((INT)vecScreenStart.x, (INT)vecScreenStart.y, (INT)vecScreenEnd.x, (INT)vecScreenEnd.y, g_Red); break;
					case 6: D::DrawLine((INT)vecScreenStart.x, (INT)vecScreenStart.y, (INT)vecScreenEnd.x, (INT)vecScreenEnd.y, g_Orange); break;
					case 7: D::DrawLine((INT)vecScreenStart.x, (INT)vecScreenStart.y, (INT)vecScreenEnd.x, (INT)vecScreenEnd.y, g_Cyan); break;
					case 8: D::DrawLine((INT)vecScreenStart.x, (INT)vecScreenStart.y, (INT)vecScreenEnd.x, (INT)vecScreenEnd.y, g_Magenta); break;
					case 9: D::DrawLine((INT)vecScreenStart.x, (INT)vecScreenStart.y, (INT)vecScreenEnd.x, (INT)vecScreenEnd.y, g_DGrey); break;
					case 10: D::DrawLine((INT)vecScreenStart.x, (INT)vecScreenStart.y, (INT)vecScreenEnd.x, (INT)vecScreenEnd.y, g_DGreen); break;
					case 11: D::DrawLine((INT)vecScreenStart.x, (INT)vecScreenStart.y, (INT)vecScreenEnd.x, (INT)vecScreenEnd.y, g_Black); break;
					case 12: D::DrawLine((INT)vecScreenStart.x, (INT)vecScreenStart.y, (INT)vecScreenEnd.x, (INT)vecScreenEnd.y, g_LBlue); break;
					case 13: D::DrawLine((INT)vecScreenStart.x, (INT)vecScreenStart.y, (INT)vecScreenEnd.x, (INT)vecScreenEnd.y, g_Gold); break;
					default: D::DrawLine((INT)vecScreenStart.x, (INT)vecScreenStart.y, (INT)vecScreenEnd.x, (INT)vecScreenEnd.y, g_White); break;
					};
				};

				vecStart = vecEnd;
			};
		};

		if (Vars.Misc.bAutoSniperCrosshair && IS_SNIPER(G::iWeapId) && !G::pSelf->IsScoped())
			DrawCrosshair(G::pSelf);

		if (G::bGun && !G::bEmpty && Vars.Misc.bRecoilCrosshair && (G::iFlags & FL_ONGROUND) && G::pSelf->GetVelocity().Length() < 128.f && (!(angThrow = G::pSelf->GetPunch()).IsZero() || G::pSelf->IsScoped()))
		{
			vecStart = G::pSelf->GetEyePosition(), M::AngVec(G::pCmd->viewang + angThrow * 2.f, &vecEnd), \
				TraceLine(vecStart, vecStart + vecEnd, MASK_SHOT, G::pSelf, &xTrace);

			if (D::WorldToScreen(xTrace.endpos, vecNewPos))
				xColor = ClrByOption(Vars.Misc.iRecoilCrosshairType), \
				D::DrawLine((INT)(vecNewPos.x - 5.f), (INT)(vecNewPos.y), (INT)(vecNewPos.x + 5.f), (INT)(vecNewPos.y), xColor), \
				D::DrawLine((INT)(vecNewPos.x), (INT)(vecNewPos.y - 5.f), (INT)(vecNewPos.x), (INT)(vecNewPos.y + 5.f), xColor);
		};
	};

	if (!Vars.Visuals.Enabled)
	{
		for (iId = 0; iId < I::Ents->GetHighestEntityIndex(); iId++)
		{
			if (!(pEntity = I::Ents->GetClEntity(iId)) || pEntity == G::pSelf || pEntity->GetDormant() || \
				(vecEnemyPos = pEntity->GetOrigin()).IsZero() || !(pClass = pEntity->GetClientClass()))
				continue;

			if (pClass->m_ClassID == CCSPlayer)
			{
				if (pEntity->GetHealth() < 1 || (iTeam = pEntity->GetTeam()) < 2 || iTeam == G::iTeam || iTeam > 3 || !(pWeapon = pEntity->GetWeapon()))
					continue;

				if (Vars.Visuals.RadarHack)
					pEntity->SetSpotted();

				if (g_bBombPlanted && !pEntity->IsVisible())
					Skeleton(pEntity);
			}

			else
			{
				if (Vars.Misc.bGrenTrace && g_pNadeTraj->IsEntityGrenade(pEntity, pClass))
					g_pNadeTraj->AddGrenade(pEntity->m_iId);

				if (Vars.Misc.bDroppedWeaponsName)
				{
					if (pClass->m_ClassID == CAK47 || \
						pClass->m_ClassID == CDEagle || \
						pClass->m_ClassID == CBaseCSGrenade || \
						pClass->m_ClassID == CBaseGrenade || \
						pClass->m_ClassID == CDecoyGrenade || \
						pClass->m_ClassID == CSensorGrenade || \
						pClass->m_ClassID == CFlashbang || \
						pClass->m_ClassID == CSmokeGrenade || \
						pClass->m_ClassID == CHEGrenade || \
						pClass->m_ClassID == CMolotovGrenade || \
						pClass->m_ClassID == CIncendiaryGrenade || \
						strncmp(pClass->m_pNetworkName, XorString("CWeapon"), 7) == 0)
					{
						if (D::WorldToScreen(vecEnemyPos, vecNewPos))
							D::FDrawStr(INT(vecNewPos.x), INT(vecNewPos.y), g_White, true, ((Weapon_T *)pEntity)->FGetWeaponName());
					}

					else if (pClass->m_ClassID == CC4)
					{
						if (D::WorldToScreen(vecEnemyPos, vecNewPos))
							D::FDrawStr(INT(vecNewPos.x), INT(vecNewPos.y), g_Orange, true, XorString("C4"));
					}

					else if (pClass->m_ClassID == CHostage)
					{
						if (D::WorldToScreen(vecEnemyPos, vecNewPos))
							D::FDrawStr(INT(vecNewPos.x), INT(vecNewPos.y), g_Green, true, XorString("HOSTAGE"));
					}

					else if (pClass->m_ClassID == CDecoyProjectile)
					{
						if (D::WorldToScreen(vecEnemyPos, vecNewPos))
							D::FDrawStr(INT(vecNewPos.x), INT(vecNewPos.y), g_Purple, true, XorString("DECOY"));
					};
				};

				if (pClass->m_ClassID == CPlantedC4 && Vars.Misc.bUseLegitVisualFeatures && D::WorldToScreen(vecEnemyPos, vecNewPos))
				{
					if ((fTimer = pEntity->GetBombTimer()) > 30.f)
						D::DrawStr(INT(vecNewPos.x), INT(vecNewPos.y), g_White, true, XorString("C4 [ %.1f ]"), fTimer);

					else if (fTimer > 20.f)
						D::DrawStr(INT(vecNewPos.x), INT(vecNewPos.y), g_Gold, true, XorString("C4 [ %.1f ]"), fTimer);

					else if (fTimer > 10.f)
						D::DrawStr(INT(vecNewPos.x), INT(vecNewPos.y), g_Orange, true, XorString("C4 [ %.1f ]"), fTimer);

					else
						D::DrawStr(INT(vecNewPos.x), INT(vecNewPos.y), g_Red, true, XorString("C4 [ %.1f ]"), fTimer);
				};
			};
		};
	}

	else
	{
		if (Vars.Visuals.Glow)
			DrawGlow();

		for (iId = 0; iId < I::Ents->GetHighestEntityIndex(); iId++)
		{
			if (!(pEntity = I::Ents->GetClEntity(iId)) || pEntity == G::pSelf || pEntity->GetDormant() || \
				(vecEnemyPos = pEntity->GetOrigin()).IsZero() || !D::WorldToScreen(vecEnemyPos, vecNewPos))
				continue;

			if (iId >= 0 && iId <= I::Globals->m_iMaxClients)
				PlayerESP(pEntity);

			else
				WorldESP(pEntity, vecNewPos);
		};
	};
};

VOID CVisuals::DrawCrosshair(Entity_T * pEntity)
{
	static INT iWidth = 0, iHeight = 0, iCrX = 0, iCrY = 0;
	static Color Clr;

	Clr = ClrByOption(Vars.Misc.SniperCrosshairColor), \
		I::Engine->GetScreenSize(iWidth, iHeight), \
		iCrX = iWidth / 2, iCrY = iHeight / 2;

	D::DrawLine(iCrX - 5, iCrY, iCrX + 5, iCrY, Clr), \
		D::DrawLine(iCrX, iCrY - 5, iCrX, iCrY + 5, Clr);
};

VOID CVisuals::PlayerESP(Entity_T * Entity)
{
	static bool bEnemy = false;

	if (Entity->GetHealth() > 0)
	{
		if ((bEnemy = Entity->IsEnemy()) && Vars.Visuals.RadarHack)
			Entity->SetSpotted();

		if (!Vars.Visuals.Skeleton || (!Vars.Visuals.Filter.Friendlies && !bEnemy) || (!Vars.Visuals.Filter.Enemies && bEnemy))
			return;

		Skeleton(Entity);
	};
};

VOID CVisuals::WorldESP(Entity_T * Entity, Vector & World)
{
	static ClientClass * pClass = NULL;
	static FLOAT fTimer = 0.f;

	if (!(pClass = Entity->GetClientClass()))
		return;

	if (Vars.Misc.bGrenTrace && g_pNadeTraj->IsEntityGrenade(Entity, pClass))
		g_pNadeTraj->AddGrenade(Entity->m_iId);

	if (Vars.Visuals.Filter.Weapons && \
		(pClass->m_ClassID == CAK47 || \
			pClass->m_ClassID == CDEagle || \
			pClass->m_ClassID == CBaseCSGrenade || \
			pClass->m_ClassID == CBaseGrenade || \
			pClass->m_ClassID == CDecoyGrenade || \
			pClass->m_ClassID == CSensorGrenade || \
			pClass->m_ClassID == CFlashbang || \
			pClass->m_ClassID == CSmokeGrenade || \
			pClass->m_ClassID == CHEGrenade || \
			pClass->m_ClassID == CMolotovGrenade || \
			pClass->m_ClassID == CIncendiaryGrenade || \
			strncmp(pClass->m_pNetworkName, XorString("CWeapon"), 7) == 0))
		D::FDrawStr(INT(World.x), INT(World.y), g_White, true, ((Weapon_T *)Entity)->FGetWeaponName());

	else if (pClass->m_ClassID == CC4 && Vars.Visuals.Filter.C4)
		D::FDrawStr(INT(World.x), INT(World.y), g_Orange, true, XorString("C4"));

	else if (pClass->m_ClassID == CHostage && Vars.Visuals.Filter.Hostages)
		D::FDrawStr(INT(World.x), INT(World.y), g_Green, true, XorString("HOSTAGE"));

	else if (pClass->m_ClassID == CDecoyProjectile && Vars.Visuals.Filter.Decoy)
		D::FDrawStr(INT(World.x), INT(World.y), g_Purple, true, XorString("DECOY"));

	else if (pClass->m_ClassID == CPlantedC4 && Vars.Visuals.Filter.C4)
	{
		if ((fTimer = Entity->GetBombTimer()) > 30.f)
			D::DrawStr(INT(World.x), INT(World.y), g_White, true, XorString("C4 [ %.1f ]"), fTimer);

		else if (fTimer > 20.f)
			D::DrawStr(INT(World.x), INT(World.y), g_Gold, true, XorString("C4 [ %.1f ]"), fTimer);

		else if (fTimer > 10.f)
			D::DrawStr(INT(World.x), INT(World.y), g_Orange, true, XorString("C4 [ %.1f ]"), fTimer);

		else
			D::DrawStr(INT(World.x), INT(World.y), g_Red, true, XorString("C4 [ %.1f ]"), fTimer);
	};
};

VOID CVisuals::DrawGlow(VOID)
{
	static INT iIter = 0;
	static CGlowObjMan::GlowObjDef_T * pGlowEntity = NULL;
	static bool bIsEnemy = false;
	static ClientClass * pClass = NULL;
	static CGlowObjMan * pGlowObjMan = NULL;

	if (g_Offsets.m_dwGlowManager && (pGlowObjMan = (CGlowObjMan *)g_Offsets.m_dwGlowManager))
	{
		for (iIter = 0; iIter < pGlowObjMan->m_nSize; iIter++)
		{
			if (!(pGlowEntity = &pGlowObjMan->m_pGlowObjDefs[iIter]) || !pGlowEntity->m_pEntity || \
				pGlowEntity->m_nNextFreeSlot != -2 || pGlowEntity->m_pEntity->GetDormant() || \
				!(pClass = pGlowEntity->m_pEntity->GetClientClass()))
				continue;

			switch (pClass->m_ClassID)
			{
			case CC4:
				if (Vars.Visuals.Filter.C4)
					pGlowEntity->Set(g_Orange);

				else
					pGlowEntity->UnSet();

				break;

			case CHostage:
				if (Vars.Visuals.Filter.Hostages)
					pGlowEntity->Set(g_Green);

				else
					pGlowEntity->UnSet();

				break;

			case CCSPlayer:
				bIsEnemy = pGlowEntity->m_pEntity->IsEnemy();

				if ((!Vars.Visuals.Filter.Friendlies && !bIsEnemy) || (!Vars.Visuals.Filter.Enemies && bIsEnemy))
					break;

				if (pGlowEntity->m_pEntity->IsVisible())
				{
					if ((bIsEnemy && !Vars.Visuals.iEnemiesOGlowColor) || (!bIsEnemy && !Vars.Visuals.iTeammatesOGlowColor))
						pGlowEntity->UnSet();

					else
						pGlowEntity->Set(bIsEnemy ? ClrByOption(Vars.Visuals.iEnemiesOGlowColor - 1) : ClrByOption(Vars.Visuals.iTeammatesOGlowColor - 1));
				}

				else
					pGlowEntity->Set(bIsEnemy ? ClrByOption(Vars.Visuals.iEnemiesGlowColor) : ClrByOption(Vars.Visuals.iTeammatesGlowColor));

				break;

			case CDecoyProjectile:
				if (Vars.Visuals.Filter.Decoy)
					pGlowEntity->Set(g_Purple);

				else
					pGlowEntity->UnSet();

				break;

			case CPlantedC4:
				if (Vars.Visuals.Filter.C4)
					pGlowEntity->Set(g_Red);

				else
					pGlowEntity->UnSet();

				break;

			default:
				if (pClass->m_ClassID == CAK47 || \
					pClass->m_ClassID == CDEagle || \
					pClass->m_ClassID == CBaseCSGrenade || \
					pClass->m_ClassID == CBaseGrenade || \
					pClass->m_ClassID == CDecoyGrenade || \
					pClass->m_ClassID == CSensorGrenade || \
					pClass->m_ClassID == CFlashbang || \
					pClass->m_ClassID == CSmokeGrenade || \
					pClass->m_ClassID == CHEGrenade || \
					pClass->m_ClassID == CMolotovGrenade || \
					pClass->m_ClassID == CIncendiaryGrenade || \
					strncmp(pClass->m_pNetworkName, XorString("CWeapon"), 7) == 0)
				{
					if (Vars.Visuals.Filter.Weapons)
						pGlowEntity->Set(ClrByOption(Vars.Visuals.iDroppedWeaponsGlowColor));

					else
						pGlowEntity->UnSet();
				};

				break;
			};
		};
	};
};

VOID CVisuals::Skeleton(Entity_T * pEntity)
{
	static Matrix34_T pBoneToWorldOut[MAXSTUDIOBONES];
	static Vector vecBonePosA, vecBonePosB;
	static INT iIter = 0;
	static StdHdr_T * pStudioModel = NULL;
	static StdBone_T * pBone = NULL;
	static Model_T * pModel = NULL;

	pModel = pEntity->GetModel(), pStudioModel = pModel ? I::MDLInfo->GetStudioModel(pModel) : NULL;

	if (pStudioModel && pEntity->SetupBones(pBoneToWorldOut, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, I::Globals->m_fCurTime))
	{
		for (iIter = 0; iIter < pStudioModel->iNumBones; iIter++)
		{
			if (!(pBone = pStudioModel->pBone(iIter)) || !(pBone->iFlags & 256) || (pBone->iParent == -1) || \
				!D::WorldToScreen(Vector(pBoneToWorldOut[iIter][0][3], pBoneToWorldOut[iIter][1][3], pBoneToWorldOut[iIter][2][3]), vecBonePosA) || \
				!D::WorldToScreen(Vector(pBoneToWorldOut[pBone->iParent][0][3], pBoneToWorldOut[pBone->iParent][1][3], pBoneToWorldOut[pBone->iParent][2][3]), vecBonePosB))
				continue;

			D::DrawLine((INT)vecBonePosA.x, (INT)vecBonePosA.y, (INT)vecBonePosB.x, (INT)vecBonePosB.y, g_White);
		};
	};
};
