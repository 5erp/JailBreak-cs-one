// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

Show_BraceletesMenu(id, iPos = 0, iMessage = 0) // users menu
{
	if (iPos < 0) return PLUGIN_HANDLED;
	
	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserConnected, i))
			continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
		
		// при открытии меню делаем пасивную проверку игрокам
		is_user_active_bracelets(i); 
	}

	if (!iPlayersNum)
	{
		iMessage && client_print_color(id, 0, "%s ^1Подходящие игроки не найдены.", g_szBrPrefix);
		return PLUGIN_HANDLED;
	}

	menu_list_title("Меню наручников", iPos, iPlayersNum);
	menu_item("\dВыберите игрока:^n");

	new iTarget;
	while (menu_list_count())
	{
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];

		if (g_iUserBraceletsType[iTarget])
			menu_list_item("\y%d. \w%n \y[\r%d\y]^n", menu_list_num(),
				iTarget, g_iUserBraceletsTime[iTarget] / 60);
		else
			menu_list_item("\y%d. \w%n^n", menu_list_num(), iTarget);
	}

	return menu_list_show(id, "Show_BraceletesMenu");
}

public Handle_BraceletesMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_BraceletesMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_BraceletesMenu(id, --g_iMenuPosition[id]);
		default:
		{
			g_iMenuTarget[id] = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];

			if (IsNotSetBit(g_iBitUserConnected, g_iMenuTarget[id]))
				return client_print_color(id, id, "%s Игрок вышел с сервера.", g_szChatPrefix);

			if (!g_iUserBraceletsType[ g_iMenuTarget[id] ])
			{
				g_iBraceletsSelect[id] = MB_SET;
				return Show_BraceletesTimeMenu(id);
			}
			else
			{
				g_iBraceletsSelect[id] = MB_EDIT;
				return Show_BraceletesEditMenu(id);
			}		
		}
	}
	return Show_BraceletesMenu(id, g_iMenuPosition[id]);
}

Show_BraceletesTimeMenu(id, iPos = 0)
{
	if (iPos < 0) return PLUGIN_HANDLED;

	if (BR_ATTACK_BUTTON < g_iMenuType[id] || g_iMenuType[id] < BR_ATTACK)
		g_iMenuType[id] = BR_ATTACK;
 
	menu_list_title("Управление наручниками", iPos, sizeof(g_iszBraceletesTime), 6);
	menu_item("^n");
	menu_list_item("\y1. \wИгрок: \r%n^n", g_iMenuTarget[id]);
	menu_list_item("\y2. \wТип блока: \y%s^n^n", g_szBrTypes[g_iMenuType[id]]);
		
	while (menu_list_count())
		menu_list_item("\y%d. \w%s^n", menu_list_num(),
			g_iszBraceletesTime[menu_list_countid()][BRACELETES_NAME]);

	return menu_list_show(id, "Show_BraceletesTimeMenu");
}

public Handle_BraceletesTimeMenu(id, iKey)
{
	switch (iKey)
	{
		case 0: return Show_BraceletesMenu(id);
		case 1: 
		{
			if (g_iMenuType[id] == BR_ATTACK_BUTTON || g_iMenuType[id] == BR_NONE)
				g_iMenuType[id] = BR_ATTACK;

			else
				g_iMenuType[id] = BR_ATTACK_BUTTON;
		}
		case 8: return Show_BraceletesTimeMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_BraceletesTimeMenu(id, --g_iMenuPosition[id]);
		default:
		{
			new iItem = menu_list_itemid(id, iKey - 2);

			is_user_active_bracelets(g_iMenuTarget[id]);

			if (g_iBraceletsSelect[id] == MB_SET && g_iUserBraceletsType[g_iMenuTarget[id]])
				return client_print_color(id, id, "%s Игрок уже в наручниках.", g_szChatPrefix);

			return jbe_set_user_bracelets(id,
					.iTarget =  g_iMenuTarget[id],
					.iType = g_iMenuType[id],
					.iTime = g_iszBraceletesTime[iItem][BRACELETES_TIME]);
		}
	}

	return Show_BraceletesTimeMenu(id, g_iMenuPosition[id]);
}

Show_BraceletesEditMenu(id)
{
	menu_title("Управление наручниками^n^n");
	menu_item("\y1. \wИгрок: \r%n^n^n", g_iMenuTarget[id]);

	menu_item("\wТип блока: \y%s^n", g_szBrTypes[g_iUserBraceletsType[g_iMenuTarget[id]]]);
	menu_item("\wВремя до снятия: \y%s^n^n", seconds_to_str(g_iUserBraceletsTime[g_iMenuTarget[id]]));

	menu_item("\y2. \wСменить блок^n");
	menu_item("\y3. \wСнять наручники^n^n^n");

	menu_item("^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");

	return menu_show(id, (1<<0|1<<1|1<<2|1<<9), "Show_BraceletesEditMenu");
}

public Handle_BraceletesEditMenu(id, iKey)
{
	switch (iKey)
	{
		case 0: return Show_BraceletesMenu(id);
		case 1: return Show_BraceletesTimeMenu(id);
		case 2:
		{
			is_user_active_bracelets(g_iMenuTarget[id]);

			if (!g_iUserBraceletsType[g_iMenuTarget[id]])
				return client_print_color(id, id, "%s Игрок не в наручниках.", g_szChatPrefix);

			jbe_set_user_bracelets(id, g_iMenuTarget[id], 0, 0);
		}
		case 8: return Show_BraceletesMenu(id);
	}
	return PLUGIN_HANDLED;
}