#pragma once

// OPTIONS //

#define ENABLE_RAGE_BOT			1		// 0 = NO, 1 = YES

// OPTIONS //

#define ISINRANGE(X, A, B)	(X >= A && X <= B)

#define GETBITS(X) (ISINRANGE((X & (~0x00000020)), 'A', 'F') ? ((X & (~0x00000020)) - 'A' + 0x0000000A) : (ISINRANGE(X, '0', '9') ? X - '0' : 0x00000000))
#define GETBYTE(X) (GETBITS(X[0]) << 4 | GETBITS(X[1]))

#include <WinSock2.h>
#include <d3d9.h>

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <ShlObj.h>
#include <Psapi.h>
#include <io.h>
#include <TlHelp32.h>

#include <algorithm>
#include <string>
#include <unordered_map>
#include <fstream>

#include "Hattrick.H"
#include "XorString.h"

extern Hattrick g_Hattrick;

#include "imgui.h"
#include "json\json.h"

template <typename Type> FORCEINLINE Type CallVFunc(PVOID pClassBase, INT nFnID)
{
	return (Type)(*(PDWORD *)pClassBase)[nFnID];
};

template <typename Type> FORCEINLINE Type GetVFunc(PVOID pTable, INT nID)
{
	return (Type)(*(Type **)pTable)[nID];
};

template <typename Type = FLOAT> FORCEINLINE Type FRand(Type MIN, Type HI)
{
	return (Type)(MIN + static_cast <Type> (rand()) / (static_cast <Type> (RAND_MAX / (HI - MIN))));
};

template <typename Type = INT> FORCEINLINE Type IRand(Type MIN, Type MAX)
{
	return (Type)((rand() % (MAX - MIN + 1)) + MIN);
};

template <typename Type = FLOAT> FORCEINLINE Type FClamp(Type Val, Type Min, Type Max)
{
	if (Val < Min) return Min;
	else if (Val > Max) return Max;
	return Val;
};

template <typename Type = INT> FORCEINLINE Type IClamp(Type Val, Type Min, Type Max)
{
	if (Val < Min) return Min;
	else if (Val > Max) return Max;
	return Val;
};

FORCEINLINE bool stristr(CONST CHAR * A, CONST CHAR * B)
{
	static std::string _A = "", _B = "";
	_A = A, _B = B, std::transform(_A.begin(), _A.end(), _A.begin(), ::tolower), std::transform(_B.begin(), _B.end(), _B.begin(), ::tolower);
	return strstr(_A.c_str(), _B.c_str()) ? true : false;
};

FORCEINLINE DWORD FindPattern(std::string Module, std::string Pattern)
{
	static Hattrick::ModuleTuple * pModule = NULL;
	static CONST CHAR * pszPattern = NULL;
	static DWORD dwFirst = (DWORD)0, dwStart = (DWORD)0, dwEnd = (DWORD)0, dwCur = (DWORD)0;

	if (!(pModule = g_Hattrick.ProcessModule(Module)))
		return DWORD(0);

	pszPattern = Pattern.c_str(), dwFirst = (DWORD)0, dwStart = (DWORD)pModule->m_pHandle, dwEnd = dwStart + pModule->m_Info.SizeOfImage;

	for (dwCur = dwStart; dwCur < dwEnd; dwCur++)
	{
		if (*pszPattern == '\0')
			return dwFirst;

		if (*(PBYTE)pszPattern == '\?' || *(PBYTE)dwCur == GETBYTE(pszPattern))
		{
			if (dwFirst == DWORD(0))
				dwFirst = dwCur;

			if (pszPattern[2] == '\0')
				return dwFirst;

			if (*(PWORD)pszPattern == '\?\?' || *(PBYTE)pszPattern != '\?') pszPattern += 3;
			else pszPattern += 2;
		}

		else
		{
			pszPattern = Pattern.c_str(), dwFirst = (DWORD)0;
		};
	};

	return NULL;
};

typedef struct PlrInfo_S
{
	INT64 __DUMMY__;

	union
	{
		INT64 xuid;

		struct
		{
			INT xuidlow;
			INT xuidhigh;
		};
	};

	CHAR name[128];
	INT userid;
	CHAR guid[33];
	UINT friendsid;
	CHAR friendsname[128];
	bool fakeplayer;
	bool ishltv;
	UINT customfiles[4];
	UCHAR filesdownloaded;
} PlrInfo_T;

template <typename Type> FORCEINLINE Type * CaptureInterface(std::string Module, std::string Interface)
{
	typedef Type * (*FnType) (CONST CHAR *, INT);

	static Hattrick::ModuleTuple * pModule = NULL;

	if (!(pModule = g_Hattrick.ProcessModule(Module)))
		return (Type *)NULL;

	FnType pfnAddr = (FnType)pModule->m_pCreateInterfaceAddress;
	if (!pfnAddr)
		return (Type *)NULL;

	Type * pData = pfnAddr(Interface.c_str(), NULL);
	if (!pData)
		return (Type *)NULL;

	return (Type *)pData;
};

class Entity_T;
class CVisuals;
class IGameEvent;
class IEnts;
class IVPanel;
class IInputSystem;
class IEngineClient;
class IVModelInfo;
class IGlobalVarsBase;
class ISurface;
class IEngineTrace;
class IBaseClientDll;
class DownloadProgressDLL;
class IClientModeShared;
class Vector;
class QAngle;
class CUserCmd;
class CGrenTrajObj;
class CGrenTraj;
class Weapon_T;
class Color;
class AudioState_t;
class GameTrace_T;
class CViewSetup;
class pfnDemoCustomDataCallback;
class ClientClass;
class IClientEntity;
class VTHook;
class COffsets;

class IGameEventManager2 { };
class IClientModeShared { };

template <typename Type> class CFGVal;

class QAngle
{
public:

	FLOAT x, y, z;

	FORCEINLINE QAngle(VOID)
	{
		x = y = z = 0.f;
	};

	FORCEINLINE QAngle(FLOAT X, FLOAT Y, FLOAT Z)
	{
		x = X, y = Y, z = Z;
	};

	FORCEINLINE bool IsZero(VOID)
	{
		return x == 0.f && y == 0.f && z == 0.f;
	};

	FORCEINLINE FLOAT Length(VOID)
	{
		return sqrtf(x * x + y * y);
	};

	FORCEINLINE QAngle & Clamp(VOID)
	{
		if (x < -89.f) x = -89.f;
		if (x > 89.f) x = 89.f;
		while (y < -180.f) y += 360.f;
		while (y > 180.f) y -= 360.f;
		z = 0.f;
		return *this;
	};

	FORCEINLINE QAngle & operator = (CONST QAngle & Other)
	{
		x = Other.x, y = Other.y, z = Other.z;
		return *this;
	};

	FORCEINLINE QAngle & operator += (CONST QAngle & Other)
	{
		x += Other.x, y += Other.y, z += Other.z;
		return *this;
	};

	FORCEINLINE QAngle & operator + (CONST QAngle & Other)
	{
		static QAngle Res;
		Res.x = x + Other.x, Res.y = y + Other.y, Res.z = z + Other.z;
		return Res;
	};

