Show_GrabSkinMenu(id)
{
	if (is_not_use_grab_skin_menu(id))
		return PLUGIN_HANDLED;

	new iKeys = (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7);

	menu_title("\r[\yGrab\r] \wВыставить скин^n^n");
	menu_item("\y1. \wОранжевый^n");
	menu_item("\y2. \wСерый^n");
	menu_item("\y3. \wЖелтый^n");
	menu_item("\y4. \wСиний^n");
	menu_item("\y5. \wКоричневый^n");
	menu_item("\y6. \wЧерный^n");
	menu_item("\y7. \wЗеленый^n");
	menu_item("\y8. \wКрасный^n");

	if (IsSetBit(g_iBitGrabUse, id))
	{
		menu_item("^n\y%-28s[ЛКМ] - Slay %d HP", "[E] - Grab Menu", GRAB_CHOKE_DMG);
		menu_item("^n\y%-27s[ПКМ] - 100 Health", "[R] - Glow Menu");
	}
	else // Открыто через fast admin menu
	{
		menu_item("\y9. \wGlow Menu^n");
		menu_item("\y0. \wВыход");
		iKeys |= (1<<8|1<<9);
	}
	return menu_show(id, iKeys, "Show_GrabSkinMenu");
}

public Handle_GrabSkinMenu(id, iKey)
{
	if (is_not_use_grab_skin_menu(id))
		return PLUGIN_HANDLED;
	
	switch (iKey)
	{
		case 8: return Show_GrabGlowMenu(id);
		case 9: return PLUGIN_HANDLED;
		default:
		{
			new iTarget;
			if (IsSetBit(g_iBitGrabUse, id))
				iTarget = g_iGrabData[id][GRABBED];
			else
				iTarget = g_iMenuTarget[id];

			jbe_set_player_prisoner_model(iTarget);
			set_entvar(iTarget, var_skin, iKey);
			
			new szLangPlayer[][] = {
				"оранжевую", "серую", "жёлтую", "синюю",
				"коричневую", "черную", "зеленую", "красную"
			};

			client_print_color(0, iTarget, "%s ^4%n ^1Выдал %s одежду ^3%n^1.", 
				g_szGrabPrefix, id, szLangPlayer[iKey], iTarget);
		}
	}
	return Show_GrabSkinMenu(id);
}