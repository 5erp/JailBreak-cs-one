get_user_setting_sql(id, szSetting[], iLen)
{
	formatex(szSetting, iLen,
		"%d %d %d %d \
		%d %d %d %d %d %d \
		%d %d %d %d %d %d %d \
		%d %d \
		%d %d",
		GetBitBool(g_iBitUserRusChat, id),
		GetBitBool(g_iBitUserRoundSound, id),
		GetBitBool(g_iBitShowDeathMenu, id),
		GetBitBool(g_iBitUserOffPM, id),

		GetBitBool(g_iBitAutoWeapons, id),
		g_iUserAutoWeapon[id],
		g_iGrabMenuType[id],
		g_iUserSkin[id],
		g_eUserCostumes[id][COSTUMES],
		g_iPersonalModel[id],

		// hook setting
		g_eHookSetting[id][HOOK_SPEED],
		g_eHookSetting[id][HOOK_COLOUR],
		g_eHookSetting[id][HOOK_WIDTH],
		g_eHookSetting[id][HOOK_AMPLITUDE],
		g_eHookSetting[id][HOOK_TRAIL_COLOUR],
		g_eHookSetting[id][HOOK_TRAIL_WIDTH],
		g_eHookSetting[id][HOOK_TRAIL_LENGTH],

		GetBitBool(g_iBitNewHook, id),
		GetBitBool(g_iBitHookMenu, id),

		GetBitBool(g_iBitButtonSound, id),
		g_iUserDemoID[id]
	);
}

