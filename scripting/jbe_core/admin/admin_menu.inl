Show_AdminPlayerMenu(id) { g_iMenuTarget[id] = 0; return Show_GrabAdminMenu(id); }

Show_AdminMenu(id, iPos = 0)
{
	if (iPos < 0) return PLUGIN_HANDLED;

	new const aMenu[][] = {
		"Кикнуть", //1
		"Забанить", //2
		"Ударить|Убить", //3
		"Сменить команду", //4
		"Голосование за карту", //5
		"Мут|Гаг", //6
		"Блок CT", //7
		"Наручники", //8

		"Офф Бан", //9
		"Сменить карту" //10
	};

	menu_list_title("Админ Меню", iPos, sizeof(aMenu));
	menu_item("^n");

	while (menu_list_count())
		menu_list_item("\y%d. \w%s^n", menu_list_num(), aMenu[menu_list_countid()]);

	return menu_list_show(id, "Show_AdminMenu");
}

public Handle_AdminMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_AdminMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_AdminMenu(id, --g_iMenuPosition[id]);
	}

	switch (menu_list_itemid(id, iKey))
	{
		case 0: return Cmd_AdminKickMenu(id);
		case 1: return amxclient_cmd(id, "amx_banmenu");
		case 2: return Cmd_AdminSlapMenu(id);
		case 3: return Cmd_AdminTeamMenu(id);
		case 4: return amxclient_cmd(id, "amx_votemap_menu");
		case 5: return amxclient_cmd(id, "amx_gagmenu");
		case 6: return Cmd_BlockGuardMenu(id);
		case 7: return Show_BraceletesMenu(id, _, 1);

		case 8: return amxclient_cmd(id, "amx_offbanmenu");
		case 9: return amxclient_cmd(id, "amx_changemap_menu");
	}

	return Show_AdminMenu(id, g_iMenuPosition[id]);
}

// Admin Team Menu
Cmd_AdminTeamMenu(id) { g_iMenuType[id] = 0; return Show_AdminTeamMenu(id, g_iMenuPosition[id] = 0, .bOpen = true); }

Show_AdminTeamMenu(id, iPos, bool:bOpen = false)
{
	if (iPos < 0 || IsNotSetBit(g_iBitUserAdmin, id))
		return PLUGIN_HANDLED;
	
	new const szTeams[][] = {
		"Зеки",
		"Охрана",
		"Спектр"
	};

	new const szTeamName[][] = {
		"\dun",
		"\yTR",
		"\yCT",
		"\dspec"
	};
	
	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++) {
		if (IsNotSetBit(g_iBitUserConnected, i)) continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}
	
	if (!menu_list_title("Team Menu", iPos, iPlayersNum, 7)) {
		if (bOpen)
			client_print_color(id, 0, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
		return Show_AdminMenu(id, g_iMenuPosition[id]);
	}
	menu_item("\dВыберите игрока:^n");

	new iTarget;
	while (menu_list_count())
	{
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];

		if (g_iUserTeam[iTarget] != ((g_iMenuType[id] % 3) + 1)
		&& g_iUserTeam[iTarget] != TEAM_UNASSIGNED
		&& (iTarget != id || iTarget == id && !jbe_is_user_guard_block(id)))
			menu_list_item("\y%d. \w%n \r[%s\r]^n", menu_list_num(),
				iTarget, szTeamName[g_iUserTeam[iTarget]]);

		else if (access(iTarget, ADMIN_IMMUNITY) && iTarget != id)
			menu_list_item("\d%d. %n \d[%s\d]^n", menu_list_num(),
				iTarget, szTeamName[g_iUserTeam[iTarget]]);
		else
			menul_item_block("\y%d. \d%n \r[%s\r]^n", menu_list_num(),
				iTarget, szTeamName[g_iUserTeam[iTarget]]);
	}

	menu_list_empty_items();
	menu_list_item("^n\y8. \r%s", szTeams[g_iMenuType[id] % 3]);

	return menu_list_show(id, "Show_AdminTeamMenu");
}

