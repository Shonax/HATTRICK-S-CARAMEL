#pragma once

#include "Misc.h"

class Entity_T
{
public:

	CHAR m_szDummy[0x00000064];
	INT m_iId;

	bool IsVisibleAim(Vector &);
	VOID GetPredicted2Way_Smoothed(Vector &);
	VOID GetPredicted2Way_NS(Vector &);
	VOID SetSpotted(VOID);
	INT GetHealth(VOID);
	INT GetTeam(VOID);
	INT GetFlags(VOID);
	FLOAT GetElasticity(VOID);
	INT GetTickBase(VOID);
	INT GetHitboxSet(VOID);
	bool GetDormant(VOID);
	bool GetImmune(VOID);
	bool IsEnemy(VOID);
	bool IsVisible(VOID);
	FLOAT GetBombTimer(VOID);
	QAngle GetPunch(VOID);
	QAngle GetEyeAngles(VOID);
	Vector GetOrigin(VOID);
	Vector GetEyePosition(VOID);
	bool IsScoped(VOID);
	bool IsScopedV2(VOID);
	bool SetupBones(Matrix34_T *, INT, INT, FLOAT);
	Vector GetVelocity(VOID);
	Vector GetPredicted(Vector);
	Model_T * GetModel(VOID);
	Weapon_T * GetWeapon(VOID);
	ClientClass * GetClientClass(VOID);
	bool IsTargetingLocal(VOID);
	bool IsTargetingLocalHead(VOID);
};

class Weapon_T
{
public:

	CHAR m_szDummy[0x00000064];
	INT m_iId;

	FLOAT GetNextPrimaryAttack(VOID);
	INT & GetItemDefinitionIndex(VOID);
	bool IsEmpty(VOID);
	bool IsGun(INT);
	CONST CHAR * FGetWeaponName(VOID);
};