	FORCEINLINE QAngle & operator -= (CONST QAngle & Other)
	{
		x -= Other.x, y -= Other.y, z -= Other.z;
		return *this;
	};

	FORCEINLINE QAngle & operator - (CONST QAngle & Other)
	{
		static QAngle Res;
		Res.x = x - Other.x, Res.y = y - Other.y, Res.z = z - Other.z;
		return Res;
	};

	FORCEINLINE QAngle & operator *= (FLOAT fVal)
	{
		x *= fVal, y *= fVal, z *= fVal;
		return *this;
	};

	FORCEINLINE QAngle & operator * (CONST QAngle & Other)
	{
		static QAngle Res;
		Res.x = x * Other.x, Res.y = y * Other.y, Res.z = z * Other.z;
		return Res;
	};

	FORCEINLINE QAngle & operator * (FLOAT fVal)
	{
		static QAngle Other;
		Other.x = x * fVal, Other.y = y * fVal, Other.z = z * fVal;
		return Other;
	};

	FORCEINLINE QAngle & operator /= (FLOAT fVal)
	{
		x /= fVal, y /= fVal, z /= fVal;
		return *this;
	};

	FORCEINLINE QAngle & operator / (CONST QAngle & Other)
	{
		static QAngle Res;
		Res.x = x / Other.x, Res.y = y / Other.y, Res.z = z / Other.z;
		return Res;
	};

	FORCEINLINE QAngle & operator / (FLOAT fVal)
	{
		static QAngle Other;
		Other.x = x / fVal, Other.y = y / fVal, Other.z = z / fVal;
		return Other;
	};
};

class Vector
{
public:

	FLOAT x, y, z;

	FORCEINLINE Vector(VOID) { x = y = z = 0.f; };
	FORCEINLINE Vector(FLOAT X, FLOAT Y, FLOAT Z) { x = X, y = Y, z = Z; };

	FORCEINLINE VOID Zero(VOID) { x = y = z = 0.f; };
	FORCEINLINE FLOAT Length(VOID) { return sqrtf(x * x + y * y + z * z); };
	FORCEINLINE FLOAT LengthSqr(VOID) { return x * x + y * y + z * z; };
	FORCEINLINE FLOAT Length2D(VOID) { return sqrtf(x * x + y * y); };
	FORCEINLINE bool IsZero(FLOAT fTol = .01f) { return x > -fTol && x < fTol && y > -fTol && y < fTol && z > -fTol && z < fTol; };
	FORCEINLINE FLOAT Dot(CONST Vector & Other) { return x * Other.x + y * Other.y + z * Other.z; };
	FORCEINLINE FLOAT Dot(PFLOAT pData) { return x * pData[0] + y * pData[1] + z * pData[2]; };

	FORCEINLINE VOID Normalize(VOID)
	{
		static FLOAT fLen = 0.f;
		fLen = Length();
		if (fLen != 0.f) *this /= fLen;
		else x = y = 0.f, z = 1.f;
	};

	FORCEINLINE Vector & operator = (CONST Vector & Other)
	{
		x = Other.x, y = Other.y, z = Other.z;
		return *this;
	};

	FORCEINLINE Vector & operator += (CONST Vector & Other)
	{
		x += Other.x, y += Other.y, z += Other.z;
		return *this;
	};

	FORCEINLINE Vector & operator + (CONST Vector & Other)
	{
		static Vector Res;
		Res.x = x + Other.x, Res.y = y + Other.y, Res.z = z + Other.z;
		return Res;
	};

	FORCEINLINE Vector & operator -= (CONST Vector & Other)
	{
		x -= Other.x, y -= Other.y, z -= Other.z;
		return *this;
	};

	FORCEINLINE Vector & operator - (CONST Vector & Other)
	{
		static Vector Res;
		Res.x = x - Other.x, Res.y = y - Other.y, Res.z = z - Other.z;
		return Res;
	};

	FORCEINLINE Vector & operator *= (FLOAT fVal)
	{
		x *= fVal, y *= fVal, z *= fVal;
		return *this;
	};

	FORCEINLINE Vector & operator * (CONST Vector & Other)
	{
		static Vector Res;
		Res.x = x * Other.x, Res.y = y * Other.y, Res.z = z * Other.z;
		return Res;
	};

	FORCEINLINE Vector & operator * (FLOAT fVal)
	{
		static Vector Other;
		Other.x = x * fVal, Other.y = y * fVal, Other.z = z * fVal;
		return Other;
	};

	FORCEINLINE Vector & operator /= (FLOAT fVal)
	{
		x /= fVal, y /= fVal, z /= fVal;
		return *this;
	};

	FORCEINLINE Vector & operator / (CONST Vector & Other)
	{
		static Vector Res;
		Res.x = x / Other.x, Res.y = y / Other.y, Res.z = z / Other.z;
		return Res;
	};

	FORCEINLINE Vector & operator / (FLOAT fVal)
	{
		static Vector Other;
		Other.x = x / fVal, Other.y = y / fVal, Other.z = z / fVal;
		return Other;
	};
};

struct IDirect3DDevice9;
struct VMatrix_T;
struct RecvProp_T;
struct Model_T;
struct StdBone_T;
struct StdHdr_T;
struct Ray_T;

struct Vector2D
{
public:

	FLOAT m_fX, m_fY;

	FORCEINLINE Vector2D(VOID)
	{
		m_fX = m_fY = 0.f;
	};

	FORCEINLINE Vector2D(FLOAT X, FLOAT Y)
	{
		m_fX = X, m_fY = Y;
	};
};

struct StdioBBox_T
{
	INT m_iBone;
	INT m_iGroup;
	Vector m_vecMin;
	Vector m_vecMax;
	INT iHBoxNameID;
	INT m_pUnusedA[0x00000003];
	FLOAT m_fRadius;
	INT m_pUnusedB[0x00000004];
};

struct Matrix34_T
{
	PFLOAT operator [] (INT iId)
	{
		return m_pArray[iId];
	};

	float m_pArray[3][4];
};

typedef PVOID(*CreateClientClassFn) (INT, INT);
typedef PVOID(*CreateEventFn) (VOID);
typedef bool(__thiscall * FireEventClientSide) (PVOID, IGameEvent *);
typedef bool (WINAPI * CreateMoveFn) (FLOAT, CUserCmd *);
typedef VOID(WINAPI * PaintTraverseFn) (UINT, bool, bool);
typedef LONG(WINAPI * EndSceneFn) (IDirect3DDevice9 *);
typedef LONG(WINAPI * ResetFn) (IDirect3DDevice9 *, D3DPRESENT_PARAMETERS *);
typedef bool(WINAPIV * ServerRankRevealAllFn) (PFLOAT);
typedef ClientClass * (__thiscall * _GetClientClassFn) (PVOID);

#include "NetVarManager.h"
#include "Entity.h"
#include "Trace.h"
#include "Math.h"
#include "IBaseClientDLL.h"
#include "IClientEntityList.h"
#include "IEngineClient.h"
#include "IEngineTrace.h"
#include "ISurface.h"
#include "IVPanel.h"
#include "Cfg.h"
#include "LegitBot.h"
#include "RageBot.h"
#include "Draw.h"
#include "IVModelInfo.h"

