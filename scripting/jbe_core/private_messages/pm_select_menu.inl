
/*
	Select user to private messages
*/

Cmd_PM_SelectMenu(id) return Show_PM_SelectMenu(id, g_iMenuPosition[id] = 0);
Show_PM_SelectMenu(id, iPos)
{
	if (iPos < 0) return PLUGIN_HANDLED;

	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++) {
		if (IsNotSetBit(g_iBitUserConnected, i) || i == id) continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!menu_list_title("\wВыберите игрока", iPos, iPlayersNum)) {
		client_print_color(id, id, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
		return Show_PrivateMessageMenu(id);
	}
	menu_item("^n");

	new iPlayer;
	while (menu_list_count()) {
		iPlayer = g_iMenuPlayers[id][menu_list_countid()];
		menu_list_item("\y%d. \w%n^n", menu_list_num(), iPlayer);
	}

	return menu_list_show(id, "Show_PM_SelectMenu");
}

public Handle_PM_SelectMenu(id, iKey) {
	switch (iKey) {
		case 8: return Show_PM_SelectMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_PM_SelectMenu(id, --g_iMenuPosition[id]);
		
		default: {
			g_PM_CurrentTarget[id] = g_iMenuPlayers[id][menu_list_itemid(id, iKey)];
			
			if (!check_not_pm_access(id))
				client_cmd(id, "messagemode ^"pm^"");
		}
	}
	return Show_PM_SelectMenu(id, g_iMenuPosition[id]);
}

