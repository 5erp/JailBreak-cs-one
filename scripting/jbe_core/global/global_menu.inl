Show_GlobalMenu(id)
{
	if (is_not_global_menu(id)) return PLUGIN_HANDLED;
	menu_title("Global Menu^n^n");
	menu_item("\y1. \wВозродить^n");
	menu_item("\y2. \wОружие^n");
	menu_item("\y3. \wНавыки^n");
	menu_item("\y4. \wВыставить^n");
	menu_item("\y5. \wУправление^n");
	menu_item("\y6. \wБлокировка^n");
	menu_item("\y7. \wБлок объектов^n");
	menu_item("^n^n\y0. \wВыход");
	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<9), "Show_GlobalMenu");
}

public Handle_GlobalMenu(id, iKey)
{
	if (is_not_global_menu(id)) return PLUGIN_HANDLED;

	g_iGlobalMenuType[id] = iKey + 1;

	switch (iKey)
	{
		case 0: return Show_GlobalTeamMenu(id);
		case 1: return Cmd_GlobalGunMenu(id);
		case 2: return Show_GlobalSkillMenu(id);
		case 3: return Show_GlobalSetMenu(id);
		case 4: return Show_GlobalControlMenu(id);
		case 5: return Show_GlobalBlockMenu(id);
		case 6: return Show_GlobalEntityBlockMenu(id);
	}
	return PLUGIN_HANDLED;
}

Show_GlobalSetMenu(id)
{
	if (is_not_global_menu(id)) return PLUGIN_HANDLED;

	menu_title("Global Set^n^n");
	menu_item("\y1. \wЖизни^n");
	menu_item("\y2. \wБроня^n");
	menu_item("\y3. \wСкорость^n");
	menu_item("\y4. \wГравитация^n");
	menu_item("\y5. \wОсвещение^n");
	menu_item("\y6. \wИмя режима^n");

	menu_item("^n^n\y9. \wНазад^n");
	menu_item("\y0. \wВыход^n");
	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<8|1<<9), "Show_GlobalSetMenu");
}

public Handle_GlobalSetMenu(id, iKey)
{
	if (is_not_global_menu(id)) return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 4: return Show_SetLightsMenu(id);
		case 5: client_cmd(id, "messagemode ^"global_name^"");
		case 8: return Show_GlobalMenu(id);
		case 9: return PLUGIN_HANDLED;
		default:
		{
			g_iGlobalMenuType[id] = iKey + GLOBAL_HEALTH;
			client_cmd(id, "messagemode ^"global_num^"");
		}
	}
	return PLUGIN_HANDLED;
}

Show_GlobalControlMenu(id)
{
	if (is_not_global_menu(id)) return PLUGIN_HANDLED;

	menu_title("Game Control^n^n");

	menu_item("\y1. \wОбнулить навыки^n");
	menu_item("\y2. \wЭкран Выкл \r+^n");
	menu_item("\y3. \wЭкран Вкл \y-^n");
	menu_item("\y4. \wЗаморозить\r+^n");
	menu_item("\y5. \wРазморозить \y-^n");
	menu_item("\y6. \wЗакопать \r+^n");
	menu_item("\y7. \wОткопать \y-^n");
	menu_item("\y8. \wРадар %s^n", g_bGameRadarStatus ? "\y[Вкл]" : "\r[Выкл]");

	menu_item("^n\y9. \wНазад^n");
	menu_item("\y0. \wВыход^n");
	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Show_GlobalControlMenu");
}

public Handle_GlobalControlMenu(id, iKey)
{
	if (is_not_global_menu(id)) return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 7:
		{
			if (g_bGameRadarStatus)
				jbe_radar_off();
			else
				jbe_radar_on();

			client_print_color(0, id, "%s ^4%n ^1%s радар.", g_szGlobalPrefix, id, g_bGameRadarStatus ? "включил" : "выключил");
		}
		case 8: return Show_GlobalMenu(id);
		case 9: return PLUGIN_HANDLED;
		default:
		{
			g_iGlobalMenuType[id] = iKey + GLOBAL_DEL_SKILLS;
			return Show_GlobalTeamMenu(id);
		}
	}
	return PLUGIN_HANDLED;
}