#define IS_SNIPER(X) (X == WEAPON_SCAR20 || X == WEAPON_G3SG1 || X == WEAPON_AWP || X == WEAPON_SSG08)
#define IS_REVOLVER(X) (X == WEAPON_REVOLVER || X == WEAPON_NEGEV)
#define IS_PISTOL(X) (X == WEAPON_USP_SILENCER || X == WEAPON_HKP2000 || X == WEAPON_CZ75A || X == WEAPON_TEC9 || X == WEAPON_DEAGLE || X == WEAPON_GLOCK || X == WEAPON_P250 || X == WEAPON_ELITE || X == WEAPON_FIVESEVEN || X == WEAPON_REVOLVER)
#define IS_GRENADE(X) (X == WEAPON_DECOY || X == WEAPON_FLASHBANG || X == WEAPON_INCGRENADE || X == WEAPON_MOLOTOV || X == WEAPON_HEGRENADE || X == WEAPON_SMOKEGRENADE)

struct Variables
{

#if defined(ENABLE_RAGE_BOT) && ENABLE_RAGE_BOT == 1

	struct
	{
		bool 	Enabled;
		bool 	AutoFire;
		bool	AutoScope;
		float	FOV;
		bool	bAutoWall;
		int		extrapolationMethod;
	} Ragebot;

#endif

	struct
	{
		struct
		{
			bool 	Enabled;
			bool	bNonSilent;
			float 	FOV;
			float	FOV_Pistols;
		} Aimbot;

		struct
		{
			bool	Enabled;
			bool	AutoFire;
			int		Key;
			float	fSecondsDelay;

			struct
			{
				bool Head;
				bool Chest;
				bool Stomach;
				bool Arms;
				bool Legs;
				bool Weapons;
				bool Snipers;
				bool StandSnipers;
				bool StandWeapons;
			} Filter;

		} Triggerbot;

	} Legitbot;

	struct
	{
		bool 	Enabled;
		bool	Skeleton;
		bool	Glow;
		bool	RadarHack;
		int		iGlowAlphaIntensity;
		float	fGlowBloomAmount;
		int		iDroppedWeaponsGlowColor;
		int		iEnemiesGlowColor;
		int		iTeammatesGlowColor;
		int		iEnemiesOGlowColor;
		int		iTeammatesOGlowColor;

		struct
		{
			bool Enemies;
			bool Friendlies;
			bool Weapons;
			bool Decoy;
			bool C4;
			bool Hostages;
		} Filter;
	} Visuals;

	struct
	{
		bool	Ranks;
		bool	bAutoLeftRight;
		bool	bSpecialFlickShots;
		bool	bAutoStrafe;
		bool	bDroppedWeaponsName;
		bool	bAutoSniperCrosshair;
		bool	bBombSiteSkeletons;
		int		SniperCrosshairColor;
		bool	bGrenadePrediction;
		bool	bBunnyHop;
		bool	bGrenTrace;
		bool	bDoNotEditRadarScale;
		bool	bSpam;
		bool	bDoNotEnforceBestPingLagSettings;
		bool	bRecoilCrosshair;
		int		iRecoilCrosshairType;
		bool	bUseLegitVisualFeatures;
	} Misc;

	struct
	{
		bool	Opened;
	} Menu;
};

namespace G
{
	extern bool bAlive;
	extern Entity_T * pSelf;
	extern INT iWeapId;
	extern INT iFlags;
	extern INT iTeam;
	extern CUserCmd * pCmd;
	extern HWND pWnd;
	extern bool Keys[256];
	extern bool bD3DOK;
	extern bool bGun;
	extern bool bEmpty;
	extern Weapon_T * pWeap;
};

namespace H
{
	extern VTHook * VPanel;
	extern VTHook * CLMode;
	extern VTHook * CL;
	extern VTHook * D3D;
};

namespace I
{
	extern IBaseClientDll * Client;
	extern IClientModeShared * CLMode;
	extern IEnts * Ents;
	extern IEngineClient * Engine;
	extern IEngineTrace * EngTrace;
	extern IGlobalVarsBase * Globals;
	extern IInputSystem * InputSys;
	extern IGameEventManager2 * EvntsMgr;
	extern ISurface * Surf;
	extern IVPanel * VPanel;
	extern IVModelInfo * MDLInfo;
};

extern bool g_bVisFlip;
extern CGrenTraj g_nadeTraj;
extern bool g_bBombPlanted;
extern CGrenTraj * g_pNadeTraj;
extern UINT g_uiDrawPanel;
extern CONST CHAR * g_pszPanelName;
extern PaintTraverseFn oPaintTraverse;
extern bool g_bFrozen;
extern Variables Vars;
extern bool m_bVis[128];
extern ServerRankRevealAllFn ServerRankRevealAllEx;
extern std::string g_CFGFile;
extern Json::StyledWriter g_JsonStyledWriter;
extern COffsets g_Offsets;
extern Color g_Grey;
extern Color g_Red;
extern Color g_Blue;
extern Color g_LBlue;
extern Color g_Green;
extern Color g_DGrey;
extern Color g_Black;
extern Color g_White;
extern clock_t g_zoomTime;
extern Color g_Purple;
extern CHAR g_szClanTag[256];
extern CHAR g_szFormattedClanTag[256];
extern Color g_Orange;
extern Color g_Magenta;
extern CONST CHAR * g_pExtrapolationOptions[4];
extern time_t g_timeLastEventStamp;
extern bool g_bLoaded;

#if defined(ENABLE_RAGE_BOT) && ENABLE_RAGE_BOT == 1

extern CFM g_FM;
extern CRBOT * g_pRageBOT;

#endif

extern Color g_DGreen;
extern Color g_Cyan;
extern Color g_Gold;
extern CNetVarMan * g_pNetVarsMan;
extern CConfig * Config;
extern WNDPROC g_pOldWndProc;
extern bool g_bOffline;
extern Json::Reader g_JsonReader;
extern time_t g_bombDropTime;
extern Json::Value g_JsonAimWaveCFGTree;
extern CreateMoveFn oCreateMove;
extern time_t g_roundStartTime;
extern clock_t g_saveTime;
extern clock_t g_loadTime;
extern ResetFn oReset;
extern EndSceneFn oEndScene;
extern INT g_nStyle;
extern CHAR g_Phrases[256][256];
extern INT g_PhrasesSize;
extern FireEventClientSide fnOriginalFireEventClientSide;
extern BOOL g_bCheckForLastEnemy;
extern CLBOT * g_pLegitBOT;
extern CVisuals * g_pVisuals;
extern FLOAT g_fBestFOV;
extern INT g_iTopLBOTEnemy;
extern FLOAT g_fDeltaTime;
extern FLOAT g_fCurAimTime;
extern SIZE_T g_uTraces;
extern bool g_bRoundEnded;
extern Vector g_vecLBOTHBox;
extern Vector g_vecTopHBox;
extern FLOAT g_fFoV;
extern Entity_T * g_pTopLBOTEnemy;
extern INT g_iEnemyTeam;
extern time_t g_lastDoPlantMsg;
extern Entity_T * g_pTopEnemy;
extern QAngle g_angSelfView;
extern time_t g_LoadTime;
extern FLOAT g_fMaxWallLen;
extern CONST CHAR * g_pCustomColorOptions[14];
extern CONST CHAR * g_pCustomGlowColorOptions[15];
extern CONST CHAR * g_pTriggerKeys[124];
extern INT g_nHitBoxes[20];

