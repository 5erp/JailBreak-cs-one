// choice_duel_menu.inl

is_not_choiseduel_menu(id)
{
	if (IsNotSetBit(g_iBitUserAlive, id))
		client_print_color(id, id, "%s Меню не доступно мертвым.", g_szChatPrefix);

	else if (id != g_iLastPnId && id != g_iChiefId)
	{
		switch (g_iUserTeam[id])
		{
			case TEAM_PRISONER:
				client_print_color(id, id, "%s Меню доступно только последнему заключенному.", g_szChatPrefix);
			
			case TEAM_GUARD:
				client_print_color(id, id, "%s Меню доступно только начальнику.", g_szChatPrefix);
		}
	}

	else return false;

	return true;
}

Show_ChoiceDuelMenu(id)
{
	if (is_not_choiseduel_menu(id))
		return PLUGIN_HANDLED;

	menu_title("Выберите дуэль^n^n");
	menu_item("\y1. \wНа 'Пистолетах'^n");
	menu_item("\y2. \wНа 'Дробовиках'^n");
	menu_item("\y3. \wНа 'Гранатах'^n");
	menu_item("\y4. \wНа 'Пулемётах'^n");
	menu_item("\y5. \wНа 'Снайперках'^n");
	menu_item("\y6. \wНа 'Ножах'^n");
	menu_item("\y7. \wНа 'AK-47'^n");
	menu_item("\y8. \wНа 'Scout'^n");
	menu_item("^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Show_ChoiceDuelMenu");
}

public Handle_ChoiceDuelMenu(id, iKey)
{
	if (is_not_choiseduel_menu(id))
		return PLUGIN_HANDLED;
	
	if (iKey == 8)
	{
		if (id == g_iLastPnId)
			return Show_LastPrisonerMenu(id);
		else
			return Show_GameDuelMenu(id);
	}
	
	if (iKey == 9)
		return PLUGIN_HANDLED;

	g_iDuelType = iKey + 1;

	if (id == g_iLastPnId)
	{
		return Show_DuelPrizeMenu(id);
	}

	else if (id == g_iChiefId)
	{
		if (IsSetBit(g_iBitUserAlive, g_iDuelUsersId[0])
		&& IsSetBit(g_iBitUserAlive, g_iDuelUsersId[1]))
		{
			jbe_duel_start_ready(g_iDuelUsersId[0], g_iDuelUsersId[1]);
			client_print_color(0, print_team_blue,
				"%s ^1Начальник сказал ^3дуэль ^1между ^4%n ^1и ^4%n^1.", g_szDuelPrefix,
				g_iDuelUsersId[0], g_iDuelUsersId[1]);
		}
		else
		{
			client_print_color(id, id, "%s Отсутствует один из дуэлянтов!", g_szDuelPrefix);
			return Show_GameDuelMenu(id);
		}
	}
	
	return PLUGIN_HANDLED;
}