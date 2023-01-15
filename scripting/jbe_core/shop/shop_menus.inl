Show_ShopPrisonersMenu(id, iPos = 0)
{
	if (iPos < 0) return PLUGIN_HANDLED;

	enum { ITEM_ID, ITEM_NAME };
	new const aMenu[][] = {
		{ CLOSE_CASE,		"Закрыть дело" },
		{ FROSTNADE,		"Граната-заморозка" },
		{ LATCHKEY,			"Отмычка" },
		{ RESOLUTION_VOICE,	"Голос на раунд" },
		{ LOTTERY_TICKET,	"Лотерея" },
		{ PRANK_PRISONER,	"Подставить зека" },
		{ NONE,				"Продать ствол" },
		{ TRANSFER_GUARD,	"Перевод за охрану",  },

		{ CLOTHING_GUARD,	"Одежда охраны" },
		{ FREE_DAY_SHOP,	"Выходной",  },
		{ SKIN,				"Смена одежды \d(skin)",  },
		{ NONE,				"Ограбить Магазин",  },
		{ BALISONG,			"Нож-Бабочка" },
		{ GUARD_BATON,		"Дубинка" },
		{ BASABALLBAT,		"Бита" },
		{ USP,				"USP" },
		
		{ GRENADES,			"HE+Flesh гранаты" },
		{ KOKAIN,			"Кокаин \d[урон/2]" },
		{ STIMULATOR,		"Стимулятор \d[200HP]" },
		{ ARMOR,			"Бронежилет" },
		{ HING_JUMP,		"Высокий прыжок" },
		{ FAST_RUN,			"Быстрый бег" },
		{ DOUBLE_JUMP,		"Двойной прыжок",  },
		{ FALL_NO_DMG,		"Защита от падений",  },

		{ AUTO_BHOP,		"Автораспрыг" },
		{ DOUBLE_DAMAGE,	"Двойной урон",  },
		{ LOW_GRAVITY,		"Низкая гравитация",  },
		{ LONG_JUMP,		"Длиный прыжок" },
	};

	new iMenuSize = jbe_is_shop_time_close(id) ? 8 : sizeof(aMenu);
	menu_list_title("Магазин", iPos, iMenuSize);
	menu_item("^n");

	while (menu_list_count())
	{
		if (g_eShopCvars[aMenu[menu_list_countid()][ITEM_ID]])
		{
			if (g_iUserMoney[id] >= g_eShopCvars[aMenu[menu_list_countid()][ITEM_ID]])
				menu_list_item("\y%d. \w%s \r[%d$]^n", menu_list_num(), aMenu[menu_list_countid()][ITEM_NAME], g_eShopCvars[aMenu[menu_list_countid()][ITEM_ID]]);
			else
				menul_item_block("\d%d. %s \r[%d$]^n", menu_list_num(), aMenu[menu_list_countid()][ITEM_NAME], g_eShopCvars[aMenu[menu_list_countid()][ITEM_ID]]);
		}
		else
			menu_list_item("\y%d. \w%s^n", menu_list_num(), aMenu[menu_list_countid()][ITEM_NAME]);
	}

	return menu_list_show(id, "Show_ShopPrisonersMenu");
}

public Handle_ShopPrisonersMenu(id, iKey)
{
	if (is_not_shop_menu(id)) return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 8: return Show_ShopPrisonersMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_ShopPrisonersMenu(id, --g_iMenuPosition[id]);
	}

	switch (menu_list_itemid(id, iKey))
	{
		case 0: jbe_shop_slose_case(id);
		case 1: jbe_shop_frostnade(id);
		case 2: jbe_shop_latchkey(id);
		case 3: jbe_shop_resolution_voice(id);
		case 4: jbe_shop_lottery_ticket(id);
		case 5: return Show_PrankPrisonerMenu(id);
		case 6: jbe_price_gun_prisoner(id);
		case 7: jbe_shop_transfer_guard(id);

		case 8: jbe_shop_cloting_guard(id);
		case 9: jbe_shop_free_day(id);
		case 10: return Show_SkinMenu(id);
		case 11: return jbe_rob_shop(id);
		case 12: jbe_shop_balisong(id);
		case 13: jbe_shop_guard_baton(id);
		case 14: jbe_shop_baseballbat(id);
		case 15: jbe_shop_usp(id);

		case 16: jbe_shop_grenades(id);
		case 17: jbe_shop_kokain(id);
		case 18: jbe_shop_stimulator(id);
		case 19: jbe_shop_armor(id);
		case 20: jbe_shop_hing_jump(id);
		case 21: jbe_shop_fast_run(id);
		case 22: jbe_shop_double_jump(id);
		case 23: jbe_shop_fall_no_dmg(id);

		case 24: jbe_shop_auto_bhop(id);	
		case 25: jbe_shop_double_damage(id);
		case 26: jbe_shop_low_gravity(id);
		case 27: jbe_shop_long_jump(id);
	}

	return PLUGIN_HANDLED;
}

