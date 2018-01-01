
#include "Misc.h"

VOID CConfig::Setup(VOID)
{

#if defined(ENABLE_RAGE_BOT) && ENABLE_RAGE_BOT == 1

	SetupVal(Vars.Ragebot.Enabled, false, XorString("RageBOT"), XorString("bON"));
	SetupVal(Vars.Ragebot.AutoFire, false, XorString("RageBOT"), XorString("bAutoFire"));
	SetupVal(Vars.Ragebot.extrapolationMethod, 0, XorString("RageBOT"), XorString("iExtrapolationMethod"));
	SetupVal(Vars.Ragebot.FOV, 180.f, XorString("RageBOT"), XorString("fFOV"));
	SetupVal(Vars.Ragebot.bAutoWall, true, XorString("RageBOT"), XorString("bAutoWall"));
	SetupVal(Vars.Ragebot.AutoScope, true, XorString("RageBOT"), XorString("bAutoScope"));

#endif

	SetupVal(Vars.Legitbot.Aimbot.Enabled, false, XorString("AimBOT"), XorString("bON"));
	SetupVal(Vars.Legitbot.Aimbot.bNonSilent, false, XorString("AimBOT"), XorString("bNonSilent"));
	SetupVal(Vars.Legitbot.Aimbot.FOV, 1.f, XorString("AimBOT"), XorString("fFOV"));
	SetupVal(Vars.Legitbot.Aimbot.FOV_Pistols, .6f, XorString("AimBOT"), XorString("fPistolsFOV"));

	SetupVal(Vars.Legitbot.Triggerbot.Enabled, false, XorString("TriggerBOT"), XorString("bON"));
	SetupVal(Vars.Legitbot.Triggerbot.AutoFire, false, XorString("TriggerBOT"), XorString("bAutoFire"));
	SetupVal(Vars.Legitbot.Triggerbot.fSecondsDelay, .3f, XorString("TriggerBOT"), XorString("fSecondsDelay"));
	SetupVal(Vars.Legitbot.Triggerbot.Key, VK_LBUTTON, XorString("TriggerBOT"), XorString("iKey"));
	SetupVal(Vars.Legitbot.Triggerbot.Filter.Head, true, XorString("TriggerBOT"), XorString("bFilterHead"));
	SetupVal(Vars.Legitbot.Triggerbot.Filter.Chest, true, XorString("TriggerBOT"), XorString("bFilterChest"));
	SetupVal(Vars.Legitbot.Triggerbot.Filter.Stomach, true, XorString("TriggerBOT"), XorString("bFilterStomach"));
	SetupVal(Vars.Legitbot.Triggerbot.Filter.Arms, true, XorString("TriggerBOT"), XorString("bFilterArms"));
	SetupVal(Vars.Legitbot.Triggerbot.Filter.Legs, true, XorString("TriggerBOT"), XorString("bFilterLegs"));
	SetupVal(Vars.Legitbot.Triggerbot.Filter.Weapons, true, XorString("TriggerBOT"), XorString("bWeapons"));
	SetupVal(Vars.Legitbot.Triggerbot.Filter.Snipers, true, XorString("TriggerBOT"), XorString("bSnipers"));
	SetupVal(Vars.Legitbot.Triggerbot.Filter.StandSnipers, true, XorString("TriggerBOT"), XorString("bStandSnipers"));
	SetupVal(Vars.Legitbot.Triggerbot.Filter.StandWeapons, true, XorString("TriggerBOT"), XorString("bStandWeapons"));

	SetupVal(Vars.Visuals.Enabled, false, XorString("Visuals"), XorString("bON"));
	SetupVal(Vars.Visuals.Skeleton, false, XorString("Visuals"), XorString("bSkeleton"));
	SetupVal(Vars.Visuals.iDroppedWeaponsGlowColor, 13, XorString("Visuals"), XorString("iDroppedWeaponsGlowColor"));
	SetupVal(Vars.Visuals.iEnemiesGlowColor, 2, XorString("Visuals"), XorString("iVisibleEnemiesGlowColor"));
	SetupVal(Vars.Visuals.iTeammatesGlowColor, 3, XorString("Visuals"), XorString("iVisibleTeammatesGlowColor"));
	SetupVal(Vars.Visuals.iEnemiesOGlowColor, 0, XorString("Visuals"), XorString("iEnemiesOGlowColor"));
	SetupVal(Vars.Visuals.iTeammatesOGlowColor, 0, XorString("Visuals"), XorString("iTeammatesOGlowColor"));
	SetupVal(Vars.Visuals.Glow, true, XorString("Visuals"), XorString("bGlow"));
	SetupVal(Vars.Visuals.fGlowBloomAmount, .5f, XorString("Visuals"), XorString("fGlowBloomAmount"));
	SetupVal(Vars.Visuals.iGlowAlphaIntensity, 255, XorString("Visuals"), XorString("iGlowAlphaIntensity"));
	SetupVal(Vars.Visuals.RadarHack, true, XorString("Visuals"), XorString("bRadarHack"));
	SetupVal(Vars.Visuals.Filter.Enemies, true, XorString("Visuals"), XorString("bFilterEnemies"));
	SetupVal(Vars.Visuals.Filter.Friendlies, false, XorString("Visuals"), XorString("bFilterFriendlies"));
	SetupVal(Vars.Visuals.Filter.Weapons, true, XorString("Visuals"), XorString("bFilterWeapons"));
	SetupVal(Vars.Visuals.Filter.Decoy, true, XorString("Visuals"), XorString("bFilterDecoy"));
	SetupVal(Vars.Visuals.Filter.C4, true, XorString("Visuals"), XorString("bFilterC4"));
	SetupVal(Vars.Visuals.Filter.Hostages, true, XorString("Visuals"), XorString("bFilterHostages"));

	SetupVal(Vars.Misc.Ranks, true, XorString("Misc"), XorString("bRanks"));
	SetupVal(Vars.Misc.bGrenTrace, true, XorString("Misc"), XorString("bGrenTrace"));
	SetupVal(Vars.Misc.bDoNotEnforceBestPingLagSettings, false, XorString("Misc"), XorString("bDoNotEnforceBestPingLagSettings"));
	SetupVal(Vars.Misc.bAutoSniperCrosshair, true, XorString("Misc"), XorString("bAutoSniperCrosshair"));
	SetupVal(Vars.Misc.SniperCrosshairColor, 1, XorString("Misc"), XorString("SniperCrosshairColor"));
	SetupVal(Vars.Misc.bGrenadePrediction, true, XorString("Misc"), XorString("bGrenadePrediction"));
	SetupVal(Vars.Misc.bDroppedWeaponsName, true, XorString("Misc"), XorString("bDroppedWeaponsName"));
	SetupVal(Vars.Misc.bRecoilCrosshair, false, XorString("Misc"), XorString("bRecoilCrosshair"));
	SetupVal(Vars.Misc.iRecoilCrosshairType, 0, XorString("Misc"), XorString("iRecoilCrosshairType"));
	SetupVal(Vars.Misc.bBombSiteSkeletons, true, XorString("Misc"), XorString("bBombSiteSkeletons"));
	SetupVal(Vars.Misc.bDoNotEditRadarScale, false, XorString("Misc"), XorString("bDoNotEditRadarScale"));
	SetupVal(Vars.Misc.bSpam, false, XorString("Misc"), XorString("bSpam"));
	SetupVal(Vars.Misc.bUseLegitVisualFeatures, true, XorString("Misc"), XorString("bUseLegitVisualFeatures"));
	SetupVal(Vars.Misc.bBunnyHop, false, XorString("Misc"), XorString("bBunnyHop"));
	SetupVal(Vars.Misc.bAutoLeftRight, false, XorString("Misc"), XorString("bAutoLeftRight"));
	SetupVal(Vars.Misc.bAutoStrafe, false, XorString("Misc"), XorString("bAutoStrafe"));
	SetupVal(Vars.Misc.bSpecialFlickShots, true, XorString("Misc"), XorString("bSpecialFlickShots"));
};