FLOAT GetDamage(Vector &, INT, Entity_T *);
bool TraceToExit(Vector &, GameTrace_T &, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, GameTrace_T *);
LRESULT WINAPI OnWindProc(HWND, UINT, WPARAM, LPARAM);
IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
VOID ImGui_ImplDX9_NewFrame(VOID);
VOID ImGui_ImplDX9_InvalidateDeviceObjects(VOID);
VOID SetClanTag(VOID);
INT FilterClanTag(ImGuiTextEditCallbackData *);
bool ImGui_ImplDX9_CreateDeviceObjects(VOID);
static bool ImGui_ImplDX9_CreateFontsTexture(VOID);
bool ImGui_ImplDX9_Init(PVOID, IDirect3DDevice9 *);
VOID ImGui_ImplDX9_RenderDrawLists(ImDrawData *);
VOID CreateGUI(IDirect3DDevice9 *);
VOID GrabOffsets(VOID);
bool WINAPI CreateMove(FLOAT, CUserCmd *);
VOID WINAPI PaintTraverse(UINT, bool, bool);
LONG WINAPI EndScene(IDirect3DDevice9 *);
LONG WINAPI Reset(IDirect3DDevice9 *, D3DPRESENT_PARAMETERS *);
bool __fastcall FireEventClientSideThink(PVOID, PVOID, IGameEvent *);
FLOAT GetDisp(VOID);
FLOAT GetFactor(VOID);
FLOAT GetDetonationTime(VOID);
VOID JWPPS(CONST CHAR *, CONST CHAR *, CONST CHAR *);
VOID JGPPS(CONST CHAR *, CONST CHAR *, PCHAR, INT);
Entity_T * GetSelf(VOID);
VOID TraceLine(Vector &, Vector &, UINT, Entity_T *, GameTrace_T *);
VOID ServerRankRevealAll(VOID);
VOID SetupInterfaces(VOID);
VOID SetupHooks(VOID);
VOID SetupOffsets(VOID);
VOID SetupAll(VOID);
INT RandomINT(INT, INT);
FLOAT RandomFL(FLOAT, FLOAT);
VOID replaceAll(std::string &, std::string, std::string);
BOOL xIsCharUpper(CHAR x);
BOOL xIsCharLower(CHAR x);
SIZE_T countChars(std::string, CHAR);
std::string ShortenName(std::string);
VOID PrepareSpamString(PCHAR, PlrInfo_T *);

class IGameEvent
{
public:
	CONST CHAR * GetName(VOID)
	{
		return CallVFunc<CONST CHAR * (__thiscall *)(PVOID)>(this, 1)(this);
	};

	INT GetInt(CONST CHAR * pszKey, INT iDefVal = 0)
	{
		return CallVFunc<INT(__thiscall *)(PVOID, CONST CHAR *, INT)>(this, 6)(this, pszKey, iDefVal);
	};

	CONST CHAR * GetString(CONST CHAR * pszKey)
	{
		return CallVFunc<CONST CHAR * (__thiscall *)(PVOID, CONST CHAR *, INT)>(this, 9)(this, pszKey, NULL);
	};

	VOID SetString(CONST CHAR * pszKey, CONST CHAR * pszVal)
	{
		return CallVFunc<VOID(__thiscall *)(PVOID, CONST CHAR *, CONST char *)>(this, 16)(this, pszKey, pszVal);
	};
};

