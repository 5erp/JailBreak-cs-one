// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

// duel_prize_accept.inl

Show_DuelPrizeAccept(id)
{
	if (IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;

	menu_title("Заключенный предложил^n\
		сыграть на \r%d %s^n^n", g_iDualPrizeNum,
		g_iDuelPrizeType == DUEL_PRIZE_MONEY_CUSTOM ? "$" : "опыта");

	menu_item("\dПринять предложение?^n^n");
	menu_item("\y1. \wДа^n");
	menu_item("\y2. \wНет^n");
	menu_item("^n^n\d%d секунд, чтобы отказаться", g_iDuelCountDown);

	return menu_show(id, (1<<0|1<<1), "Show_DuelPrizeAccept");
}

public Handle_DuelPrizeAccept(id, iKey)
{
	if (IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 0:
		{
			client_print_color(0, print_team_blue,
				"%s ^1Охранник ^3%n ^1принял дуэль на ^4%d %s^1.",
				g_szDuelPrefix, id, g_iDualPrizeNum,
				g_iDuelPrizeType == DUEL_PRIZE_MONEY_CUSTOM ? "$" : "опыта");
		}
		case 1:
		{
			client_print_color(0, print_team_blue,
				"%s ^1Охранник ^3%n ^1отказался играть на ^4%d %s^1.",
				g_szDuelPrefix, id, g_iDualPrizeNum,
				g_iDuelPrizeType == DUEL_PRIZE_MONEY_CUSTOM ? "$" : "опыта");

			switch (g_iDuelPrizeType)
			{
				case DUEL_PRIZE_MONEY_CUSTOM: g_iDuelPrizeType = DUEL_PRIZE_MONEY;
				case DUEL_PRIZE_EXP_CUSTOM: g_iDuelPrizeType = DUEL_PRIZE_EXP;
			}
		}
	}
	return PLUGIN_HANDLED;
}