// duel_prize_target_menu.inl

Cmd_DuelPrizeTargetMenu(id) return Show_DuelPrizeTargetMenu(id, g_iMenuPosition[id] = 0);
Show_DuelPrizeTargetMenu(id, iPos)
{
	if (iPos < 0 || id != g_iLastPnId || IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;
	
	new iPlayersNum;
	g_iMenuPlayers[id][iPlayersNum++] = id; // игрок 1-й в списке
	for (new i = 1; i <= MaxClients; i++)
	{
		if (g_iUserTeam[i] != TEAM_PRISONER || i == id) continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	menu_list_title("Выберите игрока:", iPos, iPlayersNum);
	menu_item("^n");

	new iTarget;
	while (menu_list_count())
	{
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		if (iTarget == id)
			menu_list_item("\y%d. \wНаграда мне^n", menu_list_num());
		else
			menu_list_item("\y%d. \w%n^n", menu_list_num(), iTarget);
	}

	return menu_list_show(id, "Show_DuelPrizeTargetMenu");
}

public Handle_DuelPrizeTargetMenu(id, iKey)
{
	if (id != g_iLastPnId || IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 8: Show_DuelPrizeTargetMenu(id, ++g_iMenuPosition[id]);
		case 9: Show_DuelPrizeTargetMenu(id, --g_iMenuPosition[id]);
		default:
		{
			new iTarget = g_iMenuPlayers[id][menu_list_itemid(id, iKey)];
			if (g_iUserTeam[iTarget] == TEAM_PRISONER)
			{
				g_iDuelPrizeTarget = iTarget;
				return Show_DuelPrizeMenu(id);
			}
			else
				Show_DuelPrizeTargetMenu(id, g_iMenuPosition[id]);
		}
	}
	return PLUGIN_HANDLED;
}