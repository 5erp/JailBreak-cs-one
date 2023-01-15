Show_ChiefMenu(id, iPos = 0)
{
	if (iPos < 0 || is_not_chief_menu(id))
		return PLUGIN_HANDLED;

	static aMenu[][64] = {
		"(Открыть|Закрыть) Клетки",
		"Отсчёт|Звуки",
		"Обыскать|Открыть",
		"Выходной",
		"Снять розыск",
		"Мини игры",
		"Вылечить",
		"Поделить",

		"Забрать навыки",
		"Выдать|Забрать голос",
		"Наказать охранника",
		"Игровые режимы",
		"Глобальные игры",
		"Передать начальника",
		"Мини музыка", // -
		"Музыка" 
		// Парашют ВЫКЛ/ВКЛ
	};

	if (iPos == 0)
	{
		aMenu[0] = g_bDoorStatus ? "Закрыть клетки" : "Открыть клетки";

		switch (g_iChiefGameType)
		{
			case GAME_NONE:		aMenu[5] = g_iDayMode != DAY_NORMAL ? "\dМини игры" : "Мини игры";
			case GAME_SOCCER:	aMenu[5] = " > Футбол";
			case GAME_BOXING:	aMenu[5] = " > Бокс";
			case GAME_FFIRE:	aMenu[5] = " > Огонь по своим";
			case GAME_MAFIA:	aMenu[5] = " > Мафия";
			case GAME_JIHAD:	aMenu[5] = " > Битва за джихад";
			default:			aMenu[5] = "\d > Идет игра";
		}

		aMenu[7] = g_iDayMode != DAY_NORMAL ? "\dПоделить зеков" : "Поделить зеков";
	}
	else if (iPos == 1)
		aMenu[8] = g_iDayMode != DAY_NORMAL ? "\dЗабрать навыки" : "Забрать навыки";

	menu_list_title("Меню Начальника", iPos, sizeof aMenu);
	menu_item("^n");

	while (menu_list_count())
		menu_list_item("\y%d. \w%s^n", 
			menu_list_num(), aMenu[menu_list_countid()]);

	return menu_list_show(id, "Show_ChiefMenu");
}

public Handle_ChiefMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_ChiefMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_ChiefMenu(id, --g_iMenuPosition[id]);
	}

	switch (menu_list_itemid(id, iKey))
	{
		case 0:
		{
			if (is_user_cmd_flood(id, FLOOD_DOORS, 2.0, 5.0))
				return Show_ChiefMenu(id);
				
			g_bDoorStatus ? jbe_close_doors() : jbe_open_doors();

			jbe_info_center_msg(0, "Начальник %s клетки", g_bDoorStatus ? "открыл" : "закрыл");
		}
		case 1: return Show_CountDownMenu(id);
		case 2: ClCmd_Radio3(id);
		case 3: return Show_FreeDayMenu(id);
		case 4: return Show_WantedMenu(id, _, 1, RETURN_CHIEF);
		case 5:
		{
			switch (g_iChiefGameType)
			{
				case GAME_NONE: return Show_MiniGameMenu(id);
				case GAME_SOCCER: return Show_SoccerMenu(id);
				case GAME_BOXING: return Show_BoxingMenu(id);
				case GAME_FFIRE:
				{
					jbe_minigame_friendlyfire(id);
					return Show_MiniGameMenu(id);
				}
				case GAME_MAFIA: return Show_MafiaMenu(id);
				case GAME_JIHAD: return Show_JihadMenu(id);
				default: is_yes_start_minigame(id, GAME_NONE);
			}
		}
		case 6: return Show_TreatPrisonerMenu(id, _, 1, RETURN_CHIEF);
		case 7: return Show_PrisonersDivideColorMenu(id);
		// page 2
		case 8: return Show_ChiefTakeSkillMenu(id);
		case 9: return Show_VoiceControlMenu(id, _, 1, RETURN_CHIEF);
		case 10: return Show_PunishGuardMenu(id, _, 1, RETURN_CHIEF);
		case 11: return Show_DayModeChiefMenu(id);
		case 12: return Show_GlobalGamesMenu(id);
		case 13: return Show_TransferChiefMenu(id, _, 1, RETURN_CHIEF);
		case 14: return Show_SoundMenu(id);
		case 15: return Show_MusicMenu(id);
	}
	return Show_ChiefMenu(id, g_iMenuPosition[id]);
}

Show_GlobalGamesMenu(id)
{
	menu_title("Глобальные игры^n^n");
	menu_item("\y1. \wМафия^n");
	menu_item("\y2. \wБитва за джихад^n");
	menu_item("^n^n^n^n^n^n");
	menu_item("^n\y9. \wНазад");
	menu_item("^n\y0. \wВыход");
	return menu_show(id, (1<<0|1<<1|1<<8|1<<9), "Show_GlobalGamesMenu");
}

public Handle_GlobalGamesMenu(id, iKey)
{
	switch (iKey)
	{
		case 8:	return Show_ChiefMenu(id, 1);
		case 9: return PLUGIN_HANDLED;
	}

	if (IsNotSetBit(g_iBitUserAdmin, id) && IsNotSetBit(g_iBitUserGlobalAdmin, id))
	{
		client_print_color(id, id, "%s Доступно только админам.", g_szChatPrefix);
		return Show_ChiefMenu(id, 1);
	}

	switch (iKey)
	{
		case 0: return Show_MafiaMenu(id);
		case 1: return Show_JihadMenu(id);
	}

	return PLUGIN_HANDLED;
}

