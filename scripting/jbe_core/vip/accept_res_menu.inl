Show_UsersAcceptResMenu(id, iPos = 0)
{
	if (iPos < 0)
	{
		g_iBitVipResAccept = 0;
		return PLUGIN_HANDLED;
	}

	new iPlayers;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserConnected, i)
		|| IsSetBit(g_iBitUserAlive, i) || IsNotSetBit(g_iBitVipResAccept, i)
		|| g_iUserTeam[i] == TEAM_SPECTATOR || g_iUserTeam[i] == TEAM_UNASSIGNED)
			continue;

		g_iMenuPlayers[id][iPlayers++] = i;
	}

	if (!iPlayers)
	{
		g_iBitVipResAccept = 0;
		return PLUGIN_HANDLED;
	}

	menu_list_title("Хотят возродится", iPos, iPlayers);
	menu_item("^n");

	while (menu_list_count())
		menu_list_item("\y%d. \w%n^n", menu_list_num(),
			g_iMenuPlayers[id][menu_list_countid()]);

	return menu_list_show(id, "Show_UsersAcceptResMenu");
}

public Handle_UsersAcceptResMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_UsersAcceptResMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_UsersAcceptResMenu(id, --g_iMenuPosition[id]);
	}

	g_iMenuTarget[id] = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];
	
	return Show_AcceptResMenu(id);
}

Show_AcceptResMenu(id)
{
	if (id != g_iChiefId)
		return PLUGIN_HANDLED;
	
	menu_title("Игрок \r%n^n\
		\wХочет возродится^n^n", g_iMenuTarget[id]);

	menu_item("\y3. \wРазрешить^n^n");
	menu_item("\y5. \wЗапретить^n^n^n^n");

	menu_item("\d0. Выход");

	return menu_show(id, (1<<2|1<<4|1<<9), "Show_AcceptResMenu");
}

public Handle_AcceptResMenu(id, iKey)
{
	new iTarget = g_iMenuTarget[id];
	if (IsNotSetBit(g_iBitUserConnected, iTarget))
		return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 2:
		{
			vip_respawn(iTarget);
			g_iVipResAcceptTime[iTarget] = 0;
			ClearBit(g_iBitVipResAccept, iTarget);
			client_print_color(0, iTarget, "%s Начальник разрешил возродиться ^3%n^1.", g_szChatPrefix, iTarget);
		}
		case 4:
		{
			g_iVipResAcceptTime[iTarget] = get_systime() + VIP_RES_ACCEPT_TIME;
			ClearBit(g_iBitVipResAccept, iTarget);
			client_print_color(iTarget, iTarget, "%s Начальник запретил вам возродиться.", g_szChatPrefix);
			client_print_color(id, iTarget, "%s Вы запретили возродиться ^3%n^1.", g_szChatPrefix, iTarget);
		}
		case 9:
		{
			if (g_iBitVipResAccept)
				return Show_UsersAcceptResMenu(id);
		}
	}

	if (g_iBitVipResAccept)
		return Show_UsersAcceptResMenu(id);

	return PLUGIN_HANDLED;
}

