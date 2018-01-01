
#include "Misc.h"

LONG WINAPI EndScene(IDirect3DDevice9 * pDevice)
{
	static INT iX = 0, iY = 0;

	if (!Vars.Menu.Opened)
	{
		if (G::bD3DOK)
			ImGui::GetIO().MouseDrawCursor = false;

		return oEndScene(pDevice);
	};

	if (!G::bD3DOK)
		CreateGUI(pDevice);

	ImGuiIO & X = ImGui::GetIO();
	ImGui_ImplDX9_NewFrame();

	I::InputSys->GetCursorPosition(&iX, &iY), X.MousePos.x = (FLOAT)(iX), X.MousePos.y = (FLOAT)(iY), X.MouseDrawCursor = true;

	ImGui::SetNextWindowSize(ImVec2(800, 500));
	ImGui::Begin(XorString("CARAMEL BETA by Hattrick HKS"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
	ImGui::Columns(4, nullptr, false);
	ImGui::Text(XorString("EZ Set-Up"));
	ImGui::Text(XorString("Very Safe Modes (Match-Making, ..)"));
	if (ImGui::Button(XorString("Mode Competitive 0.6"), ImVec2(140.f, 20.f))) Config->LoadLegitBase(), Vars.Legitbot.Aimbot.FOV = .6f;
	if (ImGui::Button(XorString("Mode Competitive 0.8"), ImVec2(140.f, 20.f))) Config->LoadLegitBase(), Vars.Legitbot.Aimbot.FOV = .8f;
	if (ImGui::Button(XorString("Mode Competitive 1.0"), ImVec2(140.f, 20.f))) Config->LoadLegitBase(), Vars.Legitbot.Aimbot.FOV = 1.f;
	if (ImGui::Button(XorString("Mode Competitive 1.2"), ImVec2(140.f, 20.f))) Config->LoadLegitBase(), Vars.Legitbot.Aimbot.FOV = 1.2f;

#if defined(ENABLE_RAGE_BOT) && ENABLE_RAGE_BOT == 1

	ImGui::Text(XorString("Rage Modes (Not For Match-Making)"));
	if (ImGui::Button(XorString("Mode Rage 1.0"), ImVec2(140.f, 20.f))) Config->LoadRageBase(), Vars.Ragebot.AutoFire = true, Vars.Ragebot.FOV = 1.f;
	if (ImGui::Button(XorString("Mode Rage 1.5"), ImVec2(140.f, 20.f))) Config->LoadRageBase(), Vars.Ragebot.AutoFire = true, Vars.Ragebot.FOV = 1.5f;
	if (ImGui::Button(XorString("Mode Rage 2.0"), ImVec2(140.f, 20.f))) Config->LoadRageBase(), Vars.Ragebot.AutoFire = true, Vars.Ragebot.FOV = 2.f;
	if (ImGui::Button(XorString("Mode Rage 3.0"), ImVec2(140.f, 20.f))) Config->LoadRageBase(), Vars.Ragebot.AutoFire = true, Vars.Ragebot.FOV = 3.f;
	if (ImGui::Button(XorString("Mode Rage+ 60"), ImVec2(140.f, 20.f))) Config->LoadRageBase(), Vars.Ragebot.AutoScope = true, Vars.Ragebot.bAutoWall = true, Vars.Ragebot.AutoFire = true, Vars.Ragebot.FOV = 60.f;
	if (ImGui::Button(XorString("Mode Rage+ 90"), ImVec2(140.f, 20.f))) Config->LoadRageBase(), Vars.Ragebot.AutoScope = true, Vars.Ragebot.bAutoWall = true, Vars.Ragebot.AutoFire = true, Vars.Ragebot.FOV = 90.f;
	if (ImGui::Button(XorString("Mode Rage+ 120"), ImVec2(140.f, 20.f))) Config->LoadRageBase(), Vars.Ragebot.AutoScope = true, Vars.Ragebot.bAutoWall = true, Vars.Ragebot.AutoFire = true, Vars.Ragebot.FOV = 120.f;
	if (ImGui::Button(XorString("Mode Rage+ 180"), ImVec2(140.f, 20.f))) Config->LoadRageBase(), Vars.Ragebot.AutoScope = true, Vars.Ragebot.bAutoWall = true, Vars.Ragebot.AutoFire = true, Vars.Ragebot.FOV = 180.f;

#endif

	ImGui::Text(XorString("Non-Aim Modes (Match-Making, ..)"));
	if (ImGui::Button(XorString("Mode Radar Only"), ImVec2(140.f, 20.f))) Config->LoadLegitBase(), Vars.Legitbot.Aimbot.Enabled = false, Vars.Legitbot.Triggerbot.Enabled = false;
	if (ImGui::Button(XorString("Mode Radar + Trigger"), ImVec2(140.f, 20.f))) Config->LoadLegitBase(), Vars.Legitbot.Aimbot.Enabled = false;
	ImGui::Text(XorString("Save/ Load Configuration Files"));
	if (FLOAT(((DOUBLE)clock() - g_saveTime) / CLOCKS_PER_SEC) < FLOAT(1.F)) ImGui::Button(XorString("SAVED"), ImVec2(140.f, 20.f));
	else
	{
		if (ImGui::Button(XorString("SAVE"), ImVec2(140.f, 20.f)))
			Config->Save(), g_saveTime = clock();
	};
	if (FLOAT(((DOUBLE)clock() - g_loadTime) / CLOCKS_PER_SEC) < FLOAT(1.F)) ImGui::Button(XorString("LOADED"), ImVec2(140.f, 20.f));
	else
	{
		if (ImGui::Button(XorString("LOAD"), ImVec2(140.f, 20.f)))
			Config->Load(), g_loadTime = clock();
	};
	ImGui::NextColumn();

#if defined(ENABLE_RAGE_BOT) && ENABLE_RAGE_BOT == 1

	if (!Vars.Ragebot.Enabled)
	{
		ImGui::Text(XorString("Legit Set-Up"));
		ImGui::Checkbox(XorString("Aim BOT"), &Vars.Legitbot.Aimbot.Enabled);
		ImGui::Checkbox(XorString("Non-Silent"), &Vars.Legitbot.Aimbot.bNonSilent);
		ImGui::SliderFloat(XorString("Weapons FoV"), &Vars.Legitbot.Aimbot.FOV, .1f, 3.f, XorString("%.1f"));
		ImGui::SliderFloat(XorString("Pistols FoV"), &Vars.Legitbot.Aimbot.FOV_Pistols, .1f, 3.f, XorString("%.1f"));
		ImGui::Checkbox(XorString("Trigger BOT"), &Vars.Legitbot.Triggerbot.Enabled);
		ImGui::Checkbox(XorString("Auto-Fire"), &Vars.Legitbot.Triggerbot.AutoFire);
		ImGui::Combo(XorString("Hold Key"), &Vars.Legitbot.Triggerbot.Key, g_pTriggerKeys, ARRAYSIZE(g_pTriggerKeys));
		ImGui::SliderFloat(XorString("Seconds"), &Vars.Legitbot.Triggerbot.fSecondsDelay, .1f, 1.f, XorString("%.1f"));
		ImGui::Text(XorString("Snipers/ Pistols Hit Boxes"));
		ImGui::Checkbox(XorString("Head"), &Vars.Legitbot.Triggerbot.Filter.Head);
		ImGui::Checkbox(XorString("Chest"), &Vars.Legitbot.Triggerbot.Filter.Chest);
		ImGui::Checkbox(XorString("Stomach"), &Vars.Legitbot.Triggerbot.Filter.Stomach);
		ImGui::Checkbox(XorString("Arms"), &Vars.Legitbot.Triggerbot.Filter.Arms);
		ImGui::Checkbox(XorString("Legs"), &Vars.Legitbot.Triggerbot.Filter.Legs);
		ImGui::Text(XorString("Weapons Included"));
		ImGui::Checkbox(XorString("Include Snipers"), &Vars.Legitbot.Triggerbot.Filter.Snipers);
		ImGui::Checkbox(XorString("Include Weapons"), &Vars.Legitbot.Triggerbot.Filter.Weapons);
		ImGui::Text(XorString("Must Stand Still For"));
		ImGui::Checkbox(XorString("Snipers"), &Vars.Legitbot.Triggerbot.Filter.StandSnipers);
		ImGui::Checkbox(XorString("Weapons"), &Vars.Legitbot.Triggerbot.Filter.StandWeapons);
	}
	else
	{
		ImGui::Text(XorString("Rage Set-Up"));
		ImGui::Checkbox(XorString("Rage Aim BOT"), &Vars.Ragebot.Enabled);
		ImGui::Combo(XorString("Extrapolation"), &Vars.Ragebot.extrapolationMethod, g_pExtrapolationOptions, ARRAYSIZE(g_pExtrapolationOptions));
		ImGui::Checkbox(XorString("Auto-Shoot"), &Vars.Ragebot.AutoFire);
		ImGui::Checkbox(XorString("Auto-Zoom"), &Vars.Ragebot.AutoScope);
		ImGui::SliderFloat(XorString("FoV"), &Vars.Ragebot.FOV, .1f, 180.f, XorString("%.1f"));
		ImGui::Checkbox(XorString("Auto-Wall"), &Vars.Ragebot.bAutoWall);
	};

#else

	ImGui::Text(XorString("Legit Set-Up"));
	ImGui::Checkbox(XorString("Aim BOT"), &Vars.Legitbot.Aimbot.Enabled);
	ImGui::Checkbox(XorString("Non-Silent"), &Vars.Legitbot.Aimbot.bNonSilent);
	ImGui::SliderFloat(XorString("Weapons FoV"), &Vars.Legitbot.Aimbot.FOV, .1f, 3.f, XorString("%.1f"));
	ImGui::SliderFloat(XorString("Pistols FoV"), &Vars.Legitbot.Aimbot.FOV_Pistols, .1f, 3.f, XorString("%.1f"));
	ImGui::Checkbox(XorString("Trigger BOT"), &Vars.Legitbot.Triggerbot.Enabled);
	ImGui::Checkbox(XorString("Auto-Fire"), &Vars.Legitbot.Triggerbot.AutoFire);
	ImGui::Combo(XorString("Hold Key"), &Vars.Legitbot.Triggerbot.Key, g_pTriggerKeys, ARRAYSIZE(g_pTriggerKeys));
	ImGui::SliderFloat(XorString("Seconds"), &Vars.Legitbot.Triggerbot.fSecondsDelay, .1f, 1.f, XorString("%.1f"));
	ImGui::Text(XorString("Snipers/ Pistols Hit Boxes"));
	ImGui::Checkbox(XorString("Head"), &Vars.Legitbot.Triggerbot.Filter.Head);
	ImGui::Checkbox(XorString("Chest"), &Vars.Legitbot.Triggerbot.Filter.Chest);
	ImGui::Checkbox(XorString("Stomach"), &Vars.Legitbot.Triggerbot.Filter.Stomach);
	ImGui::Checkbox(XorString("Arms"), &Vars.Legitbot.Triggerbot.Filter.Arms);
	ImGui::Checkbox(XorString("Legs"), &Vars.Legitbot.Triggerbot.Filter.Legs);
	ImGui::Text(XorString("Weapons Included"));
	ImGui::Checkbox(XorString("Include Snipers"), &Vars.Legitbot.Triggerbot.Filter.Snipers);
	ImGui::Checkbox(XorString("Include Weapons"), &Vars.Legitbot.Triggerbot.Filter.Weapons);
	ImGui::Text(XorString("Must Stand Still For"));
	ImGui::Checkbox(XorString("Snipers"), &Vars.Legitbot.Triggerbot.Filter.StandSnipers);
	ImGui::Checkbox(XorString("Weapons"), &Vars.Legitbot.Triggerbot.Filter.StandWeapons);

#endif

	ImGui::NextColumn();
	ImGui::Text(XorString("Wall-Hacks Set-Up"));
	ImGui::Checkbox(XorString("Radar Hack"), &Vars.Visuals.RadarHack);
	ImGui::Checkbox(XorString("Wall Hack"), &Vars.Visuals.Enabled);
	ImGui::Checkbox(XorString("Skeleton"), &Vars.Visuals.Skeleton);
	ImGui::Checkbox(XorString("Glow"), &Vars.Visuals.Glow);
	ImGui::Combo(XorString("Guns"), &Vars.Visuals.iDroppedWeaponsGlowColor, g_pCustomColorOptions, ARRAYSIZE(g_pCustomColorOptions));
	ImGui::Combo(XorString("Inv. Enem."), &Vars.Visuals.iEnemiesGlowColor, g_pCustomColorOptions, ARRAYSIZE(g_pCustomColorOptions));
	ImGui::Combo(XorString("Inv. Team."), &Vars.Visuals.iTeammatesGlowColor, g_pCustomColorOptions, ARRAYSIZE(g_pCustomColorOptions));
	ImGui::Combo(XorString("Vis. Enem."), &Vars.Visuals.iEnemiesOGlowColor, g_pCustomGlowColorOptions, ARRAYSIZE(g_pCustomGlowColorOptions));
	ImGui::Combo(XorString("Vis. Team."), &Vars.Visuals.iTeammatesOGlowColor, g_pCustomGlowColorOptions, ARRAYSIZE(g_pCustomGlowColorOptions));
	ImGui::SliderInt(XorString("Alpha"), &Vars.Visuals.iGlowAlphaIntensity, 1, 255);
	ImGui::SliderFloat(XorString("Bloom"), &Vars.Visuals.fGlowBloomAmount, .1f, 1.f, XorString("%.1f"));
	ImGui::Text(XorString("Visuals"));
	ImGui::Checkbox(XorString("Enemies"), &Vars.Visuals.Filter.Enemies);
	ImGui::Checkbox(XorString("Teammates"), &Vars.Visuals.Filter.Friendlies);
	ImGui::Checkbox(XorString("World Weapons"), &Vars.Visuals.Filter.Weapons);
	ImGui::Checkbox(XorString("Hostages"), &Vars.Visuals.Filter.Hostages);
	ImGui::Checkbox(XorString("Decoy"), &Vars.Visuals.Filter.Decoy);
	ImGui::Checkbox(XorString("C4 Bomb"), &Vars.Visuals.Filter.C4);
	ImGui::NextColumn();
	ImGui::Text(XorString("Miscellaneous Set-Up"));
	ImGui::Checkbox(XorString("Do Not Edit Radar"), &Vars.Misc.bDoNotEditRadarScale);
	ImGui::Checkbox(XorString("Do Not Enforce Ping Settings"), &Vars.Misc.bDoNotEnforceBestPingLagSettings);
	ImGui::Checkbox(XorString("Grenade Trajectory"), &Vars.Misc.bGrenTrace);
	ImGui::Checkbox(XorString("Grenade Prediction"), &Vars.Misc.bGrenadePrediction);
	ImGui::Checkbox(XorString("Fake Sniper Crosshair"), &Vars.Misc.bAutoSniperCrosshair);
	ImGui::Combo(XorString("Color"), &Vars.Misc.SniperCrosshairColor, g_pCustomColorOptions, ARRAYSIZE(g_pCustomColorOptions));
	ImGui::Checkbox(XorString("Recoil Crosshair"), &Vars.Misc.bRecoilCrosshair);
	ImGui::Combo(XorString("Paint"), &Vars.Misc.iRecoilCrosshairType, g_pCustomColorOptions, ARRAYSIZE(g_pCustomColorOptions));
	ImGui::Checkbox(XorString("Chat Spam"), &Vars.Misc.bSpam);
	ImGui::Checkbox(XorString("Dropped Weapons' Name"), &Vars.Misc.bDroppedWeaponsName);
	ImGui::Checkbox(XorString("Bomb Site Enemy Skeletons"), &Vars.Misc.bBombSiteSkeletons);
	ImGui::Checkbox(XorString("C4 Timer"), &Vars.Misc.bUseLegitVisualFeatures);
	ImGui::Checkbox(XorString("Matchmaking Ranks"), &Vars.Misc.Ranks);
	ImGui::Checkbox(XorString("Bunny Hop"), &Vars.Misc.bBunnyHop);
	ImGui::Checkbox(XorString("Auto-Left-Right When Aimed"), &Vars.Misc.bAutoLeftRight);
	ImGui::Checkbox(XorString("Auto-Strafe While In-Air"), &Vars.Misc.bAutoStrafe);
	ImGui::Checkbox(XorString("Special Flick Shots (Snipers)"), &Vars.Misc.bSpecialFlickShots);
	ImGui::InputText("Clan Tag", g_szClanTag, ARRAYSIZE(g_szClanTag), ImGuiInputTextFlags_CallbackCharFilter, FilterClanTag);

	if (strlen(g_szClanTag) > 12)
		g_szClanTag[12] = '\0';

	ImGui::Columns();
	ImGui::End();
	ImGui::Render();

	return oEndScene(pDevice);
};

LONG WINAPI Reset(IDirect3DDevice9 * pDevice, D3DPRESENT_PARAMETERS * pPresentationParameters)
{
	static LONG lRes = 0;

	ImGui_ImplDX9_InvalidateDeviceObjects();
	lRes = oReset(pDevice, pPresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();

	if (g_nStyle >= 0)
	{
		g_nStyle = I::Surf->AddFont();
		I::Surf->SetFontGlyphSet(g_nStyle, XorString("MS Sans Serif"), 11, FW_MEDIUM, NULL, NULL, FONTFLAG_OUTLINE | FONTFLAG_ANTIALIAS);
	};

	return lRes;
};