Show_CountDownMenu(id)
{
	if (is_not_chief_menu(id)) return PLUGIN_HANDLED;

	if (g_iDayMode != DAY_NORMAL)
	{
		client_print_color(id, id, "%s Доступно только в обычный день!", g_szChatPrefix);
		return Show_ChiefMenu(id);
	}
	
	new iKeys = (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<8|1<<9);

	//menu_title("Отсчёт|Звуки|Рингтоны^n^n");
	menu_title("Отсчёт|Звуки^n^n");

	menu_item("\y1. \wДесять секунд^n");
	menu_item("\y2. \wПять секунд^n");
	menu_item("\y3. \wТри секунды^n");
	menu_item("\y4. \wДать гонг^n");
	menu_item("\y5. \wСвисток^n");
	menu_item("\y6. \wСвой отсчет^n");
	if (task_exists(TASK_COUNT_DOWN_TIMER))
	{
		menu_item("^n\y7. \wВыключить отсчет^n");
		iKeys |= (1<<6);
	}
	else menu_item("^n^n");
	
	menu_item("^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	return menu_show(id, iKeys, "Show_CountDownMenu");
}

public Handle_CountDownMenu(id, iKey)
{
	if (is_not_chief_menu(id) || iKey == 9)
		return PLUGIN_HANDLED;
	
	if (iKey == 8)
		return Show_ChiefMenu(id);

	if (g_iCountDown)
	{
		remove_task(TASK_COUNT_DOWN_TIMER);
		g_iCountDown = 0;
	}

	if (iKey == 5)
	{
		client_cmd(id, "messagemode ^"countdown^"");
		return Show_CountDownMenu(id);
	}

	if (iKey == 6)
	{
		client_print_center(0, "^n"); // убираем текст
		return Show_CountDownMenu(id);
	}

	if (is_user_cmd_flood(id, FLOOD_COUNT_DOWN, 1.0, 3.0)) 
		return Show_CountDownMenu(id);

	switch (iKey)
	{
		case 0: g_iCountDown = 11;
		case 1: g_iCountDown = 6;
		case 2: g_iCountDown = 4;
		case 3:
		{
			rg_send_audio(0, sound_gong), g_iCountDown = 0;
			client_print_center(0, "•• ГОНГ ••");
		}
		case 4:
		{
			rg_send_audio(0, sound_whitle_start), g_iCountDown = 0;
			client_print_center(0, "•• СВИСТОК ••");
		}
	}
	set_task(1.0, "jbe_count_down_timer", TASK_COUNT_DOWN_TIMER, _, _, "a", g_iCountDown);

	return Show_CountDownMenu(id);
}

Show_MiniGameMenu(id, iPos = 0)
{
	if (iPos < 0 || is_not_chief_game_menu(id)) return PLUGIN_HANDLED;

	new const aMenu[][] = {
		"Футбол",
		"Бокс",
		"Обновить лого",
		"Дальность дропа",
		"Случайная одежда",
		"Огонь по своим",
		"Создать мяч",
		"Слово в чат",

		"Дуэли",
		"Случайное число"	
	};

	menu_list_title("Мини игры", iPos, sizeof aMenu, 8);
	menu_item("^n");

	while (menu_list_count())
		menu_list_item("\y%d. \w%s^n", menu_list_num(), 
			aMenu[menu_list_countid()]);

	return menu_list_show(id, "Show_MiniGameMenu");
}

public Handle_MiniGameMenu(id, iKey)
{
	if (is_not_chief_game_menu(id))
		return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 8: return Show_MiniGameMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_MiniGameMenu(id, --g_iMenuPosition[id]);
	}

	switch (menu_list_itemid(id, iKey))
	{
		case 0: if (is_yes_start_minigame(id, GAME_SOCCER)) return Show_SoccerMenu(id);
		case 1: if (is_yes_start_minigame(id, GAME_BOXING)) return Show_BoxingMenu(id);
		case 2: jbe_minigame_logo(id);
		case 3: if (is_yes_start_minigame(id, GAME_NONE)) jbe_minigame_deagle();
		case 4: jbe_minigame_random_skin();
		case 5:
		{
			if (is_yes_start_minigame(id, GAME_FFIRE))
			{
				jbe_minigame_friendlyfire(id);
				return Show_ChiefMenu(id);
			} 
		}
		case 6: return Show_BallMenu(id);
		case 7: jbe_minigame_random_word();
		
		// page 2
		case 8: return Show_GameDuelMenu(id);
		case 9: return Show_RandomNumMenu(id);
	}
	return Show_MiniGameMenu(id, g_iMenuPosition[id]);
}

Show_DayModeChiefMenu(id, iPos = 0)
{
	if (iPos < 0 || is_not_chief_game_menu(id)) return PLUGIN_HANDLED;

	new const aMenu[][] = {
		"Штурм",
		"Голодные игры",
		"Мясо",
		"Невидимки",
		"Снайпера",
		"Бомбер"		
	};

	menu_list_title("Игровые режимы", iPos, sizeof aMenu, 8);
	menu_item("^n");

	while (menu_list_count())
		menu_list_item("\y%d. \w%s^n", menu_list_num(), 
			aMenu[menu_list_countid()]);

	return menu_list_show(id, "Show_DayModeChiefMenu");
}

public Handle_DayModeChiefMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_DayModeChiefMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_DayModeChiefMenu(id, --g_iMenuPosition[id]);
	}

	switch (menu_list_itemid(id, iKey))
	{
		case 0: if (is_yes_start_minigame(id, GAME_SHTURM))	jbe_minigame_chief(GAME_SHTURM);
		case 1: if (is_yes_start_minigame(id, GAME_HUNGER))	jbe_minigame_chief(GAME_HUNGER);
		case 2: if (is_yes_start_minigame(id, GAME_M9SO))	jbe_minigame_chief(GAME_M9SO);
		case 3: if (is_yes_start_minigame(id, GAME_GHOST))	jbe_minigame_chief(GAME_GHOST);
		case 4: if (is_yes_start_minigame(id, GAME_SNIPER))	jbe_minigame_chief(GAME_SNIPER);
		case 5: if (is_yes_start_minigame(id, GAME_BOMBER))	jbe_minigame_chief(GAME_BOMBER);
	}

	return PLUGIN_HANDLED;
}

