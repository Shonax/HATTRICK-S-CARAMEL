
#include "Misc.h"

DWORD WINAPI runAll(HANDLE)
{
	clock_t menuStamp = clock_t(NULL);
	time_t theTime = NULL, timeStamp = NULL;
	CHAR szData[256] = {};
	INT iPlayer = 0, Players[128] = {}, iSize = 0, iHealth = 0, iEnemiesSize = 0, iEnemy = 0, iTeam = 0;
	Entity_T * pPlayer = NULL, *pEnemy = NULL;
	PlrInfo_T PlrInfo;

	SetupAll(), Sleep(100);

	while (true)
	{
		if (Vars.Visuals.iDroppedWeaponsGlowColor == Vars.Visuals.iEnemiesGlowColor == Vars.Visuals.iTeammatesGlowColor)
			Vars.Visuals.iDroppedWeaponsGlowColor = 13, Vars.Visuals.iEnemiesGlowColor = 2, Vars.Visuals.iTeammatesGlowColor = 3;

		if (Vars.Visuals.iGlowAlphaIntensity == 0)
			Vars.Visuals.iGlowAlphaIntensity = 255;

		if (Vars.Visuals.fGlowBloomAmount == 0.f)
			Vars.Visuals.fGlowBloomAmount = .5f;

		if ((G::Keys[VK_HOME] || G::Keys[VK_INSERT] || G::Keys[0x000000FF]) && FLOAT(((DOUBLE)clock() - menuStamp) / CLOCKS_PER_SEC) > .1f)
		{
			Vars.Menu.Opened = !Vars.Menu.Opened;

			if (!g_bOffline && G::iFlags & FL_CLIENT && !G::pSelf->GetOrigin().IsZero())
				I::Engine->ClientCmd_Unrestricted(XorString("toggleconsole"), NULL);

			if (!Vars.Menu.Opened && G::bD3DOK)
				ImGui::GetIO().MouseDrawCursor = false;

			menuStamp = clock();
		};

		if (!g_bOffline && G::iTeam > 1 && G::iTeam < 4)
			SetClanTag();

		if (Vars.Misc.bSpam && !g_bOffline && G::iTeam > 1 && G::iTeam < 4)
		{
			if (g_PhrasesSize > 0)
			{
				theTime = time(NULL);

				if (theTime - timeStamp > RandomINT(2, 6) && theTime - g_timeLastEventStamp > RandomINT(2, 6))
				{
					if ((g_Phrases[g_PhrasesSize - 1][0] >= 'a' && g_Phrases[g_PhrasesSize - 1][0] <= 'z') || (g_Phrases[g_PhrasesSize - 1][0] >= 'A' && g_Phrases[g_PhrasesSize - 1][0] <= 'Z'))
					{
						if (RandomINT(0, 1) == 0)
							g_Phrases[g_PhrasesSize - 1][0] = toupper(g_Phrases[g_PhrasesSize - 1][0]);

						else if (xIsCharLower(g_Phrases[g_PhrasesSize - 1][1]))
							g_Phrases[g_PhrasesSize - 1][0] = tolower(g_Phrases[g_PhrasesSize - 1][0]);
					};

					sprintf(szData, XorString("Say \"%s\";"), g_Phrases[g_PhrasesSize - 1]);
					I::Engine->ClientCmd_Unrestricted(szData, NULL);
					g_PhrasesSize--;

					timeStamp = theTime;
				};
			}

			else
			{
				if (g_bCheckForLastEnemy)
				{
					iEnemiesSize = 0;

					for (iPlayer = 0; iPlayer <= I::Globals->m_iMaxClients; iPlayer++)
					{
						pPlayer = I::Ents->GetClEntity(iPlayer);

						if (!pPlayer || pPlayer == G::pSelf || (iTeam = pPlayer->GetTeam()) < 2 || pPlayer->GetHealth() < 1 || iTeam == G::iTeam)
							continue;

						iEnemiesSize++, iEnemy = iPlayer;
					};

					if (iEnemiesSize == 1)
					{
						pEnemy = I::Ents->GetClEntity(iEnemy);

						if (pEnemy)
						{
							iHealth = pEnemy->GetHealth();

							if (iHealth <= 25)
							{
								I::Engine->GetPlayerInfo(pEnemy->m_iId, &PlrInfo);

								switch (RandomINT(0, 4))
								{
								case 0: sprintf(szData, XorString("%s is low, %d HP"), ShortenName(std::string(PlrInfo.name)).c_str(), iHealth); break;
								case 1: sprintf(szData, XorString("%s is LOW (%d)"), ShortenName(std::string(PlrInfo.name)).c_str(), iHealth); break;
								case 2: sprintf(szData, XorString("-%d last"), 100 - iHealth); break;
								case 3: sprintf(szData, XorString("-%d"), 100 - iHealth); break;
								case 4: sprintf(szData, XorString("he's %d HP"), iHealth); break;
								};

								sprintf(g_Phrases[g_PhrasesSize++], szData);
							};
						};
					};

					g_bCheckForLastEnemy = FALSE;
				}

				else
				{
					theTime = time(NULL);

					if (theTime - timeStamp > RandomINT(32, 64) && theTime - g_timeLastEventStamp > RandomINT(2, 6))
					{
						iSize = 0;

						for (iPlayer = 0; iPlayer <= I::Globals->m_iMaxClients; iPlayer++)
						{
							pPlayer = I::Ents->GetClEntity(iPlayer);

							if (!pPlayer || pPlayer == G::pSelf || (iTeam = pPlayer->GetTeam()) == G::iTeam || iTeam < 2 || iTeam > 3)
								continue;

							Players[iSize++] = iPlayer;
						};

						if (iSize > 0)
						{
							pPlayer = I::Ents->GetClEntity(Players[RandomINT(0, iSize - 1)]);

							if (pPlayer)
							{
								I::Engine->GetPlayerInfo(pPlayer->m_iId, &PlrInfo);

								PrepareSpamString(szData, &PlrInfo);

								sprintf(g_Phrases[g_PhrasesSize++], szData);

								if ((g_Phrases[g_PhrasesSize - 1][0] >= 'a' && g_Phrases[g_PhrasesSize - 1][0] <= 'z') || (g_Phrases[g_PhrasesSize - 1][0] >= 'A' && g_Phrases[g_PhrasesSize - 1][0] <= 'Z'))
								{
									if (RandomINT(0, 1) == 0)
										g_Phrases[g_PhrasesSize - 1][0] = toupper(g_Phrases[g_PhrasesSize - 1][0]);

									else if (xIsCharLower(g_Phrases[g_PhrasesSize - 1][1]))
										g_Phrases[g_PhrasesSize - 1][0] = tolower(g_Phrases[g_PhrasesSize - 1][0]);
								};

								sprintf(szData, XorString("Say \"%s\";"), g_Phrases[g_PhrasesSize - 1]);
								I::Engine->ClientCmd_Unrestricted(szData, NULL);
								g_PhrasesSize--;

								timeStamp = theTime;
							};
						};
					};
				};
			};
		};

		if (!g_bOffline && Vars.Misc.Ranks && G::iTeam > 0 && (G::Keys[VK_TAB] || G::pCmd->buttons & IN_SCORE) && !G::pSelf->GetOrigin().IsZero())
			ServerRankRevealAll();

		Sleep(50);
	};

	return EXIT_SUCCESS;
};

DWORD WINAPI runModule(HANDLE)
{
	Sleep(1000), Vars.Menu.Opened = false, g_LoadTime = time(NULL);

	while (true)
	{
		if (time(NULL) - g_LoadTime >= 5)
		{
			while (!(G::pWnd = FindWindow(XorString("Valve001"), NULL))) Sleep(500);

			Sleep(250);
			g_pOldWndProc = (WNDPROC)SetWindowLongPtr(G::pWnd, GWL_WNDPROC, (LONG_PTR)OnWindProc);
			CreateThread(NULL, NULL, runAll, NULL, NULL, NULL);

			return EXIT_SUCCESS;
		};

		Sleep(100);
	};

	return EXIT_SUCCESS;
};

DWORD WINAPI DllMain(HMODULE, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH && !g_bLoaded)
		CreateThread(NULL, NULL, runModule, NULL, NULL, NULL), g_bLoaded = true;

	return BOOL(true);
};
