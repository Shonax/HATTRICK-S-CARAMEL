
#include "Misc.h"

#if defined(ENABLE_RAGE_BOT) && ENABLE_RAGE_BOT == 1

VOID Entity_T::GetPredicted2Way_Smoothed(Vector & Vec)
{
	Vec += (GetVelocity() / 100.f) * 1.333333f, Vec -= (G::pSelf->GetVelocity() / 100.f) * 1.333333f;
};

VOID Entity_T::GetPredicted2Way_NS(Vector & Vec)
{
	Vec += (GetVelocity() / 100.f), Vec -= (G::pSelf->GetVelocity() / 100.f);
};

#endif

bool Entity_T::IsTargetingLocal(VOID)
{
	static Vector S, D, F;
	static GameTrace_T T;

	M::AngVec(GetEyeAngles(), &F), F *= 8192.f, S = GetEyePosition();
	TraceLine(S, S + F, MASK_SHOT, this, &T);

	if (T.ent == G::pSelf)
		return true;

	M::AngVec(GetEyeAngles() + GetPunch() * 2.f, &F), F *= 8192.f;
	TraceLine(S, S + F, MASK_SHOT, this, &T);

	return T.ent == G::pSelf;
};

bool Entity_T::IsTargetingLocalHead(VOID)
{
	static Vector S, D, F;
	static GameTrace_T T;

	M::AngVec(GetEyeAngles(), &F), F *= 8192.f, S = GetEyePosition();
	TraceLine(S, S + F, MASK_SHOT, this, &T);

	if (T.ent == G::pSelf && T.hitgroup == HITGROUP_HEAD)
		return true;

	M::AngVec(GetEyeAngles() + GetPunch() * 2.f, &F), F *= 8192.f;
	TraceLine(S, S + F, MASK_SHOT, this, &T);

	return T.ent == G::pSelf && T.hitgroup == HITGROUP_HEAD;
};

VOID Entity_T::SetSpotted(VOID)
{
	*(bool *)((DWORD) this + g_Offsets.m_bSpotted) = true;
};

INT	Entity_T::GetHealth(VOID)
{
	return *(PINT)((DWORD) this + g_Offsets.m_iHealth);
};

INT Entity_T::GetTeam(VOID)
{
	return *(PINT)((DWORD) this + g_Offsets.m_iTeamNum);
};

bool Entity_T::IsScoped(VOID)
{
	return *(bool *)((DWORD)this + g_Offsets.m_bIsScoped);
};

bool Entity_T::IsScopedV2(VOID)
{
	if (G::iWeapId == WEAPON_DEAGLE || G::iWeapId == WEAPON_HKP2000 || G::iWeapId == WEAPON_USP_SILENCER)
		return true;

	return *(bool *)((DWORD) this + g_Offsets.m_bIsScoped);
};

INT Entity_T::GetFlags(VOID)
{
	return *(PINT)((DWORD) this + g_Offsets.m_fFlags);
};

INT Entity_T::GetTickBase(VOID)
{
	return *(PINT)((DWORD) this + g_Offsets.m_nTickBase);
};

INT Entity_T::GetHitboxSet(VOID)
{
	return *(PINT)((DWORD) this + g_Offsets.m_nHitboxSet);
};

FLOAT Entity_T::GetBombTimer(VOID)
{
	static FLOAT fBombTime, fVal;

	fBombTime = *(PFLOAT)((DWORD) this + g_Offsets.m_flC4Blow), \
		fVal = fBombTime - I::Globals->m_fCurTime;

	return fVal < 0.f ? 0.f : fVal;
};

FLOAT Entity_T::GetElasticity(VOID)
{
	return *(PFLOAT)((DWORD) this + g_Offsets.m_flElasticity);
};

bool Entity_T::GetDormant(VOID)
{
	return *(bool *)((DWORD) this + g_Offsets.m_bDormant);
};

bool Entity_T::GetImmune(VOID)
{
	return *(bool *)((DWORD) this + g_Offsets.m_bGunGameImmunity);
};

Model_T * Entity_T::GetModel(VOID)
{
	return *(Model_T **)((DWORD) this + 0x0000006C);
};

bool Entity_T::IsEnemy(VOID)
{
	static INT iEnemyTeam;
	return (iEnemyTeam = GetTeam()) > 0 && iEnemyTeam != G::iTeam;
};

