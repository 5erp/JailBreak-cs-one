// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

Cmd_BlockGuardMenu(id) return Show_BlockGuardMenu(id, g_iMenuPosition[id] = 0);
Show_BlockGuardMenu(id, iPos)
{
	if (iPos < 0) return PLUGIN_HANDLED;

	new iPlayersNum, i;
	for (i = 1; i <= MaxClients; i++)
	{ 
		if (g_iUserTeam[i] != TEAM_GUARD) continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}
	for (i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserConnected, i) || g_iUserTeam[i] == TEAM_GUARD) continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	menu_list_title("Выберите игрока:", iPos, iPlayersNum);
	menu_item("^n");

	new iTarget;
	while (menu_list_count()) {
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];

		menu_list_item("\y%d. \w%n%s^n", menu_list_num(), iTarget,
			IsSetBit(g_iBitUserGrBlock, iTarget) ? " \r[block]" : "");
	}

	return menu_list_show(id, "Show_BlockGuardMenu");
}

public Handle_BlockGuardMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_BlockGuardMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_BlockGuardMenu(id, --g_iMenuPosition[id]);
	}

	new iTarget = g_iMenuPlayers[id][menu_list_itemid(id, iKey)];
	g_iMenuPlayer[id] = iTarget;

	if (IsNotSetBit(g_iBitUserGrBlock, iTarget))
		return Cmd_BlockGuardReasonMenu(id);
	
	else if (iTarget == id)
	  client_print_color(id, id, "%s ^1Нельзя снимать блок с себя!", g_szGrBlockPrefix);

	else //Show_BlockGuardReadMenu(id); // меню редактирования блока
		fnUnBlockPlayer(id, iTarget);

	return PLUGIN_HANDLED;
}

Cmd_BlockGuardReasonMenu(id) return Show_BlockGuardReasonMenu(id, g_iMenuPosition[id] = 0);
Show_BlockGuardReasonMenu(id, iPos)
{
	if (iPos < 0) return PLUGIN_HANDLED;

	menu_list_title("Выберите причину блока:", iPos, sizeof(g_szReasons));
	menu_item("^n");

	while (menu_list_count()) {
		menu_list_item("\y%d. \w%s^n", menu_list_num(),
			g_szReasons[ menu_list_countid() ]);
	}

	return menu_list_show(id, "Show_BlockGuardReasonMenu");
}

public Handle_BlockGuardReasonMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_BlockGuardReasonMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_BlockGuardReasonMenu(id, --g_iMenuPosition[id]);
	}

	g_iMenuReason[id] = menu_list_itemid(id, iKey);

	if (g_iMenuReason[id] == 0) // своя причина
		client_cmd(id, "messagemode gr_reason");
	else
		return Cmd_BlockGuardTimeMenu(id);

	return PLUGIN_HANDLED;
}

Cmd_BlockGuardTimeMenu(id) return Show_BlockGuardTimeMenu(id, g_iMenuPosition[id] = 0);
Show_BlockGuardTimeMenu(id, iPos)
{
	if (iPos < 0) return PLUGIN_HANDLED;
	menu_list_title("Выберите время блока:", iPos, sizeof(g_iszTimeBlock));
	menu_item("^n");
	
	while (menu_list_count()) {
		menu_list_item("\y%d. \w%s^n", menu_list_num(), 
			g_iszTimeBlock[ menu_list_countid() ][1]);
	}
	
	return menu_list_show(id, "Show_BlockGuardTimeMenu");
}

public Handle_BlockGuardTimeMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_BlockGuardTimeMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_BlockGuardTimeMenu(id, --g_iMenuPosition[id]);
	}

	g_iMenuTime[id] = menu_list_itemid(id, iKey);

	if (IsNotSetBit(g_iBitUserGrBlock, g_iMenuPlayer[id]))
	{
		fnBlockPlayer(id);
		if (jbe_get_user_team(g_iMenuPlayer[id]) != TEAM_PRISONER)
			jbe_set_user_team(g_iMenuPlayer[id], TEAM_PRISONER);
	}
	else
	{
		client_print_color(id, g_iMenuPlayer[id], 
			"%s Игрок ^3%n ^1уже заблокирован.", g_szGrBlockPrefix, g_iMenuPlayer[id]);
	}

	return PLUGIN_HANDLED;
}