// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

// informers.inl ％

public jbe_main_informer(id)
{
	id -= TASK_SHOW_INFORMER;

	set_hudmessage(0, 255, 0, g_fMainInformerPosX[id], g_fMainInformerPosY[id], 0, 0.0, 0.8, 0.2, 0.2, -1);

	ShowSyncHudMsg(id, g_iSyncMainInformer,
		"День %d, %s\
		%s%s\
		%s\
		^nЗаключенные: %d|%d\
		^nОхранники: %d|%d\
		^n\
		^n%s:^t%d%%\
		%s%s\
		%s%s",
	g_iDay, g_szDaysWeek[g_iDayWeek],
	g_szDayMode, g_szDayModeTimer,
	g_szChiefName,
	g_iAlivePlayersNum[1], g_iPlayersNum[1],
	g_iAlivePlayersNum[2], g_iPlayersNum[2],
	g_iszRankExpName[g_iUserLevel[id]][RANK_NAME], g_iUserExpPercent[id],
	g_szFreeLang[g_iFreeLang], g_szFreeNames,
	g_szWantedLang[g_iWantedLang], g_szWantedNames);
}

public jbe_main_informer_death(id) {
	id -= TASK_SHOW_INFORMER;
	new pPlayer = pev(id, pev_iuser2);
	if (jbe_is_user_valid(pPlayer) && pPlayer != id) {
		set_hudmessage(30, 220, 30, 0.6, 0.35, 0, 0.0, 0.8, 0.2, 0.2, -1);
		ShowSyncHudMsg(id, g_iSyncMainInformer,
		"Наш сайт: CS-One.Ru\
		%s%s\
		%s\
		^n^nИгрок: %n^n\
		HP: %d | AP: %d | %d$^n\
		%s:^t%d|%d\
		%s%s\
		%s%s",
		g_szDayMode, g_szDayModeTimer,
		g_szChiefName,
		pPlayer,
		get_user_health(pPlayer), get_user_armor(pPlayer), g_iUserMoney[pPlayer],
		g_iszRankExpName[g_iUserLevel[pPlayer]][RANK_NAME], g_iUserExp[pPlayer], g_iszRankExpName[g_iUserLevel[pPlayer]+1][RANK_EXP],
		g_szFreeLang[g_iFreeLang], g_szFreeNames,
		g_szWantedLang[g_iWantedLang], g_szWantedNames);
	} else {
		set_hudmessage(30, 220, 30, 0.6, 0.35, 0, 0.0, 0.8, 0.2, 0.2, -1);
		ShowSyncHudMsg(id, g_iSyncMainInformer,
		"Наш сайт: CS-One.Ru\
		%s%s\
		%s\
		^nЗаключенные: %d|%d\
		^nОхранники: %d|%d\
		%s%s\
		%s%s",
		g_szDayMode, g_szDayModeTimer,
		g_szChiefName,
		g_iAlivePlayersNum[TEAM_PRISONER], g_iPlayersNum[TEAM_PRISONER],
		g_iAlivePlayersNum[TEAM_GUARD], g_iPlayersNum[TEAM_GUARD],
		g_szFreeLang[g_iFreeLang], g_szFreeNames,
		g_szWantedLang[g_iWantedLang], g_szWantedNames);
	}
	jbe_hud_adverts_death(id+TASK_SHOW_INFORMER);
}

public jbe_hud_adverts_death(id) {
	id -= TASK_SHOW_INFORMER;
	g_iHudAdvertUserCount{id}++;
	if (g_iHudAdvertUserCount{id} > DHUD_TIME) {
		g_iHudAdvertUserCount{id} = 0;
		new aData[128];
		ArrayGetArray(g_aHudAdverts, random_num(0, g_iHudAdvertsSize - 1), aData);
		UTIL_DirectorMessage(id, aData, random_num(50, 255), random_num(50, 255), random_num(50, 255), -1.0, 0.65, 1, 3.0, 9.0, 0.1, 0.8);
		client_print(id, print_console, " ** [INFO] %s", aData);
	}
}

