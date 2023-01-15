// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

// duel_custom_prize_menu.inl

Show_DuelCustomPrizeMenu(id)
{
	if (IsNotSetBit(g_iBitUserAlive, id) || id != g_iLastPnId)
		return PLUGIN_HANDLED;

	if (!g_iMenuTarget[id] || !jbe_is_user_valid(g_iMenuTarget[id])
	|| IsNotSetBit(g_iBitUserAlive, g_iMenuTarget[id])
	|| g_iUserTeam[g_iMenuTarget[id]] != TEAM_GUARD)
		g_iMenuTarget[id] = fnGetRandomAlive(random_num(1, g_iAlivePlayersNum[TEAM_GUARD]), TEAM_GUARD);

	g_iDuelPrizeTarget = id;

	if (g_iDuelPrizeType < DUEL_PRIZE_MONEY_CUSTOM)
		g_iDuelPrizeType = DUEL_PRIZE_MONEY_CUSTOM;

	if (!g_iDualPrizeNum)
	{
		switch (g_iDuelPrizeType)
		{
			case DUEL_PRIZE_MONEY_CUSTOM: g_iDualPrizeNum = PRIZE_MONEY_NUM;
			case DUEL_PRIZE_EXP_CUSTOM: g_iDualPrizeNum = PRIZE_EXP_NUM;
		}
	}
	
	_duel_custom_prize_check(.bMessage = false);

	menu_title("Победитель \rзабирает \wнаграду у \yпроигравшего\w!^n^n");
	menu_item("\y1. \wСтарт дуэли^n^n");
	
	if (g_iDuelPrizeType == DUEL_PRIZE_MONEY_CUSTOM)
		menu_item("\y2. \wПротивник: \y%n \d(%d $)^n",
			g_iMenuTarget[id], g_iUserMoney[g_iMenuTarget[id]]);
	else
		menu_item("\y2. \wПротивник: \y%n \d(%d опыт)^n",
			g_iMenuTarget[id], g_iUserExp[g_iMenuTarget[id]]);
	
	menu_item("\y3. \wНаграда: \r%s^n", g_szDuelPrizes[g_iDuelPrizeType]);
	
	menu_item("^n\dСумма: \r%d %s^n", g_iDualPrizeNum,
		g_iDuelPrizeType == DUEL_PRIZE_MONEY_CUSTOM ? "$" : "опыта");
	
	menu_item("\y4. \wПовысить сумму^n");
	menu_item("\y5. \wПонизить сумму^n");
	menu_item("\y6. \wВвести сумму^n");
	
	menu_item("^n^n^n\y0. \w%L", id, "JBE_MENU_BACK");
	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<9), "Show_DuelCustomPrizeMenu");
}

public Handle_DuelCustomPrizeMenu(id, iKey)
{
	if (IsNotSetBit(g_iBitUserAlive, id) || id != g_iLastPnId)
		return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 0:
		{
			if (IsSetBit(g_iBitUserAlive, g_iMenuTarget[id]))
			{
				jbe_duel_start_ready(id, g_iMenuTarget[id]);
				g_iLastPnType = LAST_PN_DUEL;
				return PLUGIN_HANDLED;
			}
			else
			{
				client_print_color(id, 0, "%s Игрок мертв..", g_szDuelPrefix);
				return Show_DuelCustomPrizeMenu(id);
			}
		}
		case 1: return Cmd_DuelUsersMenu(id);
		case 2:
		{
			if (g_iDuelPrizeType == DUEL_PRIZE_MONEY_CUSTOM)
			{
				g_iDuelPrizeType = DUEL_PRIZE_EXP_CUSTOM;
				g_iDualPrizeNum = PRIZE_EXP_NUM;
			}
			else
			{
				g_iDuelPrizeType = DUEL_PRIZE_MONEY_CUSTOM;
				g_iDualPrizeNum = PRIZE_MONEY_NUM;
			}
			_duel_custom_prize_check();
		}
		case 3: duel_custom_prize(true);
		case 4: duel_custom_prize(false);
		case 5:
		{
			client_cmd(id, "messagemode ^"duel_num^"");
			client_print_color(id, 0, "%s ^1Введите сумму ^4%s ^1на сколько хотите играть.", g_szDuelPrefix,
				g_iDuelPrizeType == DUEL_PRIZE_MONEY_CUSTOM ? "денег" : "опыта");
			return PLUGIN_HANDLED;
		}
		case 9: return Show_DuelPrizeMenu(id);
	}
	return Show_DuelCustomPrizeMenu(id);
}