#if defined(ENABLE_RAGE_BOT) && ENABLE_RAGE_BOT == 1

VOID CConfig::LoadRageBase(VOID)
{
	Vars.Ragebot.Enabled = true;
	Vars.Ragebot.AutoFire = false;
	Vars.Ragebot.FOV = 180.f;
	Vars.Ragebot.bAutoWall = false;
	Vars.Ragebot.AutoScope = false;

	Vars.Legitbot.Aimbot.Enabled = false;
	Vars.Legitbot.Triggerbot.Enabled = false;

	Vars.Visuals.RadarHack = true;
	Vars.Visuals.Enabled = true;
	Vars.Visuals.Skeleton = false;
	Vars.Visuals.Glow = true;
	Vars.Visuals.Filter.Enemies = true;
	Vars.Visuals.Filter.Friendlies = false;
	Vars.Visuals.Filter.Weapons = true;
	Vars.Visuals.Filter.Decoy = true;
	Vars.Visuals.Filter.C4 = true;
	Vars.Visuals.Filter.Hostages = true;

	Vars.Misc.bBunnyHop = true;
	Vars.Misc.Ranks = true;
	Vars.Misc.bAutoSniperCrosshair = true;
	Vars.Misc.bGrenTrace = true;
	Vars.Misc.bGrenadePrediction = true;
	Vars.Misc.bSpecialFlickShots = true;
};

