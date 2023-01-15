Show_MafiaMenu(id)
{
	new iKeys = (1<<8|1<<9);

	menu_title("Мафия^n^n");

	if (g_bMafiaGame && id == g_iChiefId)
	{
		menu_item("\y1. \wВключить %s^n", g_bMafiaDay ? "Ночь" : "День");
		menu_item("\y2. %sВключить экран для роли^n", g_bMafiaDay ? "\d" : "\w");
		menu_item("\y3. \wФорма по прицелу^n^n");

		menu_item("\y4. %sЗаморозка по прицелу^n", g_bMafiaDay ? "\w" : "\d");
		menu_item("\y5. \w%s всех игроков^n^n", g_bMafieFreezed ? "Разморозить" : "Заморозить");
		
		menu_item("\y6. \wСписок ролей в игре^n");
		menu_item("\y7. %sВыдать роль^n^n", g_bMafiaOneDay ? "\d" : "\w");
		
		iKeys |= (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6);
	}
	else
	{
		menu_item("\d1. Включить ночь^n");
		menu_item("\d2. Включить экран для роли^n");
		menu_item("\d3. Форма по прицелу^n^n");

		menu_item("\d4. Заморозка по прицелу^n");
		menu_item("\d5. Заморозить всех игроков^n^n");

		menu_item("\d6. Список ролей в игре^n");
		menu_item("\d7. Выдать роль^n^n");		
	}

	menu_item("\y9. \w%s игру^n", g_bMafiaGame ? "Выключить" : "Включить");
	menu_item("\d0. Выход");

	return menu_show(id, iKeys, "Show_MafiaMenu");
}

public Handle_MafiaMenu(id, iKey)
{
	if (!jbe_is_acces_mafia_menu(id))
		return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 0: 
		{
			if (!g_bMafiaSetsRoles)
				client_print_center(g_iChiefId, "Раздайте роли!");

			else g_bMafiaDay ? jbe_mafia_night() : jbe_mafia_day();
		}
		case 1: return Show_MafiaScreenRoleMenu(id);
		case 2: jbe_mafia_skin_aiming(id);

		case 3: jbe_mafia_freeze_aiming(id);
		case 4: jbe_mafia_freeze(!g_bMafieFreezed);

		case 5: jbe_mafia_list_roles(id);
		case 6: return Show_MafiaRolesMenu(id, _, .iOneOpen = 1);

		case 8: g_bMafiaGame ? jbe_mafia_ended() : jbe_mafia_start();
		case 9: return PLUGIN_HANDLED;
	}

	return Show_MafiaMenu(id);
}

Show_MafiaScreenRoleMenu(id, iPos = 0)
{
	if (iPos < 0) return Show_MafiaMenu(id);

	if (g_bMafiaDay)
	{
		client_print_center(id, "Доступно только ночью");
		return Show_MafiaMenu(id);
	}

	menu_list_title("Включить экран ролям^n\dОстальным экран будет выключен^n", iPos, sizeof(g_szMafiaRoles));

	while (menu_list_count())
	{
		if (menu_list_countid() == DMR_PEACE_CITIZEN)
			menul_item_block("\y%d. \d%s^n", menu_list_num(), g_szMafiaRoles[ menu_list_countid() ]);
		else
			menu_list_item("\y%d. \w%s^n", menu_list_num(), g_szMafiaRoles[ menu_list_countid() ]);
	}

	return menu_list_show(id, "Show_MafiaScreenRoleMenu");
}

public Handle_MafiaScreenRoleMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_MafiaScreenRoleMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_MafiaScreenRoleMenu(id, --g_iMenuPosition[id]);
	}

	new iRole = menu_list_itemid(id, iKey);
	new iRoleNum;
	g_iMafiaCurrentRole = iRole;

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || g_iUserTeam[i] != TEAM_PRISONER)
			continue;

		if (g_iUserMafiaRole[i] == iRole)
		{
			UTIL_ScreenFade(i, 0, 0, 0, 0, 0, 0, 255, 1);
			client_print_color(i, print_team_blue, "%s ^1Вам включен экран, ваш ход. Вы ^3%s^1.", g_szMafiaPrefix, g_szMafiaRoles[iRole]);
			iRoleNum++;
			ClearBit(g_iBitMafiaNight, i);
		}
		else
		{
			UTIL_ScreenFade(i, 0, 0, 4, 0, 0, 0, 255, 1);
			SetBit(g_iBitMafiaNight, i);
		}
	}

	if (iRoleNum)
	{
		client_print_center(id, "Включен экран ролям: %s", g_szMafiaRoles[iRole]);
		client_print_color(id, print_team_blue, "%s Вы включили экран игрокам с ролью: ^3%s^1, (остальным ролям экран выключен).", g_szMafiaPrefix, g_szMafiaRoles[iRole]);
	}
	else client_print_color(id, print_team_blue, "%s Нет игроков с ролью: ^3%s^1.", g_szMafiaPrefix, g_szMafiaRoles[iRole]);

	return Show_MafiaScreenRoleMenu(id, g_iMenuPosition[id]);
}

Show_MafiaRolesMenu(id, iPos = 0, iOneOpen = 0)
{
	if (iOneOpen && !g_bMafiaSetsRoles)
	{
		jbe_mafia_night();
		jbe_info_center_msg(0, "Раздача ролей, включена ночь");
		client_print_color(id, id, "%s ^1Раздайте роли, потом включите день.", g_szMafiaPrefix);	
	}
	else if (g_bMafiaOneDay)
	{
		client_print_color(id, id, "%s ^1Нельзя менять роли среди игры.", g_szMafiaPrefix);
		return Show_MafiaMenu(id);
	}

	if (iPos < 0) return Show_MafiaMenu(id);

	menu_list_title("Выдача ролей^n\dНаведите прицел^n", iPos, sizeof(g_szMafiaRoles));

	while (menu_list_count())
	{
		menu_list_item("\y%d. \w%s^n", menu_list_num(), g_szMafiaRoles[ menu_list_countid() ]);
	}

	return menu_list_show(id, "Show_MafiaRolesMenu");
}

public Handle_MafiaRolesMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_MafiaRolesMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_MafiaRolesMenu(id, --g_iMenuPosition[id]);
	}

	new iTarget, iBody;
	get_user_aiming(id, iTarget, iBody);

	if (jbe_is_user_valid(iTarget) && g_iUserTeam[iTarget] == TEAM_PRISONER)
	{
		new iRole = menu_list_itemid(id, iKey);
		new iOldRole = g_iUserMafiaRole[iTarget];

		if (iOldRole == iRole)
		{
			client_print_color(id, print_team_blue, "%s У игрока ^4%n ^1уже выбрана роль ^3%s^1.", g_szMafiaPrefix, iTarget, g_szMafiaRoles[iRole]);
			return Show_MafiaRolesMenu(id, g_iMenuPosition[id]);
		}

		g_bMafiaSetsRoles = true;

		g_iUserMafiaRole[iTarget] = iRole;

		client_print_color(iTarget, print_team_blue, "%s ^1Вам назначена роль: ^3%s^1.", g_szMafiaPrefix, g_szMafiaRoles[iRole]);

		client_print_color(id, print_team_blue, "%s ^1Вы назначили ^4%n ^1на роль: ^3%s^1, (прежняя роль: %s).", 
			g_szMafiaPrefix, iTarget, g_szMafiaRoles[iRole], g_szMafiaRoles[iOldRole]);
	}

	else client_print_center(id, "Наведите прицел");

	return Show_MafiaRolesMenu(id, g_iMenuPosition[id]);
}