Show_ShopGuardMenu(id, iPos = 0)
{
	if (iPos < 0) return PLUGIN_HANDLED;

	enum { ITEM_ID, ITEM_NAME };
	new aMenu[][] = {
		{ STIMULATOR,		"Стимулятор \d[200HP]" },
		{ LOTTERY_TICKET,	"Лотерея" },
		{ WEAPONS_GR,		"Оружейная" },
		{ ELECTRO_COSTUME,	"Электро костюм" },
		{ KOKAIN,			"Кокаин \d[урон/2]" },
		{ DOUBLE_JUMP,		"Двойной прыжок" },
		{ DOUBLE_DAMAGE,	"Двойной урон" },
		{ FAST_RUN,			"Скорость" },

		{ LOW_GRAVITY,		"Низкая гравитация" },
		{ HING_JUMP,		"Высокий прыжок" },
		{ AUTO_BHOP,		"Автораспрыг" },
		{ LONG_JUMP,		"Длиный прыжок" },
		{ FALL_NO_DMG,		"Защита от падений" },
	};

	menu_list_title("Магазин", iPos, sizeof(aMenu));
	menu_item("^n");

	while (menu_list_count())
	{
		if (g_eShopCvars[aMenu[menu_list_countid()][ITEM_ID]])
		{
			if (g_iUserMoney[id] >= g_eShopCvars[aMenu[menu_list_countid()][ITEM_ID]])
				menu_list_item("\y%d. \w%s \r[%d$]^n", menu_list_num(), aMenu[menu_list_countid()][ITEM_NAME], g_eShopCvars[aMenu[menu_list_countid()][ITEM_ID]]);
			else
				menul_item_block("\d%d. %s \r[%d$]^n", menu_list_num(), aMenu[menu_list_countid()][ITEM_NAME], g_eShopCvars[aMenu[menu_list_countid()][ITEM_ID]]);
		}
		else
			menu_list_item("\y%d. \w%s^n", menu_list_num(), aMenu[menu_list_countid()][ITEM_NAME]);
	}

	return menu_list_show(id, "Show_ShopGuardMenu");
}

public Handle_ShopGuardMenu(id, iKey)
{
	if (is_not_shop_menu(id)) return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 8: return Show_ShopGuardMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_ShopGuardMenu(id, --g_iMenuPosition[id]);
	}

	switch (menu_list_itemid(id, iKey))
	{
		case 0: jbe_shop_stimulator(id);
		case 1: jbe_shop_lottery_ticket(id);
		case 2: jbe_shop_weapons_guard(id);
		case 3: jbe_shop_electro_costume(id);
		case 4: jbe_shop_kokain(id);
		case 5: jbe_shop_double_jump(id);
		case 6: jbe_shop_double_damage(id);
		case 7: jbe_shop_fast_run(id);

		case 8: jbe_shop_low_gravity(id);
		case 9: jbe_shop_hing_jump(id);
		case 10: jbe_shop_auto_bhop(id);
		case 11: jbe_shop_long_jump(id);
		case 12: jbe_shop_fall_no_dmg(id);
	}

	return PLUGIN_HANDLED;
}