Show_PrisonersDivideColorMenu(id)
{
	if (is_not_chief_game_menu(id))
		return Show_ChiefMenu(id);
		
	new iKeys = (1<<5|1<<7|1<<8|1<<9);
	menu_title("Выберите количество команд^n^n");
	
	if (g_iAlivePlayersNum[TEAM_PRISONER] >= 2) {
		menu_item("\y1. \wДве команды^n");
		iKeys |= (1<<0);
	} else menu_item("\y1. \dДве команды^n");
	
	if (g_iAlivePlayersNum[TEAM_PRISONER] >= 3) {
		menu_item("\y2. \wТри команды^n");
		iKeys |= (1<<1);
	} else menu_item("\y2. \dТри команды^n");
	
	if (g_iAlivePlayersNum[TEAM_PRISONER] >= 4) {
		menu_item("\y3. \wЧетыре команды^n");
		iKeys |= (1<<2);
	} else menu_item("\y3. \dЧетыре команды^n");
	
	if (g_iAlivePlayersNum[TEAM_PRISONER] >= 5) {
		menu_item("\y4. \wПять команд^n");
		iKeys |= (1<<3);
	} else menu_item("\y4. \dПять команд^n");
	
	if (g_iAlivePlayersNum[TEAM_PRISONER] >= 6) {
		menu_item("\y5. \wШесть команд^n");
		iKeys |= (1<<4);
	} else menu_item("\y5. \dШесть команд^n");
	
	menu_item("\y6. \wДелить в ручную^n^n");

	menu_item("\y8. \wКомандный огонь по своим \r[%s]^n", g_iFriendlyFire == FF_TEAM ? "Вкл" : "Выкл");
	
	menu_item("^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	return menu_show(id, iKeys, "Show_PrisonersDivideColorMenu");
}

public Handle_PrisonersDivideColorMenu(id, iKey)
{
	if (is_not_chief_game_menu(id))
		return PLUGIN_HANDLED;
	
	switch(iKey)
	{
		case 5: return Show_SetSkinMenu(id);
		case 7:
		{
			if (g_iChiefGameType && g_iChiefGameType != GAME_FF_TEAM)
			{
				client_print_color(id, id, "%s Недоступно, запущенна другая игра!", g_szChatPrefix);
				return PLUGIN_HANDLED;
			}

			if (g_iFriendlyFire == FF_TEAM)
				g_iFriendlyFire = FF_OFF;

			else
				g_iFriendlyFire = FF_TEAM;

			client_print_color(0, id, "%s Начальник %s командный огонь по своим.", g_szChatPrefix, g_iFriendlyFire == FF_TEAM ? "включил" : "выключил");
		}
		case 8: return Show_ChiefMenu(id);
		case 9: return PLUGIN_HANDLED;
		default: jbe_prisoners_divide_color(iKey + 2);
	}
	return Show_ChiefMenu(id);
}

/* Free Day menus
=========================*/
Show_FreeDayMenu(id)
{
	menu_title("Управление выходным^n^n");

	menu_item("\y1. \w%s общий выходной^n", g_iDayMode == DAY_NORMAL ? "Начать" : "Завершить");
	menu_item("\y2. %sВыдать|Забрать игроку^n", g_iDayMode == DAY_NORMAL ? "\w" : "\d");
	menu_item("\y3. \wВыходной на завтра^n");

	menu_item("\yОтменить выходной всем свободным^n\
		\y5. %sОтменить^n", g_iDayMode == DAY_NORMAL ? "\w" : "\d");
	menu_item("\yПеренести всем выходной на завтра^n\
		\y7. %sПеренести^n", g_iDayMode == DAY_NORMAL ? "\w" : "\d");

	menu_item("^n");
	menu_item("^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");

	return menu_show(id, (1<<0|1<<1|1<<2|1<<4|1<<6|1<<8|1<<9), "Show_FreeDayMenu");
}

public Handle_FreeDayMenu(id, iKey)
{
	switch (iKey)
	{
		case 0:
		{
			if (g_iDayMode == DAY_NORMAL && (g_iDayWeek != 1 || g_iDay == 1))
			{
				if (is_user_cmd_flood(id, FLOOD_FREEDAY, 10.0, 20.0))
					return Show_FreeDayMenu(id);
				jbe_free_day_start();
			}
			else if (g_iDayWeek == 1 && g_iDay != 1 && g_iDayMode != DAY_FREEDAY)
			{
				client_print_color(id, id, "%s ^3Общий выходной ^1только в ^4воскресенье^1!", g_szChatPrefix);
			}
			else
			{
				if (is_user_cmd_flood(id, FLOOD_FREEDAY, 10.0, 20.0))
					return Show_FreeDayMenu(id);
				jbe_free_day_ended();
			}
		}
		case 1:
		{
			if (is_shief_global_game())
			{
				client_print_color(id, id, "%s Недоступно во время игры!", g_szChatPrefix);
				return Show_FreeDayMenu(id);
			}
			if (g_iDayMode == DAY_NORMAL)
				return Show_FreeDayControlMenu(id, _, 1, RETURN_CHIEF);
			else
			{
				client_print_color(id, id, "%s Доступно только в обычный день!", g_szChatPrefix);
				return Show_FreeDayMenu(id);
			}
		}
		case 2: return Show_FreeDayNextMenu(id, _, 1, RETURN_CHIEF);
		case 4:
		{
			if (g_iDayMode == DAY_NORMAL)
			{
				jbe_freeday_disable_all();
				client_print_color(0, id, "%s %s отменил всем выходной.", g_szChatPrefix, g_szChiefPrefix);
			}
			else
			{
				client_print_color(id, id, "%s Доступно только в обычный день!", g_szChatPrefix);
				return Show_FreeDayMenu(id);
			}
		}
		case 6:
		{
			if (g_iDayMode == DAY_NORMAL)
			{
				jbe_freeday_transfer_all();
				client_print_color(0, id, "%s %s перенес всем выходной на завтра.", g_szChatPrefix, g_szChiefPrefix);
			}
			else
			{
				client_print_color(id, id, "%s Доступно только в обычный день!", g_szChatPrefix);
				return Show_FreeDayMenu(id);
			}
		}
		case 8: return Show_ChiefMenu(id);
	}
	return PLUGIN_HANDLED;
}

Show_FreeDayControlMenu(id, iPos = 0, iMessage = 0, iReturn = 0)
{
	if (iPos < 0 || g_iDayMode != DAY_NORMAL
	|| (id != g_iChiefId && IsNotSetBit(g_iBitUserAdmin, id))
	|| IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;
	
	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (g_iUserTeam[i] != TEAM_PRISONER || IsNotSetBit(g_iBitUserAlive, i)
		|| IsSetBit(g_iBitUserWanted, i))
			continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}
	
	if (!iPlayersNum)
	{
		iMessage && client_print_color(id, id, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
		switch (iReturn)
		{
			case 1: return Show_FreeDayMenu(id);
			case 2: return Show_AdminModMenu(id, .iPos = 0);
			default: return PLUGIN_HANDLED;
		}
	}

	menu_list_title("Выдать|Забрать выходной", iPos, iPlayersNum);
	menu_item("\dВыберите игрока^n");

	new iTarget;
	while (menu_list_count()) {
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		menu_list_item("\y%d. \w%n%s^n", menu_list_num(),
			iTarget, IsSetBit(g_iBitUserFree, iTarget) ? " \r[FD]" : "");
	}

	return menu_list_show(id, "Show_FreeDayControlMenu");
}

public Handle_FreeDayControlMenu(id, iKey)
{
	if (g_iDayMode != DAY_NORMAL || IsNotSetBit(g_iBitUserAlive, id)
	|| (id != g_iChiefId && IsNotSetBit(g_iBitUserAdmin, id)))
		return PLUGIN_HANDLED;
	
	switch (iKey)
	{
		case 8: return Show_FreeDayControlMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_FreeDayControlMenu(id, --g_iMenuPosition[id]);
	}

	new iTarget = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];
	
	if (g_iUserTeam[iTarget] != TEAM_PRISONER || IsSetBit(g_iBitUserWanted, iTarget))
		return Show_FreeDayControlMenu(id, g_iMenuPosition[id]);
	
	if (IsSetBit(g_iBitUserFree, iTarget))
	{
		client_print_color(0, iTarget, "%s ^1%s ^4%n ^1забрал ^4Выходной ^3%n^1.", g_szChatPrefix,
			id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id, iTarget);
		jbe_sub_user_free(iTarget);
	}
	else
	{
		client_print_color(0, iTarget, "%s ^1%s ^4%n ^1выдал ^4Выходной ^3%n^1.", g_szChatPrefix, 
			id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id, iTarget);
		jbe_add_user_free(iTarget);
	}
	
	return Show_FreeDayControlMenu(id, g_iMenuPosition[id]);
}

Show_FreeDayNextMenu(id, iPos = 0, iMessage = 0, iReturn = 0)
{
	if (iPos < 0 || g_iDayMode != 1 && g_iDayMode != 2
	|| (id != g_iChiefId && IsNotSetBit(g_iBitUserAdmin, id)) || IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;
	
	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++) {
		if (g_iUserTeam[i] != TEAM_PRISONER) continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!iPlayersNum)
	{
		iMessage && client_print_color(id, id, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
		switch (iReturn)
		{
			case 1: return Show_FreeDayMenu(id);
			default: return PLUGIN_HANDLED;
		}
	}

	menu_list_title("Выходной на завтра", iPos, iPlayersNum);
	menu_item("\dВыберите игрока:^n");

	new iTarget;
	while (menu_list_count()) {
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		menu_list_item("\y%d. \w%n%s^n", menu_list_num(),
			iTarget, IsSetBit(g_iBitUserFreeNextRound, iTarget) ? "\r[+]" : "\r[-]");
	}

	return menu_list_show(id, "Show_FreeDayNextMenu");
}

public Handle_FreeDayNextMenu(id, iKey)
{
	if (g_iDayMode != 1 && g_iDayMode != 2 || (id != g_iChiefId && IsNotSetBit(g_iBitUserAdmin, id)))
		return PLUGIN_HANDLED;
	
	switch(iKey)
	{
		case 8: return Show_FreeDayNextMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_FreeDayNextMenu(id, --g_iMenuPosition[id]);
	}

	new iTarget = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];
	
	if (g_iUserTeam[iTarget] != TEAM_PRISONER)
		return Show_FreeDayNextMenu(id, g_iMenuPosition[id]);
	
	if (IsSetBit(g_iBitUserFreeNextRound, iTarget))
	{
		client_print_color(0, iTarget, "%s ^1%s ^4%n ^1забрал ^4Выходной на завтра ^3%n^1.", g_szChatPrefix, 
			id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id, iTarget);
		
		ClearBit(g_iBitUserFreeNextRound, iTarget);
	}
	else
	{
		client_print_color(0, iTarget, "%s ^1%s ^4%n ^1выдал ^4Выходной на завтра ^3%n^1.", g_szChatPrefix, 
			id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id, iTarget);

		client_print_color(0, iTarget, "%s ^1Заключённый ^3%n ^1получит Выходной в следующем раунде.", 
			g_szChatPrefix, iTarget);

		SetBit(g_iBitUserFreeNextRound, iTarget);
	}
	
	return Show_FreeDayNextMenu(id, g_iMenuPosition[id]);
}

/* Chief Take Skills
===============================*/
Show_ChiefTakeSkillMenu(id)
{
	if (is_not_chief_game_menu(id))
		return Show_ChiefMenu(id, g_iMenuPosition[id]);

	new iKeys = (1<<0|1<<1|1<<2|1<<3|1<<4|1<<8|1<<9);

	menu_title("Забрать навыки зекам^n^n");

	menu_item("\y1. \wПрыжок и Гравитация^n");
	menu_item("\y2. \wСкорость^n");
	menu_item("\y3. \w100 HP + 0 AP^n");
	menu_item("\y4. \wУрон^n");
	menu_item("\y5. \wСвечение^n");
	
	menu_item("^n\rВнимание!^n\
		\yМеню разрешено использовать только для игр^n\
		\dПравила: cs-one.ru/rules (в чат /rules)^n");
	
	menu_item("^n\y9. \wНазад");
	menu_item("^n\y0. \wВыход");
	
	return menu_show(id, iKeys, "Show_ChiefTakeSkillMenu");
}
 
public Handle_ChiefTakeSkillsMenu(id, iKey)
{
	if (is_not_chief_game_menu(id))
		return PLUGIN_HANDLED;
	
	switch (iKey)
	{
		case 8:	return Show_ChiefMenu(id, g_iMenuPosition[id]);
		case 9: return PLUGIN_HANDLED;
		default:
		{
			g_iMenuType[id] = iKey;
			_chief_takeskill_select(id);
		}
	}

	return Show_ChiefTakeSkillMenu(id);
}

Show_PunishGuardMenu(id, iPos = 0, iMessage = 0, iReturn = 0)
{
	if (iPos < 0 || g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY
	|| id != g_iChiefId || IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;

	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (g_iUserTeam[i] != TEAM_GUARD || i == g_iChiefId
		|| IsSetBit(g_iBitUserAdmin, i) || IsNotSetBit(g_iBitUserAlive, i))
			continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!iPlayersNum)
	{
		iMessage && client_print_color(id, id, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
		switch (iReturn)
		{
			case 1: return Show_ChiefMenu(id, iPos = 1);
			default: return PLUGIN_HANDLED;
		}
	}

	menu_list_title("Наказать охраника", iPos, iPlayersNum);
	menu_item("\dВыберите игрока:^n");

	new iTarget;
	while (menu_list_count()) {
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		menu_list_item("\y%d. \w%n^n", menu_list_num(), iTarget);
	}

	return menu_list_show(id, "Show_PunishGuardMenu");
}

public Handle_PunishGuardMenu(id, iKey)
{
	if (g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY || id != g_iChiefId
	|| IsNotSetBit(g_iBitUserAlive, id)) return PLUGIN_HANDLED;
	
	switch (iKey)
	{
		case 8: return Show_PunishGuardMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_PunishGuardMenu(id, --g_iMenuPosition[id]);
		default:
		{
			new iTarget = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];
			if (g_iUserTeam[iTarget] == TEAM_GUARD)
			{
				rg_remove_all_items(iTarget);
				if (jbe_set_user_team(iTarget, TEAM_PRISONER))
				{
					client_print_color(0, id, "%s ^1Начальник ^3%n ^1наказал охранника ^3%n^1.", g_szChatPrefix, id, iTarget);
				}
			}
		}
	}
	return PLUGIN_HANDLED;
}

Show_TransferChiefMenu(id, iPos = 0, iMessage = 0, iReturn = 0)
{
	if(iPos < 0 || g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY
	|| id != g_iChiefId || IsNotSetBit(g_iBitUserAlive, id) || g_iDuelStatus)
		return PLUGIN_HANDLED;

	new iPlayersNum;
	for(new i = 1; i <= MaxClients; i++)
	{
		if (g_iUserTeam[i] != TEAM_GUARD || IsNotSetBit(g_iBitUserAlive, i) || i == g_iChiefId)
			continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!iPlayersNum)
	{
		iMessage && client_print_color(id, id, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
		switch (iReturn)
		{
			case 1: return Show_ChiefMenu(id, .iPos = 1);
			default: return PLUGIN_HANDLED;
		}
	}

	menu_list_title("Выберите кому передать", iPos, iPlayersNum);
	menu_item("^n");

	new iTarget;
	while (menu_list_count()) {
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		menu_list_item("\y%d. \w%n^n", menu_list_num(), iTarget);
	}

	return menu_list_show(id, "Show_TransferChiefMenu");
}

public Handle_TransferChiefMenu(id, iKey)
{
	if (g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY
	|| id != g_iChiefId || IsNotSetBit(g_iBitUserAlive, id) || g_iDuelStatus)
		return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 8: return Show_TransferChiefMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_TransferChiefMenu(id, --g_iMenuPosition[id]);
		default:
		{
			new iTarget = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];

			if (IsNotSetBit(g_iBitUserAlive, iTarget))
				return client_print_color(id, id, "%s Игрок мертв..", g_szChatPrefix);

			if (g_iUserTeam[iTarget] != TEAM_GUARD)
				client_print_color(id, id, "%s Можно передать только охране.", g_szChatPrefix);

			if(jbe_set_user_chief(iTarget))
			{
				client_print_color(0, print_team_blue,
					"%s ^1Начальник ^4%n ^1передал полномочия ^3%n^1.", g_szChatPrefix, id, iTarget);
				return PLUGIN_HANDLED;
			}
		}
	}
	return Show_TransferChiefMenu(id, g_iMenuPosition[id]);
}

