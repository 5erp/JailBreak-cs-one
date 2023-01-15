// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8
// Новое граб меню


Show_GrabMenu(id) {
	if (IsNotSetBit(g_iBitUserGrab, id)) return PLUGIN_HANDLED;
	new iKeys = (1<<0|1<<4|1<<5);
	
	menu_title("\wМеню граба^n");
	
	if (jbe_is_user_valid(g_iGrabData[id][GRABBED]))
		menu_item("\yВы держите: \r%n^n^n", g_iGrabData[id][GRABBED]);
	else menu_item("\yВы держите: \rОружие^n^n");
	
	menu_item("\y1. \w%s^n",
		jbe_is_user_valid(g_iGrabData[id][GRABBED]) ? "Убить" : "Удалить");
	
	
	if (~(g_iGrabData[id][GRAB_FLAGS] & CDF_IN_PULL)) {
		menu_item("\y2. \wПритягивать^n");
		iKeys |= (1<<1);
	} else menu_item("\y2. \dПритягивать^n");
	
	if (~(g_iGrabData[id][GRAB_FLAGS] & CDF_IN_PUSH)) {
		menu_item("\y3. \wОтдалять^n");
		iKeys |= (1<<2);
	} else menu_item("\y3. \dОтдалять^n");

	if (g_iGrabData[id][GRAB_FLAGS] & (CDF_IN_PUSH|CDF_IN_PULL)) {
		menu_item("\y4. \wОстановить^n");
		iKeys |= (1<<3);
	} else menu_item("\y4. \dОстановить^n");

	if (jbe_is_user_valid(g_iGrabData[id][GRABBED])) 
	{
		if (IsSetBit(g_iBitUserAdmin, id)) 
		{
			menu_item("\y5. \wВылечить^n");
			menu_item("\y6. \wУдарить %d HP^n", GRAB_CHOKE_DMG);
			iKeys |= (1<<4|1<<5);
		}
	} menu_item("^n^n");

	if (jbe_is_user_valid(g_iGrabData[id][GRABBED]))
	{
		if (IsSetBit(g_iBitUserAdmin, id))
		{
			menu_item("^n\y%-28s[ЛКМ] - Притянуть", "[E] - Admin Grab Menu");
			menu_item("^n\y%-32s[ПКМ] - Отдалить", "[R] - Skin Menu");
		} 
		else 
		{
			menu_item("^n\y-%28s[ЛКМ] - Притянуть", "[E] - Off Grab Menu");
			menu_item("^n\y%-32s[ПКМ] - Отдалить", "");
		}
	} 
	else
	{
		menu_item("^n\y%-28s[ЛКМ] - Притянуть", "[E] - Off Grab Menu");
		menu_item("^n\y%-37s[ПКМ] - Отдалить", "");
	}

	return menu_show(id, iKeys, "Show_GrabMenu");
}

public Handle_GrabMenu(id, iKey)
{
	jbe_informer_offset_down(id);
	switch (iKey)
	{
		case 0:
		{
			if (jbe_is_user_valid(g_iGrabData[id][GRABBED]))
			{
				client_print_color(0, g_iGrabData[id][GRABBED], "%s ^4%n ^1убил ^3%n^1.", g_szGrabPrefix,
					id, g_iGrabData[id][GRABBED]);
				user_kill(g_iGrabData[id][GRABBED], 1); //ExecuteHamB(Ham_Killed, g_iGrabData[id][GRABBED], g_iGrabData[id][GRABBED], 0);
			}
			else
			{
				if (pev_valid(g_iGrabData[id][GRABBED]) == 2)
				{
					client_print_color(0, print_team_blue, "%s ^4%n ^1удалил ^3Оружие^1.", g_szGrabPrefix, id);
					engfunc(EngFunc_RemoveEntity, g_iGrabData[id][GRABBED]);
				}
				else client_print_color(id, 0, "%s ^1Что-то с оружием не так..", g_szGrabPrefix);
			}
			return ClCmd_GrabOff(id);
		}
		case 1:
		{
			g_iGrabData[id][GRAB_FLAGS] ^= CDF_IN_PULL;
			g_iGrabData[id][GRAB_FLAGS] &= ~CDF_IN_PUSH;
		}
		case 2:
		{
			g_iGrabData[id][GRAB_FLAGS] ^= CDF_IN_PUSH;
			g_iGrabData[id][GRAB_FLAGS] &= ~CDF_IN_PULL;
		}
		case 3: g_iGrabData[id][GRAB_FLAGS] &= ~(CDF_IN_PULL|CDF_IN_PUSH);
		case 4: grab_menu_key_health(id);// ClCmd_GrabPull_2(id);
		case 5: grab_menu_key_damage(id);// ClCmd_GrabPush_2(id);
	}
	return Show_GrabMenu(id);
}

