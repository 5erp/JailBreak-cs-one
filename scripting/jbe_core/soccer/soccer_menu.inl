// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

soccer_menu_init()
{
	register_menucmd(register_menuid("Show_SoccerMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_SoccerMenu");
		
	register_menucmd(register_menuid("Show_SoccerTeamMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<8|1<<9), "Handle_SoccerTeamMenu");
		
	register_menucmd(register_menuid("Show_SoccerScoreMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<8|1<<9), "Handle_SoccerScoreMenu");

	register_menucmd(register_menuid("Show_BallMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_BallMenu");
}

Show_BallMenu(id)
{	
	new iKeys = (1<<0|1<<2|1<<9);
	menu_title("Меню мяча^n^n");

	menu_item("\y1. \w%s мяч^n", g_iSoccerBall ? "Удалить" : "Создать");

	if (g_iSoccerBall)
	{
		menu_item("\y2. \wВернуть на место^n");
		if (!g_bSoccerStatus)
		{
			menu_item("\y3. \wРежим: %s^n^n", g_szBallTypes[g_iBallType]);
			iKeys |= (1<<2);
		}
		else menu_item("\d3. Режим: %s^n^n", g_szBallTypes[g_iBallType]);
		menu_item("\yСкорость мяча: \r%d^n", g_iBallSpeed);
		if (g_iBallSpeed < 50)
		{
			menu_item("\y5. \wУвеличить^n");
			iKeys |= (1<<4);
		}
		else menu_item("\d5. Увеличить^n");
		if (g_iBallSpeed > 1)
		{
			menu_item("\y6. \wУменьшить^n");
			iKeys |= (1<<5);
		}
		else menu_item("\d6. Уменьшить^n");
		if (g_iBallSpeed != 10)
		{
			menu_item("\y7. \wСтандартная^n");
			iKeys |= (1<<6);
		}
		else menu_item("\d7. Стандартная^n");
		iKeys |= (1<<1);
	}
	else
	{
		menu_item("\d2. Вернуть на место^n");
		if (!g_bSoccerStatus)
		{
			menu_item("\y3. \wРежим: %s^n^n", g_szBallTypes[g_iBallType]);
			iKeys |= (1<<2);
		}
		else menu_item("\d3. Режим: %s^n^n", g_szBallTypes[g_iBallType]);
		menu_item("\yСкорость мяча: \r%d^n", g_iBallSpeed);
		menu_item("\d5. Увеличить^n");
		menu_item("\d6. Уменьшить^n");
		menu_item("\d7. Стандартная^n");
	}

	menu_item("^n^n\y0. \wВыход");

	return menu_show(id, iKeys, "Show_BallMenu");
}

public Handle_BallMenu(id, iKey)
{
	if (iKey == 9) return PLUGIN_HANDLED;
	if (g_bSoccerGame && id != g_iChiefId)
	{
		client_print_color(id, id, "%s ^1Идет футбол, ^3создать^1/^3удалить ^1мяч может только ^4Начальник.", g_szChatPrefix);
		return PLUGIN_HANDLED;
	}
	
	switch (iKey)
	{
		case 0: //jbe_minigame_ball(id);
		{
			if (g_iSoccerBall)
			{
				jbe_info_center_msg(0, "%s %n удалил мяч", g_iChiefId == id ? "Начальник" : "Админ", id);
				jbe_soccer_remove_ball();
				g_bAllUseBall = false;
			}
			else
			{
				jbe_info_center_msg(0, "%s %n создал %s мяч", g_iChiefId == id ? "Начальник" : "Админ", id, g_szBallTypes[g_iBallType]);
				jbe_soccer_create_ball(id);
				g_bAllUseBall = true;
			}
		}
		case 1:
		{
			jbe_info_center_msg(0, "%s %n вернул мяч", g_iChiefId == id ? "Начальник" : "Админ", id);
			jbe_soccer_update_ball();
		}
		case 2:
		{
			g_iBallType = ((g_iBallType + 1) % 3);
			if (g_iSoccerBall)
			{
				jbe_info_center_msg(0, "%s %n сменил мяч на %s", g_iChiefId == id ? "Начальник" : "Админ", id, g_szBallTypes[g_iBallType]);
				jbe_soccer_update_ball();
			}
		}
		case 4:
		{
			if (g_iBallSpeed >= 50) return Show_BallMenu(id);

			g_iBallSpeed++;
			jbe_info_center_msg(0, "%s %n увеличил скорость мяча до %d", g_iChiefId == id ? "Начальник" : "Админ", id, g_iBallSpeed);
		}
		case 5:
		{
			if (g_iBallSpeed <= 1) return Show_BallMenu(id);

			g_iBallSpeed--;
			jbe_info_center_msg(0, "%s %n уменьшил скорость мяча до %d", g_iChiefId == id ? "Начальник" : "Админ", id, g_iBallSpeed);
		}
		case 6:
		{
			if (g_iBallSpeed == 10) return Show_BallMenu(id);
			
			g_iBallSpeed = 10;
			jbe_info_center_msg(0, "%s %n сбросил скорость мяча", g_iChiefId == id ? "Начальник" : "Админ", id);
		}
	}
	return Show_BallMenu(id);
}


Show_SoccerMenu(id)
{
	if (g_iDayMode != DAY_NORMAL || id != g_iChiefId || IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;
		
	new iKeys = (1<<0|1<<1|1<<6|1<<8|1<<9);
	
	menu_title("Управление футболом^n^n");
	
	if (g_bSoccerStatus)
		menu_item("\y1. \w%s мяч^n", g_iSoccerBall ? "Убрать" : "Поставить");
	else menu_item("\y1. \dПоставить мяч^n");
	
	if (!g_bSoccerGame && g_iSoccerBall)
		menu_item("\y2. \wНачать матч^n^n");
	else if (g_bSoccerBallTouch)
		menu_item("\y2. \wВернуть мяч на место^n^n");
	else if (g_iSoccerBall)
		menu_item("\y2. \wДать свисток^n^n");
	else menu_item("\y2. \dНачать матч^n^n");
	
	if (g_bSoccerGame) {
		menu_item("\y3. \wДобавить гол красным^n");
		menu_item("\y4. \wДобавить гол синим^n");
		iKeys |= (1<<2|1<<3);
	} else {
		menu_item("\y3. \dДобавить гол красным^n");
		menu_item("\y4. \dДобавить гол синим^n");
	}
	
	if (g_iSoccerScore[0]) {
		menu_item("\y5. \wЗабрать гол у красных^n");
		iKeys |= (1<<4);
	} else menu_item("\y5. \dЗабрать гол у красных^n");
	
	if (g_iSoccerScore[1]) {
		menu_item("\y6. \wЗабрать гол у синих^n^n");
		iKeys |= (1<<5);
	} else menu_item("\y6. \dЗабрать гол у синих^n^n");
	
	if (g_bSoccerGame) { // футбол активен идет игра
		menu_item("\y7. \dУправление командами^n");
		menu_item("\y8. \wВыключить футбол^n");
		iKeys |= (1<<7);
	} else if (g_bSoccerStatus) { // Включен футбол
		menu_item("\y7. \wУправление командами^n");
		menu_item("\y8. \wВыключить футбол^n");
		iKeys |= (1<<7);
	} else {
		menu_item("\y7. \wУправление командами^n");
		menu_item("\y8. \dВыключить футбол^n");
	}
	
	menu_item("^n\y9. \w%L", id, "JBE_MENU_BACK");	
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");

	return menu_show(id, iKeys, "Show_SoccerMenu");
}
public Handle_SoccerMenu(id, iKey)
{
	if (g_iDayMode != DAY_NORMAL || id != g_iChiefId || IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;

	switch (iKey) {
		case 0: {
			if (g_bSoccerStatus)
			{
				g_iBallType = BT_FOOTBALL;
				g_iBallSpeed = 10;
				g_iSoccerBall ? jbe_soccer_remove_ball() : jbe_soccer_create_ball(id);
			}
			else
			{
				client_print_color(id, id, "%s Поделите игроков на команды!", g_szChatPrefix);
				return Show_SoccerTeamMenu(id);
			}
		}
		case 1: {
			if (!g_bSoccerGame && g_iSoccerBall)
				return jbe_soccer_game_start(id);
			else if (g_iSoccerBall && g_bSoccerBallTouch) {
				jbe_soccer_update_ball();
				rg_send_audio(0, sound_whitle_start);
			} else if (g_iSoccerBall) {
				rg_send_audio(0, sound_whitle_start);
				g_bSoccerBallTouch = true;
			} else client_print_color(id, id, "%s Поставьте мяч!", g_szChatPrefix);
		}
		case 2: {
			g_iSoccerScore[0]++;
			client_print_color(0, print_team_red, "%s ^1%s ^4%n ^1добавил ^4+1 ^3красным.", 
				g_szChatPrefix, id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id);
		}
		case 3: {
			g_iSoccerScore[1]++;
			client_print_color(0, print_team_blue, "%s ^1%s ^4%n ^1добавил ^4+1 ^3синим.", 
				g_szChatPrefix, id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id);
		}
		case 4: {
			g_iSoccerScore[0]--;
			client_print_color(0, print_team_red, "%s ^1%s ^4%n ^1отнял ^4-1 ^3красным.", 
				g_szChatPrefix, id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id);
		}
		case 5: {
			g_iSoccerScore[1]--;
			client_print_color(0, print_team_blue, "%s ^1%s ^4%n ^1отнял ^4-1 ^3синим.", 
				g_szChatPrefix, id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id);
		}
		case 6: return Show_SoccerTeamMenu(id);
		case 7: if (g_bSoccerGame || g_bSoccerStatus) jbe_soccer_disable_all();
		case 8: {
			if (g_bSoccerStatus) {
				client_print_color(id, id, "%s Сперва выключите футбол!", g_szChatPrefix);
				return Show_SoccerMenu(id);
			}
			return Show_MiniGameMenu(id);
		}
		case 9: return PLUGIN_HANDLED;
	}
	return Show_SoccerMenu(id);
}

Show_SoccerTeamMenu(id)
{
	if (g_iDayMode != DAY_NORMAL || id != g_iChiefId || IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;
	menu_title("Управление командами^n^n");
	menu_item("\y1. \wПоделить заключенных^n");
	menu_item("\y2. \wПоделить всех (и КТ)^n");
	menu_item("^n\dДелить по прицелу:^n");
	menu_item("\y3. \wДобавить за красных^n");
	menu_item("\y4. \wДобавить за синих^n");
	menu_item("\y5. \wУдалить^n");
	menu_item("^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<8|1<<9), "Show_SoccerTeamMenu");
}

public Handle_SoccerTeamMenu(id, iKey)
{
	if (g_iDayMode != DAY_NORMAL || id != g_iChiefId || IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 0:
		{
			jbe_soccer_divide_team(1);
			client_print_color(id, print_team_blue, "%s ^1Теперь ^4поставьте ^3мяч ^1и начните ^4матч^1!", g_szChatPrefix);
			return Show_SoccerMenu(id);
		}
		case 1:
		{
			jbe_soccer_divide_team(0);
			client_print_color(id, print_team_blue, "%s ^1Теперь ^4поставьте ^3мяч ^1и начните ^4матч^1!", g_szChatPrefix);
			return Show_SoccerMenu(id);
		}
		case 2, 3: jbe_soccer_set_team(id, iKey - 2);
		case 4: jbe_soccer_set_team(id, -1);
		case 8: return Show_SoccerMenu(id);
		case 9: return PLUGIN_HANDLED;
	}
	return Show_SoccerTeamMenu(id);
}


Show_SoccerScoreMenu(id)
{
	if (g_iDayMode != DAY_NORMAL || (id != g_iChiefId && IsNotSetBit(g_iBitUserAdmin, id))) return PLUGIN_HANDLED;
	new iKeys = (1<<5|1<<8|1<<9);
	menu_title("\wУправление счётом^n^n");
	if (task_exists(TASK_SHOW_SOCCER_SCORE)) {
		menu_item("\y1. \wДобавить гол красным^n");
		if (g_iSoccerScore[0]) {
			menu_item("\y2. \wЗабрать гол у красных^n");
			iKeys |= (1<<1);
		} else menu_item("\y2. \dЗабрать гол у красных^n");
		menu_item("\y3. \wДобавить гол синим^n");
		if (g_iSoccerScore[1]) {
			menu_item("\y4. \wЗабрать гол у синих^n");
			iKeys |= (1<<3);
		} else menu_item("\y4. \dЗабрать гол у синих^n");
		menu_item("\y5. \wОбнулить счёт^n");
		menu_item("\y6. \wВыключить счёт^n^n^n");
		iKeys |= (1<<0|1<<2|1<<4);
	} else {
		menu_item("\y1. \dДобавить гол красным^n");
		menu_item("\y2. \dЗабрать гол у красных^n");
		menu_item("\y3. \dДобавить гол синим^n");
		menu_item("\y4. \dЗабрать гол у синих^n");
		menu_item("\y5. \dОбнулить счёт^n");
		menu_item("\y6. \wВключить счёт^n^n^n");
	}
	if (g_iChiefId == id)
		menu_item("^n\y9. \w%L", id, "JBE_MENU_BACK");
	
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	return menu_show(id, iKeys, "Show_SoccerScoreMenu");
}

public Handle_SoccerScoreMenu(id, iKey)
{
	if (g_iDayMode != DAY_NORMAL || (id != g_iChiefId && IsNotSetBit(g_iBitUserAdmin, id)))
		return PLUGIN_HANDLED;
	switch (iKey) {
		case 0: {
			g_iSoccerScore[0]++;
			client_print_color(0, print_team_red, "%s %s ^4%n ^1добавил ^4+1 ^3красным.", g_szChatPrefix, 
				id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id);
		}
		case 1: {
			g_iSoccerScore[0]--;
			client_print_color(0, print_team_red, "%s %s ^4%n ^1отнял ^4-1 ^3красным.", g_szChatPrefix, 
				id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id);
		}
		case 2: {
			g_iSoccerScore[1]++;
			client_print_color(0, print_team_blue, "%s %s ^4%n ^1добавил ^4+1 ^3синим.", g_szChatPrefix, 
				id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id);
		}
		case 3: {
			g_iSoccerScore[1]--;
			client_print_color(0, print_team_blue, "%s %s ^4%n ^1отнял ^4-1 ^3синим.", g_szChatPrefix, 
				id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id);
		}
		case 4: {
			g_iSoccerScore = {0, 0};
			client_print_color(0, print_team_grey, "%s %s ^4%n ^1обнулил ^3счет^1.", g_szChatPrefix, 
				id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id);
		}
		case 5:
		{
			if (task_exists(TASK_SHOW_SOCCER_SCORE))
				remove_task(TASK_SHOW_SOCCER_SCORE);
			else
				set_task(1.0, "jbe_score_informer", TASK_SHOW_SOCCER_SCORE, _, _, "b");
			
			client_print_color(0, print_team_grey, "%s %s ^4%n ^3%s ^1счет.", g_szChatPrefix, 
				id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix,
				id, task_exists(TASK_SHOW_SOCCER_SCORE) ? "включил" : "выключил");
		}
		case 8: return Show_SoccerMenu(id);
		case 9: return PLUGIN_HANDLED;
	}
	return Show_SoccerScoreMenu(id);
}