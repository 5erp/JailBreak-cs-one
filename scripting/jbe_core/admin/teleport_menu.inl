Show_AdminTeleportMenu(id, iPos = 0)
{
	if (iPos < 0) return PLUGIN_HANDLED;

	new iPlayersNum;
	
	if (g_iAdminCurPosTeleport[id] != TMP_CURRENT
	&& g_iAdminCurPosTeleport[id] != TMP_TO_PLAYER
	&& IsSetBit(g_iBitUserAlive, id))
	{
		g_iMenuPlayers[id][iPlayersNum++] = id;
	}
		
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || i == id)
			continue;

		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	menu_list_title("Телепорт", iPos, iPlayersNum, 7);
	menu_item("^n");

	if (!iPlayersNum)
		menul_item_block("\dПодходящих игроков нет^n");
	else
	{
		while (menu_list_count())
			menu_list_item("\y%d. \w%n^n", menu_list_num(),
				g_iMenuPlayers[id][menu_list_countid()]);
	}
	menu_list_empty_items();
	new szPos[][] = { "По прицелу", "Моя позиция", "Сохраненная позиция", "Меня к игроку" };
	menu_list_item("^n\y8. \w%s", szPos[g_iAdminCurPosTeleport[id]]);

	return menu_list_show(id, "Show_AdminTeleportMenu");
}

public Handle_AdminTeleportMenu(id, iKey)
{
	switch (iKey)
	{
		case 7: return Show_PositionTeleportMenu(id);
		case 8: return Show_AdminTeleportMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_AdminTeleportMenu(id, --g_iMenuPosition[id]);
	}

	new iTarget = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];
	if (IsNotSetBit(g_iBitUserAlive, iTarget))
	{
		client_print_color(id, 0, "%s ^1Игрок мертв.", g_szChatPrefix);
		return Show_AdminTeleportMenu(id, g_iMenuPosition[id]);
	}

	if (g_iAdminCurPosTeleport[id] == TMP_AIMING)
	{
		new Float:fOrigin[3];
		get_aiming_position(id, fOrigin);
		admin_teleport_player(id, iTarget, fOrigin);
	}
	else if (g_iAdminCurPosTeleport[id] == TMP_CURRENT)
	{
		new Float:fOrigin[3], Float:fAngle[3];
		get_entvar(id, var_origin, fOrigin);
		get_entvar(id, var_v_angle, fAngle);

		admin_teleport_player(id, iTarget, fOrigin, fAngle);
		unstuck_user(id);
	} 
	else if (g_iAdminCurPosTeleport[id] == TMP_SAVED)
	{
		if (xs_vec_equal(g_fAdminTeleportOrigin[id], Float:{0.0, 0.0, 0.0}))
			client_print_color(id, id, "%s Сперва сохраните позицию!", g_szChatPrefix);
		else
			admin_teleport_player(id, iTarget, g_fAdminTeleportOrigin[id], g_fAdminTeleportAngle[id]);
	}
	else if (g_iAdminCurPosTeleport[id] == TMP_TO_PLAYER)
	{
		if (IsNotSetBit(g_iBitUserAlive, id))
			return Show_AdminTeleportMenu(id, g_iMenuPosition[id]);
			
		new Float:fOrigin[3], Float:fAngle[3];
		get_entvar(iTarget, var_origin, fOrigin);
		get_entvar(iTarget, var_v_angle, fAngle);

		admin_teleport_player(id, id, fOrigin, fAngle);
		unstuck_user(iTarget);
	}
	
	return Show_AdminTeleportMenu(id, g_iMenuPosition[id]);
}

Show_PositionTeleportMenu(id)
{
	menu_title("Выбор позиции Телепорта^n^n");

	menu_item("\y1. \wПо прицелу^n");
	menu_item("\y2. \wМоя позиция^n");
	if (xs_vec_equal(g_fAdminTeleportOrigin[id], Float:{0.0, 0.0, 0.0}))
		menu_item("\y3. \dСохраненная позиция^n");
	else menu_item("\y3. \wСохраненная позиция^n");
	menu_item("\y4. \wСохранить позицию^n");
	menu_item("\y5. \wМеня к игроку^n^n^n");

	menu_item("\y9. \wНазад^n");
	menu_item("\y0. \wВыход^n");

	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<8|1<<9), "Show_PositionTeleportMenu");
}

public Handle_PositionTeleportMenu(id, iKey)
{
	switch (iKey)
	{
		case 0: g_iAdminCurPosTeleport[id] = TMP_AIMING;
		case 1: g_iAdminCurPosTeleport[id] = TMP_CURRENT;
		case 2:
		{
			if (xs_vec_equal(g_fAdminTeleportOrigin[id], Float:{0.0, 0.0, 0.0}))
			{
				client_print_color(id, id, "%s Сперва сохраните позицию!", g_szChatPrefix);
				return Show_PositionTeleportMenu(id);
			}
			else g_iAdminCurPosTeleport[id] = TMP_SAVED;			
		}
		case 3:
		{
			get_entvar(id, var_origin, g_fAdminTeleportOrigin[id]);
			get_entvar(id, var_v_angle, g_fAdminTeleportAngle[id]);

			g_iAdminCurPosTeleport[id] = TMP_SAVED;
			client_print_color(id, print_team_blue,
				"%s ^1Позиция сохранена ^3(%0.f %0.f %0.f)^1.", g_szChatPrefix,
				g_fAdminTeleportOrigin[id][0],
				g_fAdminTeleportOrigin[id][1],
				g_fAdminTeleportOrigin[id][2]);
		}
		case 4: g_iAdminCurPosTeleport[id] = TMP_TO_PLAYER;
	}

	return Show_AdminTeleportMenu(id, g_iMenuPosition[id]);
}