Show_TreatPrisonerMenu(id, iPos = 0, iMessage = 0, iReturn = 0)
{
	if (iPos < 0 || g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY
	|| (id != g_iChiefId && IsNotSetBit(g_iBitUserAdmin, id)))
		return PLUGIN_HANDLED;
		
	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (g_iUserTeam[i] != TEAM_PRISONER || IsNotSetBit(g_iBitUserAlive, i)
		|| IsSetBit(g_iBitUserBoxing, id) || IsSetBit(g_iBitUserDuel, id)
		|| get_user_health(i) >= 100)
			continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!iPlayersNum)
	{
		iMessage && client_print_color(id, id, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
		switch (iReturn)
		{
			case 1: return Show_ChiefMenu(id);
			case 2: return Show_AdminModMenu(id, 0);
			default: return PLUGIN_HANDLED;
		}
	}

	menu_list_title("Выберите кого лечить", iPos, iPlayersNum);
	menu_item("^n");

	if (!iPos)
		menu_list_item("\y1. \wВылечить всех^n");

	new iTarget;
	while (menu_list_count()) {
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		menu_list_item("\y%d. \w%n \r[%dHP]^n", menu_list_num(),
			iTarget, get_user_health(iTarget));
	}

	return menu_list_show(id, "Show_TreatPrisonerMenu");
}

