
#include "Misc.h"

Hattrick g_Hattrick;
CreateMoveFn oCreateMove = NULL;
Entity_T * g_pTopEnemy = NULL;
Vector g_vecTopHBox;
bool g_bFrozen = false;
time_t g_roundStartTime = 0;

#if defined(ENABLE_RAGE_BOT) && ENABLE_RAGE_BOT == 1

CFM g_FM;
CRBOT * g_pRageBOT = new CRBOT;

#endif

bool g_bLoaded = false;
CHAR g_szClanTag[256] = {};
time_t g_lastDoPlantMsg = 0;
CHAR g_szFormattedClanTag[256] = {};
CHAR g_Phrases[256][256];
time_t g_bombDropTime = 0;
INT g_PhrasesSize = 0;
FLOAT g_fMaxWallLen = 0.f;
PaintTraverseFn oPaintTraverse = NULL;
UINT g_uiDrawPanel = NULL;
time_t g_timeLastEventStamp = NULL;
CONST CHAR * g_pszPanelName = NULL;
Json::StyledWriter g_JsonStyledWriter;
Json::Reader g_JsonReader;
Json::Value g_JsonAimWaveCFGTree;
bool m_bVis[128] = { false };
bool g_bRoundEnded = true;
clock_t g_saveTime = 0;
clock_t g_loadTime = 0;
ResetFn oReset;
EndSceneFn oEndScene;
WNDPROC g_pOldWndProc = NULL;
BOOL g_bCheckForLastEnemy = FALSE;
CNetVarMan * g_pNetVarsMan = new CNetVarMan;
INT g_nStyle = -1;
Color g_Grey = Color::Grey();
Color g_LBlue = Color::LBlue();
Color g_Red = Color::Red();
Color g_Green = Color::Green();
Color g_DGrey = Color::DGrey();
Color g_Black = Color::Black();
Color g_Blue = Color::Blue();
Color g_White = Color::White();
Color g_Purple = Color::Purple();
Color g_Cyan = Color::Cyan();
Color g_Magenta = Color::Magenta();
Color g_DGreen = Color::DGreen();
bool g_bBombPlanted = false;
Color g_Orange = Color::Orange();
Color g_Gold = Color::Gold();
CGrenTraj g_nadeTraj;
CGrenTraj * g_pNadeTraj = &g_nadeTraj;
COffsets g_Offsets;
CConfig * Config = new CConfig;
Variables Vars;
ServerRankRevealAllFn ServerRankRevealAllEx = NULL;
FireEventClientSide fnOriginalFireEventClientSide = NULL;
bool g_bOffline = true;
CLBOT * g_pLegitBOT = new CLBOT;
CVisuals * g_pVisuals = new CVisuals;
FLOAT g_fBestFOV = 0;
INT g_iTopLBOTEnemy = -1;
FLOAT g_fDeltaTime = 0;
FLOAT g_fCurAimTime = 0;
SIZE_T g_uTraces = 0;
Vector g_vecLBOTHBox;
FLOAT g_fFoV = 0;
Entity_T * g_pTopLBOTEnemy = NULL;
INT g_iEnemyTeam = 0;
QAngle g_angSelfView;
time_t g_LoadTime = 0;
bool G::bAlive = false;
Entity_T * G::pSelf = NULL;
CUserCmd * G::pCmd = NULL;
HWND G::pWnd = NULL;
INT G::iTeam = 0;
INT G::iFlags = 0;
bool G::Keys[256] = { false };
bool G::bD3DOK = false;
INT G::iWeapId = 0;
bool G::bGun = false;
clock_t g_zoomTime = NULL;
bool G::bEmpty = false;
Weapon_T * G::pWeap = NULL;
VTHook * H::VPanel = NULL;
VTHook * H::CLMode = NULL;
VTHook * H::CL = NULL;
VTHook * H::D3D = NULL;
IGameEventManager2 * I::EvntsMgr = NULL;
IBaseClientDll * I::Client = NULL;
IClientModeShared * I::CLMode = NULL;
IEnts * I::Ents = NULL;
IEngineClient * I::Engine = NULL;
IEngineTrace * I::EngTrace = NULL;
IGlobalVarsBase * I::Globals = NULL;
ISurface * I::Surf = NULL;
IVPanel * I::VPanel = NULL;
IVModelInfo * I::MDLInfo = NULL;
IInputSystem * I::InputSys = NULL;

VOID JWPPS(CONST CHAR * pCategory, CONST CHAR * pName, CONST CHAR * pValue)
{
	static CHAR szPrepared[256] = {};
	sprintf(szPrepared, XorString("%s.%s"), pCategory, pName);
	g_JsonAimWaveCFGTree[szPrepared] = pValue;
};

VOID JGPPS(CONST CHAR * pCategory, CONST CHAR * pName, PCHAR pValue, INT maxSize)
{
	static CHAR szPrepared[256] = {};
	static PCHAR pszData = {};
	sprintf(szPrepared, XorString("%s.%s"), pCategory, pName);
	pszData = (PCHAR)g_JsonAimWaveCFGTree[szPrepared].asCString();
	_snprintf(pValue, maxSize, XorString("%s"), pszData && *pszData ? pszData : XorString(""));
};

