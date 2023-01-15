// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

box_menu_init() {
	register_menucmd(register_menuid("Show_BoxingMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<8|1<<9), "Handle_BoxingMenu");
}

// Обновленное меню без необходимости включения/выключения бокса
Show_BoxingMenu(id)
{
	if (g_iDayMode != DAY_NORMAL || is_not_chief_menu(id))
		return PLUGIN_HANDLED;

	new iKeys = (1<<0|1<<4|1<<5|1<<8|1<<9);

	menu_title("\wУправление боксом^n^n");
	
	if (g_iBoxingGame && g_iBoxingGame != BOXING_WAIT)
	{
		menu_item("\y1. \wЗавершить %sбой^n", 
			g_iBoxingGame == 1 ? "" : "командный ");
		menu_item("\y2. \dНачать командный бой^n");
		menu_item("\y3. \dРаздать всем перчатки^n");
	}
	else
	{
		if (g_iBoxingGame == BOXING_WAIT)
		{
			menu_item("\y1. \wНачать бой^n");
			menu_item("\y2. \wНачать командный бой^n");
		}
		else
		{
			menu_item("\y1. \dНачать бой^n");
			menu_item("\y2. \dНачать командный бой^n");
		}
		menu_item("\y3. \wРаздать всем перчатки^n");
		iKeys |= (1<<1|1<<2);
	}
	if (g_iBoxingGame)
	{
		menu_item("\y4. \wВыключить бокс^n");
		iKeys |= (1<<3);
	}
	else
		menu_item("\y4. \dВыключить бокс^n");

	menu_item("^n\dПерчатки по прицелу:^n");
	menu_item("\y5. \wДать красные перчатки^n");
	menu_item("\y6. \wДать синие перчатки^n");

	if (g_iBoxingGame)
	{
		menu_item("\y7. \wЗабрать перчатки^n");
		iKeys |= (1<<6);
	}
	else
		menu_item("\y7. \dЗабрать перчатки^n");

	menu_item("^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	
	return menu_show(id, iKeys, "Show_BoxingMenu");
}

public Handle_BoxingMenu(id, iKey)
{
	if (g_iDayMode != DAY_NORMAL || is_not_chief_menu(id))
		return PLUGIN_HANDLED;
	
	switch (iKey)
	{
		case 0:
		{
			if (g_iBoxingGame && g_iBoxingGame != BOXING_WAIT)
				jbe_boxing_game_end();
			else 
				jbe_boxing_game_start(id);
		}
		case 1: jbe_boxing_game_team_start(id);
		case 2: jbe_boxing_divide_team();
		case 3: jbe_boxing_disable_all();
		case 4, 5: jbe_boxing_set_gloves_model(id, iKey - 4);
		case 6: jbe_boxing_set_gloves_model(id, -1);
		case 8:
		{
			if (g_iBoxingGame)
			{
				client_print_color(id, id, "%s Сперва выключите бокс!", g_szChatPrefix);
				return Show_BoxingMenu(id);
			}
			return Show_MiniGameMenu(id);
		}
		case 9: return PLUGIN_HANDLED;
	}
	return Show_BoxingMenu(id);
}