Show_GlobalTeamMenu(id)
{
	if (is_not_global_menu(id)) return PLUGIN_HANDLED;

	new iKeys = (1<<0|1<<1|1<<2|1<<3|1<<8|1<<9);
	menu_title("Выберите игрока(ов)^n");

	_global_format_menu_type(id);

	menu_item("\y1. \wВыбрать игрока^n");
	menu_item("\y2. \wЗаключенные^n");
	menu_item("\y3. \wОхранники^n");
	menu_item("\y4. \wВсе игроки^n");
	
	if (g_iGlobalMenuType[id] != GLOBAL_RESPAWN)
	{
		menu_item("\y5. \wПо прицелу^n");
		iKeys |= (1<<4);
	}
	else menu_item("\y5. \dПо прицелу^n");
	
	if ((g_iUserTeam[id] == TEAM_PRISONER || g_iUserTeam[id] == TEAM_GUARD)
	&& ((g_iGlobalMenuType[id] != GLOBAL_RESPAWN && IsSetBit(g_iBitUserAlive, id))
	|| (g_iGlobalMenuType[id] == GLOBAL_RESPAWN && IsNotSetBit(g_iBitUserAlive, id))))
	{
		menu_item("\y6. \wВыбрать себя^n");
		iKeys |= (1<<5);
	}
	else menu_item("\y6. \dВыбрать себя^n");

//	menu_item("\y7. \wВыдача: %s^n", g_iGlobalTeamType[id] ? "Игроки" : "Команды");

	if (g_iGlobalMenuType[id] == GLOBAL_SKILL)
	{
		menu_item("^n\y8. \wТип: \y%s^n", g_iGlobalSkillType[id][g_iGlobalItem[id]] ? "Выдать" : "Забрать");
		iKeys |= (1<<7);
	}
	else menu_item("^n");

	menu_item("^n\y9. \wНазад");
	menu_item("^n\y0. \wВыход");

	return menu_show(id, iKeys, "Show_GlobalTeamMenu");
}

public Handle_GlobalTeamMenu(id, iKey) 
{
	if (is_not_global_menu(id)) return PLUGIN_HANDLED;

	new iTarget;
	switch (iKey) 
	{
		case 0: return Cmd_GlobalUserMenu(id);
		case 1: iTarget = GT_PRISONER;
		case 2: iTarget = GT_GUARD;
		case 3: iTarget = GT_ALL;
		case 4: 
		{
			new iBody;
			get_user_aiming(id, iTarget, iBody);

			if (!jbe_is_user_valid(iTarget) || IsNotSetBit(g_iBitUserAlive, iTarget))
			{
				client_print_color(id, print_team_blue,
					"%s ^1Наведите ^4прицел ^1на игрока и ^3повторите попытку^1.", g_szGlobalPrefix);
				return Show_GlobalTeamMenu(id);
			}
		}
		case 5: 
		{
			if ((g_iGlobalMenuType[id] != GLOBAL_RESPAWN && IsSetBit(g_iBitUserAlive, id))
			|| (g_iGlobalMenuType[id] == GLOBAL_RESPAWN && IsNotSetBit(g_iBitUserAlive, id)))
				iTarget = id;
			else
				return Show_GlobalTeamMenu(id);
		}
		case 7:
		{
			g_iGlobalSkillType[id][g_iGlobalItem[id]] = !g_iGlobalSkillType[id][g_iGlobalItem[id]];
			
			return Show_GlobalTeamMenu(id);
		}
		case 8: 
		{
			switch (g_iGlobalMenuType[id]) 
			{
				case GLOBAL_RESPAWN:						return Show_GlobalMenu(id);
				case GLOBAL_GIVE_GUN:						return Show_GlobalGunMenu(id, g_iMenuPosition[id]);
				case GLOBAL_SKILL:							return Show_GlobalSkillMenu(id, g_iMenuPosition[id]);
				case GLOBAL_HEALTH..GLOBAL_GRAVITY:			return Show_GlobalSetMenu(id);
				case GLOBAL_DEL_SKILLS..GLOBAL_UNBURIED:	return Show_GlobalControlMenu(id);
			}
		}
		case 9: return PLUGIN_HANDLED;
	}

	_global_menu_select(id, iTarget);

	if (0 < iTarget <= MaxClients)
		return Show_GlobalTeamMenu(id);

	return PLUGIN_HANDLED;
}