set_user_setting_sql(id, const szSetting[])
{
	if (!szSetting[0] || szSetting[0] == EOS) return;
		
	new szParse[DATA_SAVE_SETTING][3], iData[DATA_SAVE_SETTING];
	
	parse(szSetting, 
		szParse[SETTING_CHAT_LANG],			charsmax(szParse),	// g_iBitUserRusChat
		szParse[SETTING_ROUND_SOUND],		charsmax(szParse),	// Round Sound on | off
		szParse[SETTING_DEATH_MENU],		charsmax(szParse),	// Меню мертвого при смерти on | off
		szParse[SETTING_OFF_PM],			charsmax(szParse),	// Приват сообщения on | off
		
		szParse[SETTING_GR_AABUY],			charsmax(szParse),	// Авто закуп оружия для КТ
		szParse[SETTING_GR_AATYPE],			charsmax(szParse),	// Выбранное оружие для авто-закупа КТ
		szParse[SETTING_GRAB_MTYPE],		charsmax(szParse),	// Выбранный тип меню граба
		szParse[SETTING_PN_SKIN],			charsmax(szParse),	// g_iUserSkin[id]
		szParse[SETTING_COSTUMES],			charsmax(szParse),	// g_eUserCostumes[id][COSTUMES]
		szParse[SETTING_PERSONAL_MODEL],	charsmax(szParse),	// g_iPersonalModel[id]
		
		szParse[SETTING_HOOK_SPEED],		charsmax(szParse),	// g_eHookSetting[id][HOOK_SPEED]
		szParse[SETTING_HOOK_COLOUR],		charsmax(szParse),	// g_eHookSetting[id][HOOK_COLOUR]
		szParse[SETTING_HOOK_WIDTH],		charsmax(szParse),	// g_eHookSetting[id][HOOK_WIDTH]
		szParse[SETTING_HOOK_AMPLITUDE],	charsmax(szParse),	// g_eHookSetting[id][HOOK_AMPLITUDE]
		szParse[SETTING_HOOK_TRAIL_COLOUR],	charsmax(szParse),	// g_eHookSetting[id][HOOK_TRAIL_COLOUR]
		szParse[SETTING_HOOK_TRAIL_WIDTH],	charsmax(szParse),	// g_eHookSetting[id][HOOK_TRAIL_WIDTH]
		szParse[SETTING_HOOK_TRAIL_LENGHT],	charsmax(szParse),	// g_eHookSetting[id][HOOK_TRAIL_LENGTH]
	
		szParse[SETTING_HOOK_NEW_HOOK],		charsmax(szParse),	// g_iBitNewHook
		szParse[SETTING_HOOK_MENU_ON],		charsmax(szParse),	// g_iBitHookMenu

		szParse[SETTING_BUTTON_SOUND],		charsmax(szParse),	// g_iBitButtonSound
		szParse[SETTING_DEMO_ID],			charsmax(szParse)	// g_iUserDemoID[id]
	);

	for (new i; i < DATA_SAVE_SETTING; i++)
		iData[i] = str_to_num(szParse[i]);

	SetBitBool(g_iBitUserRusChat, id, iData[SETTING_CHAT_LANG]);
	SetBitBool(g_iBitUserRoundSound, id, iData[SETTING_ROUND_SOUND]);
	SetBitBool(g_iBitShowDeathMenu, id, iData[SETTING_DEATH_MENU]);
	SetBitBool(g_iBitUserOffPM, id, iData[SETTING_OFF_PM]);

	SetBitBool(g_iBitAutoWeapons, id, iData[SETTING_GR_AABUY]);
	if (iData[SETTING_GR_AATYPE] < GUARD_WEAPON_MENU_NUM)
		g_iUserAutoWeapon[id] = iData[SETTING_GR_AATYPE];

	// Выбранный тип меню граба
	if (iData[SETTING_GRAB_MTYPE] >= 0
	&& IsSetBit(g_iBitUserAdmin, id) && iData[SETTING_GRAB_MTYPE] <= GRAB_MENU_GLOW)
		g_iGrabMenuType[id] = iData[SETTING_GRAB_MTYPE];

	// skin за зеков
	if (iData[SETTING_PN_SKIN] < PRISONER_MODEL_SKINS && iData[SETTING_PN_SKIN] >= 0) {
		if (iData[SETTING_PN_SKIN] == PRISONER_SKIN_VIP && IsNotSetBit(g_iBitUserVip, id))
			g_iUserSkin[id] = random_num(SKIN_ORANGE, SKIN_BROWN);
		else
			g_iUserSkin[id] = iData[SETTING_PN_SKIN];
	}
	
	// костюм
	if (iData[SETTING_COSTUMES] > 0 && iData[SETTING_COSTUMES] < g_iCostumesListSize)
	{
		new aDataCostumes[DATA_COSTUMES_FILE];
		ArrayGetArray(g_aDataCostumes, iData[SETTING_COSTUMES], aDataCostumes);

		if (aDataCostumes[COSTUME_ACCESS][0] == '1' && IsNotSetBit(g_iBitUserVip, id)
		|| aDataCostumes[COSTUME_ACCESS][0] == '2' && IsNotSetBit(g_iBitUserAdmin, id))
			g_eUserCostumes[id][COSTUMES] = 0;
			
		else if (IsSetBit(g_iBitUserAlive, id)
		&& g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY || g_iDayMode == DAY_DEATHMATCH)
			jbe_set_user_costumes(id, iData[SETTING_COSTUMES]);
		else
		{
			g_eUserCostumes[id][COSTUMES] = iData[SETTING_COSTUMES];
			g_eUserCostumes[id][HIDE] = true;
		}
	}

	// Персональная модель
	if ((IsNotSetBit(g_iBitUserModelsPack, id) && !g_iPersonalModelsUserSize[id])
	|| !g_iPersonalModelsAllSize)
	{
		g_iPersonalModel[id] = 0;
	}
	else
	{
		new iModel = iData[SETTING_PERSONAL_MODEL];
		if (iModel)
		{
			if (iModel >= g_iPersonalModelsAllSize)
			{
				g_iPersonalModel[id] = -1;
			}
			
			else if (iModel > 0)
			{
				
				if (IsNotSetBit(g_iBitUserModelsPack, id))
				{
					if (iModel < g_iPersonalModelsUserSize[id])
						iModel = ArrayGetCell(g_aPersonalModelsUser[id], iModel);
					else
						iModel = -1;
				}
				
				if (iModel > -1)
				{
					new aDataPersonalModels[DATA_PERSONAL_MODELS];
	
					ArrayGetArray(g_aDataPersonalModels, iModel, aDataPersonalModels);
					
					if (IsSetBit(g_iBitUserModelsPack, id)
					|| cmsapi_get_user_services(id, "", aDataPersonalModels[FLAG_PM], 0))
						g_iPersonalModel[id] = iModel;
				}
			}
			else if (g_iPersonalModelsUserSize[id])
				g_iPersonalModel[id] = -1;
			else
				g_iPersonalModel[id] = 0;
		}
	}
	
	// hook setting
	for (new i = SETTING_HOOK_SPEED; i <= SETTING_HOOK_TRAIL_LENGHT; i++)
	{
		g_eHookSetting[id][i - SETTING_HOOK_SPEED] = iData[i];
		
		if (g_eHookSetting[id][i - SETTING_HOOK_SPEED] > g_eHookSettingSize[i - SETTING_HOOK_SPEED]
		|| g_eHookSetting[id][i - SETTING_HOOK_SPEED] < 0)
			g_eHookSetting[id][i - SETTING_HOOK_SPEED] = 0;
	}

	SetBitBool(g_iBitNewHook, id, iData[SETTING_HOOK_NEW_HOOK]);
	SetBitBool(g_iBitHookMenu, id, iData[SETTING_HOOK_MENU_ON]);

	// other
	SetBitBool(g_iBitButtonSound, id, iData[SETTING_BUTTON_SOUND]);

	g_iUserDemoID[id] = iData[SETTING_DEMO_ID];
}