#endif

VOID CConfig::LoadLegitBase(VOID)
{

#if defined(ENABLE_RAGE_BOT) && ENABLE_RAGE_BOT == 1

	Vars.Ragebot.Enabled = false;

#endif

	Vars.Legitbot.Aimbot.Enabled = true;
	Vars.Legitbot.Aimbot.FOV = .6f;
	Vars.Legitbot.Aimbot.FOV_Pistols = .6f;

	Vars.Legitbot.Triggerbot.Enabled = true;
	Vars.Legitbot.Triggerbot.AutoFire = true;
	Vars.Legitbot.Triggerbot.Filter.Head = true;
	Vars.Legitbot.Triggerbot.Filter.Chest = true;

	Vars.Legitbot.Triggerbot.Filter.Stomach = true;
	Vars.Legitbot.Triggerbot.Filter.Arms = true;
	Vars.Legitbot.Triggerbot.Filter.Legs = true;
	Vars.Legitbot.Triggerbot.Filter.StandSnipers = true;
	Vars.Legitbot.Triggerbot.Filter.StandWeapons = false;
	Vars.Legitbot.Triggerbot.Filter.Weapons = true;
	Vars.Legitbot.Triggerbot.Filter.Snipers = true;
	Vars.Legitbot.Triggerbot.fSecondsDelay = .3f;

	Vars.Visuals.Enabled = false;

	Vars.Misc.bBunnyHop = true;
	Vars.Misc.bUseLegitVisualFeatures = true;
	Vars.Misc.bSpecialFlickShots = true;
	Vars.Misc.Ranks = true;
	Vars.Misc.bAutoSniperCrosshair = true;
	Vars.Misc.bGrenTrace = true;
	Vars.Misc.bGrenadePrediction = true;
};

VOID CConfig::SetupVal(INT & iVal, INT iDef, std::string Cat, std::string Key)
{
	std::string Data = std::to_string(iDef);
	std::string Prepared = Cat;
	
	Prepared.append(XorString("."));
	Prepared.append(Key);
	g_JsonAimWaveCFGTree[Prepared] = Data;
	iVal = iDef;
	m_pInts.push_back(new CFGVal<INT>(Cat, Key, &iVal));
};