Cmd_GlobalUserMenu(id) return Show_GlobalUserMenu(id, g_iMenuPosition[id] = 0);
Show_GlobalUserMenu(id, iPos)
{
	if (iPos < 0 || is_not_global_menu(id))
		return PLUGIN_HANDLED;

	new iPlayersNum;

	for (new i = 1; i <= MaxClients; i++)
	{
		if (g_iGlobalMenuType[id] != GLOBAL_RESPAWN && IsNotSetBit(g_iBitUserAlive, i)
		|| (g_iGlobalMenuType[id] == GLOBAL_RESPAWN && IsSetBit(g_iBitUserAlive, i)
		|| g_iUserTeam[i] == TEAM_UNASSIGNED || g_iUserTeam[i] == TEAM_SPECTATOR))
			continue;

		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!menu_list_title("Выберите игрока", iPos, iPlayersNum))
		return client_print_color(id, 0, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);

	_global_format_menu_type(id);

	new iTarget;
	while (menu_list_count()) {
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		menu_list_item("\y%d. \w%n^n", menu_list_num(), iTarget);
	}

	return menu_list_show(id, "Show_GlobalUserMenu");
}

public Handle_GlobalUserMenu(id, iKey)
{
	if (is_not_global_menu(id))
		return PLUGIN_HANDLED;
		
	switch (iKey)
	{
		case 8: return Show_GlobalUserMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_GlobalUserMenu(id, --g_iMenuPosition[id]);
		default:
		{
			new iTarget = g_iMenuPlayers[id][menu_list_itemid(id, iKey)];

			_global_menu_select(id, iTarget);

			return Show_GlobalUserMenu(id, g_iMenuPosition[id]);
		}
	}
	return PLUGIN_HANDLED;
}

Show_GlobalEntityBlockMenu(id, iPos = 0)
{
	if (iPos < 0 || is_not_global_menu(id)) return PLUGIN_HANDLED;
	
	menu_list_title("Entity Block", iPos, sizeof szHamHookEntityBlock);
	menu_item("^n");

	while (menu_list_count())
	{
		menu_list_item("\y%d. \w%s%s^n", menu_list_num(), szHamHookEntityBlock[menu_list_countid()],
			IsSetBit(g_iBitGlobalEntBlock, menu_list_countid()) ? " \r[Block]" : "");
	}

	return menu_list_show(id, "Show_GlobalEntityBlockMenu");
}

public Handle_GlobalEntityBlockMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_GlobalEntityBlockMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_GlobalEntityBlockMenu(id, --g_iMenuPosition[id]);
	}

	_global_entity_block(id, menu_list_itemid(id, iKey));

	return Show_GlobalEntityBlockMenu(id, g_iMenuPosition[id]);
}

Cmd_GlobalGunMenu(id) return Show_GlobalGunMenu(id, g_iMenuPosition[id] = 0);
Show_GlobalGunMenu(id, iPos)
{
	if (iPos < 0 || is_not_global_menu(id)) return PLUGIN_HANDLED;

	menu_list_title("Выберите оружие", iPos, sizeof(g_iszGlobalWeapons));
	menu_item("^n");

	while (menu_list_count())
	{
		menu_list_item("\y%d. \w%s^n", menu_list_num(), global_weapon_name(menu_list_countid()));
	}

	return menu_list_show(id, "Show_GlobalGunMenu");
}

