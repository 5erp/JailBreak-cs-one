// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

Show_GrabGlowMenu(id)
{
	if (is_not_use_grab_glow_menu(id)) return PLUGIN_HANDLED;
	new iKeys = (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7);

	menu_title("\r[\yGrab\r] \wВыставить свечение^n^n");
	menu_item("\y1. \wУбрать^n");
	menu_item("\y2. \wЗеленый^n");
	menu_item("\y3. \wКрасный^n");
	menu_item("\y4. \wСиний^n");
	menu_item("\y5. \wЖелтый^n");
	menu_item("\y6. \wФиолетовый^n");
	menu_item("\y7. \wОранжевый^n");
	menu_item("\y8. \wСерый^n");

	if (IsSetBit(g_iBitGrabUse, id))
	{
		menu_item("^n\y%-28s[ЛКМ] - Slay %d HP", "[E] - Grab Menu", GRAB_CHOKE_DMG);
		menu_item("^n\y%-27s[ПКМ] - 100 Health", "[R] - Off Grab Menu");
	}
	else // Открыто через fast admin menu
	{
		menu_item("^n\y9. \wFast Admin Menu^n");
		menu_item("\y0. \wВыход");
		iKeys |= (1<<8|1<<9);
	}
	return menu_show(id, iKeys, "Show_GrabGlowMenu");
}

public Handle_GrabGlowMenu(id, iKey)
{
	if (is_not_use_grab_glow_menu(id))
		return PLUGIN_HANDLED;
	
	new iTarget;
	if (IsSetBit(g_iBitGrabUse, id))
		iTarget = g_iGrabData[id][GRABBED];
	else
		iTarget = g_iMenuTarget[id];

	switch (iKey)
	{
		case 0:
		{
			jbe_set_user_rendering(iTarget, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
			jbe_save_user_rendering(iTarget);
			client_print_color(0, iTarget, "%s ^4%n ^1убрал свечение ^3%n^1.", g_szGrabPrefix, id, iTarget);
		}
		case 8: return Show_GrabAdminMenu(id);
		case 9: return PLUGIN_HANDLED;
		default:
		{
			jbe_set_user_rendering(iTarget, kRenderFxGlowShell,
				g_iGrabMenuGlows[iKey][0],
				g_iGrabMenuGlows[iKey][1],
				g_iGrabMenuGlows[iKey][2],
				kRenderNormal, 30);
			jbe_save_user_rendering(iTarget);
			client_print_color(0, iTarget, "%s ^4%n ^1выставил ^4%s ^1свечение ^3%n^1.", g_szGrabPrefix, id, g_iGrabMenuGlows[iKey][3], iTarget);
		}
	}
	return Show_GrabGlowMenu(id);
}