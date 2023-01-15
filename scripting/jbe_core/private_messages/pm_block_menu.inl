
/*
	Blocked user in private messages
*/

Cmd_PM_BlockMenu(id) return Show_PM_BlockMenu(id, g_iMenuPosition[id] = 0);
Show_PM_BlockMenu(id, iPos)
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
		menu_list_item("\y%d. \w%n%s^n", menu_list_num(), iPlayer,
			IsSetBit(g_iBitBlockedPM[id], iPlayer) ? " \r[block]" : "");
	}

	return menu_list_show(id, "Show_PM_BlockMenu");
}

public Handle_PM_BlockMenu(id, iKey) {
	switch (iKey) {
		case 8: return Show_PM_BlockMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_PM_BlockMenu(id, --g_iMenuPosition[id]);
		
		default: {
			new iTarget = g_iMenuPlayers[id][menu_list_itemid(id, iKey)];
			if (IsNotSetBit(g_iBitUserConnected, iTarget)) {
				client_print_color(id, id, "%s Игрок вышел с сервера", g_szChatPrefix);
				return Show_PM_BlockMenu(id, g_iMenuPosition[id]);
			}

			InvertBit(g_iBitBlockedPM[id], iTarget);

			client_print_color(id, iTarget, "%s Вы ^4%s ^1игрока ^3%n^1.", g_szChatPrefix, 
				IsSetBit(g_iBitBlockedPM[id], iTarget) ? "заблокировали" : "разблокировали",
				iTarget);
		}
	}
	return Show_PM_BlockMenu(id, g_iMenuPosition[id]);
}