public Handle_AdminTeamMenu(id, iKey)
{
	if (IsNotSetBit(g_iBitUserAdmin, id)) return PLUGIN_HANDLED;
	switch (iKey)
	{
		case 7:
		{
			g_iMenuType[id] = ((g_iMenuType[id] + 1) % 3);
			return Show_AdminTeamMenu(id, g_iMenuPosition[id]);
		}
		case 8: return Show_AdminTeamMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_AdminTeamMenu(id, --g_iMenuPosition[id]);
		default:
		{
			new iTarget = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];
			if (IsNotSetBit(g_iBitUserConnected, iTarget))
			{
				client_print_color(id, 0, "%s Игрок вышел с сервера", g_szChatPrefix);
				return PLUGIN_HANDLED;
			}

			if (IsSetBit(g_iBitUserAlive, iTarget) && g_iUserTeam[iTarget] == TEAM_GUARD)
			{
				rg_remove_all_items(iTarget);
			}

			jbe_set_user_team(iTarget, ((g_iMenuType[id] % 3) + 1));

			new const szTeams[][] = { "", "заключенных", "охрану", "спектаторов" }; 
			client_print_color(0, iTarget, "%s %s ^4%n ^1перевел ^4%n ^1за ^3%s^1.",
					g_szChatPrefix, g_szAdminPrefix, id, iTarget, szTeams[g_iUserTeam[iTarget]]);
			
			return Show_AdminTeamMenu(id, g_iMenuPosition[id]);
		}
	}
	return PLUGIN_HANDLED;
}

// Admin Slap Menu
Cmd_AdminSlapMenu(id) {
	g_iMenuType[id] = 0;
	return Show_AdminSlapMenu(id, g_iMenuPosition[id] = 0, 1); 
}
Show_AdminSlapMenu(id, iPos, iOpen = 0)
{
	if (iPos < 0 || IsNotSetBit(g_iBitUserAdmin, id))
		return PLUGIN_HANDLED;
	
	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++) {
		if (IsNotSetBit(g_iBitUserAlive, i)) continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!menu_list_title("Slap Menu", iPos, iPlayersNum, 7))
	{
		if (iOpen)
			client_print_color(id, 0, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
		return Show_AdminMenu(id, g_iMenuPosition[id]);
	}
	menu_item("\dВыберите игрока:^n");

	new iTarget;
	while (menu_list_count()) {
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];

		menu_list_item("\y%d. \w%n \r[%d\r]^n", menu_list_num(),
			iTarget, get_user_health(iTarget));
	}

	menu_list_empty_items();

	if (g_iMenuType[id])
		menu_list_item("^n\y8. \yУдарить на \r%d HP^n", g_iAdminSlaps[g_iMenuType[id] % sizeof(g_iAdminSlaps)]);
	else
		menu_list_item("^n\y8. \yУбить^n");

	return menu_list_show(id, "Show_AdminSlapMenu");
}

public Handle_AdminSlapMenu(id, iKey)
{
	if (IsNotSetBit(g_iBitUserAdmin, id)) return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 7:
		{
			++g_iMenuType[id];
			g_iMenuType[id] %= sizeof(g_iAdminSlaps);
			Show_AdminSlapMenu(id, g_iMenuPosition[id]);
		}
		case 8: return Show_AdminSlapMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_AdminSlapMenu(id, --g_iMenuPosition[id]);
		default:
		{
			new iTarget = g_iMenuPlayers[id][menu_list_itemid(id, iKey)];
			
			if (IsNotSetBit(g_iBitUserAlive, iTarget))
				return client_print_color(id, id, "%s ^1Игрок мертв..", g_szChatPrefix);

			if (is_user_cmd_flood(id, FLOOD_SLAY, 3.0, 6.0))
				return PLUGIN_HANDLED;

			if (g_iMenuType[id])
			{
				user_slap(iTarget, (get_user_health(iTarget) > g_iAdminSlaps[g_iMenuType[id]]) ? g_iAdminSlaps[g_iMenuType[id]] : 0);
				client_print_color(0, iTarget, "%s %s ^4%n ^1ударил ^3%n ^1на ^3%d HP^1.", g_szChatPrefix,
					g_szAdminPrefix, id, iTarget, g_iAdminSlaps[g_iMenuType[id]]);
			}
			else
			{
				client_print_color(0, iTarget, "%s %s ^4%n ^1убил ^3%n^1.", g_szChatPrefix, g_szAdminPrefix, id, iTarget);

				user_kill(iTarget, .flag = 1);
			}
			
			return Show_AdminSlapMenu(id, g_iMenuPosition[id]);
		}
	}
	return PLUGIN_HANDLED;
}