bool __fastcall FireEventClientSideThink(PVOID ECX, PVOID, IGameEvent * pEvent)
{
	static CONST CHAR * pszName = NULL, *pszWeapon = NULL;
	static CHAR szData[256] = {}, szWeapon[64] = {};
	static time_t tNow = 0, tStamp = 0;
	static INT iVictim = 0, iAttacker = 0, iPlayer = 0, iTeam = 0, iTerroristsAlive = 0;
	static bool bFlip = false, bHS = false;
	static Vector vecOrigin;
	static Entity_T * pPlayer = NULL;
	static PlrInfo_T PlrInfo, SelfInfo;
	static std::string Weapon = {}, Name = {};

	tNow = time(NULL);

	if (!(G::pSelf = GetSelf()) || !pEvent || !(pszName = pEvent->GetName()) || *pszName == NULL)
		return fnOriginalFireEventClientSide(ECX, pEvent);

	else if (_stricmp(pszName, XorString("CS_Game_Disconnected")) == NULL)
	{
		I::Engine->ClientCmd_Unrestricted(XorString("bind tab +showscores; bind y messagemode; bind u messagemode2; cl_mouseenable 1"), 0), \
			g_fMaxWallLen = 0.f, g_bFrozen = G::bAlive = G::bGun = g_bBombPlanted = false, g_bOffline = G::bEmpty = g_bRoundEnded = true, G::iTeam = G::iFlags = NULL, g_pNadeTraj->Clear();

		return fnOriginalFireEventClientSide(ECX, pEvent);
	}

	else if (_stricmp(pszName, XorString("Player_Connect_Full")) == NULL || _stricmp(pszName, XorString("Player_Full_Connect")) == NULL || \
		_stricmp(pszName, XorString("Player_Connected_Full")) == NULL || _stricmp(pszName, XorString("Player_Full_Connected")) == NULL)
	{
		I::Engine->ClientCmd_Unrestricted(XorString("bind tab +showscores; bind y messagemode; bind u messagemode2"), 0), \
			g_fMaxWallLen = 0.f, g_bOffline = g_bFrozen = g_bBombPlanted = false, g_bRoundEnded = true, g_pNadeTraj->Clear();

		return fnOriginalFireEventClientSide(ECX, pEvent);
	}

	else if (_stricmp(pszName, XorString("Bomb_Defuse")) == 0 || _stricmp(pszName, XorString("Bomb_Explode")) == 0 || \
		_stricmp(pszName, XorString("Bomb_Defused")) == 0 || _stricmp(pszName, XorString("Bomb_Exploded")) == 0 || \
		_stricmp(pszName, XorString("Round_End")) == 0 || _stricmp(pszName, XorString("Round_End_Post")) == 0)
	{
		g_bRoundEnded = true;

		if (Vars.Misc.bSpam)
		{
			if (_stricmp(pszName, XorString("Round_End")) == 0)
			{
				if (RandomINT(0, 1) == 0 && pEvent->GetInt(XorString("winner")) > 0)
				{
					if (pEvent->GetInt(XorString("winner")) == G::iTeam)
					{
						switch (RandomINT(0, 5))
						{
						case 0: sprintf(szData, XorString("nS+")); break;
						case 1: sprintf(szData, XorString("very good round")); break;
						case 2: sprintf(szData, XorString("splendid round+")); break;
						case 3: sprintf(szData, XorString("good round+")); break;
						case 4: sprintf(szData, XorString("nice Round man")); break;
						case 5: sprintf(szData, XorString("cool round")); break;
						};
					}

					else
					{
						switch (RandomINT(0, 5))
						{
						case 0: sprintf(szData, XorString("i fuck all ur family")); break;
						case 1: sprintf(szData, XorString("u all 5 hack man?")); break;
						case 2: sprintf(szData, XorString("full hack pre-mates?")); break;
						case 3: sprintf(szData, XorString("why hack?")); break;
						case 4: sprintf(szData, XorString("go away guys..")); break;
						case 5: sprintf(szData, XorString("nice man all 5 hackers..")); break;
						};
					};

					sprintf(g_Phrases[g_PhrasesSize++], szData);
				};
			};
		};

		g_bBombPlanted = false;
	}

	else if (_stricmp(pszName, XorString("Bomb_Plant")) == 0 || _stricmp(pszName, XorString("Bomb_Planted")) == 0)
	{
		g_bBombPlanted = true;
	}

	else if (_stricmp(pszName, XorString("Round_Start")) == NULL || _stricmp(pszName, XorString("Round_Pre_Start")) == NULL || \
		_stricmp(pszName, XorString("Round_Start_Pre")) == NULL || _stricmp(pszName, XorString("Pre_Round_Start")) == NULL || \
		_stricmp(pszName, XorString("Player_Spawn")) == NULL || _stricmp(pszName, XorString("Player_Spawned")) == NULL)
	{
		g_pNadeTraj->Clear();

		if (_stricmp(pszName, XorString("Round_Start")) == NULL || _stricmp(pszName, XorString("Round_Pre_Start")) == NULL || \
			_stricmp(pszName, XorString("Round_Start_Pre")) == NULL || _stricmp(pszName, XorString("Pre_Round_Start")) == NULL)
		{
			g_roundStartTime = tNow, g_bBombPlanted = g_bRoundEnded = false;
		};

		if (tNow - tStamp > 0)
		{
			if (bFlip)
			{
				if (!Vars.Misc.bDoNotEditRadarScale)
					I::Engine->ClientCmd_Unrestricted(XorString("cl_radar_scale .4; cl_radar_always_centered 0"), 0);

				if (!Vars.Misc.bDoNotEnforceBestPingLagSettings)
					I::Engine->ClientCmd_Unrestricted(XorString("cl_interp_ratio 0; cl_interp 0"), 0);
			}

			else
			{
				if (!Vars.Misc.bDoNotEnforceBestPingLagSettings)
				{
					I::Engine->ClientCmd_Unrestricted(XorString("rate 786432; cl_cmdrate 128; cl_updaterate 128"), 0), \
						I::Engine->ClientCmd_Unrestricted(XorString("mm_dedicated_search_maxping 350; fps_max 8192; fps_max_menu 8192"), 0);
				};
			};

			tStamp = tNow, bFlip = !bFlip;
		};
	}

	else if (Vars.Misc.bSpam && NULL == _stricmp(pszName, XorString("Player_Death")))
	{
		iVictim = pEvent->GetInt(XorString("userid"));
		iAttacker = pEvent->GetInt(XorString("attacker"));
		pszWeapon = pEvent->GetString(XorString("weapon"));
		bHS = pEvent->GetInt(XorString("headshot")) ? true : false;

		g_bCheckForLastEnemy = TRUE;

		if (iAttacker > 0 && iVictim > 0 && iAttacker != iVictim)
		{
			I::Engine->GetPlayerInfo(I::Engine->GetSelf(), &SelfInfo);

			Weapon = pszWeapon, replaceAll(Weapon, XorString("weapon_"), "");
			sprintf(szWeapon, XorString("%s"), Weapon.c_str()), pszWeapon = &szWeapon[0];

			if (iAttacker == SelfInfo.userid)
			{
				for (iPlayer = 0; iPlayer <= I::Globals->m_iMaxClients; iPlayer++)
				{
					pPlayer = I::Ents->GetClEntity(iPlayer);

					if (!pPlayer || pPlayer == G::pSelf || (iTeam = pPlayer->GetTeam()) < 2 || iTeam > 3)
						continue;

					I::Engine->GetPlayerInfo(pPlayer->m_iId, &PlrInfo);

					if (PlrInfo.userid != iVictim)
						continue;

					g_timeLastEventStamp = tNow;

					if (bHS && RandomINT(0, 8) == 0)
					{
						switch (RandomINT(0, 4))
						{
						case 0: sprintf(szData, XorString("n1 HS, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 1: sprintf(szData, XorString("dat HS, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 2: sprintf(szData, XorString("%s, ns HS"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 3: sprintf(szData, XorString("nize HS %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 4: sprintf(szData, XorString("fkn HS, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						};
						
						sprintf(g_Phrases[g_PhrasesSize++], szData);
						break;
					};

					if (pszWeapon[0] == 'm' && pszWeapon[1] == '4' && RandomINT(0, 5) == 0)
					{
						switch (RandomINT(0, 3))
						{
						case 0: sprintf(szData, XorString("n1 m4 kill, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 1: sprintf(szData, XorString("m4 skills, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 2: sprintf(szData, XorString("M4 expert, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 3: sprintf(szData, XorString("ns M4, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						};

						sprintf(g_Phrases[g_PhrasesSize++], szData);
						break;
					};

					if (pszWeapon[0] == 'a' && pszWeapon[1] == 'k' && RandomINT(0, 5) == 0)
					{
						switch (RandomINT(0, 3))
						{
						case 0: sprintf(szData, XorString("n1 AK kill, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 1: sprintf(szData, XorString("ak47 skills, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 2: sprintf(szData, XorString("Ak47 expert, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 3: sprintf(szData, XorString("ns Ak47, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						};

						sprintf(g_Phrases[g_PhrasesSize++], szData);
						break;
					};

					if (pszWeapon[0] == 'a' && pszWeapon[1] == 'w' && RandomINT(0, 5) == 0)
					{
						switch (RandomINT(0, 3))
						{
						case 0: sprintf(szData, XorString("ofc AWP, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 1: sprintf(szData, XorString("ns AWP %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 2: sprintf(szData, XorString("u go AWP, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 3: sprintf(szData, XorString("n1 AWP trigger, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						};

						sprintf(g_Phrases[g_PhrasesSize++], szData);
						break;
					};

					if (pszWeapon[0] == 't' && pszWeapon[1] == 'a')
					{
						switch (RandomINT(0, 4))
						{
						case 0: sprintf(szData, XorString("got rekt, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 1: sprintf(szData, XorString("REKT, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 2: sprintf(szData, XorString("enjoyed dat x27, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 3: sprintf(szData, XorString("ns x27 %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 4: sprintf(szData, XorString("WTF %s tased ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						};

						sprintf(g_Phrases[g_PhrasesSize++], szData);
						break;
					};

					if (pszWeapon[0] == 'k' && pszWeapon[1] == 'n')
					{
						switch (RandomINT(0, 4))
						{
						case 0: sprintf(szData, XorString("got rekt, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 1: sprintf(szData, XorString("REKT, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 2: sprintf(szData, XorString("enjoyed dat Kniv, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 3: sprintf(szData, XorString("ns Knif %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 4: sprintf(szData, XorString("WTF %s sliced ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						};

						sprintf(g_Phrases[g_PhrasesSize++], szData);
						break;
					};

					PrepareSpamString(szData, &PlrInfo);
					sprintf(g_Phrases[g_PhrasesSize++], szData);

					break;
				};
			}

			else if (iVictim == SelfInfo.userid)
			{
				for (iPlayer = 0; iPlayer <= I::Globals->m_iMaxClients; iPlayer++)
				{
					pPlayer = I::Ents->GetClEntity(iPlayer);

					if (!pPlayer || pPlayer == G::pSelf || (iTeam = pPlayer->GetTeam()) < 2 || iTeam > 3)
						continue;

					I::Engine->GetPlayerInfo(pPlayer->m_iId, &PlrInfo);

					if (PlrInfo.userid != iAttacker)
						continue;

					g_timeLastEventStamp = tNow;

					if (pPlayer->GetHealth() <= 25 && RandomINT(0, 1) == 0 && pPlayer->GetHealth() > 0)
					{
						switch (RandomINT(0, 4))
						{
						case 0: sprintf(szData, XorString("%s%d HP"), "-", 100 - pPlayer->GetHealth()); break;
						case 1: sprintf(szData, XorString("%s%d"), "-", 100 - pPlayer->GetHealth()); break;
						case 2: sprintf(szData, XorString("%s is %d HP"), ShortenName(std::string(PlrInfo.name)).c_str(), pPlayer->GetHealth()); break;
						case 3: sprintf(szData, XorString("%s is low (%d)"), ShortenName(std::string(PlrInfo.name)).c_str(), pPlayer->GetHealth()); break;
						case 4: sprintf(szData, XorString("fuck u %s, %s%d"), ShortenName(std::string(PlrInfo.name)).c_str(), "-", 100 - pPlayer->GetHealth()); break;
						};

						sprintf(g_Phrases[g_PhrasesSize++], szData);
						break;
					};

					if (bHS && RandomINT(0, 8) == 0)
					{
						switch (RandomINT(0, 4))
						{
						case 0: sprintf(szData, XorString("n1 HS, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 1: sprintf(szData, XorString("dat HS, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 2: sprintf(szData, XorString("%s, ns HS"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 3: sprintf(szData, XorString("nize HS %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 4: sprintf(szData, XorString("fkn HS, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						};

						sprintf(g_Phrases[g_PhrasesSize++], szData);
						break;
					};

					if (pszWeapon[0] == 'm' && pszWeapon[1] == '4' && RandomINT(0, 5) == 0)
					{
						switch (RandomINT(0, 4))
						{
						case 0: sprintf(szData, XorString("n1 m4 kill, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 1: sprintf(szData, XorString("m4 skills, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 2: sprintf(szData, XorString("who teached u M4, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 3: sprintf(szData, XorString("M4 expert, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 4: sprintf(szData, XorString("ns M4, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						};

						sprintf(g_Phrases[g_PhrasesSize++], szData);
						break;
					};

					if (pszWeapon[0] == 'a' && pszWeapon[1] == 'k' && RandomINT(0, 5) == 0)
					{
						switch (RandomINT(0, 4))
						{
						case 0: sprintf(szData, XorString("n1 AK kill, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 1: sprintf(szData, XorString("ak47 skills, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 2: sprintf(szData, XorString("who teached u AK47, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 3: sprintf(szData, XorString("Ak47 expert, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 4: sprintf(szData, XorString("ns Ak47, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						};

						sprintf(g_Phrases[g_PhrasesSize++], szData);
						break;
					};

					if (pszWeapon[0] == 'a' && pszWeapon[1] == 'w' && RandomINT(0, 5) == 0)
					{
						switch (RandomINT(0, 4))
						{
						case 0: sprintf(szData, XorString("ofc AWP, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 1: sprintf(szData, XorString("how $$ for AWP, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 2: sprintf(szData, XorString("ns AWP %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 3: sprintf(szData, XorString("u go AWP, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 4: sprintf(szData, XorString("n1 AWP trigger, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						};

						sprintf(g_Phrases[g_PhrasesSize++], szData);
						break;
					};

					if (pszWeapon[0] == 't' && pszWeapon[1] == 'a')
					{
						switch (RandomINT(0, 4))
						{
						case 0: sprintf(szData, XorString("i got rekt, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 1: sprintf(szData, XorString("REKT, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 2: sprintf(szData, XorString("i enjoyed dat x27, %s !"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 3: sprintf(szData, XorString("ns x27 %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 4: sprintf(szData, XorString("WTF %s tased me ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						};

						sprintf(g_Phrases[g_PhrasesSize++], szData);
						break;
					};

					if (pszWeapon[0] == 'k' && pszWeapon[1] == 'n')
					{
						switch (RandomINT(0, 4))
						{
						case 0: sprintf(szData, XorString("i got rekt, %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 1: sprintf(szData, XorString("REKT, %s ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 2: sprintf(szData, XorString("enjoyed dat Kniv, %s !"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 3: sprintf(szData, XorString("ns Knif %s"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						case 4: sprintf(szData, XorString("WTF %s sliced me ?"), ShortenName(std::string(PlrInfo.name)).c_str()); break;
						};

						sprintf(g_Phrases[g_PhrasesSize++], szData);
						break;
					};

					PrepareSpamString(szData, &PlrInfo);
					sprintf(g_Phrases[g_PhrasesSize++], szData);

					break;
				};
			};
		};
	};

	if (Vars.Misc.bSpam && RandomINT(0, 1) == 0 && (NULL == _stricmp(pszName, XorString("Bomb_Plant")) || NULL == _stricmp(pszName, XorString("Bomb_Planted"))))
	{
		switch (RandomINT(0, 4))
		{
		case 0: sprintf(szData, XorString("u planted bomb already man?")); break;
		case 1: sprintf(szData, XorString("why u plant+?")); break;
		case 2: sprintf(szData, XorString("whos gonna defuse now?")); break;
		case 3: sprintf(szData, XorString("man go defus dat bomb ?")); break;
		case 4: sprintf(szData, XorString("n1 plant+")); break;
		};

		sprintf(g_Phrases[g_PhrasesSize++], szData);
		g_timeLastEventStamp = tNow;
	}

	if (Vars.Misc.bSpam && RandomINT(0, 2) == 0 && (NULL == _stricmp(pszName, XorString("Bomb_Drop")) || NULL == _stricmp(pszName, XorString("Bomb_Dropped"))))
	{
		if (tNow - g_bombDropTime >= 16 && tNow - g_roundStartTime >= 24)
		{
			switch (RandomINT(0, 4))
			{
			case 0: sprintf(szData, XorString("man why u drop dat bomb?")); break;
			case 1: sprintf(szData, XorString("stop dropping the bomb, LOL")); break;
			case 2: sprintf(szData, XorString("man dont drop the bomb go plant it")); break;
			case 3: sprintf(szData, XorString("stop dropping that bomb")); break;
			case 4: sprintf(szData, XorString("go plant the bomb dont drop it")); break;
			};

			sprintf(g_Phrases[g_PhrasesSize++], szData);
			g_timeLastEventStamp = tNow, g_bombDropTime = tNow;
		};
	}

	if (Vars.Misc.bSpam && (NULL == _stricmp(pszName, XorString("Player_Disconnect"))))
	{
		iVictim = pEvent->GetInt(XorString("userid"));

		if (iVictim >= 0)
		{
			for (iPlayer = 0; iPlayer <= I::Globals->m_iMaxClients; iPlayer++)
			{
				pPlayer = I::Ents->GetClEntity(iPlayer);

				if (pPlayer && pPlayer != G::pSelf)
				{
					I::Engine->GetPlayerInfo(pPlayer->m_iId, &PlrInfo);

					if (iVictim == PlrInfo.userid)
					{
						switch (RandomINT(0, 4))
						{
						case 0: sprintf(szData, XorString("%s, n1 rage quit"), ShortenName(PlrInfo.name).c_str()); break;
						case 1: sprintf(szData, XorString("LOL, %s rage quitted"), ShortenName(PlrInfo.name).c_str()); break;
						case 2: sprintf(szData, XorString("why %s rage quitted?"), ShortenName(PlrInfo.name).c_str()); break;
						case 3: sprintf(szData, XorString("%s rage quit?"), ShortenName(PlrInfo.name).c_str()); break;
						case 4: sprintf(szData, XorString("nice, %s rage quitted"), ShortenName(PlrInfo.name).c_str()); break;
						};

						sprintf(g_Phrases[g_PhrasesSize++], szData);
						g_timeLastEventStamp = tNow;

						break;
					};
				};
			};
		};
	};

	if (Vars.Misc.bSpam && (NULL == _stricmp(pszName, XorString("Player_Connect"))))
	{
		iVictim = pEvent->GetInt(XorString("userid"));

		if (iVictim >= 0)
		{
			for (iPlayer = 0; iPlayer <= I::Globals->m_iMaxClients; iPlayer++)
			{
				pPlayer = I::Ents->GetClEntity(iPlayer);

				if (pPlayer && pPlayer != G::pSelf)
				{
					I::Engine->GetPlayerInfo(pPlayer->m_iId, &PlrInfo);

					if (iVictim == PlrInfo.userid)
					{
						switch (RandomINT(0, 4))
						{
						case 0: sprintf(szData, XorString("hi, %s"), ShortenName(PlrInfo.name).c_str()); break;
						case 1: sprintf(szData, XorString("welcome back, %s"), ShortenName(PlrInfo.name).c_str()); break;
						case 2: sprintf(szData, XorString("WB, %s"), ShortenName(PlrInfo.name).c_str()); break;
						case 3: sprintf(szData, XorString("hello, %s"), ShortenName(PlrInfo.name).c_str()); break;
						case 4: sprintf(szData, XorString("ПРИВЕТ, %s"), ShortenName(PlrInfo.name).c_str()); break;
						};

						sprintf(g_Phrases[g_PhrasesSize++], szData);
						g_timeLastEventStamp = tNow;

						break;
					};
				};
			};
		};
	};

	if (Vars.Misc.bSpam && RandomINT(0, 1) == 0 && (NULL == _stricmp(pszName, XorString("Bomb_Defuse")) || NULL == _stricmp(pszName, XorString("Bomb_Defused"))))
	{
		g_bRoundEnded = true;

		switch (RandomINT(0, 4))
		{
		case 0: sprintf(szData, XorString("n1 defuse+")); break;
		case 1: sprintf(szData, XorString("splendid defuse man i ❤ it")); break;
		case 2: sprintf(szData, XorString("this defuse will go on YT™")); break;
		case 3: sprintf(szData, XorString("dat1 was Ninja?")); break;
		case 4: sprintf(szData, XorString("ninja defused+?")); break;
		};

		sprintf(g_Phrases[g_PhrasesSize++], szData);
		g_timeLastEventStamp = tNow;
	};

	if (Vars.Misc.bSpam && (NULL == _stricmp(pszName, XorString("CS_Win_Panel_Match")) || NULL == _stricmp(pszName, XorString("Announce_Phase_End")) || NULL == _stricmp(pszName, XorString("CS_Intermission")) || NULL == _stricmp(pszName, XorString("Start_HalfTime"))))
	{
		g_bRoundEnded = true;

		switch (RandomINT(0, 5))
		{
		case 0: sprintf(szData, XorString("n1 game++")); break;
		case 1: sprintf(szData, XorString("i ❤ it, GG")); break;
		case 2: sprintf(szData, XorString("dis game will go on YT™")); break;
		case 3: sprintf(szData, XorString("you will all get OW np")); break;
		case 4: sprintf(szData, XorString("u all get VAC dont worry")); break;
		case 5: sprintf(szData, XorString("hadi kazı mezarını")); break;
		};

		sprintf(g_Phrases[g_PhrasesSize++], szData);
		g_timeLastEventStamp = tNow;
	}

	if (Vars.Misc.bSpam && tNow - g_lastDoPlantMsg >= 150 && !g_bBombPlanted && !g_bRoundEnded && tNow - g_roundStartTime >= 80 && RandomINT(0, 2) == 0)
	{
		iTerroristsAlive = 0;

		for (iPlayer = 0; iPlayer <= I::Globals->m_iMaxClients; iPlayer++)
		{
			pPlayer = I::Ents->GetClEntity(iPlayer);

			if (pPlayer && pPlayer != G::pSelf && pPlayer->GetHealth() > 0 && pPlayer->GetTeam() == 2)
				iTerroristsAlive++;

			if (iTerroristsAlive > 0)
				break;
		};

		if (iTerroristsAlive > 0)
		{
			switch (RandomINT(0, 5))
			{
			case 0: sprintf(szData, XorString("mby go plant that bomb?")); break;
			case 1: sprintf(szData, XorString("no bomb plant yet?")); break;
			case 2: sprintf(szData, XorString("man stop camping, go plant")); break;
			case 3: sprintf(szData, XorString("when u plant?+")); break;
			case 4: sprintf(szData, XorString("i wait for plant+")); break;
			case 5: sprintf(szData, XorString("man time? plant pls")); break;
			};

			sprintf(g_Phrases[g_PhrasesSize++], szData);
			g_timeLastEventStamp = tNow, g_lastDoPlantMsg = tNow;
		};
	};

	if (Vars.Misc.bSpam && NULL == _stricmp(pszName, XorString("HEGrenade_Detonate")))
	{
		vecOrigin = Vector(pEvent->GetInt(XorString("x")), pEvent->GetInt(XorString("y")), pEvent->GetInt(XorString("z")));

		for (iPlayer = 0; iPlayer <= I::Globals->m_iMaxClients; iPlayer++)
		{
			pPlayer = I::Ents->GetClEntity(iPlayer);

			if (pPlayer && pPlayer != G::pSelf && (iTeam = pPlayer->GetTeam()) > 1 && iTeam < 3 && iTeam != G::iTeam && \
				pPlayer->GetHealth() > 0 && (vecOrigin - pPlayer->GetOrigin()).Length() < 320.f)
			{
				I::Engine->GetPlayerInfo(pPlayer->m_iId, &PlrInfo);

				Name = ShortenName(PlrInfo.name);

				if (RandomINT(0, 1) == 0 && pPlayer->GetHealth() <= 25)
				{
					switch (RandomINT(0, 2))
					{
					case 0: sprintf(szData, XorString("%s is low, %d HP"), Name.c_str(), pPlayer->GetHealth()); break;
					case 1: sprintf(szData, XorString("%s is %d HP"), Name.c_str(), pPlayer->GetHealth()); break;
					case 2: sprintf(szData, XorString("%s, -%d HP"), Name.c_str(), 100 - pPlayer->GetHealth()); break;
					};
				}

				else
				{
					switch (RandomINT(0, 5))
					{
					case 0: sprintf(szData, XorString("u enjoyed dat, %s ?"), Name.c_str()); break;
					case 1: sprintf(szData, XorString("u ate it, %s ?"), Name.c_str()); break;
					case 2: sprintf(szData, XorString("u got destructed, %s ?"), Name.c_str()); break;
					case 3: sprintf(szData, XorString("%s, enjoyed dat grenade?"), Name.c_str()); break;
					case 4: sprintf(szData, XorString("u liked it, %s ?"), Name.c_str()); break;
					case 5: sprintf(szData, XorString("u liked the grenade, %s ?"), Name.c_str()); break;
					};
				};

				sprintf(g_Phrases[g_PhrasesSize++], szData);
				g_timeLastEventStamp = tNow;
			};
		};
	};

	return fnOriginalFireEventClientSide(ECX, pEvent);
};

VOID SetupInterfaces(VOID)
{
	PDWORD pOriginalGameEventMgrVMT = NULL, pNewGameEventMgrVMT = NULL, *pGameEventMgrVMT = NULL;
	SIZE_T dwGameEventMgrVMTSize = NULL;

	while (true)
	{
		Sleep(250);

		Hattrick::ModuleTuple * pClient = g_Hattrick.ProcessModule(XorString("client.dll"));

		if (pClient)
			break;

		Sleep(250);
	};

	while (true)
	{
		Sleep(250);

		Hattrick::ModuleTuple * pEngine = g_Hattrick.ProcessModule(XorString("engine.dll"));

		if (pEngine)
		{
			break;
		};

		Sleep(250);
	};

	while (true)
	{
		Sleep(250);

		Hattrick::ModuleTuple * pVGUI2 = g_Hattrick.ProcessModule(XorString("vgui2.dll"));

		if (pVGUI2)
		{
			break;
		};

		Sleep(250);
	};

	while (true)
	{
		Sleep(250);

		Hattrick::ModuleTuple * pInputSystem = g_Hattrick.ProcessModule(XorString("inputsystem.dll"));

		if (pInputSystem)
		{
			break;
		};

		Sleep(250);
	};

	while (true)
	{
		Sleep(250);

		Hattrick::ModuleTuple * pVGUIMatSurface = g_Hattrick.ProcessModule(XorString("vguimatsurface.dll"));

		if (pVGUIMatSurface)
		{
			break;
		};

		Sleep(250);
	};

	while (true)
	{
		Sleep(250);

		Hattrick::ModuleTuple * pVPhysics = g_Hattrick.ProcessModule(XorString("vphysics.dll"));

		if (pVPhysics)
		{
			break;
		};

		Sleep(250);
	};

	I::EvntsMgr = CaptureInterface<IGameEventManager2>(XorString("engine.dll"), XorString("GAMEEVENTSMANAGER002"));
	I::Client = CaptureInterface<IBaseClientDll>(XorString("client.dll"), XorString("VClient018"));
	I::CLMode = **(IClientModeShared ***)((*(PDWORD *)I::Client)[10] + 0x00000005);
	I::Ents = CaptureInterface<IEnts>(XorString("client.dll"), XorString("VClientEntityList003"));
	I::Engine = CaptureInterface<IEngineClient>(XorString("engine.dll"), XorString("VEngineClient014"));
	I::EngTrace = CaptureInterface<IEngineTrace>(XorString("engine.dll"), XorString("EngineTraceClient004"));
	I::Globals = **(IGlobalVarsBase ***)((*(PDWORD *)I::Client)[0] + 0x0000001B);
	I::Surf = CaptureInterface<ISurface>(XorString("vguimatsurface.dll"), XorString("VGUI_Surface031"));
	I::VPanel = CaptureInterface<IVPanel>(XorString("vgui2.dll"), XorString("VGUI_Panel009"));
	I::MDLInfo = CaptureInterface<IVModelInfo>(XorString("engine.dll"), XorString("VModelInfoClient004"));
	I::InputSys = CaptureInterface<IInputSystem>(XorString("inputsystem.dll"), XorString("InputSystemVersion001"));

	pGameEventMgrVMT = (PDWORD *)I::EvntsMgr, pOriginalGameEventMgrVMT = *pGameEventMgrVMT;
	while ((PDWORD)(*pGameEventMgrVMT)[dwGameEventMgrVMTSize]) dwGameEventMgrVMTSize++;
	pNewGameEventMgrVMT = new DWORD[dwGameEventMgrVMTSize], \
		CopyMemory(pNewGameEventMgrVMT, pOriginalGameEventMgrVMT, sizeof DWORD * dwGameEventMgrVMTSize), \
		pNewGameEventMgrVMT[9] = (DWORD)FireEventClientSideThink, \
		fnOriginalFireEventClientSide = (FireEventClientSide)pOriginalGameEventMgrVMT[9], \
		*pGameEventMgrVMT = pNewGameEventMgrVMT;
};

Entity_T * GetSelf(VOID)
{
	static Entity_T * pEntity = NULL;

	if (!I::Engine->IsInGame() || !I::Engine->IsConnected() || !(pEntity = I::Ents->GetClEntity(I::Engine->GetSelf())))
	{
		g_fMaxWallLen = 0.f, g_pNadeTraj->Clear(), g_bOffline = G::bEmpty = true, G::bAlive = G::bGun = false, G::iTeam = G::iFlags = G::iWeapId = NULL, G::pWeap = NULL;
		return NULL;
	};

	g_bOffline = false, G::bAlive = pEntity->GetHealth() > 0 ? true : false, G::iTeam = pEntity->GetTeam(), G::iFlags = pEntity->GetFlags();

	if (G::bAlive)
	{
		if ((G::pWeap = pEntity->GetWeapon()))
			G::iWeapId = G::pWeap->GetItemDefinitionIndex(), G::bEmpty = G::pWeap->IsEmpty(), G::bGun = G::pWeap->IsGun(G::iWeapId);

		else
			G::iWeapId = NULL, G::bEmpty = true, G::bGun = false;
	};

	return pEntity;
};

VOID TraceLine(Vector & vecStart, Vector & vecEnd, UINT uiMask, Entity_T * pIgnore, GameTrace_T * pTrace)
{
	static Ray_T _Ray;
	static TraceFilter_T _Filter;

	_Ray.Run(vecStart, vecEnd), _Filter.pSkip = pIgnore, I::EngTrace->TraceRay(_Ray, uiMask, &_Filter, pTrace);
};

VOID ServerRankRevealAll(VOID)
{
	static FLOAT pArray[3] = { 0.f, 0.f, 0.f, };

	if (g_Offsets.m_dwServerRankRevealAllEx)
	{
		ServerRankRevealAllEx = (ServerRankRevealAllFn)(g_Offsets.m_dwServerRankRevealAllEx);

		if (ServerRankRevealAllEx)
			ServerRankRevealAllEx(pArray);
	};
};

VOID SetupHooks(VOID)
{
	H::VPanel = new VTHook((PDWORD *)I::VPanel);
	H::CLMode = new VTHook((PDWORD *)I::CLMode);
	H::CL = new VTHook((PDWORD *)I::Client);
	H::D3D = new VTHook((PDWORD *)g_Offsets.m_dwD3DDevice);

	oPaintTraverse = (PaintTraverseFn)H::VPanel->HookFunction((DWORD)PaintTraverse, 41);
	oCreateMove = (CreateMoveFn)H::CLMode->HookFunction((DWORD)CreateMove, 24);
	oReset = (ResetFn)H::D3D->HookFunction((DWORD)Reset, 16);
	oEndScene = (EndSceneFn)H::D3D->HookFunction((DWORD)EndScene, 42);
};

VOID SetupOffsets(VOID)
{
	g_pNetVarsMan->Run(), GrabOffsets();
};

VOID SetupAll(VOID)
{
	Sleep(500), SetupInterfaces(), Sleep(200), SetupOffsets(), Sleep(200), SetupHooks(), Sleep(125), D::SetupFonts(), Sleep(50);
};

FLOAT GetDisp(VOID)
{
	if ((G::pCmd->buttons & IN_ATTACK) && (G::pCmd->buttons & IN_ATTACK2)) return -6.f;
	else if ((G::pCmd->buttons & IN_ATTACK)) return 0.f;
	return -12.f;
};

FLOAT GetFactor(VOID)
{
	if ((G::pCmd->buttons & IN_ATTACK) && (G::pCmd->buttons & IN_ATTACK2)) return .6f;
	else if ((G::pCmd->buttons & IN_ATTACK)) return .9f;
	return .27f;
};

FLOAT GetDetonationTime(VOID)
{
	return (G::iWeapId == WEAPON_HEGRENADE || G::iWeapId == WEAPON_FLASHBANG) ? 1.5f : 3.f;
};

VOID GrabOffsets(VOID)
{
	g_Offsets.m_iHealth = g_pNetVarsMan->GetOffset(XorString("DT_CSPlayer"), XorString("m_iHealth"));
	g_Offsets.m_bSpotted = g_pNetVarsMan->GetOffset(XorString("DT_CSPlayer"), XorString("m_bSpotted"));
	g_Offsets.m_iTeamNum = g_pNetVarsMan->GetOffset(XorString("DT_CSPlayer"), XorString("m_iTeamNum"));
	g_Offsets.m_bIsScoped = g_pNetVarsMan->GetOffset(XorString("DT_CSPlayer"), XorString("m_bIsScoped"));
	g_Offsets.m_ArmorValue = g_pNetVarsMan->GetOffset(XorString("DT_CSPlayer"), XorString("m_ArmorValue"));
	g_Offsets.m_flElasticity = g_pNetVarsMan->GetOffset(XorString("DT_CSPlayer"), XorString("m_flElasticity"));
	g_Offsets.m_bDormant = 0x000000E9;
	g_Offsets.m_bGunGameImmunity = g_pNetVarsMan->GetOffset(XorString("DT_CSPlayer"), XorString("m_bGunGameImmunity"));
	g_Offsets.m_fFlags = g_pNetVarsMan->GetOffset(("DT_CSPlayer"), XorString("m_fFlags"));
	g_Offsets.m_nTickBase = g_pNetVarsMan->GetOffset(("DT_CSPlayer"), XorString("m_nTickBase"));
	g_Offsets.m_angEyeAngles = g_pNetVarsMan->GetOffset(("DT_CSPlayer"), XorString("m_angEyeAngles"));
	g_Offsets.m_flFlashDuration = g_pNetVarsMan->GetOffset(XorString("DT_CSPlayer"), XorString("m_flFlashDuration"));
	g_Offsets.m_aimPunchAngle = g_pNetVarsMan->GetOffset(XorString("DT_BasePlayer"), XorString("m_aimPunchAngle"));
	g_Offsets.m_vecOrigin = g_pNetVarsMan->GetOffset(XorString("DT_BasePlayer"), XorString("m_vecOrigin"));
	g_Offsets.m_vecViewOffset = g_pNetVarsMan->GetOffset(XorString("DT_CSPlayer"), XorString("m_vecViewOffset[0]"));
	g_Offsets.m_vecVelocity = g_pNetVarsMan->GetOffset(XorString("DT_CSPlayer"), XorString("m_vecVelocity[0]"));
	g_Offsets.m_hActiveWeapon = g_pNetVarsMan->GetOffset(XorString("DT_CSPlayer"), XorString("m_hActiveWeapon"));
	g_Offsets.m_nHitboxSet = g_pNetVarsMan->GetOffset(XorString("DT_BasePlayer"), XorString("m_nHitboxSet"));
	g_Offsets.m_flC4Blow = g_pNetVarsMan->GetOffset(XorString("DT_PlantedC4"), XorString("m_flC4Blow"));
	g_Offsets.m_flNextPrimaryAttack = g_pNetVarsMan->GetOffset(XorString("DT_BaseCombatWeapon"), XorString("m_flNextPrimaryAttack"));
	g_Offsets.m_iClip1 = g_pNetVarsMan->GetOffset(XorString("DT_BaseCombatWeapon"), XorString("m_iClip1"));
	g_Offsets.m_iItemDefinitionIndex = g_pNetVarsMan->GetOffset(XorString("DT_BaseAttributableItem"), XorString("m_iItemDefinitionIndex"));

	DWORD dwMyGlowAddr = FindPattern(XorString("client.dll"), XorString("0F 11 05 ?? ?? ?? ?? 83 C8 01 C7 05 ?? ?? ?? ?? 00 00 00 00")) + 0x00000003;

	if (dwMyGlowAddr && dwMyGlowAddr != 0x00000003)
		g_Offsets.m_dwGlowManager = *(PDWORD)dwMyGlowAddr;

	else
		g_Offsets.m_dwGlowManager = NULL;

	DWORD dwMyD3DDevAddr = FindPattern(XorString("shaderapidx9.dll"), XorString("A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C")) + 0x00000001;

	if (dwMyD3DDevAddr && dwMyD3DDevAddr != 0x00000001)
		g_Offsets.m_dwD3DDevice = **(DWORD **)dwMyD3DDevAddr;

	g_Offsets.m_dwServerRankRevealAllEx = FindPattern(XorString("client.dll"), XorString("55 8B EC 8B 0D ? ? ? ? 68 ? ? ? ? "));

	if (!g_Offsets.m_flElasticity)
		g_Offsets.m_flElasticity = g_pNetVarsMan->GetOffset(XorString("DT_BaseEntity"), XorString("m_flElasticity"));

	if (!g_Offsets.m_flElasticity)
		g_Offsets.m_flElasticity = g_pNetVarsMan->GetOffset(XorString("DT_BasePlayer"), XorString("m_flElasticity"));

	if (!g_Offsets.m_flElasticity)
		g_Offsets.m_flElasticity = g_pNetVarsMan->GetOffset(XorString("DT_BaseGrenade"), XorString("m_flElasticity"));

	if (!g_Offsets.m_flElasticity)
		g_Offsets.m_flElasticity = g_pNetVarsMan->GetOffset(XorString("DT_CSGrenade"), XorString("m_flElasticity"));

	if (!g_Offsets.m_flElasticity)
		g_Offsets.m_flElasticity = g_pNetVarsMan->GetOffset(XorString("DT_BaseAttributableItem"), XorString("m_flElasticity"));

	if (!g_Offsets.m_flElasticity)
		g_Offsets.m_flElasticity = g_pNetVarsMan->GetOffset(XorString("DT_BaseCombatWeapon"), XorString("m_flElasticity"));

	if (!g_Offsets.m_flElasticity)
		g_Offsets.m_flElasticity = g_pNetVarsMan->GetOffset(XorString("DT_WeaponCSBase"), XorString("m_flElasticity"));
};

CONST CHAR * g_pCustomColorOptions[14] =
{
	"White",
	"Green",
	"Red",
	"Blue",
	"L. Blue",
	"Grey",
	"Purple",
	"Cyan",
	"Orange",
	"D. Green",
	"Magenta",
	"D. Grey",
	"Black",
	"Gold",
};

CONST CHAR * g_pCustomGlowColorOptions[15] =
{
	"Default",
	"Whit",
	"Green",
	"Red",
	"Blue",
	"L. Blue",
	"Grey",
	"Purple",
	"Cyan",
	"Orange",
	"D. Green",
	"Magenta",
	"D. Grey",
	"Black",
	"Gold",
};

CONST CHAR * g_pExtrapolationOptions[4] =
{
	"Velocity",
	"None",
	"Vel. 2-Way (+Smooth)",
	"Vel. 2-Way (-Smooth)",
};

CONST CHAR * g_pTriggerKeys[124] =
{
	"", "L. MOUSE (#1)", "R. MOUSE (#2)", "CANCEL", "MID. MOUSE", "MOUSE #4", "MOUSE #5", "", "B. SPACE", "TAB", "", "", \
	"CLEAR", "ENTER", "", "", "SHIFT", "CTRL", "ALT", "PAUSE", "C. LOCK", "", "", "", "", "", "", "ESC", "", "", \
	"", "", "SPACE", "PG. UP", "PG. DOWN", "END", "HOME", "LEFT", "UP", "RIGHT", "DOWN", "", "", "", "PRINT", \
	"INSERT", "DELETE", "", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "", "", "", "", "", "", "", "A", \
	"B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", \
	"X", "Y", "Z", "", "", "", "", "", "N. PAD #0", "N. PAD #1", "N. PAD #2", "N. PAD #3", "N. PAD #4", "N. PAD #5", \
	"N. PAD #6", "N. PAD #7", "N. PAD #8", "N. PAD #9", "MULTIPLY", "ADD", "", "SUBTRACT", "DECIMAL", "DIVIDE", \
	"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
};

VOID WINAPI PaintTraverse(UINT uiPanel, bool bForceRePaint, bool bAllowForce)
{
	oPaintTraverse(uiPanel, bForceRePaint, bAllowForce);

	if (!g_uiDrawPanel)
	{
		g_pszPanelName = I::VPanel->GetName(uiPanel);

		if (g_pszPanelName[0] == 'M' && g_pszPanelName[2] == 't')
		{
			g_uiDrawPanel = uiPanel;
		};
	};

	if (uiPanel == g_uiDrawPanel)
	{
		g_pVisuals->Run();
	};
};

INT g_nHitBoxes[20] =
{
	HITBOX_HEAD,
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

VOID CreateGUI(IDirect3DDevice9 * pDevice)
{
	ImGui_ImplDX9_Init(G::pWnd, pDevice);
	ImGuiStyle & style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.26f, 0.26f, 0.26f, 0.95f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);

	style.GrabRounding = 0.f;
	style.WindowRounding = 0.f;
	style.ScrollbarRounding = 3.f;
	style.FrameRounding = 3.f;
	style.WindowTitleAlign = ImVec2(.5f, .5f);

	G::bD3DOK = true;
};

void IInputSystem::GetCursorPosition(int* m_pX, int* m_pY)
{
	typedef void(__thiscall* OriginalFn)(void*, int*, int*);
	return GetVFunc< OriginalFn >(this, 56)(this, m_pX, m_pY);
};

bool WINAPI CreateMove(FLOAT, CUserCmd * pCmd)
{
	static INT iEnemy = 0, iWeapon = 0, iTeam = 0;
	static Entity_T * pEnemy = NULL;
	static Weapon_T * pWeapon = NULL;
	static Vector vecZone, vecSelfEyePos;
	static QAngle angAim;
	static bool bLastJumped = false, bShouldFake = false;

	G::pSelf = GetSelf(), G::pCmd = pCmd;

	if (!pCmd || pCmd->cmdnum == 0 || !G::pSelf || G::iTeam < 1)
	{
		if (pCmd) pCmd->viewang.Clamp();
		return false;
	};

	if (!G::bAlive)
	{
		pCmd->viewang.Clamp();
		return false;
	};

	if (G::iFlags & FL_FROZEN)
	{
		if (!g_bFrozen)
		{
			I::Engine->ClientCmd_Unrestricted(XorString("cl_mouseenable 0; unbind tab; unbind y; unbind u"), 0);
			g_bFrozen = true;
		};

		G::pCmd->buttons &= ~IN_SCORE;
		G::Keys[VK_TAB] = false;
	}

	else
	{
		if (g_bFrozen)
		{
			I::Engine->ClientCmd_Unrestricted(XorString("cl_mouseenable 1; bind tab +showscores; bind y messagemode; bind u messagemode2"), 0);
			g_bFrozen = false;
		};
	};

	if (Vars.Menu.Opened)
	{
		pCmd->viewang.Clamp();
		return false;
	};

	if ((Vars.Misc.bAutoLeftRight || Vars.Misc.bSpecialFlickShots) && !(G::pCmd->buttons & IN_WALK) && !(G::pCmd->buttons & IN_DUCK) && !(G::pCmd->buttons & IN_ATTACK) && !(G::iFlags & FL_DUCKING) && (G::iFlags & FL_ONGROUND) && !(G::pCmd->buttons & IN_JUMP))
	{
		for (iEnemy = 0; iEnemy <= I::Globals->m_iMaxClients; iEnemy++)
		{
			if (!(pEnemy = I::Ents->GetClEntity(iEnemy)) || pEnemy == G::pSelf || pEnemy->GetDormant() || pEnemy->GetHealth() < 1 || \
				!(pWeapon = pEnemy->GetWeapon()) || (iWeapon = pWeapon->GetItemDefinitionIndex()) < 1)
				continue;

			switch (iWeapon)
			{
			case WEAPON_AWP: case WEAPON_SSG08: case WEAPON_SCAR20: case WEAPON_G3SG1:
			{
				if (pEnemy->IsTargetingLocal())
				{
					if (Vars.Misc.bSpecialFlickShots && (G::iWeapId == WEAPON_HKP2000 || G::iWeapId == WEAPON_USP_SILENCER || G::iWeapId == WEAPON_AWP || G::iWeapId == WEAPON_SSG08 || G::iWeapId == WEAPON_DEAGLE) && pEnemy->GetTeam() != G::iTeam && G::pSelf->IsScopedV2() && !pEnemy->GetImmune() && g_pLegitBOT->BestAimPointAll(pEnemy, vecZone) && M::FOV(G::pCmd->viewang + G::pSelf->GetPunch() * 2.f, M::CalcAng((vecSelfEyePos = G::pSelf->GetEyePosition()), vecZone)) < 1.33f)
					{
						angAim = M::CalcAng(vecSelfEyePos, pEnemy->GetPredicted(vecZone)) - G::pSelf->GetPunch() * 2.f;
						G::pCmd->viewang = angAim, I::Engine->SetViewAngles(angAim), G::pCmd->buttons |= IN_ATTACK;
					};

					if (Vars.Misc.bAutoLeftRight)
					{
						switch (time(NULL) % 2 == 0)
						{
						case true:
						{
							G::pCmd->sidemove = -450.f;
							break;
						};

						case false:
						{
							G::pCmd->sidemove = 450.f;
							break;
						};
						};

						if (G::pCmd->fwdmove != 0.f)
						{
							G::pCmd->fwdmove = -450.f;
						};
					};
				};

				break;
			};

			default:
			{
				if (pEnemy->IsTargetingLocalHead())
				{
					if (Vars.Misc.bSpecialFlickShots && (G::iWeapId == WEAPON_HKP2000 || G::iWeapId == WEAPON_USP_SILENCER || G::iWeapId == WEAPON_AWP || G::iWeapId == WEAPON_SSG08 || G::iWeapId == WEAPON_DEAGLE) && pEnemy->GetTeam() != G::iTeam && G::pSelf->IsScopedV2() && !pEnemy->GetImmune() && g_pLegitBOT->BestAimPointAll(pEnemy, vecZone) && M::FOV(G::pCmd->viewang + G::pSelf->GetPunch() * 2.f, M::CalcAng((vecSelfEyePos = G::pSelf->GetEyePosition()), vecZone)) < 1.33f)
					{
						angAim = M::CalcAng(vecSelfEyePos, pEnemy->GetPredicted(vecZone)) - G::pSelf->GetPunch() * 2.f;
						G::pCmd->viewang = angAim, I::Engine->SetViewAngles(angAim), G::pCmd->buttons |= IN_ATTACK;
					};

					if (Vars.Misc.bAutoLeftRight)
					{
						switch (time(NULL) % 2 == 0)
						{
						case false:
						{
							G::pCmd->sidemove = -450.f;
							break;
						};

						case true:
						{
							G::pCmd->sidemove = 450.f;
							break;
						};
						};
					};
				};

				break;
			};
			};
		};
	};

	if (Vars.Misc.bAutoStrafe && !(G::iFlags & FL_ONGROUND) && (G::pCmd->mousedx > 1 || G::pCmd->mousedx < -1))
		G::pCmd->sidemove = G::pCmd->mousedx < 0 ? -450.f : 450.f;

	if (!G::bEmpty && G::bGun && !IS_REVOLVER(G::iWeapId))
	{

#if defined(ENABLE_RAGE_BOT) && ENABLE_RAGE_BOT == 1

		if (Vars.Ragebot.Enabled)
			g_pRageBOT->Run();

		else

#endif

			g_pLegitBOT->Run();
	};

	if (Vars.Misc.bBunnyHop)
	{
		if (!bLastJumped && bShouldFake)
		{
			bShouldFake = false;
			G::pCmd->buttons |= IN_JUMP;
		}

		else if (G::pCmd->buttons & IN_JUMP)
		{
			if (G::pSelf->GetFlags() & FL_ONGROUND)
				bLastJumped = bShouldFake = true;

			else
			{
				G::pCmd->buttons &= ~IN_JUMP;
				bLastJumped = false;
			};
		}

		else
			bLastJumped = bShouldFake = false;
	};

	pCmd->viewang.Clamp();
	return false;
};

LRESULT WINAPI OnWindProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static UINT uiButton = 0;

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		G::Keys[VK_LBUTTON] = true;
		break;

	case WM_LBUTTONUP:
		G::Keys[VK_LBUTTON] = false;
		break;

	case WM_RBUTTONDOWN:
		G::Keys[VK_RBUTTON] = true;
		break;

	case WM_RBUTTONUP:
		G::Keys[VK_RBUTTON] = false;
		break;

	case WM_MBUTTONDOWN:
		G::Keys[VK_MBUTTON] = true;
		break;

	case WM_MBUTTONUP:
		G::Keys[VK_MBUTTON] = false;
		break;

	case WM_XBUTTONDOWN:
	{
		uiButton = GET_XBUTTON_WPARAM(wParam);

		if (uiButton == XBUTTON1)
			G::Keys[VK_XBUTTON1] = true;

		else if (uiButton == XBUTTON2)
			G::Keys[VK_XBUTTON2] = true;

		break;
	};

	case WM_XBUTTONUP:
	{
		uiButton = GET_XBUTTON_WPARAM(wParam);

		if (uiButton == XBUTTON1)
			G::Keys[VK_XBUTTON1] = false;

		else if (uiButton == XBUTTON2)
			G::Keys[VK_XBUTTON2] = false;

		break;
	};

	case WM_KEYDOWN:
		G::Keys[wParam] = true;
		break;

	case WM_KEYUP:
		G::Keys[wParam] = false;
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_MENU)
			G::Keys[VK_MENU] = true;

		break;

	case WM_SYSKEYUP:
		if (wParam == VK_MENU)
			G::Keys[VK_MENU] = false;

		break;
	};

	if (G::bD3DOK && Vars.Menu.Opened && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return LRESULT(true);

	return (LRESULT)CallWindowProc(g_pOldWndProc, hWnd, uMsg, wParam, lParam);
};

INT RandomINT(INT iMin, INT iMax)
{
	return (rand() % (iMax + 1 - iMin)) + iMin;
};

FLOAT RandomFL(FLOAT fMin, FLOAT fMax)
{
	return fMin + static_cast <FLOAT> (rand()) / (static_cast <FLOAT> (RAND_MAX / (fMax - fMin)));
};

VOID replaceAll(std::string & str, std::string from, std::string to)
{
	static INT s = 0, e = 0;

	s = e = 0;

	while ((s = str.find(from, s)) != -1)
		e = s + from.length(), str.replace(s, e, to), s += to.length();
};

BOOL xIsCharUpper(CHAR x)
{
	if ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z'))
		return IsCharUpper(x) ? TRUE : FALSE;

	return FALSE;
};

BOOL xIsCharLower(CHAR x)
{
	if ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z'))
		return IsCharLower(x) ? TRUE : FALSE;

	return FALSE;
};

SIZE_T countChars(std::string Buffer, CHAR Char)
{
	static INT iIter = 0;
	static SIZE_T iCount = 0;

	iCount = 0;

	for (iIter = 0; iIter < Buffer.length(); iIter++)
		if (Buffer.at(iIter) == Char)
			iCount++;

	return iCount;
};

std::string ShortenName(std::string Name)
{
	static INT iPos = 0, iSize = 0, iLen = 0, iMaxLen = 0, iMany = 0;
	static std::string xRes = {};
	static std::vector < std::string > xArray = {};
	static PCHAR pToken = {};
	static CHAR szBuffer[256] = {};

	xRes.assign(Name);

	if (xRes.length() < 1)
	{
		xRes.assign(XorString("???"));

		return xRes;
	};

	replaceAll(xRes, XorString("%"), XorString("*"));
	replaceAll(xRes, XorString(";"), XorString("*"));
	replaceAll(xRes, XorString(":"), XorString("*"));
	replaceAll(xRes, XorString("\""), XorString("*"));

	if (xRes.find_last_of(XorString(" ")) < 0 || xRes.length() < 8)
	{
		if ((xRes.at(0) >= 'a' && xRes.at(0) <= 'z') || (xRes.at(0) >= 'A' && xRes.at(0) <= 'Z'))
		{
			if (RandomINT(0, 1) == 0 && xIsCharLower(xRes.at(1)))
				xRes.at(0) = tolower(xRes.at(0));

			else
				xRes.at(0) = toupper(xRes.at(0));
		};

		return xRes;
	};

	sprintf(szBuffer, XorString("%s"), xRes.c_str());

	pToken = strtok(szBuffer, XorString(" "));

	xArray.clear();
	iMany = 0;

	while (pToken)
	{
		xArray.push_back(pToken);

		pToken = strtok(NULL, XorString(" "));

		iMany++;
	};

	iSize = xArray.size();

	if (iSize < 1)
	{
		if ((xRes.at(0) >= 'a' && xRes.at(0) <= 'z') || (xRes.at(0) >= 'A' && xRes.at(0) <= 'Z'))
		{
			if (RandomINT(0, 1) == 0 && xIsCharLower(xRes.at(1)))
				xRes.at(0) = tolower(xRes.at(0));

			else
				xRes.at(0) = toupper(xRes.at(0));
		};

		return xRes;
	};

	iMaxLen = 0;

	for (iPos = 0; iPos < iSize; iPos++)
	{
		iLen = xArray.at(iPos).length();

		if (iLen > iMaxLen)
		{
			if (xArray.at(iPos).length() < 3 && iMany > 1)
				continue;

			if (iMany > 1 && (stristr(xArray.at(iPos).c_str(), XorString(".COM")) || stristr(xArray.at(iPos).c_str(), XorString(".NET"))))
				continue;

			iMaxLen = iLen;
			xRes = xArray.at(iPos);
		};
	};

	if ((xRes.at(0) >= 'a' && xRes.at(0) <= 'z') || (xRes.at(0) >= 'A' && xRes.at(0) <= 'Z'))
	{
		if (RandomINT(0, 1) == 0 && xIsCharLower(xRes.at(1)))
			xRes.at(0) = tolower(xRes.at(0));

		else
			xRes.at(0) = toupper(xRes.at(0));
	};

	if ((xRes.at(0) == '-' || xRes.at(xRes.length() - 1) == '-') && countChars(xRes, '-') < 3 && xRes.length() > 3)
		g_Hattrick.ReplaceAll(xRes, XorString("-"), XorString(""));

	if ((xRes.at(0) == '!' || xRes.at(xRes.length() - 1) == '!') && countChars(xRes, '!') < 3 && xRes.length() > 3)
		g_Hattrick.ReplaceAll(xRes, XorString("!"), XorString(""));

	if ((xRes.at(0) == '?' || xRes.at(xRes.length() - 1) == '?') && countChars(xRes, '?') < 3 && xRes.length() > 3)
		g_Hattrick.ReplaceAll(xRes, XorString("!"), XorString(""));

	if ((xRes.at(0) == '.' || xRes.at(xRes.length() - 1) == '.') && countChars(xRes, '.') < 3 && xRes.length() > 3)
		g_Hattrick.ReplaceAll(xRes, XorString("."), XorString(""));

	if ((xRes.at(0) == '#' || xRes.at(xRes.length() - 1) == '#') && countChars(xRes, '#') < 3 && xRes.length() > 3)
		g_Hattrick.ReplaceAll(xRes, XorString("#"), XorString(""));

	if ((xRes.at(0) == '$' || xRes.at(xRes.length() - 1) == '$') && countChars(xRes, '$') < 3 && xRes.length() > 3)
		g_Hattrick.ReplaceAll(xRes, XorString("$"), XorString(""));

	if ((xRes.at(0) == '@' || xRes.at(xRes.length() - 1) == '@') && countChars(xRes, '@') < 3 && xRes.length() > 3)
		g_Hattrick.ReplaceAll(xRes, XorString("@"), XorString(""));

	if ((xRes.at(0) == '^' || xRes.at(xRes.length() - 1) == '^') && countChars(xRes, '^') < 3 && xRes.length() > 3)
		g_Hattrick.ReplaceAll(xRes, XorString("^"), XorString(""));

	if ((xRes.at(0) == '[' || xRes.at(xRes.length() - 1) == '[') && countChars(xRes, '[') < 3 && xRes.length() > 3)
		g_Hattrick.ReplaceAll(xRes, XorString("["), XorString(""));

	if ((xRes.at(0) == ']' || xRes.at(xRes.length() - 1) == ']') && countChars(xRes, ']') < 3 && xRes.length() > 3)
		g_Hattrick.ReplaceAll(xRes, XorString("]"), XorString(""));

	if ((xRes.at(0) == '<' || xRes.at(xRes.length() - 1) == '<') && countChars(xRes, '<') < 3 && xRes.length() > 3)
		g_Hattrick.ReplaceAll(xRes, XorString("<"), XorString(""));

	if ((xRes.at(0) == '>' || xRes.at(xRes.length() - 1) == '>') && countChars(xRes, '>') < 3 && xRes.length() > 3)
		g_Hattrick.ReplaceAll(xRes, XorString(">"), XorString(""));

	if ((xRes.at(0) == ')' || xRes.at(xRes.length() - 1) == ')') && countChars(xRes, ')') < 3 && xRes.length() > 3)
		g_Hattrick.ReplaceAll(xRes, XorString(")"), XorString(""));

	if ((xRes.at(0) == '(' || xRes.at(xRes.length() - 1) == '(') && countChars(xRes, '(') < 3 && xRes.length() > 3)
		g_Hattrick.ReplaceAll(xRes, XorString("("), XorString(""));

	return xRes;
};

VOID PrepareSpamString(PCHAR pData, PlrInfo_T * pPlayer)
{
	static std::string Name, Shorten;

	*pData = NULL;

	Shorten = ShortenName(pPlayer->name);
	Name.assign(Shorten);

	switch (RandomINT(0, 390))
	{
	case 0: sprintf(pData, XorString("%s, i fuck your mom"), Name.c_str()); break;
	case 1: sprintf(pData, XorString("geh weg, %s"), Name.c_str()); break;
	case 2: sprintf(pData, XorString("aufgeben, %s"), Name.c_str()); break;
	case 3: sprintf(pData, XorString("%s, du dumme schlampe.."), Name.c_str()); break;
	case 4: sprintf(pData, XorString("siktir git, %s"), Name.c_str()); break;
	case 5: sprintf(pData, XorString("hay siktir, %s"), Name.c_str()); break;
	case 6: sprintf(pData, XorString("%s, how much $ for hack?"), Name.c_str()); break;
	case 7: sprintf(pData, XorString("u using hack, %s ?"), Name.c_str()); break;
	case 8: sprintf(pData, XorString("cumback israel, %s ?"), Name.c_str()); break;
	case 9: sprintf(pData, XorString("i fuck your mom, %s"), Name.c_str()); break;
	case 10: sprintf(pData, XorString("nice engrish, %s"), Name.c_str()); break;
	case 11: sprintf(pData, XorString("nice frenglish, %s"), Name.c_str()); break;
	case 12: sprintf(pData, XorString("u cute, %s ?"), Name.c_str()); break;
	case 13: sprintf(pData, XorString("%s, I FUCK YOUR SISTER"), Name.c_str()); break;
	case 14: sprintf(pData, XorString("u so fucked, %s"), Name.c_str()); break;
	case 15: sprintf(pData, XorString("u horny, %s ?"), Name.c_str()); break;
	case 16: sprintf(pData, XorString("fick dich, %s"), Name.c_str()); break;
	case 17: sprintf(pData, XorString("%s, u fucking CUNT"), Name.c_str()); break;
	case 18: sprintf(pData, XorString("%s, n1 hack"), Name.c_str()); break;
	case 19: sprintf(pData, XorString("nice hack, %s"), Name.c_str()); break;
	case 20: sprintf(pData, XorString("eat my cock, %s"), Name.c_str()); break;
	case 21: sprintf(pData, XorString("să-mi bei boașele, %s"), Name.c_str()); break;
	case 22: sprintf(pData, XorString("să moară mă-ta, %s ?"), Name.c_str()); break;
	case 23: sprintf(pData, XorString("te fut, %s"), Name.c_str()); break;
	case 24: sprintf(pData, XorString("АНАЛЬНЫЙ СЕКС, %s ?"), Name.c_str()); break;
	case 25: sprintf(pData, XorString("ХУЙ ТЕБЯ, %s !"), Name.c_str()); break;
	case 26: sprintf(pData, XorString("ТЫ СОСАТЬ ХУЙ, %s ?"), Name.c_str()); break;
	case 27: sprintf(pData, XorString("%s, ТЫ КУРИТЕ ??"), Name.c_str()); break;
	case 28: sprintf(pData, XorString("%s, u like BBC?"), Name.c_str()); break;
	case 29: sprintf(pData, XorString("go surrender, %s .."), Name.c_str()); break;
	case 30: sprintf(pData, XorString("how mush paid for hax, %s ??"), Name.c_str()); break;
	case 31: sprintf(pData, XorString("temiz dişler, %s ?"), Name.c_str()); break;
	case 32: sprintf(pData, XorString("fahişe anne, %s ??"), Name.c_str()); break;
	case 33: sprintf(pData, XorString("orospu anne, %s ?"), Name.c_str()); break;
	case 34: sprintf(pData, XorString("büyük çük, %s ??"), Name.c_str()); break;
	case 35: sprintf(pData, XorString("parles-tu français, %s ?"), Name.c_str()); break;
	case 36: sprintf(pData, XorString("i hate u, %s"), Name.c_str()); break;
	case 37: sprintf(pData, XorString("%s, how mush money for hack ??"), Name.c_str()); break;
	case 38: sprintf(pData, XorString("%s, go fuck yourself.."), Name.c_str()); break;
	case 39: sprintf(pData, XorString("i bet you have hard cock, %s"), Name.c_str()); break;
	case 40: sprintf(pData, XorString("i like hard anal sex, %s"), Name.c_str()); break;
	case 41: sprintf(pData, XorString("%s, u suck for $$ ?"), Name.c_str()); break;
	case 42: sprintf(pData, XorString("%s, ur mom knows ur retard?"), Name.c_str()); break;
	case 43: sprintf(pData, XorString("hi %s"), Name.c_str()); break;
	case 44: sprintf(pData, XorString("%s, ur dick 24cm+ ?"), Name.c_str()); break;
	case 45: sprintf(pData, XorString("%s, how long your dick=?"), Name.c_str()); break;
	case 46: sprintf(pData, XorString("%s, last time u masturbate =?"), Name.c_str()); break;
	case 47: sprintf(pData, XorString("ns, %s"), Name.c_str()); break;
	case 48: sprintf(pData, XorString("u such a SWAG, %s"), Name.c_str()); break;
	case 49: sprintf(pData, XorString("ПРИВЕТ, %s"), Name.c_str()); break;
	case 50: sprintf(pData, XorString("ТЫ ЗНАЕШЬ КАРМЕЛИТА ЦЫГАНСКАЯ СТРАСТЬ, %s ?"), Name.c_str()); break;
	case 51: sprintf(pData, XorString("ТОЛЬКО ТЫ МЕНЯ КОЛЫШЕШЬ, %s ?"), Name.c_str()); break;
	case 52: sprintf(pData, XorString("ПОСТОЙ ПАРОВОЗ, %s ?"), Name.c_str()); break;
	case 53: sprintf(pData, XorString("НЕТ ТАКИХ УЖЕ ЛИЧНОСТЕЙ, %s .."), Name.c_str()); break;
	case 54: sprintf(pData, XorString("Я ВАС ЛЮБЛЮ, %s !!"), Name.c_str()); break;
	case 55: sprintf(pData, XorString("СПАСИБО ЧТО ТЫ БЫЛ, %s"), Name.c_str()); break;
	case 56: sprintf(pData, XorString("СПАСИБО, %s !"), Name.c_str()); break;
	case 57: sprintf(pData, XorString("ХОРОШИЙ АРТИСТ, %s"), Name.c_str()); break;
	case 58: sprintf(pData, XorString("КЛАСС, %s !!"), Name.c_str()); break;
	case 59: sprintf(pData, XorString("КТО СУКА, %s ??"), Name.c_str()); break;
	case 60: sprintf(pData, XorString("%s, ИДИОТ ПЕДОФИЛ .."), Name.c_str()); break;
	case 61: sprintf(pData, XorString("%s, îmi sugi pula !!"), Name.c_str()); break;
	case 62: sprintf(pData, XorString("u got nice butt, %s ?"), Name.c_str()); break;
	case 63: sprintf(pData, XorString("enjoying gay porn, %s ?"), Name.c_str()); break;
	case 64: sprintf(pData, XorString("enjoying blowjobs, %s ?"), Name.c_str()); break;
	case 65: sprintf(pData, XorString("%s, u from =?"), Name.c_str()); break;
	case 66: sprintf(pData, XorString("%s, u arab?"), Name.c_str()); break;
	case 67: sprintf(pData, XorString("%s, u arabs?"), Name.c_str()); break;
	case 68: sprintf(pData, XorString("%s, u arabiqué?"), Name.c_str()); break;
	case 69: sprintf(pData, XorString("%s, u like anal Cum?"), Name.c_str()); break;
	case 70: sprintf(pData, XorString("%s, u hav RedTube.com Acc?"), Name.c_str()); break;
	case 71: sprintf(pData, XorString("%s, u hav XNXX.com acc?"), Name.c_str()); break;
	case 72: sprintf(pData, XorString("u watchin xHamster.com, %s ?"), Name.c_str()); break;
	case 73: sprintf(pData, XorString("u got big Dick, %s ?"), Name.c_str()); break;
	case 74: sprintf(pData, XorString("i ❤ u, %s"), Name.c_str()); break;
	case 75: sprintf(pData, XorString("u fkn slave, %s"), Name.c_str()); break;
	case 76: sprintf(pData, XorString("this will go on YT™, %s"), Name.c_str()); break;
	case 77: sprintf(pData, XorString("vaffanculo, %s"), Name.c_str()); break;
	case 78: sprintf(pData, XorString("¡ven a mi cama, %s!"), Name.c_str()); break;
	case 79: sprintf(pData, XorString("¿de donde eres, %s?"), Name.c_str()); break;
	case 80: sprintf(pData, XorString("dass der Tod sie Scheidet, %s ?"), Name.c_str()); break;
	case 81: sprintf(pData, XorString("u speak Hitler, %s ?"), Name.c_str()); break;
	case 82: sprintf(pData, XorString("u NAZI, %s ?"), Name.c_str()); break;
	case 83: sprintf(pData, XorString("how mutch $ to fuck ur mom, %s ?"), Name.c_str()); break;
	case 84: sprintf(pData, XorString("how many dildos u own, %s ?"), Name.c_str()); break;
	case 85: sprintf(pData, XorString("u using fleshlight, %s ?"), Name.c_str()); break;
	case 86: sprintf(pData, XorString("%s, u havin Dildos collection?"), Name.c_str()); break;
	case 87: sprintf(pData, XorString("%s, u hav 24cm+ dildo?"), Name.c_str()); break;
	case 88: sprintf(pData, XorString("%s, i think i luv ur mom?"), Name.c_str()); break;
	case 89: sprintf(pData, XorString("%s, u incest with your sis?"), Name.c_str()); break;
	case 90: sprintf(pData, XorString("%s, u incest with your mum?"), Name.c_str()); break;
	case 91: sprintf(pData, XorString("%s, can u selfSuck for me?"), Name.c_str()); break;
	case 92: sprintf(pData, XorString("%s, last time u washd your Teeth?"), Name.c_str()); break;
	case 93: sprintf(pData, XorString("u having AIDS, %s ?"), Name.c_str()); break;
	case 94: sprintf(pData, XorString("u on Ebola, %s ?"), Name.c_str()); break;
	case 95: sprintf(pData, XorString("ЧЕЧНИ ВОДКА, %s ?"), Name.c_str()); break;
	case 96: sprintf(pData, XorString("u hav good Aimbot, %s"), Name.c_str()); break;
	case 97: sprintf(pData, XorString("ur mom has good Tits, %s ?"), Name.c_str()); break;
	case 98: sprintf(pData, XorString("how much KG u have, %s ?"), Name.c_str()); break;
	case 99: sprintf(pData, XorString("%s, how tall r you 2m?"), Name.c_str()); break;
	case 100: sprintf(pData, XorString("ur mom enjoys Anal, %s ?"), Name.c_str()); break;
	case 101: sprintf(pData, XorString("u like upSideDown sex, %s ?"), Name.c_str()); break;
	case 102: sprintf(pData, XorString("u doing 69 with your sis, %s ?"), Name.c_str()); break;
	case 103: sprintf(pData, XorString("Bazd meg, %s !"), Name.c_str()); break;
	case 104: sprintf(pData, XorString("Utállak, %s !"), Name.c_str()); break;
	case 105: sprintf(pData, XorString("van nagy faszod, %s ?"), Name.c_str()); break;
	case 106: sprintf(pData, XorString("Menj el, %s !"), Name.c_str()); break;
	case 107: sprintf(pData, XorString("add meg magad, %s !"), Name.c_str()); break;
	case 108: sprintf(pData, XorString("meg baszom az anyádat, %s !"), Name.c_str()); break;
	case 109: sprintf(pData, XorString("БОЛЬШОЙ ПЕНИС, %s ?"), Name.c_str()); break;
	case 110: sprintf(pData, XorString("ur having fun, %s ?"), Name.c_str()); break;
	case 111: sprintf(pData, XorString("u liek Porn films, %s ?"), Name.c_str()); break;
	case 112: sprintf(pData, XorString("i like ur sister, %s"), Name.c_str()); break;
	case 113: sprintf(pData, XorString("can i have ur sister, %s ?"), Name.c_str()); break;
	case 114: sprintf(pData, XorString("%s, how much $ for your mom?"), Name.c_str()); break;
	case 115: sprintf(pData, XorString("%s, is ur mom MILF?"), Name.c_str()); break;
	case 116: sprintf(pData, XorString("%s, ХУЙ ЗНАЕШЬ ?"), Name.c_str()); break;
	case 117: sprintf(pData, XorString("%s, КАК ТЕБЯ ЗОВУТ?"), Name.c_str()); break;
	case 118: sprintf(pData, XorString("КАК ДЕЛА, %s ?"), Name.c_str()); break;
	case 119: sprintf(pData, XorString("Anal seks, %s ?"), Name.c_str()); break;
	case 120: sprintf(pData, XorString("Kocaman çük, %s ?"), Name.c_str()); break;
	case 121: sprintf(pData, XorString("nize hack, %s"), Name.c_str()); break;
	case 122: sprintf(pData, XorString("%s, u cummed today?"), Name.c_str()); break;
	case 123: sprintf(pData, XorString("%s, u crying?"), Name.c_str()); break;
	case 124: sprintf(pData, XorString("cry moar, %s"), Name.c_str()); break;
	case 125: sprintf(pData, XorString("%s, cry me rivers"), Name.c_str()); break;
	case 126: sprintf(pData, XorString("cry me a river, %s"), Name.c_str()); break;
	case 127: sprintf(pData, XorString("excellent, %s"), Name.c_str()); break;
	case 128: sprintf(pData, XorString("%s, u from plastic?"), Name.c_str()); break;
	case 129: sprintf(pData, XorString("far from plastic, %s ?"), Name.c_str()); break;
	case 130: sprintf(pData, XorString("what hack u have, %s ?"), Name.c_str()); break;
	case 131: sprintf(pData, XorString("u hacking hard, %s ?"), Name.c_str()); break;
	case 132: sprintf(pData, XorString("u think im hacking, %s ?"), Name.c_str()); break;
	case 133: sprintf(pData, XorString("how can i hack with 3 karambits, %s ?"), Name.c_str()); break;
	case 134: sprintf(pData, XorString("how can i hack with 2 bayonets, %s ?"), Name.c_str()); break;
	case 135: sprintf(pData, XorString("%s, why u hack with $3000 inv?"), Name.c_str()); break;
	case 136: sprintf(pData, XorString("%s, u fan of Lisa Ann?"), Name.c_str()); break;
	case 137: sprintf(pData, XorString("%s, am pl de 1m și o să te cam FUT"), Name.c_str()); break;
	case 138: sprintf(pData, XorString("%s, care ești mă ?"), Name.c_str()); break;
	case 139: sprintf(pData, XorString("%s, când aduci banii pe HACK?"), Name.c_str()); break;
	case 140: sprintf(pData, XorString("%s, u fkn CUNT"), Name.c_str()); break;
	case 141: sprintf(pData, XorString("%s, turn off man"), Name.c_str()); break;
	case 142: sprintf(pData, XorString("%s, why u hack?"), Name.c_str()); break;
	case 143: sprintf(pData, XorString("%s, how mush $ your hack?"), Name.c_str()); break;
	case 144: sprintf(pData, XorString("%s, how mutch $ your hack?"), Name.c_str()); break;
	case 145: sprintf(pData, XorString("%s, whats your hack name ?"), Name.c_str()); break;
	case 146: sprintf(pData, XorString("Nerelisin, %s ?"), Name.c_str()); break;
	case 147: sprintf(pData, XorString("where u from, %s ?"), Name.c_str()); break;
	case 148: sprintf(pData, XorString("ОТКУДА ТЫ, %s ?"), Name.c_str()); break;
	case 149: sprintf(pData, XorString("u like gangBang, %s ?"), Name.c_str()); break;
	case 150: sprintf(pData, XorString("u liek group seks, %s ?"), Name.c_str()); break;
	case 151: sprintf(pData, XorString("%s, u like ponies?"), Name.c_str()); break;
	case 152: sprintf(pData, XorString("%s, can u suck my dick?"), Name.c_str()); break;
	case 153: sprintf(pData, XorString("buy Caramel hack, %s .."), Name.c_str()); break;
	case 154: sprintf(pData, XorString("u need Caramel hack, %s"), Name.c_str()); break;
	case 155: sprintf(pData, XorString("why u so low, %s ?"), Name.c_str()); break;
	case 156: sprintf(pData, XorString("can i suck ur balls, %s ?"), Name.c_str()); break;
	case 157: sprintf(pData, XorString("u need hack, %s .."), Name.c_str()); break;
	case 158: sprintf(pData, XorString("%s, u need better hack"), Name.c_str()); break;
	case 159: sprintf(pData, XorString("surprise, %s"), Name.c_str()); break;
	case 160: sprintf(pData, XorString("go die, %s .."), Name.c_str()); break;
	case 161: sprintf(pData, XorString("%s, go die in inferno"), Name.c_str()); break;
	case 162: sprintf(pData, XorString("die in hell, %s"), Name.c_str()); break;
	case 163: sprintf(pData, XorString("%s, u obvious fuck.."), Name.c_str()); break;
	case 164: sprintf(pData, XorString("%s, u such a dickhead"), Name.c_str()); break;
	case 165: sprintf(pData, XorString("why use cheat, %s ?"), Name.c_str()); break;
	case 166: sprintf(pData, XorString("u using siktir hook, %s ?"), Name.c_str()); break;
	case 167: sprintf(pData, XorString("%s, go die in Dust 2"), Name.c_str()); break;
	case 168: sprintf(pData, XorString("i hope ur mom dies in Dust 2, %s"), Name.c_str()); break;
	case 169: sprintf(pData, XorString("go die in inferno, %s"), Name.c_str()); break;
	case 170: sprintf(pData, XorString("u left-handed, %s ?"), Name.c_str()); break;
	case 171: sprintf(pData, XorString("%s, u traded ur mum for Hack?"), Name.c_str()); break;
	case 172: sprintf(pData, XorString("%s, trade ur sister for my hack?"), Name.c_str()); break;
	case 173: sprintf(pData, XorString("can i touch ur sister, %s ?"), Name.c_str()); break;
	case 174: sprintf(pData, XorString("i want ur sister, %s"), Name.c_str()); break;
	case 175: sprintf(pData, XorString("what u ate today, %s ?"), Name.c_str()); break;
	case 176: sprintf(pData, XorString("du svart, %s ?"), Name.c_str()); break;
	case 177: sprintf(pData, XorString("har du svart kuk, %s ?"), Name.c_str()); break;
	case 178: sprintf(pData, XorString("skrika som en gris, %s !"), Name.c_str()); break;
	case 179: sprintf(pData, XorString("du fett gris, %s .."), Name.c_str()); break;
	case 180: sprintf(pData, XorString("u tried anal sex, %s ?"), Name.c_str()); break;
	case 181: sprintf(pData, XorString("Är du vit/ svart, %s ?"), Name.c_str()); break;
	case 182: sprintf(pData, XorString("har du vit rumpa, %s ?"), Name.c_str()); break;
	case 183: sprintf(pData, XorString("u like juicy cocks, %s ?"), Name.c_str()); break;
	case 184: sprintf(pData, XorString("%s, u horny yet?"), Name.c_str()); break;
	case 185: sprintf(pData, XorString("i piss on ur family, %s"), Name.c_str()); break;
	case 186: sprintf(pData, XorString("%s, i piss in ur mouth"), Name.c_str()); break;
	case 187: sprintf(pData, XorString("%s, u fkn slave"), Name.c_str()); break;
	case 188: sprintf(pData, XorString("u were in Egypt, %s ?"), Name.c_str()); break;
	case 189: sprintf(pData, XorString("u using condom, %s ?"), Name.c_str()); break;
	case 190: sprintf(pData, XorString("u like to lick, %s ?"), Name.c_str()); break;
	case 191: sprintf(pData, XorString("how long ur Vein, %s ?"), Name.c_str()); break;
	case 192: sprintf(pData, XorString("ai vână lungă, %s ?"), Name.c_str()); break;
	case 193: sprintf(pData, XorString("u got acne, %s ?"), Name.c_str()); break;
	case 194: sprintf(pData, XorString("%s, is ur mom pregnant ?"), Name.c_str()); break;
	case 195: sprintf(pData, XorString("%s, ur mom takes it Anal?"), Name.c_str()); break;
	case 196: sprintf(pData, XorString("u swallow it, %s ?"), Name.c_str()); break;
	case 197: sprintf(pData, XorString("u swallow or spit the cum, %s ?"), Name.c_str()); break;
	case 198: sprintf(pData, XorString("can i see ur dick, %s ?"), Name.c_str()); break;
	case 199: sprintf(pData, XorString("u know Kendama, %s ?"), Name.c_str()); break;
	case 200: sprintf(pData, XorString("te îneci când sugi pula, %s ?"), Name.c_str()); break;
	case 201: sprintf(pData, XorString("muști pula cu dinții, %s ?"), Name.c_str()); break;
	case 202: sprintf(pData, XorString("har du kissat idag, %s ?"), Name.c_str()); break;
	case 203: sprintf(pData, XorString("u brushed ur teeth today, %s ?"), Name.c_str()); break;
	case 204: sprintf(pData, XorString("is ur mom horny, %s ?"), Name.c_str()); break;
	case 205: sprintf(pData, XorString("Kuruyordu nehir, %s ?"), Name.c_str()); break;
	case 206: sprintf(pData, XorString("kasvetli kutlama, %s ?"), Name.c_str()); break;
	case 207: sprintf(pData, XorString("jag vill spracker ditt anus, %s"), Name.c_str()); break;
	case 208: sprintf(pData, XorString("%s, u take it anal for $5?"), Name.c_str()); break;
	case 209: sprintf(pData, XorString("u sad, %s ?"), Name.c_str()); break;
	case 210: sprintf(pData, XorString("now im dead meat, %s"), Name.c_str()); break;
	case 211: sprintf(pData, XorString("+rep for +rep, %s"), Name.c_str()); break;
	case 212: sprintf(pData, XorString("%s, +rep for +rep ?"), Name.c_str()); break;
	case 213: sprintf(pData, XorString("commend for commend, %s ?"), Name.c_str()); break;
	case 214: sprintf(pData, XorString("%s, commend for commend ?"), Name.c_str()); break;
	case 215: sprintf(pData, XorString("%s, u like anal Sex?"), Name.c_str()); break;
	case 216: sprintf(pData, XorString("%s, is ur ass cute?"), Name.c_str()); break;
	case 217: sprintf(pData, XorString("u got cute ass, %s ?"), Name.c_str()); break;
	case 218: sprintf(pData, XorString("u got bubble butt, %s ?"), Name.c_str()); break;
	case 219: sprintf(pData, XorString("how long is ur dick, %s ?"), Name.c_str()); break;
	case 220: sprintf(pData, XorString("enjoying hard porn, %s ?"), Name.c_str()); break;
	case 221: sprintf(pData, XorString("%s, stop downloading gay porn"), Name.c_str()); break;
	case 222: sprintf(pData, XorString("%s, nice ping man"), Name.c_str()); break;
	case 223: sprintf(pData, XorString("%s, can u lag moar pls?"), Name.c_str()); break;
	case 224: sprintf(pData, XorString("why u lagging, %s ?"), Name.c_str()); break;
	case 225: sprintf(pData, XorString("%s, u like creampie?"), Name.c_str()); break;
	case 226: sprintf(pData, XorString("%s, u downloading porn?"), Name.c_str()); break;
	case 227: sprintf(pData, XorString("u like porn films, %s ?"), Name.c_str()); break;
	case 228: sprintf(pData, XorString("last time u fucked yourself, %s ?"), Name.c_str()); break;
	case 229: sprintf(pData, XorString("bel cazzo, %s"), Name.c_str()); break;
	case 230: sprintf(pData, XorString("%s, hai un bel cazzo man"), Name.c_str()); break;
	case 231: sprintf(pData, XorString("scheiße, %s"), Name.c_str()); break;
	case 232: sprintf(pData, XorString("%s, ist dein schwanz lang?"), Name.c_str()); break;
	case 233: sprintf(pData, XorString("ich fick deine mutter, %s"), Name.c_str()); break;
	case 234: sprintf(pData, XorString("ЦЫГАНСКАЯ СТРАСТЬ, %s ?"), Name.c_str()); break;
	case 235: sprintf(pData, XorString("ТЫ ЦЫГАН, %s ?"), Name.c_str()); break;
	case 236: sprintf(pData, XorString("%s, ЦЫГАН ?"), Name.c_str()); break;
	case 237: sprintf(pData, XorString("%s, ТЫ РОГАТЫЙ?"), Name.c_str()); break;
	case 238: sprintf(pData, XorString("ТЫ РОГАТЫЙ, %s ?"), Name.c_str()); break;
	case 239: sprintf(pData, XorString("ТЫ ЕШЬ СПЕРМУ, %s ?"), Name.c_str()); break;
	case 240: sprintf(pData, XorString("%s, ТЫ ПЬЕШЬ СПЕРМУ?"), Name.c_str()); break;
	case 241: sprintf(pData, XorString("reported %s"), Name.c_str()); break;
	case 242: sprintf(pData, XorString("all report %s"), Name.c_str()); break;
	case 243: sprintf(pData, XorString("5 reports for %s"), Name.c_str()); break;
	case 244: sprintf(pData, XorString("%s, u reported"), Name.c_str()); break;
	case 245: sprintf(pData, XorString("%s, u got reported m8"), Name.c_str()); break;
	case 246: sprintf(pData, XorString("%s, -acc"), Name.c_str()); break;
	case 247: sprintf(pData, XorString("%s, check ur acc in 2 hr"), Name.c_str()); break;
	case 248: sprintf(pData, XorString("%s, bb acc"), Name.c_str()); break;
	case 249: sprintf(pData, XorString("bb acc, %s"), Name.c_str()); break;
	case 250: sprintf(pData, XorString("RIP %s"), Name.c_str()); break;
	case 251: sprintf(pData, XorString("sooner or later u get banned, %s"), Name.c_str()); break;
	case 252: sprintf(pData, XorString("w8 VAC, %s"), Name.c_str()); break;
	case 253: sprintf(pData, XorString("u will get OW, %s"), Name.c_str()); break;
	case 254: sprintf(pData, XorString("OW will ban u %s"), Name.c_str()); break;
	case 255: sprintf(pData, XorString("w8 OW ban, %s"), Name.c_str()); break;
	case 256: sprintf(pData, XorString("n1 hack %s"), Name.c_str()); break;
	case 257: sprintf(pData, XorString("%s is hacking"), Name.c_str()); break;
	case 258: sprintf(pData, XorString("Robertto: %s is hacking"), Name.c_str()); break;
	case 259: sprintf(pData, XorString("Mike: %s has obv WH"), Name.c_str()); break;
	case 260: sprintf(pData, XorString("ur last match, %s"), Name.c_str()); break;
	case 261: sprintf(pData, XorString("pis off, %s"), Name.c_str()); break;
	case 262: sprintf(pData, XorString("futu-ți glijania mă-tii, %s"), Name.c_str()); break;
	case 263: sprintf(pData, XorString("îmi bei coaiele, %s"), Name.c_str()); break;
	case 264: sprintf(pData, XorString("îmi bag pula în mă-ta, %s"), Name.c_str()); break;
	case 265: sprintf(pData, XorString("futu-ți pizda mă-tii, %s"), Name.c_str()); break;
	case 266: sprintf(pData, XorString("îmi bei pula, %s"), Name.c_str()); break;
	case 267: sprintf(pData, XorString("%s, îmi fac schiuri din crucea mă-tii"), Name.c_str()); break;
	case 268: sprintf(pData, XorString("îmi fac skiuri din crucia mă-tii, %s"), Name.c_str()); break;
	case 269: sprintf(pData, XorString("să moară tac-tu pe scară, %s"), Name.c_str()); break;
	case 270: sprintf(pData, XorString("să-ți fut mama în aripă, %s"), Name.c_str()); break;
	case 271: sprintf(pData, XorString("să mă lingi sub coadă, %s"), Name.c_str()); break;
	case 272: sprintf(pData, XorString("%s, îmi sugi pula cu mă-ta?"), Name.c_str()); break;
	case 273: sprintf(pData, XorString("te bag de unde ai ieșit, %s"), Name.c_str()); break;
	case 274: sprintf(pData, XorString("să moară familia ta, %s"), Name.c_str()); break;
	case 275: sprintf(pData, XorString("%s, das kleine krokodil?"), Name.c_str()); break;
	case 276: sprintf(pData, XorString("ți-am dat viol, %s?"), Name.c_str()); break;
	case 277: sprintf(pData, XorString("ești curat la cur, %s ?"), Name.c_str()); break;
	case 278: sprintf(pData, XorString("câte pule au spart-o pe mă-ta, %s ?"), Name.c_str()); break;
	case 279: sprintf(pData, XorString("%s, u play poker?"), Name.c_str()); break;
	case 280: sprintf(pData, XorString("Panda: %s is hacking hard.."), Name.c_str()); break;
	case 281: sprintf(pData, XorString("Pasha: %s has trigger"), Name.c_str()); break;
	case 282: sprintf(pData, XorString("%s is triggering hard"), Name.c_str()); break;
	case 283: sprintf(pData, XorString("%s, HS Only?"), Name.c_str()); break;
	case 284: sprintf(pData, XorString("only 1 tap, %s ?"), Name.c_str()); break;
	case 285: sprintf(pData, XorString("u like 1 taps, %s ?"), Name.c_str()); break;
	case 286: sprintf(pData, XorString("%s, îmi bag pula în soacră-ta"), Name.c_str()); break;
	case 287: sprintf(pData, XorString("n1 trigger, %s"), Name.c_str()); break;
	case 288: sprintf(pData, XorString("%s, u using wallhack?"), Name.c_str()); break;
	case 289: sprintf(pData, XorString("nice radarhack, %s"), Name.c_str()); break;
	case 290: sprintf(pData, XorString("using EZfrags, %s ?"), Name.c_str()); break;
	case 291: sprintf(pData, XorString("u know Aimware, %s ?"), Name.c_str()); break;
	case 292: sprintf(pData, XorString("u sell hek, %s ?"), Name.c_str()); break;
	case 293: sprintf(pData, XorString("%s, add me for buy hack"), Name.c_str()); break;
	case 294: sprintf(pData, XorString("why u added me, %s ?"), Name.c_str()); break;
	case 295: sprintf(pData, XorString("%s, why added me on Steam?"), Name.c_str()); break;
	case 296: sprintf(pData, XorString("%s, stop adding me.."), Name.c_str()); break;
	case 297: sprintf(pData, XorString("%s, i hate u"), Name.c_str()); break;
	case 298: sprintf(pData, XorString("du König, %s ?"), Name.c_str()); break;
	case 299: sprintf(pData, XorString("w2lekum salam, %s ?"), Name.c_str()); break;
	case 300: sprintf(pData, XorString("SA, %s"), Name.c_str()); break;
	case 301: sprintf(pData, XorString("salam-alaikum, %s"), Name.c_str()); break;
	case 302: sprintf(pData, XorString("u pray to buddha, %s ?"), Name.c_str()); break;
	case 303: sprintf(pData, XorString("u like Buddha, %s ?"), Name.c_str()); break;
	case 304: sprintf(pData, XorString("whats ur religion, %s ?"), Name.c_str()); break;
	case 305: sprintf(pData, XorString("where u work, %s ?"), Name.c_str()); break;
	case 306: sprintf(pData, XorString("u work at brazzers, %s ?"), Name.c_str()); break;
	case 307: sprintf(pData, XorString("u porn actor, %s ?"), Name.c_str()); break;
	case 308: sprintf(pData, XorString("%s, ur mom = porn actress?"), Name.c_str()); break;
	case 309: sprintf(pData, XorString("%s, u like to watch Porn?"), Name.c_str()); break;
	case 310: sprintf(pData, XorString("%s, u lesbian or gay?"), Name.c_str()); break;
	case 311: sprintf(pData, XorString("%s, how many dicks u tasted?"), Name.c_str()); break;
	case 312: sprintf(pData, XorString("fuck off.. %s"), Name.c_str()); break;
	case 313: sprintf(pData, XorString("ПОКОЙСЯ С МИРОМ, %s"), Name.c_str()); break;
	case 314: sprintf(pData, XorString("%s, ПОКОЙСЯ С МИРОМ!"), Name.c_str()); break;
	case 315: sprintf(pData, XorString("%s, ЧЕЛОВЕК КОТОРЫЙ РОЖДАЕТСЯ РАЗ В ЖИЗНИ.."), Name.c_str()); break;
	case 316: sprintf(pData, XorString("ПЕРЕДАЮЩАЯ РЕАЛИЮ ЧЕРЕЗ ЭМОЦИЮ, %s ?"), Name.c_str()); break;
	case 317: sprintf(pData, XorString("НИКТО НЕ ВЕЧЕН, %s.."), Name.c_str()); break;
	case 318: sprintf(pData, XorString("%s, НИКТО НЕ ВЕЧЕН.."), Name.c_str()); break;
	case 319: sprintf(pData, XorString("УНИКАЛЕН И УНИВЕРСАЛЕН, %s?"), Name.c_str()); break;
	case 320: sprintf(pData, XorString("%s, УНИКАЛЕН И УНИВЕРСАЛЕН.."), Name.c_str()); break;
	case 321: sprintf(pData, XorString("%s ЖЖЁТ !!"), Name.c_str()); break;
	case 322: sprintf(pData, XorString("БОЛЬШЕ И БОЛЬШЕ, %s.."), Name.c_str()); break;
	case 323: sprintf(pData, XorString("ДЕБИЛЫ ОЧЕВИДНО, %s ?"), Name.c_str()); break;
	case 324: sprintf(pData, XorString("A ЧЕЛОВЕК ТО КАКОЙ БЫЛ, %s.."), Name.c_str()); break;
	case 325: sprintf(pData, XorString("A %s МОЙ КУМИР!"), Name.c_str()); break;
	case 326: sprintf(pData, XorString("ШИПОВАННЫЕ КОЛЕСА, %s ?"), Name.c_str()); break;
	case 327: sprintf(pData, XorString("%s, НАСТРОЕНИЕ ПОДНИМАЕТ!"), Name.c_str()); break;
	case 328: sprintf(pData, XorString("НАСТРОЕНИЕ ПОДНИМАЕТ, %s"), Name.c_str()); break;
	case 329: sprintf(pData, XorString("ОТЛИЧНО, %s!"), Name.c_str()); break;
	case 330: sprintf(pData, XorString("%s, ОТЛИЧНО!"), Name.c_str()); break;
	case 331: sprintf(pData, XorString("МОЛОДЕЦ, %s !"), Name.c_str()); break;
	case 332: sprintf(pData, XorString("%s, МОЛОДЕЦ!"), Name.c_str()); break;
	case 333: sprintf(pData, XorString("ВЕСЁЛАЯ РАБОТА %s, ПОНРАВИЛОСЬ!"), Name.c_str()); break;
	case 334: sprintf(pData, XorString("ВЕСЁЛАЯ РАБОТА %s!"), Name.c_str()); break;
	case 335: sprintf(pData, XorString("%s, ВЕСЁЛАЯ РАБОТА!"), Name.c_str()); break;
	case 336: sprintf(pData, XorString("КОНЕЧНО МНЕ ПОНРАВИЛОСЬ, %s"), Name.c_str()); break;
	case 337: sprintf(pData, XorString("%s, КОНЕЧНО МНЕ ПОНРАВИЛОСЬ!"), Name.c_str()); break;
	case 338: sprintf(pData, XorString("%s, НУ ДА, КОНЕЧНО"), Name.c_str()); break;
	case 339: sprintf(pData, XorString("НУ ДА, КОНЕЧНО, %s"), Name.c_str()); break;
	case 340: sprintf(pData, XorString("%s, ВНЕЗАПНО, ДА?"), Name.c_str()); break;
	case 341: sprintf(pData, XorString("КНИГИ ЧИТАТЬ НУЖНО, %s.."), Name.c_str()); break;
	case 342: sprintf(pData, XorString("%s, КНИГИ ЧИТАТЬ НУЖНО.."), Name.c_str()); break;
	case 343: sprintf(pData, XorString("Yarı melek, yarı canavar, %s ?"), Name.c_str()); break;
	case 344: sprintf(pData, XorString("%s, yarı melek, yarı canavar?"), Name.c_str()); break;
	case 345: sprintf(pData, XorString("bir anda, %s ?"), Name.c_str()); break;
	case 346: sprintf(pData, XorString("%s, bir anda.. ?"), Name.c_str()); break;
	case 347: sprintf(pData, XorString("ve yine %s.."), Name.c_str()); break;
	case 348: sprintf(pData, XorString("Hadi kazı mezarını, %s"), Name.c_str()); break;
	case 349: sprintf(pData, XorString("%s, hadi kazı mezarını"), Name.c_str()); break;
	case 350: sprintf(pData, XorString("Ahmaklar uluyor, %s.."), Name.c_str()); break;
	case 351: sprintf(pData, XorString("%s, ahmaklar uluyor"), Name.c_str()); break;
	case 352: sprintf(pData, XorString("Acıyı hisset, %s!"), Name.c_str()); break;
	case 353: sprintf(pData, XorString("%s, acıyı hisset!"), Name.c_str()); break;
	case 354: sprintf(pData, XorString("reportbotted %s"), Name.c_str()); break;
	case 355: sprintf(pData, XorString("reportbot for %s"), Name.c_str()); break;
	case 356: sprintf(pData, XorString("report for %s submitted"), Name.c_str()); break;
	case 357: sprintf(pData, XorString("pls report %s for hack"), Name.c_str()); break;
	case 358: sprintf(pData, XorString("stop griefing %s"), Name.c_str()); break;
	case 359: sprintf(pData, XorString("go kick %s pls"), Name.c_str()); break;
	case 360: sprintf(pData, XorString("kick %s"), Name.c_str()); break;
	case 361: sprintf(pData, XorString("!ban %s"), Name.c_str()); break;
	case 362: sprintf(pData, XorString("!kick %s Hack"), Name.c_str()); break;
	case 363: sprintf(pData, XorString("!ban %s Hacks"), Name.c_str()); break;
	case 364: sprintf(pData, XorString("!slay %s"), Name.c_str()); break;
	case 365: sprintf(pData, XorString("%s, tanz mit mir?"), Name.c_str()); break;
	case 366: sprintf(pData, XorString("tanz mit mir, %s ?"), Name.c_str()); break;
	case 367: sprintf(pData, XorString("%s, tanz mit mir der galgentanz?"), Name.c_str()); break;
	case 368: sprintf(pData, XorString("%s, ТЫ СУКА?"), Name.c_str()); break;
	case 369: sprintf(pData, XorString("ТЫ СУКА, %s ?"), Name.c_str()); break;
	case 370: sprintf(pData, XorString("%s, КТО СУКА?"), Name.c_str()); break;
	case 371: sprintf(pData, XorString("КТО СУКА, %s ?"), Name.c_str()); break;
	case 372: sprintf(pData, XorString("КТО ШЛЮХА, %s ?"), Name.c_str()); break;
	case 373: sprintf(pData, XorString("ТЫ ШЛЮХА, %s ?"), Name.c_str()); break;
	case 374: sprintf(pData, XorString("%s, КТО ШЛЮХА?"), Name.c_str()); break;
	case 375: sprintf(pData, XorString("%s, ТЫ ШЛЮХА ?"), Name.c_str()); break;
	case 376: sprintf(pData, XorString("%s, КАК ШЛЮХА.."), Name.c_str()); break;
	case 377: sprintf(pData, XorString("%s, u manager @brazzers?"), Name.c_str()); break;
	case 378: sprintf(pData, XorString("%s, u boss at brazzers?"), Name.c_str()); break;
	case 379: sprintf(pData, XorString("%s, u moderator at brazzers?"), Name.c_str()); break;
	case 380: sprintf(pData, XorString("%s, i saw ur mom @livejasmin man"), Name.c_str()); break;
	case 381: sprintf(pData, XorString("%s, mby add me for buy hack?"), Name.c_str()); break;
	case 382: sprintf(pData, XorString("%s, ХУЙ ЗНАЕШЬ?"), Name.c_str()); break;
	case 383: sprintf(pData, XorString("ХУЙ ЗНАЕШЬ, %s ?"), Name.c_str()); break;
	case 384: sprintf(pData, XorString("%s, u like horses?"), Name.c_str()); break;
	case 385: sprintf(pData, XorString("%s, ur mom fucks hard?"), Name.c_str()); break;
	case 386: sprintf(pData, XorString("ur mom fucks hard, %s ?"), Name.c_str()); break;
	case 387: sprintf(pData, XorString("LOL, %s"), Name.c_str()); break;
	case 388: sprintf(pData, XorString("%s, LOL"), Name.c_str()); break;
	case 389: sprintf(pData, XorString("why hack, %s ?"), Name.c_str()); break;
	case 390: sprintf(pData, XorString("%s, БОЛЬШОЙ ПЕНИС?"), Name.c_str()); break;
	};
};

INT FilterClanTag(ImGuiTextEditCallbackData * pData)
{
	if (pData->EventChar >= 'A' && pData->EventChar <= 'Z')
	{

	}

	else if (pData->EventChar >= 'a' && pData->EventChar <= 'z')
	{

	}

	else if (pData->EventChar == '0' || (pData->EventChar >= '1' && pData->EventChar <= '9'))
	{

	}

	else if (pData->EventChar == '!' || pData->EventChar == '@' || pData->EventChar == '#' || pData->EventChar == '$' || pData->EventChar == '^' \
		|| pData->EventChar == '&' || pData->EventChar == '*' || pData->EventChar == '(' || pData->EventChar == ')' || pData->EventChar == '-' \
		|| pData->EventChar == '_' || pData->EventChar == ' ' || pData->EventChar == '=' || pData->EventChar == '+' || pData->EventChar == '[' \
		|| pData->EventChar == ']' || pData->EventChar == '|' || pData->EventChar == ',' || pData->EventChar == '.' || pData->EventChar == '?' \
		|| pData->EventChar == '<' || pData->EventChar == '>' || pData->EventChar == '/')
	{

	}

	else
		pData->EventChar = '\0';

	if (strlen(g_szClanTag) >= 12)
	{
		pData->EventChar = '\0';

		return 1;
	};

	return 0;
};

VOID SetClanTag(VOID)
{
	static INT iBaseLen = 0, iCurLen = 0, iIter = 0;
	static time_t tStamp = 0, tNow = 0;
	static DWORD dwClanTag = {};

	if (dwClanTag == 0)
		dwClanTag = FindPattern(XorString("engine.dll"), XorString("53 56 57 8B DA 8B F9 FF 15"));

	if (dwClanTag == 0)
		return;

	static auto pSetClanTag = reinterpret_cast <VOID(__fastcall *) (CONST CHAR *, CONST CHAR *)> (dwClanTag);

	if (!pSetClanTag)
		return;

	tNow = time(NULL);

	if (tNow - tStamp > 0)
	{
		iBaseLen = strlen(g_szClanTag);
		iCurLen = strlen(g_szFormattedClanTag);

		if (iBaseLen < 1)
		{
			tStamp = tNow;

			return;
		};

		if (iBaseLen > 12)
			g_szClanTag[12] = '\0';

		if (iCurLen > iBaseLen || iCurLen == iBaseLen || iCurLen == 0)
		{
			g_szFormattedClanTag[0] = g_szClanTag[0];
			g_szFormattedClanTag[1] = '\0';

			pSetClanTag(g_szFormattedClanTag, XorString("xClanTag"));

			tStamp = tNow;

			return;
		};

		g_szFormattedClanTag[iCurLen] = g_szClanTag[iCurLen];
		g_szFormattedClanTag[iCurLen + 1] = '\0';

		pSetClanTag(g_szFormattedClanTag, XorString("xClanTag"));

		tStamp = tNow;
	};
};