Show_PrankPrisonerMenu(id, iPos = 0)
{
	if (iPos < 0 || is_not_shop_menu(id)) return PLUGIN_HANDLED;
	
	new iPlayersNum;
	for(new i = 1; i <= MaxClients; i++)
	{
		if(g_iUserTeam[i] != 1 || IsNotSetBit(g_iBitUserAlive, i)
		|| IsSetBit(g_iBitUserWanted, i) || i == id)
			continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	if (!menu_list_title("Выберите кого подставить", iPos, iPlayersNum))
		return client_print_color(id, 0, "%s ^1Подходящие игроки не найдены.", g_szChatPrefix);
	menu_item("^n");

	new iTarget;
	while (menu_list_count()) {
		iTarget = g_iMenuPlayers[id][ menu_list_countid() ];
		menu_list_item("\y%d. \w%n^n", menu_list_num(), iTarget);
	}

	return menu_list_show(id, "Show_PrankPrisonerMenu");
}

public Handle_PrankPrisonerMenu(id, iKey)
{
	if (is_not_shop_menu(id)) return PLUGIN_HANDLED;
	switch (iKey)
	{
		case 8: return Show_PrankPrisonerMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_PrankPrisonerMenu(id, --g_iMenuPosition[id]);
		default:
		{
			new iTarget = g_iMenuPlayers[id][menu_list_itemid(id, iKey)];
			if (g_eShopCvars[PRANK_PRISONER] <= g_iUserMoney[id])
			{
				if(g_iUserTeam[iTarget] == 1 || IsSetBit(g_iBitUserAlive, iTarget) || IsNotSetBit(g_iBitUserWanted, iTarget))
				{
					jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[PRANK_PRISONER], 1);
					jbe_add_user_wanted(iTarget);
					client_print_color(iTarget, iTarget, "%s ^3Вы в розыске^1, вас кто-то подставил...", g_szChatPrefix);
				}
				else return Show_PrankPrisonerMenu(id, g_iMenuPosition[id]);
			}
			else return Show_ShopPrisonersMenu(id, g_iMenuPosition[id]);
		}
	}
	return PLUGIN_HANDLED;
}

Show_SkinMenu(id)
{	
	new iKeys = (1<<8|1<<9);
	
	menu_title("Выбор одежды^n\yСтоимость \r%d$^n", g_eShopCvars[SKIN]);
	
	if (g_iUserMoney[id] >= g_eShopCvars[SKIN] && g_iPersonalModel[id] <= 0)
	{
		iKeys |= (1<<0|1<<1|1<<2|1<<3|1<<4);
		menu_item("\y1. %sОранжевая^n", g_iUserSkin[id] != 0 ? "\w" : "\d");
		menu_item("\y2. %sСерая^n", g_iUserSkin[id] != 1 ? "\w" : "\d");
		menu_item("\y3. %sЖелтая^n", g_iUserSkin[id] != 2 ? "\w" : "\d");
		menu_item("\y4. %sСиняя^n", g_iUserSkin[id] != 3 ? "\w" : "\d");
		menu_item("\y5. %sКоричневая^n", g_iUserSkin[id] != 4 ? "\w" : "\d");
		if (IsSetBit(g_iBitUserAdmin, id) || IsSetBit(g_iBitUserVip, id))
		{
			menu_item("\y6. %sЧерная \r[VIP|ADMIN]^n", g_iUserSkin[id] != 5 ? "\w" : "\d");
			iKeys |= (1<<5);
		}
		else menu_item("\y6. \dЧерная \r[VIP|ADMIN]^n");
		iKeys &= ~(1<<g_iUserSkin[id]);
	}
	else
	{
		menu_item("\y1. \dОранжевая^n\
					\y2. \dСерая^n\
					\y3. \dЖелтая^n\
					\y4. \dСиняя^n\
					\y5. \dКоричневая^n\
					\y6. \dЧерная \r[VIP]^n");
	}

	if (g_iPersonalModel[id])
	{
		menu_item("^n\y8. \wМодели^n");
		iKeys |= (1<<7);
	}
	else menu_item("^n\dКупить личную модель:^n\rcs-one.ru/store");
	
	menu_item("^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");

	return menu_show(id, iKeys, "Show_SkinMenu");
}

public Handle_SkinMenu(id, iKey)
{
	switch (iKey)
	{
		case 0..5:
		{
			jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[SKIN], 1);
			g_iUserSkin[id] = iKey;
			if (IsNotSetBit(g_iBitUserSoccer, id))
				jbe_default_player_model(id);
			new szLangPlayer[][] = { "оранжевую", "серую", "жёлтую", "синюю", "коричневую", "черную" };
			client_print_color(id, id, "%s ^1Вы купили ^4%s ^1одежду.", g_szChatPrefix, szLangPlayer[iKey]);
		}
		case 7: return Show_PersonalModelsMenu(id);
		case 8: return jbe_shop_open_menu(id);
	}
	return PLUGIN_HANDLED;
}