// Admin Kick Menu
Cmd_AdminKickMenu(id) return Show_AdminKickMenu(id, g_iMenuPosition[id] = 0, .iOpen = 1);
Show_AdminKickMenu(id, iPos, iOpen = 0)
{
	if (iPos < 0 || IsNotSetBit(g_iBitUserAdmin, id))
		return PLUGIN_HANDLED;
	
	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++) {
		if (IsNotSetBit(g_iBitUserConnected, i)) continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!menu_list_title("Kick Menu", iPos, iPlayersNum)) {
		if (iOpen)
			client_print_color(id, id, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
		return Show_AdminMenu(id, g_iMenuPosition[id]);
	}
	menu_item("\dВыберите игрока:^n");

	new iTarget;
	while (menu_list_count()) {
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];

		menu_list_item("\y%d. \w%n^n", menu_list_num(), iTarget);
	}

	return menu_list_show(id, "Show_AdminKickMenu");
}

public Handle_AdminKickMenu(id, iKey)
{
	if (IsNotSetBit(g_iBitUserAdmin, id)) return PLUGIN_HANDLED;
	switch (iKey)
	{
		case 8: return Show_AdminKickMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_AdminKickMenu(id, --g_iMenuPosition[id]);
		default:
		{
			new iTarget = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];
			
			if (IsNotSetBit(g_iBitUserConnected, iTarget))
			{
				client_print_color(id, id, "%s Игрок вышел с сервера", g_szChatPrefix);
				return PLUGIN_HANDLED;
			}
			new szAuth[2][32];
			get_user_authid(id, szAuth[0], charsmax(szAuth[]));
			get_user_authid(iTarget, szAuth[1], charsmax(szAuth[]));
			
			new iTargetId = get_user_userid(iTarget);
			log_amx("Kick: ^"%n<%d><%s><>^" kick ^"%n<%d><%s><>^"", id, get_user_userid(id), szAuth[0], iTarget, iTargetId, szAuth[1]);
			client_print(iTarget, print_console, "Вас кикнул администратор: %n", id);
			client_print_color(0, iTarget, "%s %s ^4%n ^1кикнул ^3%n^1.", g_szChatPrefix, g_szAdminPrefix, id, iTarget);
			rh_drop_client(iTarget, fmt("Вас кикнул администратор: %n", id));

			return Show_AdminKickMenu(id, g_iMenuPosition[id]);
		}
	}
	return PLUGIN_HANDLED;
}

Show_AdminModMenu(id, iPos = 0)
{
	if (iPos < 0) return PLUGIN_HANDLED;

	static aMenu[][64] = {
		"Открыть клетки", //1
		"Голос", //2
		"Одежда", //3
		"Выходной", //4
		"Снять розыск", //5
		"Телепорт", //6
		"Вылечить", //7
		"Мяч", //8

		"Огонь по своим Вкл|Выкл", //9
		"Счет игры",
		"Мини музыка", //10
		"Музыка", //11
	};

	if (iPos == 1)
	{
		if (g_iFriendlyFire) aMenu[8] = "Огонь по своим \r[Вкл]";
		else aMenu[8] = "Огонь по своим \r[Выкл]";
	}

	menu_list_title("Мод Меню", iPos, sizeof(aMenu));
	menu_item("^n");

	while (menu_list_count())
		menu_list_item("\y%d. \w%s^n", menu_list_num(), aMenu[menu_list_countid()]);

	return menu_list_show(id, "Show_AdminModMenu");
}

public Handle_AdminModMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_AdminModMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_AdminModMenu(id, --g_iMenuPosition[id]);
	}

	switch (menu_list_itemid(id, iKey))
	{
		case 0: return ClCmd_OpenDoors(id);
		case 1: return Show_VoiceControlMenu(id, _, 1, RETURN_ADMIN);
		case 2: return Show_SetSkinMenu(id);
		case 3: return Show_FreeDayControlMenu(id, _, 1, RETURN_ADMIN);
		case 4: return Show_WantedMenu(id, _, 1, RETURN_ADMIN);
		case 5: return Show_AdminTeleportMenu(id);
		case 6: return Show_TreatPrisonerMenu(id, _, 1, RETURN_ADMIN);
		case 7: return Show_BallMenu(id);

		case 8: ClCmd_FrendlyFire(id);
		case 9: return Show_SoccerScoreMenu(id);
		case 10: return ClCmd_Sound(id);
		case 11: return ClCmd_Music(id);
	}

	return Show_AdminModMenu(id, g_iMenuPosition[id]);
}

/* <- Admin Menu <- */