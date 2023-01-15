// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8


Show_HookMenu(id)
{
	if (IsNotSetBit(g_iBitUserHook, id)) return PLUGIN_HANDLED;
	
	menu_title("\wНастройки хука^n^n");
	menu_item("\y1. \wМеню хука \r[\y%s\r]^n", IsSetBit(g_iBitHookMenu, id) ? "Вкл" : "Выкл");
	menu_item("\y2. \wПолет \r[\y%s\r]^n", IsSetBit(g_iBitNewHook, id) ? "New" : "Old");
	menu_item("\y3. \wСкорость \r[\y%s\r]^n", hook_speed_name(id));
	menu_item("\y4. \wЦвет \r[\y%s\r]^n", hook_color_name(id));
	menu_item("\y5. \wШирина \r[\y%d\r]^n", hook_width_num(id));
	menu_item("\y6. \wРазмах \r[\y%d\r]^n", hook_amplitude(id));
	
	menu_item("\y7. \wТраил цвет \r[\y%s\r]^n",
		g_iszHookColour[g_eHookSetting[id][HOOK_TRAIL_COLOUR]][3]);	
	menu_item("\y8. \wТраил ширина \r[\y%d\r]^n",
		g_iHookTrailWidth[g_eHookSetting[id][HOOK_TRAIL_WIDTH]]);
	menu_item("\y9. \wТраил длина \r[\y%d\r]^n",
		g_iHookTrailLength[g_eHookSetting[id][HOOK_TRAIL_LENGTH]]);
		
	menu_item("^n\d0. \w%L", id, "JBE_MENU_EXIT");

	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Show_HookMenu");
}

public Handle_HookMenu(id, iKey)
{
	switch (iKey)
	{
		case 0:
		{
			InvertBit(g_iBitHookMenu, id);
			if (IsSetBit(g_iBitHookMenu, id))
			{
				return Show_HookMenu(id);
			}
			else
			{
				client_print_color(id, print_team_blue,
					"%s ^1Вы ^3выключили ^1меню хука ^3(^1введите в чат ^4/hook ^1чтоб открыть меню^3)^1.", g_szChatPrefix);
				return PLUGIN_HANDLED;
			}
		}
		case 1:
		{
			InvertBit(g_iBitNewHook, id);
			if (task_exists(id + TASK_HOOK_THINK))
			{
				ClCmd_HookOff(id);
				ClCmd_HookOn(id);
			}
		}
		case 2: g_eHookSetting[id][HOOK_SPEED] = ((g_eHookSetting[id][HOOK_SPEED] + 1) % g_eHookSettingSize[HOOK_SPEED]);	
		case 3:
		{
			g_eHookSetting[id][HOOK_COLOUR] = ((g_eHookSetting[id][HOOK_COLOUR] + 1) % g_eHookSettingSize[HOOK_COLOUR]);
			
			if (g_eHookSetting[id][HOOK_COLOUR])
				UTIL_ScreenFade(id, 
					.iDuration = (1<<10),
					.iHoldTime = (1<<10),
					.iFlags = 0,
					.iRed = g_iszHookColour[g_eHookSetting[id][HOOK_COLOUR]][0],
					.iGreen = g_iszHookColour[g_eHookSetting[id][HOOK_COLOUR]][1],
					.iBlue = g_iszHookColour[g_eHookSetting[id][HOOK_COLOUR]][2],
					.iAlpha = 200,
					.iReliable = 0);
		}
		case 4: g_eHookSetting[id][HOOK_WIDTH] = ((g_eHookSetting[id][HOOK_WIDTH] + 1) % g_eHookSettingSize[HOOK_WIDTH]);
		case 5: g_eHookSetting[id][HOOK_AMPLITUDE] = ((g_eHookSetting[id][HOOK_AMPLITUDE] + 1) % g_eHookSettingSize[HOOK_AMPLITUDE]);
		case 6:
		{
			g_eHookSetting[id][HOOK_TRAIL_COLOUR] = ((g_eHookSetting[id][HOOK_TRAIL_COLOUR] + 1) % g_eHookSettingSize[HOOK_COLOUR]);
			
			if (g_eHookSetting[id][HOOK_COLOUR])
				UTIL_ScreenFade(id, 
					.iDuration = (1<<10),
					.iHoldTime = (1<<10),
					.iFlags = 0,
					.iRed = g_iszHookColour[g_eHookSetting[id][HOOK_TRAIL_COLOUR]][0],
					.iGreen = g_iszHookColour[g_eHookSetting[id][HOOK_TRAIL_COLOUR]][1],
					.iBlue = g_iszHookColour[g_eHookSetting[id][HOOK_TRAIL_COLOUR]][2],
					.iAlpha = 200,
					.iReliable = 0);
		}
		case 7: g_eHookSetting[id][HOOK_TRAIL_WIDTH] = ((g_eHookSetting[id][HOOK_TRAIL_WIDTH] + 1) % g_eHookSettingSize[HOOK_TRAIL_WIDTH]);
		case 8: g_eHookSetting[id][HOOK_TRAIL_LENGTH] = ((g_eHookSetting[id][HOOK_TRAIL_LENGTH] + 1) % g_eHookSettingSize[HOOK_TRAIL_LENGTH]);
		case 9: return PLUGIN_HANDLED;
	}
	return Show_HookMenu(id);
}