public jbe_main_informer_ghost(id) {
	id -= TASK_SHOW_INFORMER;
	set_hudmessage(0, 255, 0, 0.7, 0.25, 0, 0.0, 0.8, 0.2, 0.2, -1);
	ShowSyncHudMsg(id, g_iSyncMainInformer,
		"Наш сайт: CS-One.Ru\
		%s%s\
		%s\
		^nЗаключенные: %d|%d\
		^nОхранники: %d|%d\
		^n\
		^n%s:^t%d％\
		%s%s\
		%s%s",
	g_szDayMode, g_szDayModeTimer,
	g_szChiefName,
	g_iAlivePlayersNum[1], g_iPlayersNum[1],
	g_iAlivePlayersNum[2], g_iPlayersNum[2],
	g_iszRankExpName[g_iUserLevel[id]][RANK_NAME], g_iUserExpPercent[id],
	g_szFreeLang[g_iFreeLang], g_szFreeNames,
	g_szWantedLang[g_iWantedLang], g_szWantedNames);

	jbe_hud_adverts_ghost(id + TASK_SHOW_INFORMER);
}

jbe_hud_adverts_ghost(id)
{
	id -= TASK_SHOW_INFORMER;
	g_iHudAdvertUserCount{id}++;
	if (g_iHudAdvertUserCount{id} > DHUD_TIME)
	{
		g_iHudAdvertUserCount{id} = 0;
		new aData[128];
		ArrayGetArray(g_aHudAdverts, random_num(0, g_iHudAdvertsSize - 1), aData);
		UTIL_DirectorMessage(id, aData, random_num(50, 255), random_num(50, 255), random_num(50, 255), -1.0, 0.10, 1, 3.0, 9.0, 0.1, 0.8);
		client_print(id, print_console, " ** [INFO] %s", aData);
	}
}

jbe_reset_user_informer(id)
{
	remove_task(id + TASK_SHOW_INFORMER);

	switch (g_iDayMode)
	{
		case DAY_RESTART, DAY_NORMAL, DAY_FREEDAY, DAY_DAYMODE:
		{
			if (g_bMafiaGame && IsSetBit(g_iBitUserAlive, id))
			{
				set_task(1.0, "jbe_main_mafia_informer", id + TASK_SHOW_INFORMER, _, _, "b");
			}
			else if (IsSetBit(g_iBitUserAlive, id))
			{
				set_task(1.0, "jbe_main_informer", id+TASK_SHOW_INFORMER, _, _, "b");
			}
			else if (IsSetBit(g_iBitUserGhost, id))
			{
				g_iHudAdvertUserCount{id} = DHUD_TIME + 1;
				
				set_task(1.0, "jbe_main_informer_ghost", id+TASK_SHOW_INFORMER, _, _, "b");
			}
			else
			{
				g_iHudAdvertUserCount{id} = DHUD_TIME + 1;
				
				set_task(1.0, "jbe_main_informer_death", id+TASK_SHOW_INFORMER, _, _, "b");
			}
		}
		case DAY_DEATHMATCH:
		{
			if (IsNotSetBit(g_iBitUserAlive, id))
			{
				jbe_hud_adverts_death(id+TASK_SHOW_INFORMER);
				
				set_task(1.0, "jbe_main_informer_death", id+TASK_SHOW_INFORMER, _, _, "b");
			}
		}
	}
}

public jbe_deathmatch_informer()
{ // x 0.11
	set_hudmessage(30, 220, 30, 0.15, 0.0, 0, 0.0, 0.8, 0.2, 0.2, -1);
	ShowSyncHudMsg(0, g_iSyncDeathMatchInformer,
	"День %d, %s\
	%s^n\
	Зеки: %d|%d",
	g_iDay, g_szDaysWeek[g_iDayWeek],
	g_szDayMode,
	g_iAlivePlayersNum[1], g_iPlayersNum[1]);
}