enum EClassId : INT
{
	CAK47 = 1,
	CBaseAnimating = 2,
	CBaseAnimatingOverlay = 3,
	CBaseAttributableItem = 4,
	CBaseButton = 5,
	CBaseCombatCharacter = 6,
	CBaseCombatWeapon = 7,
	CBaseCSGrenade = 8,
	CBaseCSGrenadeProjectile = 9,
	CBaseDoor = 10,
	CBaseEntity = 11,
	CBaseFlex = 12,
	CBaseGrenade = 13,
	CBaseParticleEntity = 14,
	CBasePlayer = 15,
	CBasePropDoor = 16,
	CBaseTeamObjectiveResource = 17,
	CBaseTempEntity = 18,
	CBaseToggle = 19,
	CBaseTrigger = 20,
	CBaseViewModel = 21,
	CBaseVPhysicsTrigger = 22,
	CBaseWeaponWorldModel = 23,
	CBeam = 24,
	CBeamSpotlight = 25,
	CBoneFollower = 26,
	CBreakableProp = 27,
	CBreakableSurface = 28,
	CC4 = 29,
	CCascadeLight = 30,
	CChicken = 31,
	CColorCorrection = 32,
	CColorCorrectionVolume = 33,
	CCSGameRulesProxy = 34,
	CCSPlayer = 35,
	CCSPlayerResource = 36,
	CCSRagdoll = 37,
	CCSTeam = 38,
	CDEagle = 39,
	CDecoyGrenade = 40,
	CDecoyProjectile = 41,
	CDynamicLight = 42,
	CDynamicProp = 43,
	CEconEntity = 44,
	CEconWearable = 45,
	CEmbers = 46,
	CEntityDissolve = 47,
	CEntityFlame = 48,
	CEntityFreezing = 49,
	CEntityParticleTrail = 50,
	CEnvAmbientLight = 51,
	CEnvDetailController = 52,
	CEnvDOFController = 53,
	CEnvParticleScript = 54,
	CEnvProjectedTexture = 55,
	CEnvQuadraticBeam = 56,
	CEnvScreenEffect = 57,
	CEnvScreenOverlay = 58,
	CEnvTonemapController = 59,
	CEnvWind = 60,
	CFEPlayerDecal = 61,
	CFireCrackerBlast = 62,
	CFireSmoke = 63,
	CFireTrail = 64,
	CFish = 65,
	CFlashbang = 66,
	CFogController = 67,
	CFootstepControl = 68,
	CFunc_Dust = 69,
	CFunc_LOD = 70,
	CFuncAreaPortalWindow = 71,
	CFuncBrush = 72,
	CFuncConveyor = 73,
	CFuncLadder = 74,
	CFuncMonitor = 75,
	CFuncMoveLinear = 76,
	CFuncOccluder = 77,
	CFuncReflectiveGlass = 78,
	CFuncRotating = 79,
	CFuncSmokeVolume = 80,
	CFuncTrackTrain = 81,
	CGameRulesProxy = 82,
	CHandleTest = 83,
	CHEGrenade = 84,
	CHostage = 85,
	CHostageCarriableProp = 86,
	CIncendiaryGrenade = 87,
	CInferno = 88,
	CInfoLadderDismount = 89,
	CInfoOverlayAccessor = 90,
	CItem_Healthshot = 91,
	CItemDogtags = 92,
	CKnife = 93,
	CKnifeGG = 94,
	CLightGlow = 95,
	CMaterialModifyControl = 96,
	CMolotovGrenade = 97,
	CMolotovProjectile = 98,
	CMovieDisplay = 99,
	CParticleFire = 100,
	CParticlePerformanceMonitor = 101,
	CParticleSystem = 102,
	CPhysBox = 103,
	CPhysBoxMultiplayer = 104,
	CPhysicsProp = 105,
	CPhysicsPropMultiplayer = 106,
	CPhysMagnet = 107,
	CPlantedC4 = 108,
	CPlasma = 109,
	CPlayerResource = 110,
	CPointCamera = 111,
	CPointCommentaryNode = 112,
	CPointWorldText = 113,
	CPoseController = 114,
	CPostProcessController = 115,
	CPrecipitation = 116,
	CPrecipitationBlocker = 117,
	CPredictedViewModel = 118,
	CProp_Hallucination = 119,
	CPropDoorRotating = 120,
	CPropJeep = 121,
	CPropVehicleDriveable = 122,
	CRagdollManager = 123,
	CRagdollProp = 124,
	CRagdollPropAttached = 125,
	CRopeKeyframe = 126,
	CSCAR17 = 127,
	CSceneEntity = 128,
	CSensorGrenade = 129,
	CSensorGrenadeProjectile = 130,
	CShadowControl = 131,
	CSlideshowDisplay = 132,
	CSmokeGrenade = 133,
	CSmokeGrenadeProjectile = 134,
	CSmokeStack = 135,
	CSpatialEntity = 136,
	CSpotlightEnd = 137,
	CSprite = 138,
	CSpriteOriented = 139,
	CSpriteTrail = 140,
	CStatueProp = 141,
	CSteamJet = 142,
	CSun = 143,
	CSunlightShadowControl = 144,
	CTeam = 145,
	CTeamplayRoundBasedRulesProxy = 146,
	CTEArmorRicochet = 147,
	CTEBaseBeam = 148,
	CTEBeamEntPoint = 149,
	CTEBeamEnts = 150,
	CTEBeamFollow = 151,
	CTEBeamLaser = 152,
	CTEBeamPoints = 153,
	CTEBeamRing = 154,
	CTEBeamRingPoint = 155,
	CTEBeamSpline = 156,
	CTEBloodSprite = 157,
	CTEBloodStream = 158,
	CTEBreakModel = 159,
	CTEBSPDecal = 160,
	CTEBubbles = 161,
	CTEBubbleTrail = 162,
	CTEClientProjectile = 163,
	CTEDecal = 164,
	CTEDust = 165,
	CTEDynamicLight = 166,
	CTEEffectDispatch = 167,
	CTEEnergySplash = 168,
	CTEExplosion = 169,
	CTEFireBullets = 170,
	CTEFizz = 171,
	CTEFootprintDecal = 172,
	CTEFoundryHelpers = 173,
	CTEGaussExplosion = 174,
	CTEGlowSprite = 175,
	CTEImpact = 176,
	CTEKillPlayerAttachments = 177,
	CTELargeFunnel = 178,
	CTEMetalSparks = 179,
	CTEMuzzleFlash = 180,
	CTEParticleSystem = 181,
	CTEPhysicsProp = 182,
	CTEPlantBomb = 183,
	CTEPlayerAnimEvent = 184,
	CTEPlayerDecal = 185,
	CTEProjectedDecal = 186,
	CTERadioIcon = 187,
	CTEShatterSurface = 188,
	CTEShowLine = 189,
	CTesla = 190,
	CTESmoke = 191,
	CTESparks = 192,
	CTESprite = 193,
	CTESpriteSpray = 194,
	CTest_ProxyToggle_Networkable = 194,
	CTestTraceline = 196,
	CTEWorldDecal = 197,
	CTriggerPlayerMovement = 198,
	CTriggerSoundOperator = 199,
	CVGuiScreen = 200,
	CVoteController = 201,
	CWaterBullet = 202,
	CWaterLODControl = 203,
	CWeaponAug = 204,
	CWeaponAWP = 205,
	CWeaponBaseItem = 206,
	CWeaponBizon = 207,
	CWeaponCSBase = 208,
	CWeaponCSBaseGun = 209,
	CWeaponCycler = 210,
	CWeaponElite = 211,
	CWeaponFamas = 212,
	CWeaponFiveSeven = 213,
	CWeaponG3SG1 = 214,
	CWeaponGalil = 215,
	CWeaponGalilAR = 216,
	CWeaponGlock = 217,
	CWeaponHKP2000 = 218,
	CWeaponM249 = 219,
	CWeaponM3 = 220,
	CWeaponM4A1 = 221,
	CWeaponMAC10 = 222,
	CWeaponMag7 = 223,
	CWeaponMP5Navy = 224,
	CWeaponMP7 = 225,
	CWeaponMP9 = 226,
	CWeaponNegev = 227,
	CWeaponNOVA = 228,
	CWeaponP228 = 229,
	CWeaponP250 = 230,
	CWeaponP90 = 231,
	CWeaponSawedoff = 232,
	CWeaponSCAR20 = 233,
	CWeaponScout = 234,
	CWeaponSG550 = 235,
	CWeaponSG552 = 236,
	CWeaponSG556 = 237,
	CWeaponSSG08 = 238,
	CWeaponTaser = 239,
	CWeaponTec9 = 240,
	CWeaponTMP = 241,
	CWeaponUMP45 = 242,
	CWeaponUSP = 243,
	CWeaponXM1014 = 244,
	CWorld = 245,
	DustTrail = 246,
	MovieExplosion = 247,
	ParticleSmokeGrenade = 248,
	RocketTrail = 249,
	SmokeTrail = 250,
	SporeExplosion = 251,
	SporeTrail = 252,
};

enum AntiAim : INT
{
	AA_None = 0,

	AA_FullRandom,
	AA_RandomNegative,
	AA_RandomPositive,

	AA_FullDance,
	AA_DanceNegative,
	AA_DancePositive,

	AA_Static_Low,
	AA_Static_Medium,
	AA_Static_High,

	AA_Flip_V1,
	AA_Flip_V2,
};

#define M_PI_F ((FLOAT)(3.14159265359))

#define RAD2DEG(X) ((FLOAT)(X) * (FLOAT)(57.2957795131))
#define DEG2RAD(X) ((FLOAT)(X) * (FLOAT)(0.01745329251))

#define IN_ATTACK (1 << 0)
#define IN_JUMP (1 << 1)
#define IN_DUCK (1 << 2)
#define IN_FORWARD (1 << 3)
#define IN_BACK (1 << 4)
#define IN_USE (1 << 5)
#define IN_CANCEL (1 << 6)
#define IN_LEFT (1 << 7)
#define IN_RIGHT (1 << 8)
#define IN_MOVELEFT (1 << 9)
#define IN_MOVERIGHT (1 << 10)
#define IN_ATTACK2 (1 << 11)
#define IN_RUN (1 << 12)
#define IN_RELOAD (1 << 13)
#define IN_ALT1 (1 << 14)
#define IN_ALT2 (1 << 15)
#define IN_SCORE (1 << 16)
#define IN_SPEED (1 << 17)
#define IN_WALK (1 << 18)
#define IN_ZOOM (1 << 19)
#define IN_WEAPON1 (1 << 20)
#define IN_WEAPON2 (1 << 21)
#define IN_BULLRUSH (1 << 22)

