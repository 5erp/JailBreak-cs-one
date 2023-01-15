Show_VipRespawnMenu(id, iPos = 0, iMessage = 0, iReturn = 0)
{
	if (iPos < 0 || IsNotSetBit(g_iBitUserVip, id))
		return PLUGIN_HANDLED;

	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsSetBit(g_iBitUserAlive, i) || i == id
		|| (g_iUserTeam[i] != TEAM_PRISONER && g_iUserTeam[i] != TEAM_GUARD))
			continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!menu_list_title("\wВыберите кого возродить", iPos, iPlayersNum))
	{
		iMessage && client_print_color(id, id, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
		switch (iReturn)
		{
			case 1: return Show_VipMenu(id);
			default: return PLUGIN_HANDLED;
		}
	}
	menu_item("^n");

	new iPlayer;
	while (menu_list_count())
	{
		iPlayer = g_iMenuPlayers[id][menu_list_countid()];
		menu_list_item("\y%d. \w%n^n", menu_list_num(), iPlayer);
	}

	return menu_list_show(id, "Show_VipRespawnMenu");
}

public Handle_VipRespawnMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_VipRespawnMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_VipRespawnMenu(id, --g_iMenuPosition[id]);
	}

	if ((g_iAlivePlayersNum[TEAM_PRISONER] < g_eAllCvars[RESPAWN_PLAYER_NUM]))
	{
		client_print_color(id, id, "%s ^1Возрождать можно если живо от ^4%d ^1заключенных.", g_szChatPrefix, g_eAllCvars[RESPAWN_PLAYER_NUM]);
		return Show_VipMenu(id);
	}

	new iTarget = g_iMenuPlayers[id][menu_list_itemid(id, iKey)];

	if (IsNotSetBit(g_iBitUserAlive, iTarget)
	&& (g_iUserTeam[iTarget] == TEAM_PRISONER || g_iUserTeam[iTarget] == TEAM_GUARD))
	{
		rg_round_respawn(iTarget);
		g_iVip[id][DV_RES_PLAYERS]--;
		client_print_color(0, iTarget, "%s^1[^4VIP^1] ^4%n ^1возродил ^3%n^1.", g_szChatPrefix, id, iTarget);
		respawm_set_effects(iTarget);
	}

	return Show_VipRespawnMenu(id, g_iMenuPosition[id]);
}