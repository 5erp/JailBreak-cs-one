/*===== -> Меню -> =====*///{

menu_init()
{
	register_menucmd(register_menuid("Show_ChooseTeamMenu"),
		(1<<0|1<<1|1<<5|1<<8|1<<9), "Handle_ChooseTeamMenu");
			
	register_menucmd(register_menuid("Show_WeaponsGuardMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_WeaponsGuardMenu");
	
	register_menucmd(register_menuid("Show_MainMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_MainMenu");

	register_menucmd(register_menuid("Show_MoneyTransferMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_MoneyTransferMenu");
	
	register_menucmd(register_menuid("Show_MoneyAmountMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<7|1<<8|1<<9), "Handle_MoneyAmountMenu");
	
	register_menucmd(register_menuid("Show_CostumesMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_CostumesMenu");

	register_menucmd(register_menuid("Show_KillReasonsMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_KillReasonsMenu");
	
	register_menucmd(register_menuid("Show_KilledUsersMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_KilledUsersMenu");
	
	register_menucmd(register_menuid("Show_LastPrisonerMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<8|1<<9), "Handle_LastPrisonerMenu");
	
	register_menucmd(register_menuid("Show_FriendFreeDay"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_FriendFreeDay");
	
	register_menucmd(register_menuid("Show_UserCabinetMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_UserCabinetMenu");
	
	register_menucmd(register_menuid("Show_PrivilegiesMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_PrivilegiesMenu");
	
	register_menucmd(register_menuid("Show_SoundMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_SoundMenu");
	
	register_menucmd(register_menuid("Show_InfoMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_InfoMenu");
	
	register_menucmd(register_menuid("Show_DeathUserMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<8|1<<9), "Handle_DeathUserMenu");
	
	register_menucmd(register_menuid("Show_SetSkinMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_SetSkinMenu");
	
	register_menucmd(register_menuid("Show_SetSkinUsersMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_SetSkinUsersMenu");

	register_menucmd(register_menuid("Show_AnimMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<9), "Handle_AnimMenu");

	register_menucmd(register_menuid("Show_MusicMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_MusicMenu");

	chief_menu_init();
	soccer_menu_init();
	box_menu_init();
	global_menus_init();
	vip_menu_init();
	rank_menus_init();
}

Show_ChooseTeamMenu(id, iType)
{
	if (jbe_menu_blocked(id))
		return PLUGIN_HANDLED;
		
	new iKeys;
	menu_title("Выбор команды\R\wОнлайн: %d|%d\
		^n\dБаланс: %d заключенных, 1 охранник^n^n",
		(g_iPlayersNum[0] + g_iPlayersNum[1] + g_iPlayersNum[2] + g_iPlayersNum[3]),
		MaxClients, g_eAllCvars[TEAM_BALANCE]);

	if (g_iUserTeam[id] != TEAM_PRISONER)
	{
		menu_item("\y1. \wЗаключённые \r[%d]^n", g_iPlayersNum[TEAM_PRISONER]);
		iKeys |= (1<<0);
	}
	else menu_item("\y1. \dЗаключённые \r[%d]^n", g_iPlayersNum[TEAM_PRISONER]);

	if (jbe_is_team_balance())
	{
		menu_item("\y2. \wОхранники \r[%d] \y16+^n^n^n", g_iPlayersNum[TEAM_GUARD]);
		iKeys |= (1<<1);
	}
	else
	{
		menu_item("\y2. \dОхранники \r[%d] \y16+^n^n^n", g_iPlayersNum[TEAM_GUARD]);
	}

	if (g_iUserTeam[id] != TEAM_SPECTATOR)
	{
		menu_item("\y6. \wЗритель^n^n");
		iKeys |= (1<<5);
	}
	else menu_item("\y6. \dЗритель^n^n");
	
	menu_item("\dНаш сайт: \rCS-One.Ru^n\dГруппа: \rvk.com/CsOneRu");
	
	if (iType)
	{
		menu_item("^n^n\y0. \w%L", id, "JBE_MENU_EXIT");
		iKeys |= (1<<9);
	}

	return menu_show(id, iKeys, "Show_ChooseTeamMenu"); 
}

public Handle_ChooseTeamMenu(id, iKey)
{
	new iSysTime = get_systime();
	afk_user_clear_warns(id);

	if (iKey != 9 && g_eAllCvars[CHANGETEAM_TIME] && g_iUserChangeTeamTime[id] + g_eAllCvars[CHANGETEAM_TIME] > iSysTime)
	{
		client_print_color(id, id, "%s Не так часто, жди %d сек.", g_szChatPrefix, 
			g_iUserChangeTeamTime[id] + g_eAllCvars[CHANGETEAM_TIME] - iSysTime);
	
		return Show_ChooseTeamMenu(id, .iType = 1);
	}

	switch (iKey)
	{
		case 0:
		{
			if (g_iUserTeam[id] == TEAM_PRISONER)
				return Show_ChooseTeamMenu(id, 1);
				
			if (!jbe_set_user_team(id, TEAM_PRISONER))
				return PLUGIN_HANDLED;
		}
		case 1:
		{
			if (jbe_is_user_guard_block(id))
			{
				client_print_color(id, id, "%s Вам заблокирован вход за охрану! Осталось: %s",
					g_szGrBlockPrefix, seconds_to_str(g_iTimeBlockGrEnd[id] - get_systime()));
				if (g_iUserTeam[id] != TEAM_PRISONER)
				{
					jbe_set_user_team(id, TEAM_PRISONER);
				}
				return PLUGIN_HANDLED;
			}

			if (g_iGlobalBlock[GB_GUARD_TEAM])
			{
				client_print_color(id, id, "%s Вход за охрану ^4заблокирован %s^1.",
					g_szGlobalPrefix, global_get_name_block(GB_GUARD_TEAM));
				if (g_iUserTeam[id] != TEAM_PRISONER)
				{
					jbe_set_user_team(id, TEAM_PRISONER);
				}
				return PLUGIN_HANDLED;
			}

			if (g_iUserTeam[id] == TEAM_GUARD)
				return Show_ChooseTeamMenu(id, 1);

			if (jbe_is_team_balance())
			{
				if (!jbe_set_user_team(id, TEAM_GUARD))
					return PLUGIN_HANDLED;
				jbe_informer_offset_down(id);
			}
			else
			{
				if (g_iUserTeam[id] == TEAM_PRISONER)
					return Show_ChooseTeamMenu(id, 1);
				else
					return Show_ChooseTeamMenu(id, 0);
			}
		}
		case 5:
		{
			if (g_iUserTeam[id] == TEAM_SPECTATOR)
				return Show_ChooseTeamMenu(id, 0);

			if (!jbe_set_user_team(id, TEAM_SPECTATOR))
				return PLUGIN_HANDLED;

			return Show_ChooseTeamMenu(id, .iType = 0);
		}
	}

	if (g_iUserTeam[id] == TEAM_PRISONER || g_iUserTeam[id] == TEAM_GUARD)
		g_iUserChangeTeamTime[id] = iSysTime;

	return PLUGIN_HANDLED;
}

Show_MainMenu(id)
{
	new iUserAlive = IsSetBit(g_iBitUserAlive, id);
	new iKeys = (1<<1|1<<3|1<<4|1<<5|1<<7|1<<8|1<<9);
	
	menu_title("Меню сервера^n");

	if (g_iUserTeam[id] == TEAM_PRISONER && is_user_active_bracelets(id))
		menu_item("\dНаручники: %s^n", seconds_to_str(g_iUserBraceletsTime[id]));
	else
		menu_item("^n");

	if (iUserAlive
	&& (1<<g_iDayMode) & (1<<DAY_NORMAL|1<<DAY_FREEDAY|1<<DAY_DEATHMATCH)
	&& IsNotSetBit(g_iBitUserDuel, id) && !g_iGlobalBlock[GB_SHOP])
	{
		menu_item("\y1. \wМагазин^n");
		iKeys |= (1<<0);
	}
	else menu_item("\y1. \dМагазин^n");

	menu_item("\y2. \wДенежный перевод^n");

	if ((1<<g_iDayMode) & (1<<DAY_NORMAL|1<<DAY_FREEDAY|1<<DAY_DEATHMATCH))
	{
		menu_item("\y3. \wКостюмы^n");
		iKeys |= (1<<2);
	}
	else menu_item("\y3. \dКостюмы^n");

	switch (g_iUserTeam[id])
	{
		case TEAM_PRISONER:
		{
			if (g_iLastPnId && iUserAlive && id == g_iLastPnId)
			{
				menu_item("\y4. \wЯ последний^n");
				g_iMenuType[id] = 0;
			}
			else
			{
				menu_item("\y4. \wЗадания^n");
				g_iMenuType[id] = 1;
			}
		}
		case TEAM_GUARD:
		{
			if (iUserAlive && ((1<<g_iDayMode) & (1<<DAY_NORMAL|1<<DAY_FREEDAY)))
			{
				if (id == g_iChiefId)
				{
					menu_item("\y4. \wМеню начальника^n");
					g_iMenuType[id] = 0;
				}
				else if (g_iAlivePlayersNum[TEAM_GUARD] == 1 || g_iChiefStatus != CHIEF_ALIVE)
				{
					menu_item("\y4. \wВзять начальника^n");
					g_iMenuType[id] = 0;
				}
				else
				{
					menu_item("\y4. \wЗадания^n");
					g_iMenuType[id] = 1;
				}
				
			}
			else
			{
				menu_item("\y4. \wЗадания^n");
				g_iMenuType[id] = 1;
			}
		}
	}

	menu_item("\y5. \wВыбор команды^n");

	menu_item("\y6. \wЛичные сообщения^n");

	if (iUserAlive
	&& (1<<g_iDayMode) & (1<<DAY_NORMAL|1<<DAY_FREEDAY|1<<DAY_DEATHMATCH)
	&& IsNotSetBit(g_iBitUserDuel, id) && !g_iGlobalBlock[GB_RANK_BONUS])
	{
		menu_item("\y7. \wРанг бонус^n");
		iKeys |= (1<<6);
	}
	else
		menu_item("\y7. \dРанг бонус^n");

	menu_item("\y8. \wПривилегии^n");

	menu_item("\y9. \wМой кабинет^n");
	menu_item("^n\y0. \w%l", "JBE_MENU_EXIT");

	SetBit(g_iBitOpenMainMenu, id);
	
	return menu_show(id, iKeys, "Show_MainMenu");
} 

public Handle_MainMenu(id, iKey)
{
	switch(iKey)
	{
		case 0: return jbe_shop_open_menu(id);
		case 1: return Show_MoneyTransferMenu(id);
		case 2: return Show_CostumesMenu(id);
		case 3:
		{
			if (g_iMenuType[id]) // == 1
				return Show_QuestsUserMenu(id);

			switch (g_iUserTeam[id])
			{
				case TEAM_PRISONER: return Show_LastPrisonerMenu(id);
				case TEAM_GUARD: return ClCmd_Simon(id);
			}
		}
		case 4: return Show_ChooseTeamMenu(id, 1);
		case 5: return Show_PrivateMessageMenu(id);
		case 6: return Show_RankBonusMenu(id);
		case 7: return Show_PrivilegiesMenu(id);
		case 8: return Show_UserCabinetMenu(id);
	}
	return PLUGIN_HANDLED;
}

Show_MoneyTransferMenu(id, iPos = 0)
{
	if(iPos < 0) return PLUGIN_HANDLED;
	
	new iPlayersNum;
	for(new i = 1; i <= MaxClients; i++)
	{
		if(IsNotSetBit(g_iBitUserConnected, i) || i == id) continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!menu_list_title("\wВыберите кому переводить", iPos, iPlayersNum))
		return client_print_color(id, 0, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);

	menu_item("\dУ Вас денег: %d$^n", g_iUserMoney[id]);

	new iTarget;
	while (menu_list_count()) {
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		menu_list_item("\y%d. \w%n \r[%d$]^n",
			menu_list_num(), iTarget, g_iUserMoney[iTarget]);
	}

	return menu_list_show(id, "Show_MoneyTransferMenu");
}

public Handle_MoneyTransferMenu(id, iKey)
{
	switch(iKey)
	{
		case 8: return Show_MoneyTransferMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_MoneyTransferMenu(id, --g_iMenuPosition[id]);
		default:
		{
			g_iMenuTarget[id] = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];
			return Show_MoneyAmountMenu(id);
		}
	}
	return PLUGIN_HANDLED;
}

Show_MoneyAmountMenu(id)
{
	new iKeys = (1<<8|1<<9);
	menu_title("\yВыберете сумму перевода^n\dУ Вас денег: %d$^n", g_iUserMoney[id]);
	if (g_iUserMoney[id])
	{
		menu_item("\y1. \w%d$^n", floatround(g_iUserMoney[id] * 0.10, floatround_ceil));
		menu_item("\y2. \w%d$^n", floatround(g_iUserMoney[id] * 0.25, floatround_ceil));
		menu_item("\y3. \w%d$^n", floatround(g_iUserMoney[id] * 0.50, floatround_ceil));
		menu_item("\y4. \w%d$^n", floatround(g_iUserMoney[id] * 0.75, floatround_ceil));
		menu_item("\y5. \w%d$^n^n^n", g_iUserMoney[id]);
		menu_item("\y8. \wУказать сумму^n");
		iKeys |= (1<<0|1<<1|1<<2|1<<3|1<<4|1<<7);
	}
	else
	{
		menu_item("\y1. \d0$^n\y2. \d0$^n\y3. \d0$^n\y4. \d0$^n\y5. \d0$^n^n^n");
		menu_item("\y8. \dУказать сумму^n");
	}
	menu_item("^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	return menu_show(id, iKeys, "Show_MoneyAmountMenu");
}

public Handle_MoneyAmountMenu(id, iKey)
{
	switch (iKey)
	{
		case 0: ClCmd_MoneyTransfer(id, g_iMenuTarget[id], floatround(g_iUserMoney[id] * 0.10, floatround_ceil));
		case 1: ClCmd_MoneyTransfer(id, g_iMenuTarget[id], floatround(g_iUserMoney[id] * 0.25, floatround_ceil));
		case 2: ClCmd_MoneyTransfer(id, g_iMenuTarget[id], floatround(g_iUserMoney[id] * 0.50, floatround_ceil));
		case 3: ClCmd_MoneyTransfer(id, g_iMenuTarget[id], floatround(g_iUserMoney[id] * 0.75, floatround_ceil));
		case 4: ClCmd_MoneyTransfer(id, g_iMenuTarget[id], g_iUserMoney[id]);
		case 7: client_cmd(id, "messagemode ^"money_transfer %d^"", g_iMenuTarget[id]);
		case 8: return Show_MoneyTransferMenu(id, g_iMenuPosition[id]);
	}
	return PLUGIN_HANDLED;
}

Show_CostumesMenu(id, iPos = 0)
{
	if (iPos < 0) return PLUGIN_HANDLED;

	menu_list_title("Костюмы", iPos, g_iCostumesListSize);
	menu_item("^n");

	new aDataCostumes[DATA_COSTUMES_FILE];
	new a;
	
	while (menu_list_count())
	{
		a = menu_list_countid();
		ArrayGetArray(g_aDataCostumes, a, aDataCostumes);

		if (g_eUserCostumes[id][COSTUMES] == a)
			menul_item_block("\y%d. \d%s^n", menu_list_num(), aDataCostumes[COSTUME_NAME]);
		
		else if (aDataCostumes[COSTUME_ACCESS][0] == '1')
		{
			if (IsSetBit(g_iBitUserVip, id))
				menu_list_item("\y%d. \w%s \r[VIP]^n", menu_list_num(), aDataCostumes[COSTUME_NAME]);
			else
				menu_list_item("\y%d. \d%s \r[VIP]^n", menu_list_num(), aDataCostumes[COSTUME_NAME]);
		}

		else if (aDataCostumes[COSTUME_ACCESS][0] == '2')
		{
			if (IsSetBit(g_iBitUserAdmin, id))
				menu_list_item("\y%d. \w%s \r[Admin]^n", menu_list_num(), aDataCostumes[COSTUME_NAME]);
			else
				menu_list_item("\y%d. \d%s \r[Admin]^n", menu_list_num(), aDataCostumes[COSTUME_NAME]);
		}

		else
			menu_list_item("\y%d. \w%s^n", menu_list_num(), aDataCostumes[COSTUME_NAME]);
	}

	return menu_list_show(id, "Show_CostumesMenu");
}

public Handle_CostumesMenu(id, iKey)
{
	if (g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY && g_iDayMode != DAY_DEATHMATCH)
		return PLUGIN_HANDLED;
	
	switch (iKey)
	{
		case 8: return Show_CostumesMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_CostumesMenu(id, --g_iMenuPosition[id]);
	}

	new aDataCostumes[DATA_COSTUMES_FILE];
	new iCostumes = menu_list_itemid(id, iKey);
	ArrayGetArray(g_aDataCostumes, iCostumes, aDataCostumes);
	
	if (aDataCostumes[COSTUME_ACCESS][0])
	{
		if (aDataCostumes[COSTUME_ACCESS][0] == '1' && IsNotSetBit(g_iBitUserVip, id))
		{
			client_print_color(id, print_team_blue, "%s ^1Костюм ^3%s ^1доступен только ^4VIP ^1игрокам.", g_szChatPrefix, aDataCostumes[COSTUME_NAME]);
			return Show_CostumesMenu(id, g_iMenuPosition[id]);
		}
		else if (aDataCostumes[COSTUME_ACCESS][0] == '2' && IsNotSetBit(g_iBitUserAdmin, id))
		{
			client_print_color(id, print_team_blue, "%s ^1Костюм ^3%s ^1доступен только ^4Admin ^1игрокам.", g_szChatPrefix, aDataCostumes[COSTUME_NAME]);
			return Show_CostumesMenu(id, g_iMenuPosition[id]);
		}
	}

	jbe_set_user_costumes(id, iCostumes);
		
	if (iCostumes)
		client_print_color(id, print_team_blue, "%s ^1Вы выбрали костюм^3: ^4%s^1.", g_szChatPrefix, aDataCostumes[COSTUME_NAME]);
	else
		client_print_color(id, print_team_blue, "%s ^1Вы ^3сняли ^1костюм.", g_szChatPrefix);

	return PLUGIN_HANDLED;
}

Show_KillReasonsMenu(id, iTarget)
{
	menu_title("Причина убийства: \r%n^n^n", iTarget);
	for (new i; i <= charsmax(g_szKillReason); i++)
		menu_item("\y%d. \w%s^n", i+1, g_szKillReason[i]);
	menu_item("^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \d%L", id, "JBE_MENU_EXIT");
	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Show_KillReasonsMenu");
}

public Handle_KillReasonsMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_KilledUsersMenu(id);
		case 9: g_iBitKilledUsers[id] = 0;
		default:
		{
			if (IsSetBit(g_iBitKilledUsers[id], g_iMenuTarget[id]))
			{
				if (IsNotSetBit(g_iBitUserConnected, g_iMenuTarget[id]))
				{
					client_print_color(id, id, "%s Игрок вышел..", g_szChatPrefix);
					if (g_iBitKilledUsers[id])
						return Show_KilledUsersMenu(id);
					else
						return PLUGIN_HANDLED;
				}

				client_print_color(0, id, "%s ^3%n ^1убил ^4%n^1. Причина: ^4%s^1.", g_szChatPrefix, id, g_iMenuTarget[id], g_szKillReason[iKey]);
				if (iKey == 7)
				{
					client_print_color(0, 0, "%s ^4%n ^1Получит выходной в следующем раунде.", g_szChatPrefix, g_iMenuTarget[id]);
					jbe_add_user_free_next_round(g_iMenuTarget[id]);
				}

				ClearBit(g_iBitKilledUsers[id], g_iMenuTarget[id]);
			}
			if (g_iBitKilledUsers[id])
				return Show_KilledUsersMenu(id);
		}
	}
	return PLUGIN_HANDLED;
}

Show_KilledUsersMenu(id, iPos = 0)
{
	if (iPos < 0) return PLUGIN_HANDLED;
	
	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitKilledUsers[id], i) || IsNotSetBit(g_iBitUserConnected, i))
			continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!menu_list_title("Список убитых игроков", iPos, iPlayersNum))
	{
		jbe_menu_unblock(id);
		return client_print_color(id, id, "%s ^1Убитый игрок ^3вышел ^1с сервера.", g_szChatPrefix);		
	}
	menu_item("^n");

	new iTarget;
	while (menu_list_count())
	{
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		menu_list_item("\y%d. \w%n^n", menu_list_num(), iTarget);
	}

	return menu_list_show(id, "Show_KilledUsersMenu");
}

public Handle_KilledUsersMenu(id, iKey)
{
	switch(iKey)
	{
		case 8: return Show_KilledUsersMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_KilledUsersMenu(id, --g_iMenuPosition[id]);
		default:
		{
			g_iMenuTarget[id] = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];
			
			if (IsSetBit(g_iBitKilledUsers[id], g_iMenuTarget[id]) && IsSetBit(g_iBitUserConnected, g_iMenuTarget[id]))
				return Show_KillReasonsMenu(id, g_iMenuTarget[id]);
			else if (g_iBitKilledUsers[id]) 
				return Show_KilledUsersMenu(id);
			
			client_print_color(id, print_team_red, "%s ^1Убитый игрок ^3вышел ^1с сервера.", g_szChatPrefix);
			jbe_menu_unblock(id);
		}
	}
	return PLUGIN_HANDLED;
}

is_not_lastprisoner_menu(id)
{
	if (g_iDuelStatus)
		client_print_color(id, id, "%s Меню не доступно в дуэли.", g_szChatPrefix);

	else if (IsNotSetBit(g_iBitUserAlive, id))
		client_print_color(id, id, "%s Меню не доступно мертвым.", g_szChatPrefix);

	else if (id != g_iLastPnId)
		client_print_color(id, id, "%s Меню доступно только последнему заключенному.", g_szChatPrefix);

	else if (g_iAlivePlayersNum[TEAM_PRISONER] > 1)
	{
		g_iLastPnId = 0;
		jbe_set_name_mode("");
		client_print_color(id, id, "%s Ошибка, вы не последний заключенный.", g_szChatPrefix);
		if (g_iChiefId) jbe_set_name_chief(g_iChiefId);
	}

	else return false;

	return true;
}

Show_LastPrisonerMenu(id)
{
	if (is_not_lastprisoner_menu(id))
		return PLUGIN_HANDLED;
	
	new iKeys = (1<<0|1<<8|1<<9);
	menu_title("Что Вы хотите...^n^n");
	menu_item("\y1. \wОткрыть клетки^n");
	
	if (g_iLastPnType == LAST_PN_NONE) {
		menu_item("\y2. \wВыходной^n");
		menu_item("\y3. \wНаличные %d$^n", g_eAllCvars[LAST_PRISONER_MODEY]);
		menu_item("\y4. \wГолос на раунд^n");
		iKeys |= (1<<1|1<<2|1<<3);
	} else {
		menu_item("\y2. \dВыходной^n");
		menu_item("\y3. \dНаличные %d$^n", g_eAllCvars[LAST_PRISONER_MODEY]);
		menu_item("\y4. \dГолос на раунд^n");
	}
	
	if (g_iLastPnType == LAST_PN_NONE || g_iLastPnType == LAST_PN_GUN) {
		menu_item("\y5. \wЗабрать оружие у охраны^n");
		iKeys |= (1<<4);
	} else menu_item("\y5. \dЗабрать оружие у охраны^n");
	
	if (g_iLastPnType == LAST_PN_NONE || g_iLastPnType == LAST_PN_DUEL) {
		menu_item("\y6. \wВыбор дуэли^n");
		iKeys |= (1<<5);
	} else menu_item("\y6. \dВыбор дуэли^n");
	
	if (g_iLastPnType == LAST_PN_NONE) {
		menu_item("\y7. \wВыходной другу^n^n");
		iKeys |= (1<<6);
	} else menu_item("\y7. \dВыходной другу^n^n");
	
	menu_item("^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	return menu_show(id, iKeys, "Show_LastPrisonerMenu");
}

public Handle_LastPrisonerMenu(id, iKey)
{
	if (is_not_lastprisoner_menu(id)) return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 0:
		{
			if (is_user_cmd_flood(id, FLOOD_DOORS, 2.0, 5.0))
				return PLUGIN_HANDLED;

			jbe_open_doors();
			return Show_LastPrisonerMenu(id);
		}
		case 1: {
			if (g_iPlayersNum[TEAM_PRISONER] < MIN_ALL_PL_FD)
			{
				client_print_color(id, print_team_default, "%s Выходной можно взять если на сервере от %d зеков!", g_szChatPrefix, MIN_ALL_PL_FD);
				return Show_LastPrisonerMenu(id);
			}
	
			user_kill(id, 1);

			jbe_add_user_free_next_round(id);
			client_print_color(0, id, "%s Последний заключенный ^3%n ^1выбрал ^4выходной^1.", g_szChatPrefix, id);
		}
		case 2:
		{
			user_kill(id, 1);
			jbe_set_user_money(id, g_iUserMoney[id] + g_eAllCvars[LAST_PRISONER_MODEY], 1);
			client_print_color(0, id, "%s Последний заключенный ^3%n ^1выбрал ^4%d$^1.", g_szChatPrefix, id, g_eAllCvars[LAST_PRISONER_MODEY]);
		}
		case 3:
		{
			user_kill(id, 1);
			SetBit(g_iBitUserVoiceNextRound, id);
			client_print_color(0, id, "%s Последний заключенный ^3%n ^1выбрал ^4голос^1.", g_szChatPrefix, id);
		}
		case 4:
		{
			g_iLastPnType = LAST_PN_GUN;
			for (new i = 1; i <= MaxClients; i++)
			{
				if (IsNotSetBit(g_iBitUserAlive, i)
				|| g_iUserTeam[i] != TEAM_GUARD)
				continue;
				
				rg_remove_all_items(i);
				rg_give_item(i, "weapon_knife");
			}

			rg_give_item_ex(id, "weapon_ak47", GT_APPEND, 250);
			rg_give_item_ex(id, "weapon_m4a1", GT_APPEND, 250);
			jbe_set_user_godmode(id, 1);

			client_print_color(0, id, "%s Последний заключенный ^3%n ^1выбрал ^4убить охрану^1.", g_szChatPrefix, id);
		}
		case 5: return Show_ChoiceDuelMenu(id);
		case 6: return Show_FriendFreeDay(id);
		case 8: return Show_MainMenu(id);
	}
	return PLUGIN_HANDLED;
}

Show_FriendFreeDay(id, iPos = 0) {
	if (iPos < 0 || g_iDayMode != DAY_NORMAL
	&& g_iDayMode != DAY_FREEDAY || id != g_iLastPnId)
		return PLUGIN_HANDLED;

	if (g_iPlayersNum[TEAM_PRISONER] < MIN_ALL_PL_FD)
	{
		client_print_color(id, print_team_default, "%s Выходной можно взять если на сервере от %d зеков!", g_szChatPrefix, MIN_ALL_PL_FD);
		return Show_LastPrisonerMenu(id);
	}
	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (g_iUserTeam[i] != TEAM_PRISONER || i == id) continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}
	if (!menu_list_title("Выберите кому выходной", iPos, iPlayersNum)) {
		return client_print_color(id, 0, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
	}
	menu_item("^n");

	new iTarget;
	while (menu_list_count()) {
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		menu_list_item("\y%d. \w%n^n", menu_list_num(), iTarget);
	}

	return menu_list_show(id, "Show_FriendFreeDay");
}

public Handle_FriendFreeDay(id, iKey)
{
	if (g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY || id != g_iLastPnId) return PLUGIN_HANDLED;
	
	if (g_iPlayersNum[TEAM_PRISONER] < MIN_ALL_PL_FD)
	{
		client_print_color(id, print_team_default, "%s Выходной можно взять если на сервере от %d зеков!", g_szChatPrefix, MIN_ALL_PL_FD);
		return Show_LastPrisonerMenu(id);
	}

	switch (iKey)
	{
		case 8: return Show_FriendFreeDay(id, ++g_iMenuPosition[id]);
		case 9: return Show_FriendFreeDay(id, --g_iMenuPosition[id]);
		default:
		{
			new iTarget = g_iMenuPlayers[id][ menu_list_itemid(id, iKey) ];
			user_kill(id, 1); //ExecuteHamB(Ham_Killed, id, id, 0);
			jbe_add_user_free_next_round(iTarget);
			client_print_color(0, id, "%s Последний заключенный ^3%n ^1выбрал ^4выходной ^1для друга ^3%n^1.", g_szChatPrefix, id, iTarget);
		}
	}
	return PLUGIN_HANDLED;
}

Show_UserCabinetMenu(id, iPos = 0)
{
	if (iPos < 0) return PLUGIN_HANDLED;

	static aMenu[][64] = {
		"Остановить музыку и звуки",
		"Задания \d(квесты)",
		"Меню мертвого",
		"Я застрял",
		"Выдимость костюмов ВКЛ|ВЫКЛ",
		"Музыка в конце раунда ВКЛ|ВЫКЛ",
		"Язык чата RUS|ENG",
		"Правила",
		// page 2
		"Информация",
		"Звук в меню",
		"Голосовать за смену карты",
		"Номинировать карту",
		"Эмоции",
		"Заглушить игрока",
		"Хочу випку"
	};

	if (!iPos) // page 1
	{
		formatex(aMenu[4], charsmax(aMenu[]), "Выдимость костюмов \r[\y%s\r]",
			IsSetBit(g_iBitOffCostumes, id) ? "Выкл" : "Вкл");

		formatex(aMenu[5], charsmax(aMenu[]), "Музыка в конце раунда \r[\y%s\r]",
			IsSetBit(g_iBitUserRoundSound, id) ? "Вкл" : "Выкл");

		formatex(aMenu[6], charsmax(aMenu[]), "Язык чата \r[\y%s\r]",
			IsSetBit(g_iBitUserRusChat, id) ? "RUS" : "ENG");
	}
	else if (iPos == 1)
	{
		formatex(aMenu[9], charsmax(aMenu[]), "Звук в меню \r[\y%s\r]",
			IsSetBit(g_iBitButtonSound, id) ? "Вкл" : "Выкл");
	}

	menu_list_title("\wМой кабинет", iPos, sizeof(aMenu));
	menu_item("^n");
	while (menu_list_count())
		menu_list_item("\y%d. \w%s^n", menu_list_num(), aMenu[menu_list_countid()]);

	return menu_list_show(id, "Show_UserCabinetMenu");
}

public Handle_UserCabinetMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Show_UserCabinetMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_UserCabinetMenu(id, --g_iMenuPosition[id]);
	}

	switch (menu_list_itemid(id, iKey))
	{
		case 0:
		{
			client_cmd(id, "mp3 stop;stopsound");
			if (g_iDayMode == DAY_DAYMODE)
				SetBit(g_iBitOffDayModeSound, id);
		}
		case 1: return Show_QuestsUserMenu(id);
		case 2:
		{
			if (IsSetBit(g_iBitUserAlive, id))
				return client_print_color(id, id, "%s Вы живы!", g_szChatPrefix);
			else
				return Show_DeathUserMenu(id);
		}
		case 3: return ClCmd_Stuck(id);
		case 4: return ClCmd_OffCostumes(id);
		case 5: InvertBit(g_iBitUserRoundSound, id);
		case 6:
		{
			if (IsSetBit(g_iBitUserRusChat, id))
				LangCtrlEng(id);
			else
				LangCtrlRus(id);
		}

		// page 2
		case 7: return ClCmd_ShowRules(id);
		case 8: return Show_InfoMenu(id);
		case 9: InvertBit(g_iBitButtonSound, id);
		case 10: return client_cmd(id, "say /rtv");
		case 11: return client_cmd(id, "say /maps");
		case 12: return Show_AnimMenu(id);
		case 13: return client_cmd(id, "say /mute");
		case 14:
		{
			client_print_color(id, print_team_blue,
				" ^3* ^1Ну раз уж хочешь, открывай бесплатный кейс на сайте: ^4cs-one.ru/freecase");
			client_print(id, print_console,
				" * Ну раз уж хочешь, открывай бесплатный кейс на сайте: cs-one.ru/freecase");
			return PLUGIN_HANDLED;
		}
	}
	return Show_UserCabinetMenu(id, g_iMenuPosition[id]);
}

Show_PrivilegiesMenu(id)
{
	new iKeys = (1<<8|1<<9);
	menu_title("\wМеню привилегий^n^n");
	
	if (IsSetBit(g_iBitUserVip, id))
	{
		menu_item("\y1. \wVIP Menu^n");
		iKeys |= (1<<0);
	}
	else menu_item("\d1. VIP Menu^n");
	
	if (IsSetBit(g_iBitUserAdmin, id))
	{
		menu_item("\y2. \wAdmin Menu^n");
		menu_item("\y3. \wFast Admin^n");
		menu_item("\y4. \wMod Menu^n");
		iKeys |= (1<<1|1<<2|1<<3);
	}
	else
	{
		menu_item("\d2. Admin Menu^n");
		menu_item("\d3. Fast Admin^n");
		menu_item("\d4. Mod Menu^n");
	}

	if (IsSetBit(g_iBitUserGlobalAdmin, id))
	{
		menu_item("\y5. \wGlobal^n");
		iKeys |= (1<<4);
	}
	else menu_item("\d5. Global^n");
	
	if (IsSetBit(g_iBitUserHook, id))
	{
		menu_item("\y6. \wHook Menu \r[\y%s\r]^n", IsSetBit(g_iBitHookMenu, id) ? "Вкл" : "Выкл");
		iKeys |= (1<<5);
	}
	else menu_item("\d6. Hook Menu^n");
	
	if (IsSetBit(g_iBitUserGrab, id))
	{
		menu_item("\y7. \wGrab Menu \r[\y%s\r]^n", g_iGrabMenuType[id] ? "Вкл" : "Выкл");
		iKeys |= (1<<6);
	}
	else menu_item("\d7. Grab Menu^n");
	
	if (IsSetBit(g_iBitUserModelsPack, id) || g_iPersonalModelsUserSize[id])
	{
		menu_item("\y8. \wМодели^n");
		iKeys |= (1<<7);
	}
	else menu_item("\d8. Модели^n");
	
	menu_item("^n\yЦены на привилегии: \rcs-one.ru/price_list");
	menu_item("^n\yКупить онлайн: \rcs-one.ru/buy");
	menu_item("^n^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");

	return menu_show(id, iKeys, "Show_PrivilegiesMenu");
}
public Handle_PrivilegiesMenu(id, iKey)
{
	switch (iKey)
	{
		case 0: return Show_VipMenu(id);
		case 1:	return ClCmd_AdminMenu(id);
		case 2: return Show_AdminPlayerMenu(id);
		case 3: return Show_AdminModMenu(id);
		case 4: return Show_GlobalMenu(id);
		case 5: return Show_HookMenu(id);
		case 6: return ClCmd_GrabMenu(id);
		case 7: return Show_PersonalModelsMenu(id);
		case 8: return Show_MainMenu(id);
	}
	return PLUGIN_HANDLED;
}

Show_WeaponsGuardMenu(id)
{
	if ((g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY)
	|| IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;
		
	new iKeys = (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<8|1<<9);

	menu_title("Выберите оружие^n^n");
	menu_item("\y1.\w AK47 + Deagle^n");
	menu_item("\y2.\w M4A1 + Deagle^n");
	menu_item("\y3.\w AWP + Deagle^n");
	menu_item("\y4.\w XM1014 + Deagle^n");
	menu_item("\y5.\w M249 + Deagle^n");
	menu_item("\y6.\w SG552 + Deagle^n");
	menu_item("\y7.\w G3SG1 + Deagle^n");

	if (IsSetBit(g_iBitUserVip, id))
	{
		menu_item("\y8. \wM4A1 + SG550 + All Pistol's \r[VIP]^n");
		iKeys |= (1<<7);
	}
	else
		menu_item("\y8. \dM4A1 + SG550 + All Pistol's \r[VIP]^n");
	
	menu_item("^n\y9.\w Запомнить выбор \r[\y%s\r]^n",
		IsSetBit(g_iBitAutoWeapons, id) ? "Вкл" : "Выкл");
	
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	return menu_show(id, iKeys, "Show_WeaponsGuardMenu");
}

public Handle_WeaponsGuardMenu(id, iKey)
{
	if ((g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY)
	|| IsNotSetBit(g_iBitUserAlive, id) || IsSetBit(g_iBitUserDuel, id) || iKey == 9)
		return PLUGIN_HANDLED;
	
	if (iKey == 8)
	{
		InvertBit(g_iBitAutoWeapons, id);
		client_print_color(id, id, "%s Вы ^4%s ^1авто-выбор оружия ^3(открыть меню в чат: ^4/weapon^3)^1.", g_szChatPrefix, IsSetBit(g_iBitAutoWeapons, id) ? "включили" : "выключили");
		return Show_WeaponsGuardMenu(id);
	}
	
	if (IsSetBit(g_iBitBuyWeaponsGuard, id))
	{
		ClearBit(g_iBitBuyWeaponsGuard, id);
		
		if (g_eShopCvars[WEAPONS_GR] <= g_iUserMoney[id])
		{
			jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[WEAPONS_GR], 1);
		}
		else
		{
			client_print_color(id, 0, "%s Недостаточно денег..", g_szChatPrefix);
			return PLUGIN_HANDLED;
		}
	}
		
	jbe_give_guard_weapond(id, iKey);
	g_iUserAutoWeapon[id] = iKey;

	return PLUGIN_HANDLED;
}

Show_SoundMenu(id, iPos = 0)
{
	if (iPos < 0 || g_iDayMode == DAY_DAYMODE || (g_iChiefId != id && IsNotSetBit(g_iBitUserAdmin, id))) return PLUGIN_HANDLED;
	
	if (!g_iRoundSoundSize)
		return client_print_color(id, id, "%s ^1Музыка не найдена.", g_szChatPrefix);

	menu_list_title("Меню музыки", iPos, g_iRoundSoundSize);
	menu_item("^n");

	new aDataRoundSound[DATA_ROUND_SOUND];
	while (menu_list_count()) {
		ArrayGetArray(g_aDataRoundSound, menu_list_countid(), aDataRoundSound);
		menu_list_item("\y%d. \w%s^n", menu_list_num(), aDataRoundSound[TRACK_NAME]);
	}

	return menu_list_show(id, "Show_SoundMenu");
}

public Handle_SoundMenu(id, iKey)
{
	if (g_iDayMode == DAY_DAYMODE || (g_iChiefId != id && IsNotSetBit(g_iBitUserAdmin, id)))
		return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 8: return Show_SoundMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_SoundMenu(id, --g_iMenuPosition[id]);
		default:
		{
			new aDataRoundSound[DATA_ROUND_SOUND];
			new iTrack = menu_list_itemid(id, iKey);
			ArrayGetArray(g_aDataRoundSound, iTrack, aDataRoundSound);
			client_cmd(0, "mp3 play sound/CsOneRu/jb/round_sound/%s.mp3", aDataRoundSound[FILE_NAME]);
			client_print_color(0, print_team_blue, "%s ^1%s ^4%n ^1включил ^3%s^1.",
				g_szChatPrefix, id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id, aDataRoundSound[TRACK_NAME]);
		}
	}
	return Show_SoundMenu(id, g_iMenuPosition[id]);
}

Show_InfoMenu(id)
{
	menu_title(
		"Наш IP: %s^n\
		Наш Сайт: CS-One.Ru^n\
		Группа: vk.com/CsOneRu^n\
		Онлайн Магазин: cs-one.ru/store^n\
		Форум: cs-one.ru/forum^n\
		Версия мода: %s^n\
		Дата обновления: %s^n^n\
		JailBreak Mod by Serp (CS-ONE)^n^n\
		\d0. Закрыть",
	g_szIpServer, PLUGIN_VERSION, __DATE__); // , __TIME__
	
	client_print_color(id, print_team_blue, "^4 * ^1Наш сайт: ^3CS-One.Ru ^1(откройте консоль).");
	client_print(id, print_console, " * Онлайн магазин: cs-one.ru/store");
	client_print(id, print_console, " * Форум: cs-one.ru/forum");
	client_print(id, print_console, " * Розыгрыши: cs-one.ru/sortition");
	client_print(id, print_console, " * Наш IP: %s", g_szIpServer);
	
	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Show_InfoMenu");
}
public Handle_InfoMenu(id, iKey) {
	jbe_informer_offset_down(id);
	client_print_color(id, print_team_blue, "^4 * ^1Наш сайт: ^3CS-One.Ru ^1(откройте консоль).");
	client_print(id, print_console, " * Онлайн магазин: cs-one.ru/store");
	client_print(id, print_console, " * Форум: cs-one.ru/forum");
	client_print(id, print_console, " * Розыгрыши: cs-one.ru/sortition");
	client_print(id, print_console, " * Наш IP: %s", g_szIpServer);
	return PLUGIN_HANDLED;
}

is_not_deathuser_menu(id) {
	if (IsSetBit(g_iBitUserAlive, id))
		client_print_color(id, 0, "%s Меню мертвого недоступно живым.", g_szChatPrefix);

	else if (g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY)
		client_print_color(id, 0, "%s Меню мертвого доступно только в обычные дни.", g_szChatPrefix);

	else if (g_iUserTeam[id] != TEAM_PRISONER && g_iUserTeam[id] != TEAM_GUARD)
		client_print_color(id, 0, "%s Меню мертвого недоступно наблюдателям.", g_szChatPrefix);

	else
		return false;

	return true;
}

Show_DeathUserMenu(id)
{
	if (is_not_deathuser_menu(id))
		return PLUGIN_HANDLED;

	new iKeys = (1<<0|1<<1|1<<4|1<<8|1<<9);
	menu_title("Меню мертвого^n^n");
	if (IsNotSetBit(g_iBitUserGhost, id))
	{
		menu_item("\y1. \wСтать призраком^n");
		menu_item("\y2. \wКупить возрождение \r[%d$]^n^n^n", g_eShopCvars[BUY_RESPAWN]);
	}
	else
	{
		menu_item("\y1. \wВернуться в мир мёртвых^n");
		menu_item("\y2. \wНет преград \r[\y%s\r]^n",
			IsSetBit(g_iBitGhostNoClip, id) ? "Вкл" : "Выкл");
		menu_item("\y3. \wСкорость \r[\y%s\r]^n",
			IsSetBit(g_iBitFastRun, id) ? "Вкл" : "Выкл");
		menu_item("\y4. \wГравитация \r[\y%s\r]^n^n",
			IsSetBit(g_iBitGhostGravity, id) ? "Вкл" : "Выкл");
		iKeys |= (1<<0|1<<2|1<<3);
	}
	menu_item("\y5. \wМеню при смерти \r[\y%s\r]^n^n",
		IsSetBit(g_iBitShowDeathMenu, id) ? "Вкл" : "Выкл");
	
	if (IsSetBit(g_iBitUserGhost, id))
	{
		menu_item("\y6. \wКупить возрождение \r[%d$]^n^n", g_eShopCvars[BUY_RESPAWN]);
		iKeys |= (1<<5);
	}

	menu_item("^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	return menu_show(id, iKeys, "Show_DeathUserMenu");
}

public Handle_DeathUserMenu(id, iKey)
{
	if (is_not_deathuser_menu(id))
		return PLUGIN_HANDLED;
		
	switch(iKey)
	{
		case 0:
		{
			if (IsNotSetBit(g_iBitUserGhost, id))
			{
				jbe_set_user_maxspeed(id); //ClearBit(g_iBitFastRun, id);
				ClearBit(g_iBitGhostNoClip, id);
				ClearBit(g_iBitGhostGravity, id);
				SetBit(g_iBitUserGhost, id);
				SetBit(g_iBitActiveGhost, id);
				rg_round_respawn(id);
				client_print_color(id, id, "%s ^1Нажмите ^4'N' ^1чтоб быстро открыть меню мертвого, ^3(bind n nightvision)^1.", g_szChatPrefix);
			}
			else
			{
				jbe_set_user_maxspeed(id);
				set_pev(id, pev_deadflag, DEAD_NO);
				user_silentkill(id);
				ClearBit(g_iBitUserGhost, id);
				ClearBit(g_iBitActiveGhost, id);

				new iDeath = get_user_deaths(id);
				set_member(id, m_iDeaths, iDeath - 1);
								
				jbe_reset_user_informer(id);
				return PLUGIN_HANDLED;
			}
		}
		case 1:
		{
			if (IsNotSetBit(g_iBitUserGhost, id))
			{
				jbe_buy_respawn(id);
			}
			else
			{
				rg_set_user_noclip(id, IsSetBit(g_iBitGhostNoClip, id) ? 0 : 1);
				InvertBit(g_iBitGhostNoClip, id);
			}
		}
		case 2:
		{
			if (IsSetBit(g_iBitFastRun, id))
			{
				jbe_set_user_maxspeed(id, _, true);
			}
			else
			{
				jbe_set_user_maxspeed(id, 600.0, true);
			}
		}
		case 3:
		{
			set_pev(id, pev_gravity, IsSetBit(g_iBitGhostGravity, id) ? 1.0 : 0.2);
			InvertBit(g_iBitGhostGravity, id);
		}
		case 4:
		{
			InvertBit(g_iBitShowDeathMenu, id);
			client_print_color(id, id, "%s ^1Вы ^3%s ^1меню мертвого при смерти.", g_szChatPrefix, IsSetBit(g_iBitShowDeathMenu, id) ? "включили" : "выключили");
			return PLUGIN_HANDLED;
		}
		case 5: return jbe_buy_respawn(id);
		case 8: return Show_MainMenu(id);
		case 9: return PLUGIN_HANDLED;
	}
	return Show_DeathUserMenu(id);
}

Show_MusicMenu(id, iPos = 0)
{
	if (iPos < 0) return PLUGIN_HANDLED;

	new aDataDayMode[DATA_DAY_MODE_MUSIC];

	menu_list_title("Выберите музыку", iPos, g_iGameMusicSize, 6);
	menu_item("^n");

	while (menu_list_count())
	{
		ArrayGetArray(g_aDataGameMusic, menu_list_countid(), aDataDayMode);
		menu_list_item("\y%d. \w%s^n", menu_list_num(), aDataDayMode[DM_TRACK_NAME]);
	}

	menu_list_empty_items();
	menul_item_block(""); // 7 item
	menu_list_item("^n\y8. \wВыключить музыку");

	return menu_list_show(id, "Show_MusicMenu");
}

public Handle_MusicMenu(id, iKey)
{
	switch (iKey)
	{
		case 7:
		{
			client_cmd(0, "mp3 stop");
			client_print_color(0, id, "%s %s ^4%n ^1выключил музыку.", g_szChatPrefix, funcPref(id), id);
			return Show_MusicMenu(id, g_iMenuPosition[id]);
		}
		case 8: return Show_MusicMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_MusicMenu(id, --g_iMenuPosition[id]);
	}

	new iTrack = menu_list_itemid(id, iKey);

	new aDataDayMode[DATA_DAY_MODE_MUSIC];
	ArrayGetArray(g_aDataGameMusic, iTrack, aDataDayMode);
	client_cmd(0, "mp3 play %s", aDataDayMode[DM_FILE_NAME]);
	client_print_color(0, 0, "%s %s ^4%n ^1включил музыку ^4%s^1.", g_szChatPrefix, funcPref(id), id, aDataDayMode[DM_TRACK_NAME]);
	
	return Show_MusicMenu(id, g_iMenuPosition[id]);
}

Show_AnimMenu(id)
{
	menu_title("Анимации^n^n");

	menu_item("\y1. \wПриветствие^n");
	menu_item("\y2. \wРадость^n");
	menu_item("\y3. \wДикость^n");
	menu_item("\y4. \wСпокойствие^n");
	menu_item("\y5. \wДэнс^n");
	menu_item("\y6. \wПонты^n^n^n^n");

	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<9), "Show_AnimMenu");
}

public Handle_AnimMenu(id, iKey)
{
	if (IsNotSetBit(g_iBitUserAlive, id))
		return client_print_color(id, id, "%s Доступно только живым!", g_szChatPrefix);

	if (!is_player_model_anims(id))
		return client_print_color(id, id, "%s Не доступно на вашей модели!", g_szChatPrefix);

	if (task_exists(id + TASK_MENU_ANIM))
		return client_print_color(id, id, "%s Не так часто!", g_szChatPrefix);

	if (iKey == 9)
		return PLUGIN_HANDLED;

	if (get_user_weapon(id) != CSW_KNIFE)
		engclient_cmd(id, "weapon_knife");
	
	set_member(id, m_flNextAttack, 4.0);
	set_task(4.0, "emotion_fix_anim", id + TASK_MENU_ANIM);

	set_entvar(id, var_viewmodel, model_v_hand);

	new szPlayerAnim[24];
	formatex(szPlayerAnim, charsmax(szPlayerAnim), "animation_%d", iKey + 1);
	UTIL_PlayerAnimation(id, szPlayerAnim);
	UTIL_WeaponAnimation(id, iKey + 8);

	return PLUGIN_HANDLED;
}

public emotion_fix_anim(id)
{
	id -= TASK_MENU_ANIM;

	if (IsNotSetBit(g_iBitUserAlive, id))
		return;

	if (get_user_weapon(id) != CSW_KNIFE)
		return;

	jbe_default_weapon_knife(id);
	UTIL_WeaponAnimation(id, 3);
}



/*===== <- Меню <- =====*///}