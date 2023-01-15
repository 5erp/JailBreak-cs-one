Show_JihadMenu(id)
{
	new iKeys = (1<<8|1<<9);
	menu_title("Битва за Джихад^n^n");

	if (g_bJihadGame)
	{
		menu_item("\y1. \wВыдача ролей^n");
		menu_item("\y2. \wНачало битвы^n");
		menu_item("\y3. \wСписок игроков без роли^n");
		menu_item("\y4. \wУбить кто без роли^n");
		menu_item("\y5. \wТелепорт к игроку^n");
		menu_item("\y6. \dПоджечь игрока^n");
		iKeys |= (1<<0|1<<1|1<<2|1<<3|1<<4);
	}
	else
	{
		menu_item("\d1. Выдача ролей^n");
		menu_item("\d2. Начало битвы^n");
		menu_item("\d3. Список игроков без роли^n");	
		menu_item("\d4. Убить кто без роли^n");
		menu_item("\d5. Телепорт к игроку^n");
		menu_item("\d6. Поджечь игрока^n");
	}

	menu_item("^n^n\y9. \w%s игру^n", g_bJihadGame ? "Выключить" : "Включить");
	menu_item("\d0. Выход");

	return menu_show(id, iKeys, "Show_JihadMenu");
}

public Handle_JihadMenu(id, iKey)
{
	if (is_not_access_jihad_menu(id))
		return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 0:
		{
			if (g_bJihadStart)
				client_print_color(id, id, "%s ^1Невозможно выдать раль когда начата битва.", g_szJihadPrefix);
			else
				return Show_JihadRolesMenu(id);
		}
		case 1:
		{
			if (g_bJihadStart) return PLUGIN_HANDLED;
			
			new bool:bIsNotRole = false;
			for (new i = 1; i <= MaxClients; i++)
			{
				if (IsNotSetBit(g_iBitUserAlive, i) || i == g_iChiefId || g_iJihadRole[i])
					continue;
		
				bIsNotRole = true;
			}
			
			if (bIsNotRole)
				client_print_color(g_iChiefId, 0, "%s Невозможно начать игру пока не разданы роли всем игрокам!", g_szJihadPrefix);
			else
				jihad_start();
		}
		case 2: jihad_show_no_roles(id);
		case 3: jihad_kill_no_roles();
		case 4: return Show_JihadTeleportMenu(id);
		case 8: g_bJihadGame ? jihad_disable() : jihad_enable();
		case 9: return PLUGIN_HANDLED;
	}
	return Show_JihadMenu(id);
}

Show_JihadRolesMenu(id, iPos = 0)
{
	if (iPos < 0) return Show_JihadMenu(id);

	menu_list_title("Выдача ролей^n\dНаведите прицел^n", iPos, charsmax(g_szJihadNameRoles));

	while (menu_list_count())
	{
		menu_list_item("\y%d. \w%s^n", menu_list_num(), g_szJihadNameRoles[ menu_list_countid() + 1 ]);
	}

	return menu_list_show(id, "Show_JihadRolesMenu");
}

public Handle_JihadRolesMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_JihadRolesMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_JihadRolesMenu(id, --g_iMenuPosition[id]);
	}

	new iTarget, iBody;
	get_user_aiming(id, iTarget, iBody);

	if (jbe_is_user_valid(iTarget))
	{
		new iRole = menu_list_itemid(id, iKey) + 1;

		jihad_set_user_role(iTarget, iRole);
	}

	else client_print_center(id, "Наведите прицел");

	return Show_JihadRolesMenu(id, g_iMenuPosition[id]);
}

Show_JihadTeleportMenu(id, iPos = 0)
{
	if (iPos < 0) return Show_JihadMenu(id);

	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || i == g_iChiefId)
			continue;

		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!menu_list_title("Teleport Menu", iPos, iPlayersNum))
		return Show_JihadMenu(id);

	new iTarget;
	while (menu_list_count())
	{
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];

		menu_list_item("\y%d. \w%n^n", menu_list_num(), iTarget);
	}

	return menu_list_show(id, "Show_JihadTeleportMenu");
}

public Handle_JihadTeleportMenu(id, iKey)
{
	if (is_not_access_jihad_menu(id))
		return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 8: return Show_JihadTeleportMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_JihadTeleportMenu(id, --g_iMenuPosition[id]);
	}

	new iTarget = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];

	if (IsNotSetBit(g_iBitUserAlive, iTarget))
	{
		jbe_info_center_msg(id, "Игрок мертв...", iTarget);
		return Show_JihadTeleportMenu(id, g_iMenuPosition[id]);
	}

	new Float:fOrigin[3], Float:fAngle[3];

	get_entvar(iTarget, var_origin, fOrigin);
	get_entvar(iTarget, var_v_angle, fAngle);

	set_entvar(id, var_origin, fOrigin);
	set_entvar(id, var_angles, fAngle);

	jbe_info_center_msg(id, "Вы телепортировались к %n", iTarget);

	return Show_JihadTeleportMenu(id, g_iMenuPosition[id]);
}