public Handle_GlobalGunMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_GlobalGunMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_GlobalGunMenu(id, --g_iMenuPosition[id]);
	}

	g_iGlobalItem[id] = menu_list_itemid(id, iKey);

	new iWpnID = global_weapon_id(g_iGlobalItem[id]);

	if (0 < iWpnID && iWpnID != _:WEAPON_SHIELDGUN && iWpnID != _:WEAPON_KNIFE)
		return Show_GlobalAmmoMenu(id);
	
	g_iMenuTarget[id] = 0;

	return Show_GlobalTeamMenu(id);
}

Show_GlobalAmmoMenu(id)
{
	new iKeys = (1<<0|1<<1|1<<2|1<<3|1<<4|1<<8|1<<9);
	new IsGrenade = global_weapon_is_grenade(g_iGlobalItem[id]);

	menu_title("Выберите колличество %s^n^n", IsGrenade ? "гранат" : "пуль");

	menu_item("\y1. \wВвести значение^n");
	menu_item("\y2. \wОдна %s^n", IsGrenade ? "штука" : "пуля");
	menu_item("\y3. \w120 %s^n", IsGrenade ? "штук" : "пуль");
	menu_item("\y4. \w250 %s^n", IsGrenade ? "штук" : "пуль");
	menu_item("\y5. \w1000 %s^n", IsGrenade ? "штук" : "пуль");
	if (!IsGrenade)
	{
		menu_item("\y6. \wОдна обойма^n");
		menu_item("\y7. \wВыдать пустое^n^n");
		iKeys |= (1<<5|1<<6);
	}
	else menu_item("^n^n");

	menu_item("^n\y9. \wНазад^n");
	menu_item("\y0. \wВыход");

	return menu_show(id, iKeys, "Show_GlobalAmmoMenu");
}

public Handle_Global_AmmoMenu(id, iKey)
{
	switch (iKey)
	{
		case 0: return client_cmd(id, "messagemode ^"global_num^"");
		case 1: g_iMenuTarget[id] = 1;		// Одна пуля
		case 2: g_iMenuTarget[id] = 120;	// 120 пуль
		case 3: g_iMenuTarget[id] = 250;	// 250 пуль
		case 4: g_iMenuTarget[id] = 1000;	// 1000 пуль
		case 5: g_iMenuTarget[id] = -1;		// Одна обойма
		case 6: g_iMenuTarget[id] = 0;		// Выдать пустое
		case 8: return Show_GlobalGunMenu(id, g_iMenuPosition[id]);
		case 9: return PLUGIN_HANDLED;
	}
	return Show_GlobalTeamMenu(id);
}

Show_GlobalSkillMenu(id, iPos = 0)
{
	if (iPos < 0 || is_not_global_menu(id))
		return PLUGIN_HANDLED;

	static aMenu[][] = {
		// page 1
		"Скорость",
		"Гравитацию", 
		"250HP + 250AP",
		"Бессмертие", 
		"Ноуклип", 
		"Двойной прыжок", 
		"Автораспрыг", 
		"Высокий прыжок", 
		// page 2
		"Уменьшеный урон", 
		"Двойной урон",
		"Семиклип",
		"Невидимость", 
		"Бесшумные шаги",
		"Длиный прыжок", 
		"Призрак",
		"Защита от падений",
		// page 3
		"Вид от 3-го лица"
	};

	menu_list_title("Выберите навык", iPos, sizeof(aMenu));
	menu_item("^n");

	while (menu_list_count())
		menu_list_item("\y%d. \w%s^n", menu_list_num(), aMenu[menu_list_countid()]);

	return menu_list_show(id, "Show_GlobalSkillMenu");
}

public Handle_GlobalSkillMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_GlobalSkillMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_GlobalSkillMenu(id, --g_iMenuPosition[id]);
	}

	g_iGlobalItem[id] = menu_list_itemid(id, iKey);

	return Show_GlobalTeamMenu(id);
}