public Handle_TreatPrisonerMenu(id, iKey)
{
	if (g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY
	|| (id != g_iChiefId && IsNotSetBit(g_iBitUserAdmin, id)))
		return PLUGIN_HANDLED;
	
	switch (iKey)
	{
		case 8: return Show_TreatPrisonerMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_TreatPrisonerMenu(id, --g_iMenuPosition[id]);
		default:
		{
			if (!g_iMenuPosition[id] && !iKey)
			{
				for (new i = 1; i <= MaxClients; i++) {
					if (g_iUserTeam[i] != TEAM_PRISONER || IsNotSetBit(g_iBitUserAlive, i) || get_user_health(i) >= 100
					|| IsSetBit(g_iBitUserBoxing, id) || IsSetBit(g_iBitUserDuel, id)) continue;
					set_pev(i, pev_health, 100.0);
				}
				client_print_color(0, id, "%s ^1%s ^3%n ^1вылечил всех заключенных.", g_szChatPrefix,
					id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id);
				if (g_iChiefId == id)
					return Show_ChiefMenu(id);
				
				return PLUGIN_HANDLED;
			}
			else
			{
				new iTarget = g_iMenuPlayers[id][menu_list_itemid(id, iKey) - 1];
				if (g_iUserTeam[iTarget] == TEAM_PRISONER && IsSetBit(g_iBitUserAlive, iTarget)
				&& get_user_health(iTarget) < 100 && IsNotSetBit(g_iBitUserBoxing, id))
				{
					client_print_color(0, id, "%s ^1%s ^3%n ^1вылечил заключенного ^3%n^1.", g_szChatPrefix,
						id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id, iTarget);
					set_pev(iTarget, pev_health, 100.0);
				}
			}
		}
	}
	return Show_TreatPrisonerMenu(id, g_iMenuPosition[id]);
}

Show_VoiceControlMenu(id, iPos = 0, iMessage = 0, iReturn = 0)
{
	if (iPos < 0 || (id != g_iChiefId && IsNotSetBit(g_iBitUserAdmin, id)))
		return PLUGIN_HANDLED;

	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++) {
		if (IsNotSetBit(g_iBitUserConnected, i) || g_iUserTeam[i] != TEAM_PRISONER
		|| IsNotSetBit(g_iBitUserAlive, i) || IsSetBit(g_iBitUserAdmin, i))
			continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!iPlayersNum)
	{
		iMessage && client_print_color(id, id, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
		switch (iReturn)
		{
			case 1: return Show_ChiefMenu(id, 1);
			case 2: return Show_AdminModMenu(id, 0);
			default: return PLUGIN_HANDLED;
		}
	}

	menu_list_title("Управление голосом", iPos, iPlayersNum);
	menu_item("^n");

	new iTarget;
	while (menu_list_count()) {
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		menu_list_item("\y%d. \w%n%s^n", menu_list_num(),
			iTarget, IsSetBit(g_iBitUserVoice, iTarget) ? " \r[Есть]" : "");
	}

	return menu_list_show(id, "Show_VoiceControlMenu");
}