#define FL_ONGROUND (1 << 0) 
#define FL_DUCKING (1 << 1) 
#define FL_WATERJUMP (1 << 3) 
#define FL_ONTRAIN (1 << 4) 
#define FL_INRAIN (1 << 5) 
#define FL_FROZEN (1 << 6) 
#define FL_ATCONTROLS (1 << 7) 
#define FL_CLIENT (1 << 8) 
#define FL_FAKECLIENT (1 << 9) 
#define FL_INWATER (1 << 10)

#define CONTENTS_EMPTY 0
#define CONTENTS_SOLID 0x1
#define CONTENTS_WINDOW 0x2
#define CONTENTS_AUX 0x4
#define CONTENTS_GRATE 0x8
#define CONTENTS_SLIME 0x10
#define CONTENTS_WATER 0x20
#define CONTENTS_BLOCKLOS 0x40
#define CONTENTS_OPAQUE 0x80
#define CONTENTS_TESTFOGVOLUME 0x100
#define CONTENTS_TEAM1 0x800
#define CONTENTS_TEAM2 0x1000
#define CONTENTS_IGNORE_NODRAW_OPAQUE 0x2000
#define CONTENTS_MOVEABLE 0x4000
#define CONTENTS_AREAPORTAL 0x8000
#define CONTENTS_PLAYERCLIP 0x10000
#define CONTENTS_MONSTERCLIP 0x20000
#define CONTENTS_CURRENT_0 0x40000
#define CONTENTS_CURRENT_90 0x80000
#define CONTENTS_CURRENT_180 0x100000
#define CONTENTS_CURRENT_270 0x200000
#define CONTENTS_CURRENT_UP 0x400000
#define CONTENTS_CURRENT_DOWN 0x800000
#define CONTENTS_ORIGIN 0x1000000
#define CONTENTS_MONSTER 0x2000000
#define CONTENTS_DEBRIS 0x4000000
#define CONTENTS_DETAIL 0x8000000
#define CONTENTS_TRANSLUCENT 0x10000000
#define CONTENTS_LADDER 0x20000000
#define CONTENTS_HITBOX 0x40000000

#define MASK_ALL (0xFFFFFFFF)
#define MASK_SOLID (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define MASK_PLAYERSOLID (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define MASK_NPCSOLID (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define MASK_NPCFLUID (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define MASK_WATER (CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
#define MASK_OPAQUE (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
#define MASK_OPAQUE_AND_NPCS (MASK_OPAQUE|CONTENTS_MONSTER)
#define MASK_BLOCKLOS (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
#define MASK_BLOCKLOS_AND_NPCS (MASK_BLOCKLOS|CONTENTS_MONSTER)
#define MASK_VISIBLE (MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define MASK_VISIBLE_AND_NPCS (MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define MASK_SHOT_BRUSHONLY (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)
#define MASK_SHOT_HULL (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
#define MASK_SHOT_PORTAL (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define MASK_SHOT (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
#define MASK_SOLID_BRUSHONLY (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
#define MASK_PLAYERSOLID_BRUSHONLY (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
#define MASK_NPCSOLID_BRUSHONLY (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define MASK_NPCWORLDSTATIC (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define MASK_NPCWORLDSTATIC_FLUID (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP)
#define MASK_SPLITAREAPORTAL (CONTENTS_WATER|CONTENTS_SLIME)
#define MASK_CURRENT (CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)
#define MASK_DEADSOLID (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)

enum ItemDefinitionIndex : INT
{
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516,
};

class CUserCmd
{
public:

	virtual ~CUserCmd(VOID) { };
	INT cmdnum;
	INT tickcnt;
	QAngle viewang;
	Vector aimdir;
	FLOAT fwdmove;
	FLOAT sidemove;
	FLOAT upmove;
	INT buttons;
	BYTE impulse;
	INT weaponselect;
	INT weaponsubtype;
	INT randseed;
	SHORT mousedx;
	SHORT mousedy;
};

struct VMatrix_T
{
	FLOAT pMatrix[4][4];

	float * operator [] (int iId)
	{
		return pMatrix[iId];
	};
};

class CViewSetup
{
public:

	INT x, x_old;
	INT y, y_old;
	INT width, width_old;
	INT height, height_old;
	bool m_bOrtho;
	FLOAT m_OrthoLeft;
	FLOAT m_OrthoTop;
	FLOAT m_OrthoRight;
	FLOAT m_OrthoBottom;
	bool m_bCustomViewMatrix;
	Matrix34_T m_matCustomViewMatrix;
	CHAR pad_0x68[0x00000048];
	FLOAT fov;
	FLOAT fovViewmodel;
	Vector origin;
	QAngle angles;
	FLOAT zNear;
	FLOAT zFar;
	FLOAT zNearViewmodel;
	FLOAT zFarViewmodel;
	FLOAT m_flAspectRatio;
	FLOAT m_flNearBlurDepth;
	FLOAT m_flNearFocusDepth;
	FLOAT m_flFarFocusDepth;
	FLOAT m_flFarBlurDepth;
	FLOAT m_flNearBlurRadius;
	FLOAT m_flFarBlurRadius;
	INT m_nDoFQuality;
	INT m_nMotionBlurMode;
	FLOAT m_flShutterTime;
	Vector m_vShutterOpenPosition;
	QAngle m_shutterOpenAngles;
	Vector m_vShutterClosePosition;
	QAngle m_shutterCloseAngles;
	FLOAT m_flOffCenterTop;
	FLOAT m_flOffCenterBottom;
	FLOAT m_flOffCenterLeft;
	FLOAT m_flOffCenterRight;
	INT m_EdgeBlur;
};

class Color
{
public:

	Color(VOID)
	{
		Set((BYTE)255, (BYTE)255, (BYTE)255, (BYTE)255);
	};

	Color(BYTE R, BYTE G, BYTE B, BYTE A = (BYTE)255)
	{
		Set(R, G, B, A);
	};

	VOID Set(BYTE R, BYTE G, BYTE B, BYTE A = 255)
	{
		m_Col[0] = (BYTE)R, m_Col[1] = (BYTE)G, m_Col[2] = (BYTE)B, m_Col[3] = (BYTE)A;
	};

	Vector ToVec3DBase(VOID)
	{
		return Vector(m_Col[0] / 255.f, m_Col[1] / 255.f, m_Col[2] / 255.f);
	};

	static Color Red(VOID)
	{
		return Color((BYTE)255, (BYTE)0, (BYTE)0);
	};

	static Color Gold(VOID)
	{
		return Color((BYTE)255, (BYTE)240, (BYTE)0);
	};

	static Color Green(VOID)
	{
		return Color((BYTE)0, (BYTE)255, (BYTE)0);
	};

	static Color Blue(VOID)
	{
		return Color((BYTE)0, (BYTE)0, (BYTE)255);
	};

	static Color LBlue(VOID)
	{
		return Color((BYTE)0, (BYTE)160, (BYTE)255);
	};

	static Color Grey(VOID)
	{
		return Color((BYTE)128, (BYTE)128, (BYTE)128);
	};