bool Entity_T::IsVisible(VOID)
{
	static INT _Id = 0;
	static GameTrace_T _Trace;
	static TraceFilter_T _Filter;
	static Ray_T _Ray;
	static clock_t _Stamp[128] = { 0 }, _Now = 0;
	static Vector vecEyePos, vecPos;
	static Matrix34_T Data[MAXSTUDIOBONES];

	if (m_iId >= 0 && m_iId <= 127)
	{
		_Now = clock();

		if (FLOAT(((DOUBLE)_Now - _Stamp[m_iId]) / CLOCKS_PER_SEC) > FLOAT(.075F) && \
			SetupBones(Data, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, I::Globals->m_fCurTime))
		{
			m_bVis[m_iId] = false,
				vecEyePos = G::pSelf->GetEyePosition();

			for (_Id = 0; _Id < MAXSTUDIOBONES; _Id++)
			{
				vecPos = Vector(Data[_Id].m_pArray[0][3], Data[_Id].m_pArray[1][3], Data[_Id].m_pArray[2][3]);

				if (vecPos.IsZero())
					break;

				_Filter.pSkip = G::pSelf, \
					_Ray.Run(vecEyePos, vecPos);

				I::EngTrace->TraceRay(_Ray, MASK_SHOT, &_Filter, &_Trace);

				if (_Trace.ent == this && _Trace.hitgroup > HITGROUP_NO)
				{
					m_bVis[m_iId] = true;
					break;
				};
			};

			_Stamp[m_iId] = _Now;
		};

		return m_bVis[m_iId];
	};

	return false;
};

bool Entity_T::IsVisibleAim(Vector & vecPos)
{
	static GameTrace_T _Trace;
	static TraceFilter_T _Filter;
	static Ray_T _Ray;
	static clock_t _Stamp[128] = { 0 }, _Now = 0;

	if (m_iId >= 0 && m_iId <= 127)
	{
		if (G::iWeapId == WEAPON_AWP || G::iWeapId == WEAPON_SSG08 || G::iWeapId == WEAPON_DEAGLE)
		{
			return IsVisible();
		}

		else
		{
			_Now = clock();

			if (FLOAT(((DOUBLE)_Now - _Stamp[m_iId]) / CLOCKS_PER_SEC) > FLOAT(.075F))
			{
				m_bVis[m_iId] = false, \
					_Filter.pSkip = G::pSelf, \
					_Ray.Run(G::pSelf->GetEyePosition(), vecPos);

				I::EngTrace->TraceRay(_Ray, MASK_SHOT, &_Filter, &_Trace);

				if (_Trace.ent == this && _Trace.hitgroup > HITGROUP_NO)
					m_bVis[m_iId] = true;

				_Stamp[m_iId] = _Now;
			};

			return m_bVis[m_iId];
		};
	};

	return false;
};

QAngle Entity_T::GetPunch(VOID)
{
	return *(QAngle *)((DWORD) this + g_Offsets.m_aimPunchAngle);
};

QAngle Entity_T::GetEyeAngles(VOID)
{
	return *(QAngle *)((DWORD) this + g_Offsets.m_angEyeAngles);
};

Vector Entity_T::GetOrigin(VOID)
{
	return *(Vector *)((DWORD) this + g_Offsets.m_vecOrigin);
};

Vector Entity_T::GetEyePosition(VOID)
{
	return GetOrigin() + *(Vector *)((DWORD) this + g_Offsets.m_vecViewOffset);
};

bool Entity_T::SetupBones(Matrix34_T * pBones, INT iMaxBones, INT iBoneMask, FLOAT fCurTime)
{
	__asm
	{
		mov edi, this
		lea ecx, dword ptr ds : [edi + 0x4]
		mov edx, dword ptr ds : [ecx]
		push fCurTime
		push iBoneMask
		push iMaxBones
		push pBones
		call dword ptr ds : [edx + 0x34]
	};
};

Vector Entity_T::GetVelocity(VOID)
{
	return *(Vector *)((DWORD) this + g_Offsets.m_vecVelocity);
};

Vector Entity_T::GetPredicted(Vector Vec)
{
	return M::ExtrapolateTick(Vec, GetVelocity());
};

Weapon_T * Entity_T::GetWeapon(VOID)
{
	return (Weapon_T *)I::Ents->GetClEntity((*(PDWORD)((DWORD) this + g_Offsets.m_hActiveWeapon)) & 0x00000FFF);
};

ClientClass * Entity_T::GetClientClass(VOID)
{
	static PVOID pNetworkable = NULL;

	pNetworkable = (PVOID)((DWORD)(this) + 0x00000008);
	return GetVFunc<_GetClientClassFn>(pNetworkable, 2)(pNetworkable);
};

FLOAT Weapon_T::GetNextPrimaryAttack(VOID)
{
	return *(PFLOAT)((DWORD) this + g_Offsets.m_flNextPrimaryAttack);
};

INT & Weapon_T::GetItemDefinitionIndex(VOID)
{
	return *(PINT)((DWORD) this + g_Offsets.m_iItemDefinitionIndex);
};

bool Weapon_T::IsEmpty(VOID)
{
	return (*(PINT)((DWORD) this + g_Offsets.m_iClip1)) == 0;
};

