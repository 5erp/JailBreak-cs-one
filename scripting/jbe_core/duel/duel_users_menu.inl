// duel_users_menu.inl

Cmd_DuelUsersMenu(id) return Show_DuelUsersMenu(id, g_iMenuPosition[id] = 0);
Show_DuelUsersMenu(id, iPos)
{
	if (iPos < 0 || id != g_iLastPnId || IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;
	
	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++) {
		if(g_iUserTeam[i] != TEAM_GUARD || IsNotSetBit(g_iBitUserAlive, i))
			continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	menu_list_title("Выберите противника", iPos, iPlayersNum);
	menu_item("^n");

	new iTarget;
	while (menu_list_count())
	{
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		switch (g_iDuelPrizeType) 
		{
			case DUEL_PRIZE_MONEY_CUSTOM:
				menu_list_item("\y%d. \w%n \d(%d $)^n",  menu_list_num(), 
					iTarget, g_iUserMoney[iTarget]);
			
			case DUEL_PRIZE_EXP_CUSTOM:
				menu_list_item("\y%d. \w%n \d(%d опыт)^n", menu_list_num(), 
					iTarget, g_iUserExp[iTarget]);
			
			default:
				menu_list_item("\y%d. \w%n^n", menu_list_num(), iTarget);
		}
	}

	return menu_list_show(id, "Show_DuelUsersMenu");
}

public Handle_DuelUsersMenu(id, iKey)
{
	if (id != g_iLastPnId || IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 8: Show_DuelUsersMenu(id, ++g_iMenuPosition[id]);
		case 9: Show_DuelUsersMenu(id, --g_iMenuPosition[id]);
		default:
		{
			new iTarget = g_iMenuPlayers[id][menu_list_itemid(id, iKey)];
			if (IsSetBit(g_iBitUserAlive, iTarget))
			{
				g_iMenuTarget[id] = iTarget;
				if (g_iDuelPrizeType <= DUEL_PRIZE_VOICE)
					return Show_DuelPrizeMenu(id);
				else
					return Show_DuelCustomPrizeMenu(id);
			}
			else
				Show_DuelUsersMenu(id, g_iMenuPosition[id]);
		}
	}
	return PLUGIN_HANDLED;
}