public Handle_VoiceControlMenu(id, iKey)
{
	if (id != g_iChiefId && IsNotSetBit(g_iBitUserAdmin, id))
		return PLUGIN_HANDLED;
	
	switch(iKey)
	{
		case 8: return Show_VoiceControlMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_VoiceControlMenu(id, --g_iMenuPosition[id]);
	}

	new iTarget = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];
	
	if (IsSetBit(g_iBitUserVoice, iTarget))
	{
		ClearBit(g_iBitUserVoice, iTarget);
		client_print_color(0, iTarget, "%s ^1%s ^4%n ^1забрал голос у ^3%n^1.",
			g_szChatPrefix, id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id, iTarget);
	}
	else
	{
		SetBit(g_iBitUserVoice, iTarget);
		client_print_color(0, iTarget, "%s ^1%s ^4%n ^1выдал голос ^3%n^1.",
			g_szChatPrefix, id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id, iTarget);
	}
	
	reload_user_voice(iTarget);
	return Show_VoiceControlMenu(id, g_iMenuPosition[id]);
}

Show_WantedMenu(id, iPos = 0, iMessage = 0, iReturn = 0)
{
	if (iPos < 0 || g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY
	|| (id != g_iChiefId && IsNotSetBit(g_iBitUserAdmin, id)))
		return PLUGIN_HANDLED;

	if (is_shief_global_game())
		return client_print_color(id, id, "%s Недоступно во время игры!", g_szChatPrefix);
			
	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++) {
		if (IsNotSetBit(g_iBitUserWanted, i) || g_iUserTeam[i] != TEAM_PRISONER) continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!iPlayersNum)
	{
		iMessage && client_print_color(id, id, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
		switch (iReturn)
		{
			case 1: return Show_ChiefMenu(id);
			case 2: return Show_AdminModMenu(id, 0);
			default: return PLUGIN_HANDLED;
		}
	}
	
	menu_list_title("Снять розыск", iPos, iPlayersNum);
	menu_item("^n");

	if (!iPos)
		menu_list_item("\y1. \wСнять всем^n");

	new iTarget;
	while (menu_list_count()) {
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		menu_list_item("\y%d. \w%n^n", menu_list_num(), iTarget);
	}

	return menu_list_show(id, "Show_WantedMenu");
}

public Handle_WantedMenu(id, iKey)
{
	if (g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY
	|| (id != g_iChiefId && IsNotSetBit(g_iBitUserAdmin, id)))
		return PLUGIN_HANDLED;
	
	switch (iKey)
	{
		case 8: return Show_WantedMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_WantedMenu(id, --g_iMenuPosition[id]);
		default:
		{
			if (!g_iMenuPosition[id] && !iKey)
			{
				new bool:bNotAll = false;
				
				for (new i = 1; i <= MaxClients; i++) {
					if (IsNotSetBit(g_iBitUserWanted, i) || g_iUserTeam[i] != TEAM_PRISONER)
						continue;
					
					if (is_user_weapons(i))
					{
						bNotAll = true;
					}
					else
					{
						if (IsSetBit(g_iBitUserRealWanted, i) && g_iUserWantedNum[i])
						{
							ClearBit(g_iBitUserRealWanted, i);
							g_iUserWantedNum[i]--;
						}
						jbe_sub_user_wanted(i);
					}
				}

				if (bNotAll)
				{
					client_print_color(0, id, "%s ^1%s ^3%n ^1снял розыск не всем заключенным.", g_szChatPrefix,
						g_iChiefId == id ? g_szChiefPrefix : g_szAdminPrefix, id);
					client_print_color(id, id, "%s Нельзя снять розыск игрокам с оружием!", g_szChatPrefix);
				}
				else
				{
					client_print_color(0, id, "%s ^1%s ^3%n ^1снял розыск всем заключенным.", g_szChatPrefix,
						g_iChiefId == id ? g_szChiefPrefix : g_szAdminPrefix, id);
				}
			}
			else
			{
				new iTarget = g_iMenuPlayers[id][menu_list_itemid(id, iKey) - 1];

				if (is_user_weapons(iTarget))
				{
					client_print_color(id, id, "%s Нельзя снять розыск игроку с оружием!", g_szChatPrefix);
					if (id == g_iChiefId)
						return Show_ChiefMenu(id);
					else
						return PLUGIN_HANDLED;
				}

				if (IsSetBit(g_iBitUserRealWanted, iTarget) && g_iUserWantedNum[iTarget])
				{
					ClearBit(g_iBitUserRealWanted, iTarget);
					g_iUserWantedNum[iTarget]--;
				}
				jbe_sub_user_wanted(iTarget);
				
				client_print_color(0, id, "%s ^1%s ^4%n ^1снял розыск ^3%n^1.", g_szChatPrefix,
					g_iChiefId == id ? g_szChiefPrefix : g_szAdminPrefix, id, iTarget);
			}
		}
	}
	return Show_WantedMenu(id, g_iMenuPosition[id]);
}