bool Weapon_T::IsGun(INT iId)
{
	switch (iId)
	{
	case WEAPON_DEAGLE: case WEAPON_ELITE: case WEAPON_FIVESEVEN: case WEAPON_GLOCK: case WEAPON_AK47:
	case WEAPON_AUG: case WEAPON_AWP: case WEAPON_FAMAS: case WEAPON_G3SG1: case WEAPON_GALILAR: case WEAPON_M249:
	case WEAPON_M4A1: case WEAPON_MAC10: case WEAPON_P90: case WEAPON_UMP45: case WEAPON_XM1014: case WEAPON_BIZON:
	case WEAPON_MAG7: case WEAPON_NEGEV: case WEAPON_SAWEDOFF: case WEAPON_TEC9: case WEAPON_HKP2000: case WEAPON_MP7:
	case WEAPON_MP9: case WEAPON_NOVA: case WEAPON_P250: case WEAPON_SCAR20: case WEAPON_SG556: case WEAPON_SSG08:
	case WEAPON_M4A1_SILENCER: case WEAPON_USP_SILENCER: case WEAPON_CZ75A: case WEAPON_REVOLVER: return true;
	};

	return false;
};

CONST CHAR * Weapon_T::FGetWeaponName(VOID)
{
	switch (GetItemDefinitionIndex())
	{
	case WEAPON_DEAGLE: return XorString("DEAGLE");
	case WEAPON_ELITE: return XorString("ELITES");
	case WEAPON_FIVESEVEN: return XorString("FN57");
	case WEAPON_GLOCK: return XorString("GLOCK");
	case WEAPON_AK47: return XorString("AK47");
	case WEAPON_AUG: return XorString("AUG");
	case WEAPON_AWP: return XorString("AWP");
	case WEAPON_FAMAS: return XorString("FAMAS");
	case WEAPON_G3SG1: return XorString("G3");
	case WEAPON_GALILAR: return XorString("GALIL");
	case WEAPON_M249: return XorString("M249");
	case WEAPON_M4A1: return XorString("M4A4");
	case WEAPON_MAC10: return XorString("MAC");
	case WEAPON_P90: return XorString("P90");
	case WEAPON_UMP45: return XorString("UMP");
	case WEAPON_XM1014: return XorString("XM");
	case WEAPON_BIZON: return XorString("BIZON");
	case WEAPON_MAG7: return XorString("MAG");
	case WEAPON_NEGEV: return XorString("NEGEV");
	case WEAPON_SAWEDOFF: return XorString("SAWED");
	case WEAPON_TEC9: return XorString("TEC");
	case WEAPON_TASER: return XorString("ZEUS");
	case WEAPON_HKP2000: return XorString("P2K");
	case WEAPON_MP7: return XorString("MP7");
	case WEAPON_MP9: return XorString("MP9");
	case WEAPON_NOVA: return XorString("NOVA");
	case WEAPON_P250: return XorString("P250");
	case WEAPON_SCAR20: return XorString("SCAR");
	case WEAPON_SG556: return XorString("SG");
	case WEAPON_SSG08: return XorString("SSG");
	case WEAPON_FLASHBANG: return XorString("FLASH");
	case WEAPON_HEGRENADE: return XorString("GRENADE");
	case WEAPON_SMOKEGRENADE: return XorString("SMOKE");
	case WEAPON_MOLOTOV: return XorString("MOLOTOV");
	case WEAPON_DECOY: return XorString("DECOY");
	case WEAPON_INCGRENADE: return XorString("INCENDIARY");
	case WEAPON_C4: return XorString("C4");
	case WEAPON_M4A1_SILENCER: return XorString("M4A1-S");
	case WEAPON_USP_SILENCER: return XorString("USP-S");
	case WEAPON_CZ75A: return XorString("CZ75");
	case WEAPON_REVOLVER: return XorString("R8");
	case WEAPON_KNIFE_BAYONET: return XorString("BAYONET");
	case WEAPON_KNIFE_BUTTERFLY: return XorString("BUTTERFLY");
	case WEAPON_KNIFE_FALCHION: return XorString("FALCHION");
	case WEAPON_KNIFE_FLIP: return XorString("FLIP");
	case WEAPON_KNIFE_GUT: return XorString("GUT");
	case WEAPON_KNIFE_KARAMBIT: return XorString("KARAMBIT");
	case WEAPON_KNIFE_M9_BAYONET: return XorString("M9");
	case WEAPON_KNIFE_PUSH: return XorString("DAGGERS");
	case WEAPON_KNIFE_SURVIVAL_BOWIE: return XorString("BOWIE");
	case WEAPON_KNIFE_TACTICAL: return XorString("HUNTSMAN");
	};

	return XorString("KNIFE");
};