VOID CConfig::SetupVal(FLOAT & fVal, FLOAT fDef, std::string Cat, std::string Key)
{
	std::string Data = std::to_string(fDef);
	std::string Prepared = Cat;

	Prepared.append(XorString("."));
	Prepared.append(Key);
	g_JsonAimWaveCFGTree[Prepared] = Data;
	fVal = fDef;
	m_pFloats.push_back(new CFGVal<FLOAT>(Cat, Key, &fVal));
};

VOID CConfig::SetupVal(bool & bVal, bool bDef, std::string Cat, std::string Name)
{
	std::string Prepared = Cat;

	Prepared.append(XorString("."));
	Prepared.append(Name);
	g_JsonAimWaveCFGTree[Prepared] = bDef ? XorString("True") : XorString("False");
	bVal = bDef;
	m_pBools.push_back(new CFGVal<bool>(Cat, Name, &bVal));
};

VOID CConfig::Save(VOID)
{
	static std::ofstream FileHndl;
	static CHAR Data[16] = {};
	static INT iId = 0;

	JWPPS(XorString("Misc"), XorString("szClanTag"), g_szClanTag);

	for (iId = 0; iId < m_pInts.size(); iId++)
	{
		JWPPS(m_pInts.at(iId)->m_Cat.c_str(), m_pInts.at(iId)->m_Key.c_str(), std::to_string(*m_pInts.at(iId)->m_pVal).c_str());
	};

	for (iId = 0; iId < m_pFloats.size(); iId++)
	{
		JWPPS(m_pFloats.at(iId)->m_Cat.c_str(), m_pFloats.at(iId)->m_Key.c_str(), std::to_string(*m_pFloats.at(iId)->m_pVal).c_str());
	};

	for (iId = 0; iId < m_pBools.size(); iId++)
	{
		JWPPS(m_pBools.at(iId)->m_Cat.c_str(), m_pBools.at(iId)->m_Key.c_str(), *m_pBools.at(iId)->m_pVal ? XorString("True") : XorString("False"));
	};

	FileHndl.open(XorString("HK.JSON.INI"), std::ofstream::trunc);

	if (FileHndl.is_open())
	{
		FileHndl << g_JsonStyledWriter.write(g_JsonAimWaveCFGTree);

		if (FileHndl.is_open())
		{
			FileHndl.close();
		};
	};
};

VOID CConfig::Load(VOID)
{
	static CHAR szTemp[MAX_PATH / 2] = {};
	static std::ifstream FileHndl;
	static INT iId = 0;

	FileHndl.open(XorString("HK.JSON.INI"), std::ifstream::in);

	if (FileHndl.is_open())
	{
		g_JsonAimWaveCFGTree.clear();
		g_JsonReader.parse(FileHndl, g_JsonAimWaveCFGTree, false);

		if (FileHndl.is_open())
		{
			FileHndl.close();
		};
	};

	for (iId = 0; iId < m_pInts.size(); iId++)
	{
		JGPPS(m_pInts.at(iId)->m_Cat.c_str(), m_pInts.at(iId)->m_Key.c_str(), szTemp, sizeof szTemp);

		*m_pInts.at(iId)->m_pVal = atoi(szTemp);
	};

	for (iId = 0; iId < m_pFloats.size(); iId++)
	{
		JGPPS(m_pFloats.at(iId)->m_Cat.c_str(), m_pFloats.at(iId)->m_Key.c_str(), szTemp, sizeof szTemp);

		*m_pFloats.at(iId)->m_pVal = atof(szTemp);
	};

	for (iId = 0; iId < m_pBools.size(); iId++)
	{
		JGPPS(m_pBools.at(iId)->m_Cat.c_str(), m_pBools.at(iId)->m_Key.c_str(), szTemp, sizeof szTemp);

		*m_pBools.at(iId)->m_pVal = !_stricmp(szTemp, XorString("True"));
	};

	JGPPS(XorString("Misc"), XorString("szClanTag"), g_szClanTag, ARRAYSIZE(g_szClanTag));

	if (strlen(g_szClanTag) > 12)
		g_szClanTag[12] = '\0';
};
