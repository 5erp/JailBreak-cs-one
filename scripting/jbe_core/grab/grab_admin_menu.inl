// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

/*
	Убить
	Перевести
	
	Обезоружить
	Полечить
	Заморозить
	Закопать
	
	Выходной
	Розыск
*/

Show_GrabAdminMenu(id, bool:bGrab = false) // bGrab - вызов через команду false, через граб true
{
	new iTarget;
	if (bGrab)
		iTarget = g_iGrabData[id][GRABBED];
	else
	{
		new iBody; // g_iMenuTarget[id] = iTarget;
		get_user_aiming(id, iTarget, iBody); // (id, iTarget, iBody, 9999);
		if (!jbe_is_user_valid(iTarget))
		{
			client_print_color(id, print_team_blue, 
				"%s ^1Забиндить меню, в консоль: ^4bind ^3button ^4fam ^1(вместо ^3button ^1любая кнопка).", g_szChatPrefix);
			client_print_color(id, print_team_blue,
				"%s ^1Наведите ^4прицел ^1на игрока и ^3вызовите меню^1.", g_szChatPrefix);
			
			return PLUGIN_HANDLED;
		}
	}
	
	g_iMenuTarget[id] = iTarget;
	
	new iKeys = (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<9);
	
	menu_title("\w%s Admin Menu^n\yИгрок\d: \r%n^n^n", bGrab ? "Grab" : "Fast", iTarget);
	
	menu_item("\y1. \wУбить^n");
	menu_item("\y2. \wПеревести^n");
	
	if (g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY)
	{
		menu_item("\y3. \wОбезоружить^n");
		menu_item("\y4. \wПолечить^n");

		menu_item("\y5. \w%s^n",
			IsSetBit(g_iBitUserFrozen, iTarget) ? "Разморозить" : "Заморозить");
			
		menu_item("\y6. \w%s^n",
			IsSetBit(g_iBitUserBuried, iTarget) ? "Откопать" : "Закопать");
			
		if (g_iUserTeam[iTarget] == TEAM_PRISONER)
		{
			if (IsNotSetBit(g_iBitUserWanted, iTarget))
			{
				menu_item("\y7. \w%s выходной^n",
					IsSetBit(g_iBitUserFree, iTarget) ? "Забрать" : "Выдать");
				iKeys |= (1<<6);
			}
			else menu_item("\y7. \dВыдать выходной^n");

			menu_item("\y8. \w%s розыск^n",
				IsSetBit(g_iBitUserWanted, iTarget) ? "Забрать" : "Выдать");
			iKeys |= (1<<7);
		}
		else menu_item("^n^n");


		
		if (IsSetBit(g_iBitGrabUse, id))
		{
			menu_item("^n\y%-28s[ЛКМ] - Slay %d HP", "[E] - Off Grab Menu", GRAB_CHOKE_DMG);
			menu_item("^n\y%-29s[ПКМ] - 100 Health", "[R] - Skin Menu");
		}
		else
		{
			menu_item("\y9. \wSkin | Glow Menu^n");
			iKeys |= (1<<8);
		}
	}
	else
	{
		menu_item("\y3. \dОбезоружить^n");
		menu_item("\y4. \dПолечить^n");
		menu_item("\y5. \dЗаморозить^n");
		menu_item("\y6. \dЗакопать^n");
		menu_item("^n^n");
	}

	if (!bGrab)
		menu_item("^n\d0. \w%L", id, "JBE_MENU_EXIT");

	return menu_show(id, iKeys, "Show_GrabAdminMenu");
	
}
public Handle_GrabAdminMenu(id, iKey) {
	jbe_informer_offset_down(id);
	new iTarget = g_iMenuTarget[id];
	
	if (IsNotSetBit(g_iBitUserConnected, iTarget)) {
		client_print_color(id, id, "%s ^1Игрок отключился.", g_szChatPrefix);
		return PLUGIN_HANDLED;
	} else if (IsNotSetBit(g_iBitUserAlive, iTarget)) {
		client_print_color(id, iTarget, "%s ^1Игрок ^3%n ^1мертв.", g_szChatPrefix, iTarget);
		return PLUGIN_HANDLED;
	}
	
	switch (iKey) {
		case 0: {
			client_print_color(0, iTarget, "%s %s ^4%n ^1убил ^3%n^1.", g_szChatPrefix,
				g_szAdminPrefix, id,  iTarget);
			user_kill(iTarget, 1); //ExecuteHamB(Ham_Killed, iTarget, iTarget, 0);
		}
		case 1: {
			jbe_set_user_team(iTarget, g_iUserTeam[iTarget] == TEAM_GUARD ? TEAM_PRISONER : TEAM_GUARD);
			
			client_print_color(0, iTarget, "%s %s ^4%n ^1перевел ^3%n ^1за ^3%s^1.",
				g_szGrabPrefix, g_szAdminPrefix, id,
				iTarget, g_iUserTeam[iTarget] == TEAM_GUARD ? "охрану" : "заключенных");
		}
		case 2:
		{
			if (g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY)
			{
				rg_remove_all_items(iTarget);
				rg_give_item(iTarget, "weapon_knife");
				client_print_color(0, iTarget, "%s %s ^4%n ^1забрал все оружие у ^3%n^1.",
					g_szChatPrefix, g_szAdminPrefix, id, iTarget);
			}
			else
			{
				client_print_color(id, id, "%s Сейчас это действие недоступно!", g_szChatPrefix);
				return PLUGIN_HANDLED;
			}
		}
		case 3: {
			if (g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY) {
				if (pev(iTarget, pev_health) < 100.0) {
					set_pev(iTarget, pev_health, 100.0);
					client_print_color(0, iTarget, "%s %s ^4%n ^1вылечил ^3%n^1.",
						g_szChatPrefix,	g_szAdminPrefix, id, iTarget);
				} else
					client_print_color(id, iTarget, "%s ^1У игрока ^3%n ^1полные жизни.",
						g_szChatPrefix, iTarget);
			} else {
				client_print_color(id, id, "%s Сейчас это действие недоступно!", g_szChatPrefix);
				return PLUGIN_HANDLED;
			}
		}
		case 4: {
			if (g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY) {
				IsSetBit(g_iBitUserFrozen, iTarget) 
				? set_pev(iTarget, pev_flags, pev(iTarget, pev_flags) & ~FL_FROZEN) 
				: set_pev(iTarget, pev_flags, pev(iTarget, pev_flags) | FL_FROZEN);
				InvertBit(g_iBitUserFrozen, iTarget);

				if (IsSetBit(g_iBitUserFrozen, iTarget))
					client_print_color(0, iTarget, "%s %s ^4%n ^1заморозил ^3%n^1.",
						g_szChatPrefix, g_szAdminPrefix, id, iTarget);
				else
					client_print_color(0, iTarget, "%s %s ^4%n ^1разморозил ^3%n^1.",
						g_szChatPrefix, g_szAdminPrefix, id,  iTarget);
				
			} else {
				client_print_color(id, id, "%s Сейчас это действие недоступно!", g_szChatPrefix);
				return PLUGIN_HANDLED;
			}
		}
		case 5: {
			if (g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY) {
				if (IsSetBit(g_iBitUserBuried, iTarget)) {
					if (!jbe_user_buried(iTarget, false)) { // откапываем
						client_print_color(id, id, "%s Игрок мертв..", IsSetBit(g_iBitGrabUse, id) ? g_szGrabPrefix : g_szChatPrefix);
						return PLUGIN_HANDLED;
					}

					client_print_color(0, iTarget, "%s %s ^4%n ^1откопал ^3%n^1.", 
						g_szChatPrefix, g_szAdminPrefix, id, iTarget);
				}
				else
				{
					if (!jbe_user_buried(iTarget, true)) { // закапываем
						client_print_color(id, id, "%s Игрок должен cтоять на земле!", IsSetBit(g_iBitGrabUse, id) ? g_szGrabPrefix : g_szChatPrefix);
						return PLUGIN_HANDLED;
					}

					client_print_color(0, iTarget, "%s %s ^4%n ^1закопал ^3%n^1.", 
						g_szChatPrefix, g_szAdminPrefix, id, iTarget);
				}
	
				
			} else {
				client_print_color(id, id, "%s Сейчас это действие недоступно!", g_szChatPrefix);
			}
		}
		case 6: {
			if (g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY) {
				IsSetBit(g_iBitUserFree, iTarget) ? jbe_sub_user_free(iTarget) : jbe_add_user_free(iTarget);
				
				if (IsSetBit(g_iBitUserFree, iTarget))
					client_print_color(0, iTarget, "%s %s ^4%n ^1выдал выходной ^3%n^1.", 
						g_szChatPrefix, g_szAdminPrefix, id,  iTarget);
				else
					client_print_color(0, iTarget, "%s %s ^4%n ^1забрал выходной ^3%n^1.", 
						g_szChatPrefix, g_szAdminPrefix, id, iTarget);
			}
			else
				return client_print_color(id, id, "%s Сейчас это действие недоступно!", g_szChatPrefix);
		}
		case 7: {
			if (g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY)
			{
				IsSetBit(g_iBitUserWanted, iTarget) ? jbe_sub_user_wanted(iTarget) : jbe_add_user_wanted(iTarget);
				
				if (IsSetBit(g_iBitUserWanted, iTarget))
					client_print_color(0, iTarget, "%s %s ^4%n ^1выдал розыск ^3%n^1.", 
						g_szChatPrefix, g_szAdminPrefix, id, iTarget);
				else
					client_print_color(0, iTarget, "%s %s ^4%n ^1забрал розыск ^3%n^1.", 
						g_szChatPrefix, g_szAdminPrefix, id, iTarget);
			}
			else
				return client_print_color(id, id, "%s Сейчас это действие недоступно!", g_szChatPrefix);
		}
		case 8: return Show_GrabSkinMenu(id);
	}
	return PLUGIN_HANDLED;
}