	static Color DGrey(VOID)
	{
		return Color((BYTE)48, (BYTE)48, (BYTE)48);
	};

	static Color Black(VOID)
	{
		return Color((BYTE)0, (BYTE)0, (BYTE)0);
	};

	static Color White(VOID)
	{
		return Color((BYTE)255, (BYTE)255, (BYTE)255);
	};

	static Color Purple(VOID)
	{
		return Color((BYTE)128, (BYTE)0, (BYTE)255);
	};

	static Color Magenta(VOID)
	{
		return Color((BYTE)255, (BYTE)0, (BYTE)255);
	};

	static Color Orange(VOID)
	{
		return Color((BYTE)255, (BYTE)128, (BYTE)0);
	};

	static Color Cyan(VOID)
	{
		return Color((BYTE)0, (BYTE)255, (BYTE)255);
	};

	static Color DGreen(VOID)
	{
		return Color((BYTE)160, (BYTE)255, (BYTE)0);
	};

	BYTE m_Col[4];
};

#define MAXSTUDIOSKINS 32
#define MAXSTUDIOBONES 128

#define BONE_USED_BY_HITBOX 0x00000100

#define HITGROUP_NO 0
#define HITGROUP_HEAD 1
#define HITGROUP_CHEST 2
#define HITGROUP_STOMACH 3
#define HITGROUP_LARM 4 
#define HITGROUP_RARM 5
#define HITGROUP_LLEG 6
#define HITGROUP_RLEG 7

struct Model_T
{
	CHAR szName[255];
};

class IInputSystem
{
public:

	void GetCursorPosition(int *, int *);
};

class VTHook
{
public:

	FORCEINLINE VTHook(VOID)
	{
		ZeroMemory(this, sizeof VTHook);
	};

	FORCEINLINE VTHook(PDWORD * ppdwClassBase)
	{
		Startup(ppdwClassBase);
	};

	FORCEINLINE ~VTHook(VOID)
	{
		UnHook();
	};

	FORCEINLINE VOID Startup(PDWORD * ppdwClassBase)
	{
		m_ppdwClassBase = ppdwClassBase;
		m_pdwOldVT = *ppdwClassBase;
		m_dwVTSize = GetVTCount(*ppdwClassBase);
		m_pdwNewVT = new DWORD[m_dwVTSize];
		CopyMemory(m_pdwNewVT, m_pdwOldVT, sizeof(DWORD) * m_dwVTSize);
		*ppdwClassBase = m_pdwNewVT;
	};

	FORCEINLINE VOID UnHook(VOID)
	{
		if (m_ppdwClassBase)
			*m_ppdwClassBase = m_pdwOldVT;
	};

	FORCEINLINE VOID ReHook(VOID)
	{
		if (m_ppdwClassBase)
			*m_ppdwClassBase = m_pdwNewVT;
	};

	FORCEINLINE DWORD HookFunction(DWORD dwNewFunc, INT nID)
	{
		if (m_pdwNewVT && m_pdwOldVT && nID <= m_dwVTSize && nID >= NULL)
		{
			m_pdwNewVT[nID] = dwNewFunc;
			return m_pdwOldVT[nID];
		};

		return NULL;
	};

	FORCEINLINE DWORD GetVTCount(PDWORD pdwVMT)
	{
		DWORD dwID = 0;

		for (dwID; pdwVMT[dwID]; dwID++)
		{
			if (IsBadCodePtr((FARPROC)pdwVMT[dwID]))
				break;
		};

		return dwID;
	};

	PDWORD * m_ppdwClassBase;
	PDWORD m_pdwNewVT, m_pdwOldVT;
	DWORD m_dwVTSize;
};

class CVisuals
{
public:

	VOID Run(VOID);
	VOID DrawGlow(VOID);
	VOID PlayerESP(Entity_T *);
	VOID WorldESP(Entity_T *, Vector &);
	VOID DrawCrosshair(Entity_T *);
	VOID Skeleton(Entity_T *);
};

class COffsets
{
public:

	DWORD m_bSpotted;
	DWORD m_iHealth;
	DWORD m_iTeamNum;
	DWORD m_bDormant;
	DWORD m_bGunGameImmunity;
	DWORD m_flElasticity;
	DWORD m_fFlags;
	DWORD m_nTickBase;
	DWORD m_aimPunchAngle;
	DWORD m_vecOrigin;
	DWORD m_vecViewOffset;
	DWORD m_vecVelocity;
	DWORD m_flNextPrimaryAttack;
	DWORD m_hActiveWeapon;
	DWORD m_fAccuracyPenalty;
	DWORD m_bIsScoped;
	DWORD m_nHitboxSet;
	DWORD m_ArmorValue;
	DWORD m_angEyeAngles;
	DWORD m_flC4Blow;
	DWORD m_flFlashDuration;
	DWORD m_iItemDefinitionIndex;
	DWORD m_iClip1;

	DWORD m_dwD3DDevice;
	DWORD m_dwGlowManager;
	DWORD m_dwServerRankRevealAllEx;
};

class CGrenTrajObj
{
public:

	FORCEINLINE CGrenTrajObj(VOID)
	{
		m_vecPositions.clear();
		m_Grenade = -1;
		m_lastThinkTime = 0;
		m_bActive = false;
		m_pClientClass = NULL;
		m_Color = g_White;
	};

	FORCEINLINE ~CGrenTrajObj(VOID)
	{
		m_vecPositions.clear();
		m_Grenade = -1;
		m_lastThinkTime = 0;
		m_bActive = false;
		m_pClientClass = NULL;
		m_Color = g_White;
	};

	std::vector <Vector> m_vecPositions;
	clock_t m_lastThinkTime;
	INT m_Grenade;
	ClientClass * m_pClientClass;
	Color m_Color;
	bool m_bActive;
};

class CGrenTraj
{
public:

	FORCEINLINE CGrenTraj(VOID)
	{
		Clear();
	};

	FORCEINLINE ~CGrenTraj(VOID)
	{
		Clear();
	};

	FORCEINLINE VOID AddGrenade(INT grenadeId)
	{
		static Vector vecActualPos;
		static Entity_T * pGrenade = 0;
		static CGrenTrajObj Grenade;
		static ClientClass * pClass = 0;

		if (grenadeId < 0 || IsGrenadeQueued(grenadeId))
			return;

		pGrenade = I::Ents->GetClEntity(grenadeId);
		if (!pGrenade || pGrenade->m_iId < 0 || pGrenade->GetVelocity().IsZero())
			return;

		pClass = pGrenade->GetClientClass();
		if (!pClass || !pClass->m_pNetworkName)
			return;

		vecActualPos = pGrenade->GetOrigin();
		if (vecActualPos.IsZero())
			return;

		Grenade.m_vecPositions.clear();

		Grenade.m_Grenade = grenadeId;
		Grenade.m_lastThinkTime = clock();
		Grenade.m_vecPositions.push_back(vecActualPos);
		Grenade.m_bActive = true;
		Grenade.m_pClientClass = pClass;

		m_vecGrenades.push_back(Grenade);
	};