/* Chief Duels Game
====================*/
Show_GameDuelMenu(id) {
	new iKeys = (1<<8|1<<9);
	menu_title("Меню дуэлей^n^n");
	
	if (!g_iDuelStatus) iKeys |= (1<<0|1<<1);
	
	if (g_iDuelUsersId[0]) {
		menu_item("\y1. \wДуэлянт 1: \r%n^n", g_iDuelUsersId[0]);
	} else menu_item("\y1. \wДуэлянт 1: \rНет^n");
	
	if (g_iDuelUsersId[1]) {
		menu_item("\y2. \wДуэлянт 2: \r%n^n", g_iDuelUsersId[1]);
	} else menu_item("\y2. \wДуэлянт 2: \rНет^n");
	
	if (g_iDuelStatus) {
		menu_item("\y3. \wОтмена дуэли^n");
		iKeys |= (1<<2);
	} else if (g_iDuelUsersId[0] && g_iDuelUsersId[1]) {
		menu_item("\y3. \wСтарт дуэли^n");
		iKeys |= (1<<2);
	} else menu_item("\y3. \dСтарт дуэли^n");
	
	if (!g_iDuelStatus) {
		menu_item("^n\dВыбор дуэлянтов по прицелу:^n");
		menu_item("\y4. \wДуэлянт 1^n");
		menu_item("\y5. \wДуэлянт 2^n");
		iKeys |= (1<<3|1<<4);
	} else {
		menu_item("^n^n^n^n");
	}
	
	menu_item("^n^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	return menu_show(id, iKeys, "Show_GameDuelMenu");
}

public Handle_GameDuelMenu(id, iKey)
{
	switch (iKey)
	{
		case 0: {
			g_iMenuType[id] = 0;
			return Show_GameDuelUsersMenu(id, _, 1, RETURN_CHIEF);
		}
		case 1: {
			g_iMenuType[id] = 1;
			return Show_GameDuelUsersMenu(id, _, 1, RETURN_CHIEF);
		}
		case 2: {
			if (g_iDuelStatus) {
				client_print_color(0, 0, "%s Начальник отменил дуэль.", g_szChatPrefix);
				jbe_duel_cancel();
				return Show_GameDuelMenu(id);
			} else return Show_ChoiceDuelMenu(id);
		}
		case 3, 4: {
			new iTarget, iBody;
			get_user_aiming(id, iTarget, iBody); 
			if (jbe_is_user_valid(iTarget)) {
				if (IsSetBit(g_iBitUserWanted, iTarget) || IsSetBit(g_iBitUserFree, iTarget)
				|| IsSetBit(g_iBitUserBoxing, iTarget) || IsSetBit(g_iBitUserSoccer, iTarget)) {
					client_print_color(id, 0, "%s Не применимо к данному игроку!", g_szChatPrefix);
					return Show_GameDuelMenu(id);
				}
				
				if (g_iDuelUsersId[0] == iTarget || g_iDuelUsersId[1] == iTarget)
				{
					client_print_color(id, iTarget, "%s ^1Игрок ^3%n ^1уже выбран.", g_szChatPrefix, iTarget);
					return Show_GameDuelMenu(id);
				}
				
				g_iDuelUsersId[iKey - 3] = iTarget;
				client_print_color(0, iTarget, "%s ^4%n ^1Выставил ^3%n ^1как ^4%d-го ^1дуэлянта.", g_szChatPrefix, id, iTarget, iKey - 3 == 0 ? 1 : 2);
				
			}
			else client_print_color(id, print_team_blue, "%s ^1Наведите ^4прицел ^1на игрока и ^3повторите попытку^1.", g_szChatPrefix);
			return Show_GameDuelMenu(id);
		}
		case 8: return Show_MiniGameMenu(id);
	}
	return PLUGIN_HANDLED;
}

Show_GameDuelUsersMenu(id, iPos = 0, iMessage = 0, iReturn = 0)
{
	if (iPos < 0) return Show_GameDuelMenu(id);
	
	if (g_iDayMode != DAY_NORMAL || id != g_iChiefId)
		return PLUGIN_HANDLED;

	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++)
	{
	#if !defined DEVELOPER_OPTIONS
		if (IsNotSetBit(g_iBitUserAlive, i) || IsSetBit(g_iBitUserWanted, i) || IsSetBit(g_iBitUserFree, i)
		|| IsSetBit(g_iBitUserBoxing, i) || IsSetBit(g_iBitUserSoccer, i) || i == g_iChiefId) continue;
	#else
		if (IsNotSetBit(g_iBitUserAlive, i) || IsSetBit(g_iBitUserWanted, i) || IsSetBit(g_iBitUserFree, i)
		|| IsSetBit(g_iBitUserBoxing, i) || IsSetBit(g_iBitUserSoccer, i)) continue;
	#endif
		
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!iPlayersNum)
	{
		iMessage && client_print_color(id, id, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
		switch (iReturn)
		{
			case 1: return Show_GameDuelMenu(id);
			default: return PLUGIN_HANDLED;
		}
	}

	menu_list_title("Выберите игрока", iPos, iPlayersNum);
	menu_item("^n");

	new iTarget;
	while (menu_list_count())
	{
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		menu_list_item("\y%d. \w%n%s^n", menu_list_num(), iTarget,
			(g_iDuelUsersId[0] == iTarget || g_iDuelUsersId[1] == iTarget) ? " \r[выбран]" : "");
	}

	return menu_list_show(id, "Show_GameDuelUsersMenu");
}

public Handle_GameDuelUsersMenu(id, iKey)
{
	if (g_iDayMode != DAY_NORMAL || id != g_iChiefId)
		return PLUGIN_HANDLED;
	
	switch (iKey)
	{
		case 8: return Show_GameDuelUsersMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_GameDuelUsersMenu(id, --g_iMenuPosition[id]);
	}

	new iTarget = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];

	if (g_iDuelUsersId[0] == iTarget || g_iDuelUsersId[1] == iTarget)
	{
		client_print_color(id, id, "%s ^1Игрок ^4%n ^1уже выбран.", g_szChatPrefix, iTarget);
		return Show_GameDuelUsersMenu(id, g_iMenuPosition[id]);
	}

	g_iDuelUsersId[g_iMenuType[id]] = iTarget;
	client_print_color(id, id, "%s ^4%n ^1Выставил ^4%n ^1как ^3%d-го ^1дуэлянта.", g_szChatPrefix,
		id, iTarget, g_iMenuType[id] == 0 ? 1 : 2);
	
	return Show_GameDuelMenu(id);
}