Show_GlobalBlockMenu(id)
{
	if (is_not_global_menu(id)) return PLUGIN_HANDLED;
	
	menu_title("Global Block^n^n");
	
	menu_item("\y1. \wМагазин \r[\y%s\r]^n", g_iGlobalBlock[GB_SHOP] ? "OFF" : "ON");
	menu_item("\y2. \wРанг бонус \r[\y%s\r]^n", g_iGlobalBlock[GB_RANK_BONUS] ? "OFF" : "ON");
	menu_item("\y3. \wПокупка жизни \r[\y%s\r]^n", g_iGlobalBlock[GB_BUY_RESPAWN] ? "OFF" : "ON");
	menu_item("\y4. \wВход за КТ \r[\y%s\r]^n", g_iGlobalBlock[GB_GUARD_TEAM] ? "OFF" : "ON");
	menu_item("\y5. \wVIP меню \r[\y%s\r]^n", g_iGlobalBlock[GB_VIP_MENU] ? "OFF" : "ON");
	menu_item("\y6. \wHook \r[\y%s\r]^n", g_iGlobalBlock[GB_HOOK] ? "OFF" : "ON");
	menu_item("\y7. \wЧат мертвых \r[\y%s\r]^n", g_iGlobalBlock[GB_CHAT_DEAD] ? "OFF" : "ON");
	menu_item("\y8. \wЧат общий \r[\y%s\r]^n", g_iGlobalBlock[GB_CHAT_ALL] ? "OFF" : "ON"); 
	menu_item("\y9. \wПриват сообщения \r[\y%s\r]^n", g_iGlobalBlock[GB_PM] ? "OFF" : "ON");
	
	menu_item("^n\y0. \wВыход");
	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Show_GlobalBlockMenu");
}

public Handle_GlobalBlockMenu(id, iKey)
{
	if (is_not_global_menu(id) || iKey == 9)
		return PLUGIN_HANDLED;
	
	jbe_informer_offset_down(id);

	if (!global_set_block(id, iKey))
		return PLUGIN_HANDLED;

	new szInfo[44];

	switch (iKey)
	{
		case 0: szInfo = "магазин";
		case 1: szInfo = "ранг бонус";
		case 2: szInfo = "зеленые билеты";
		case 3: szInfo = "вход за охрану";
		case 4: szInfo = "vip меню";
		case 5: szInfo = "hook";
		case 6: szInfo = "чат ^1мертвым игрокам";
		case 7: szInfo = "чат ^1всем игрокам";
		case 8: szInfo = "приватные сообщения";
	}

	SetBit(g_iBitUseGlobalBlock, id);

	client_print_color(0, g_iGlobalBlock[iKey] ? print_team_red : print_team_blue,
		"%s ^4%n ^3%s ^1%s^1.", g_szGlobalPrefix, id,
		g_iGlobalBlock[iKey] ? "заблокировал" : "разблокировал", szInfo);

	return Show_GlobalBlockMenu(id);
}

Show_SetLightsMenu(id, iPos = 0)
{
	if (iPos < 0) return PLUGIN_HANDLED;

	menu_list_title("Задать освещение", iPos, sizeof(g_szLightsMenu));
	menu_item("^n");

	while (menu_list_count())
		menu_list_item("\y%d. \w%s^n", menu_list_num(), g_szLightsMenu[ menu_list_countid() ][1]);
	
	return menu_list_show(id, "Show_SetLightsMenu");
}

public Handle_SetLightsMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_SetLightsMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_SetLightsMenu(id, --g_iMenuPosition[id]);
	}

	new iLight = menu_list_itemid(id, iKey);

	if (iLight > 0)
		g_bGlobalLight = true;
	else
		g_bGlobalLight = false;

	set_lights(g_szLightsMenu[iLight][0]);

	client_print_color(0, 0, "%s ^4%n ^1Выставил освещение ^4%s^1.", 
		g_szGlobalPrefix, id, g_szLightsMenu[iLight][1]);

	return Show_SetLightsMenu(id, g_iMenuPosition[id]);
}