	FORCEINLINE bool IsEntityGrenade(Entity_T * pEntity, ClientClass * pClientClass)
	{
		if (!pEntity || pEntity->m_iId < 0 || !pClientClass || !pClientClass->m_pNetworkName || \
			(pClientClass->m_ClassID != CBaseCSGrenadeProjectile && pClientClass->m_ClassID != CDecoyProjectile && \
				pClientClass->m_ClassID != CMolotovProjectile && pClientClass->m_ClassID != CSmokeGrenadeProjectile && \
				pClientClass->m_ClassID != CSensorGrenadeProjectile))
			return false;

		return true;
	};

	FORCEINLINE VOID Work(VOID)
	{
		static SIZE_T mIterator = 0, mPosIterator = 0, mPosCount = 0;
		static Entity_T * mGrenadeHandle = 0;
		static Vector mActualPos, mOldPos, mOldScreenPos, mOlderPos, mOlderScreenPos;

		for (mIterator = 0; mIterator < m_vecGrenades.size(); mIterator++)
		{
			if (!m_vecGrenades.at(mIterator).m_bActive)
				continue;

			mGrenadeHandle = I::Ents->GetClEntity(m_vecGrenades.at(mIterator).m_Grenade);
			if (!mGrenadeHandle || mGrenadeHandle->m_iId < 0 || mGrenadeHandle->GetVelocity().IsZero())
			{
				m_vecGrenades.at(mIterator).m_vecPositions.clear();
				m_vecGrenades.at(mIterator).m_bActive = false;

				continue;
			};

			mActualPos = mGrenadeHandle->GetOrigin();
			if (mActualPos.IsZero())
			{
				m_vecGrenades.at(mIterator).m_vecPositions.clear();
				m_vecGrenades.at(mIterator).m_bActive = false;

				continue;
			};

			m_vecGrenades.at(mIterator).m_pClientClass = mGrenadeHandle->GetClientClass();
			if (!IsEntityGrenade(mGrenadeHandle, mGrenadeHandle->GetClientClass()))
			{
				m_vecGrenades.at(mIterator).m_vecPositions.clear();
				m_vecGrenades.at(mIterator).m_bActive = false;

				continue;
			};

			if (FLOAT(((DOUBLE)clock() - m_vecGrenades.at(mIterator).m_lastThinkTime) / CLOCKS_PER_SEC) >= FLOAT(.05F))
			{
				m_vecGrenades.at(mIterator).m_vecPositions.push_back(mActualPos);
				m_vecGrenades.at(mIterator).m_lastThinkTime = clock();
			};

			mPosCount = m_vecGrenades.at(mIterator).m_vecPositions.size();

			for (mPosIterator = 0; mPosIterator < mPosCount; mPosIterator++)
			{
				if (mPosIterator >= (mPosCount - 1))
					break;

				mOlderPos = m_vecGrenades.at(mIterator).m_vecPositions.at(mPosIterator);
				mOldPos = m_vecGrenades.at(mIterator).m_vecPositions.at(mPosIterator + 1);

				if (D::WorldToScreen(mOlderPos, mOlderScreenPos) && D::WorldToScreen(mOldPos, mOldScreenPos))
					D::DrawLine(INT(mOlderScreenPos.x), INT(mOlderScreenPos.y), INT(mOldScreenPos.x), INT(mOldScreenPos.y), m_vecGrenades.at(mIterator).m_Color);
			};
		};
	};

	FORCEINLINE bool IsGrenadeQueued(INT grenadeId)
	{
		static INT mGrenadesCount = 0, mIterator = 0;

		mGrenadesCount = m_vecGrenades.size();
		if (mGrenadesCount == 0)
			return false;

		for (mIterator = 0; mIterator < mGrenadesCount; mIterator++)
		{
			if (m_vecGrenades.at(mIterator).m_Grenade == grenadeId)
				return true;
		};

		return false;
	};

	FORCEINLINE VOID Clear(VOID)
	{
		m_vecGrenades.clear();
	};

	std::vector <CGrenTrajObj> m_vecGrenades;
};

template <typename Type> class CFGVal
{
public:

	FORCEINLINE CFGVal(std::string Cat, std::string Key, Type * pVal)
	{
		m_Cat = Cat;
		m_Key = Key;
		m_pVal = pVal;
	};

	std::string m_Cat, m_Key;
	Type * m_pVal;
};

enum : INT
{
	HITBOX_HEAD = 0,
	HITBOX_NECK,
	HITBOX_LOWER_NECK,
	HITBOX_PELVIS,
	HITBOX_BODY,
	HITBOX_THORAX,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
};

class CGlowObjMan
{
public:

	class GlowObjDef_T
	{
	public:

		VOID Set(Color Col)
		{
			m_vecGlowCol = Col.ToVec3DBase(), \
				m_flGlowAlpha = (((BYTE)(Vars.Visuals.iGlowAlphaIntensity)) / 255.f), \
				m_bRenderWhenOccl = true, m_bRenderWhenUnoccl = false, \
				m_flBloomAmt = Vars.Visuals.fGlowBloomAmount;
		};

		VOID UnSet(VOID)
		{
			m_flGlowAlpha = m_flBloomAmt = 0.f, m_bRenderWhenOccl = m_bRenderWhenUnoccl = false;
		};

		Entity_T * m_pEntity;
		Vector m_vecGlowCol;
		FLOAT m_flGlowAlpha;
		CHAR m_szUnkA[4];
		FLOAT m_fUnkA;
		FLOAT m_flBloomAmt;
		FLOAT m_fUnkB;
		bool m_bRenderWhenOccl;
		bool m_bRenderWhenUnoccl;
		bool m_bFullBloomRender;
		CHAR m_szUnkB[1];
		INT m_nFullBloomStencilTestVal;
		INT m_iUnkA;
		INT m_nSplitScreenSlot;
		INT m_nNextFreeSlot;
	};

	GlowObjDef_T * m_pGlowObjDefs;

	INT m_nMaxSize;
	INT m_nPad;
	INT m_nSize;
};

class IGlobalVarsBase
{
public:

	FLOAT m_fRealTime;
	INT m_iFrameCount;
	FLOAT m_fAbsFrameTime;
	FLOAT m_fAbsFrameStartTime;
	FLOAT m_fCurTime;
	FLOAT m_fFrameTime;
	INT m_iMaxClients;
	INT m_iTickCount;
	FLOAT m_fIntervPerTick;
	FLOAT m_fInterpAmt;
	INT m_iSimTicks;
	INT m_iNetworkProto;
	PVOID m_pSaveData;
	bool m_bClient;
	bool m_bRemClient;
	INT m_iTStampNetworkBase;
	INT m_iTStampRandomWnd;
};

FORCEINLINE CONST Color ClrByOption(INT Option)
{
	if (Option < 0)
		return g_White;

	switch (Option)
	{
	case NULL: return g_White;
	case 1: return g_Green;
	case 2: return g_Red;
	case 3: return g_Blue;
	case 4: return g_LBlue;
	case 5: return g_Grey;
	case 6: return g_Purple;
	case 7: return g_Cyan;
	case 8: return g_Orange;
	case 9: return g_DGreen;
	case 10: return g_Magenta;
	case 11: return g_DGrey;
	case 12: return g_Black;
	};

	return g_Gold;
};