Show_RandomNumMenu(id)
{
	new iKeys = (1<<0|1<<1|1<<2|1<<3|1<<6|1<<8|1<<9);

	menu_title("Случайное число^n^n");

	menu_item("\y1. \wВвести число^n");
	menu_item("\y2. \wОт 1 до 3^n");
	menu_item("\y3. \wОт 1 до 6^n");
	menu_item("\y4. \wОт 1 до 10^n");

	if (g_iAlivePlayersNum[TEAM_PRISONER] > 1)
	{
		menu_item("\y5. \wОт 1 до %d^n", g_iAlivePlayersNum[TEAM_PRISONER]);
		g_iMenuPlayers[id][0] = g_iAlivePlayersNum[TEAM_PRISONER];
		iKeys |= (1<<4);
	}
	else menu_item("^n");

	if (jbe_get_players_num(NUM_ALIVE_ALL) > 1)
	{
		menu_item("\y6. \wОт 1 до %d^n", jbe_get_players_num(NUM_ALIVE_ALL));
		g_iMenuPlayers[id][1] = jbe_get_players_num(NUM_ALIVE_ALL);
		iKeys |= (1<<5);
	}
	else menu_item("^n");

	menu_item("\y7. \wОрел | Решка^n");

	menu_item("^n^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	
	return menu_show(id, iKeys, "Show_RandomNumMenu");
}

public Handle_RandomNumMenu(id, iKey)
{
	new iNum;
	switch (iKey)
	{
		case 0:
		{
			client_print_color(id, print_team_blue,
				"Введите значение максимального числа или интервал значений ( '1 999' )");
			return client_cmd(id, "messagemode ^"rnd_num^"");
		}
		case 1: iNum = 3;
		case 2: iNum = 6;
		case 3: iNum = 10;
		case 4: iNum = g_iMenuPlayers[id][0];
		case 5: iNum = g_iMenuPlayers[id][1];
		case 6:
		{
			new const szCent[][] = { "Орел", "Решка" };
			client_print_color(0, print_team_blue, "%s ^1Начальник подбросил монету, Результат: ^4%s^1.", g_szChatPrefix, szCent[random(2)]);
		}
		case 8: return Show_MiniGameMenu(id, 1);
		case 9: return PLUGIN_HANDLED;
	}

	iNum && client_print_color(0, print_team_blue, 
		"%s ^1Случайное число ^3(от 1 до %d) ^1Результат: ^4%d",
		g_szChatPrefix, iNum, random_num(1, iNum));

	return Show_RandomNumMenu(id);
}

new const g_szSkinMenuItem[][] = {
	"Оранжевая",	
	"Серая",		
	"Желтая",		
	"Синяя",		
	"Коричневая",	
	"Черная",		
//	"Зеленая",	
//	"Красная",	
//	"Футболист красный",
//	"Футболист синий"
};

new const g_szSkinMenuChat[][] = {
	"оранжевую",
	"серую",
	"жёлтую",
	"синюю",
	"коричневую", 
	"черную",
//	"зеленую",
//	"красную",
//	"футбольную красную",
//	"футбольную синюю" 
};

Show_SetSkinMenu(id, iPos = 0)
{
	if (iPos < 0) return PLUGIN_HANDLED;

	if (SKIN_FB_BLUE < g_iMenuType[id] < SKIN_ORANGE)
		g_iMenuType[id] = SKIN_ORANGE;
	
	menu_list_title("Выдать одежду", iPos, sizeof(g_szSkinMenuItem), .iPerPage = 6);
	menu_item("^n");
	menu_list_item("\y1. \wВыбрать игрока^n");
	menu_list_item("\y2. \wПо прицелу^n");
	menu_item("\dВыберите одежду:^n");

	while (menu_list_count())
	{
		menu_list_item("\y%d. %s%s^n", menu_list_num(), 
			g_iMenuType[id] == menu_list_countid() ? "\r" : "\w",
			g_szSkinMenuItem[menu_list_countid()]);
	}

	return menu_list_show(id, "Show_SetSkinMenu");
}

public Handle_SetSkinMenu(id, iKey)
{
	if (IsNotSetBit(g_iBitUserAdmin, id) && id != g_iChiefId)
		return PLUGIN_HANDLED;
			
	if (g_iDayMode == DAY_DAYMODE)
		return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 0: return Show_SetSkinUsersMenu(id);
		case 1:
		{
			new iTarget, iBody;
			get_user_aiming(id, iTarget, iBody);
			if (jbe_is_user_valid(iTarget) && g_iUserTeam[iTarget] == TEAM_PRISONER)
			{
				jbe_set_player_prisoner_model(iTarget);
				set_pev(iTarget, pev_skin, g_iMenuType[id]);

				g_iUserGameTeam[iTarget] = g_iMenuType[id];
				
				client_print_color(0, iTarget, "%s %s ^4%n ^1Выдал ^4%s ^1одежду ^3%n^1.", 
					g_szChatPrefix, id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix,
					id, g_szSkinMenuChat[g_iMenuType[id]], iTarget);
			}
			else
			{
				client_print_center(id, "Наведите прицел");
				return Show_SetSkinMenu(id, g_iMenuPosition[id]);
			}
		}
		case 8: return Show_SetSkinMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_SetSkinMenu(id, --g_iMenuPosition[id]);
		default:
		{
			g_iMenuType[id] = menu_list_itemid(id, iKey) - 2;
			client_print_color(id, id, "%s ^1Вы выбрали ^4%s ^1одежду.", g_szChatPrefix, g_szSkinMenuChat[g_iMenuType[id]]);
		}
	}

	return Show_SetSkinMenu(id, g_iMenuPosition[id]);
}

Show_SetSkinUsersMenu(id, iPos = 0)
{
	if (iPos < 0) return Show_SetSkinMenu(id);

	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || IsSetBit(g_iBitUserWanted, i)
		|| IsSetBit(g_iBitUserFree, i) || g_iUserTeam[i] != TEAM_PRISONER)
			continue;

		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!menu_list_title("Одежда", iPos, iPlayersNum))
	{
		client_print_color(id, id, "%s Подходящие игроки не найдены.", g_szChatPrefix);
		return PLUGIN_HANDLED;
	}

	menu_item("\dВыберите игрока^n");
	new iTarget;
	while (menu_list_count())
	{
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		menu_list_item("\y%d. \w%n^n", menu_list_num(), iTarget);
	}

	return menu_list_show(id, "Show_SetSkinUsersMenu");
}

public Handle_SetSkinUsersMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_SetSkinUsersMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_SetSkinUsersMenu(id, --g_iMenuPosition[id]);
		default:
		{
			new iTarget = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];
			if (IsSetBit(g_iBitUserAlive, iTarget))
			{
				if (IsSetBit(g_iBitUserWanted, iTarget) || IsSetBit(g_iBitUserFree, iTarget))
					client_print_color(iTarget, 0, "%s Невозможно выдать одежду этому игроку.", g_szChatPrefix);

				else
				{
					jbe_set_player_prisoner_model(iTarget);
					set_pev(iTarget, pev_skin, g_iMenuType[id]);

					g_iUserGameTeam[iTarget] = g_iMenuType[id];

					client_print_color(0, 0, "%s %s ^4%n ^1Выдал ^3%s ^1одежду ^4%n^1.", 
						g_szChatPrefix, id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix,
						id, g_szSkinMenuChat[g_iMenuType[id]], iTarget);
				}
			}
			else client_print_color(id, 0, "%s Игрок мертв.", g_szChatPrefix);
		}
	}
	return Show_SetSkinUsersMenu(id, g_iMenuPosition[id]);
}
