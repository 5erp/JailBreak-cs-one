/*===== -> Режимы игры -> =====*///{
game_mode_init()
{
	register_menucmd(register_menuid("Show_DayModeMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_DayModeMenu");

	g_aDataDayMode = ArrayCreate(DATA_DAY_MODE);
	g_aFileDayMode = ArrayCreate(64);
	g_aDataGameMusic = ArrayCreate(DATA_DAY_MODE_MUSIC);
	
	g_iHookDayModeStart = CreateMultiForward("jbe_day_mode_start", ET_IGNORE, FP_CELL, FP_CELL);
	g_iHookDayModeEnded = CreateMultiForward("jbe_day_mode_ended", ET_IGNORE, FP_CELL, FP_CELL);
}

stock game_mode_player_models()
{
	/*
[День рождения]
prisoner model_pn body
guard model_gr body
	*/
}

Show_DayModeMenu(id)
{
	new iKeys;
	new aDataDayMode[DATA_DAY_MODE];
	menu_title("Выберите игру^n^n");

	for (new a = 0, b; a < g_iDayModeListSize; a++)
	{
		ArrayGetArray(g_aDataDayMode, a, aDataDayMode);
		if (aDataDayMode[MODE_BLOCKED])
		{
			menu_item("\y%d. \d%s \r[Блок %d]^n", ++b,
				aDataDayMode[LANG_MODE], aDataDayMode[MODE_BLOCKED]);
		}
		else
		{
			if (IsSetBit(g_iBitUserDayModeVoted, id))
			{
				menu_item("\y%d. \d%s^n", ++b, aDataDayMode[LANG_MODE]);
			}
			else
			{
				iKeys |= (1<<b);
				menu_item("\y%d. \w%s^n", ++b, aDataDayMode[LANG_MODE]);
			}
		}
	}

	return show_menu(id, iKeys, _menu_string, g_iDayModeVoteTime, "Show_DayModeMenu");
}

public Handle_DayModeMenu(id, iKey)
{
	jbe_informer_offset_down(id);
	new aDataDayMode[DATA_DAY_MODE];
	ArrayGetArray(g_aDataDayMode, iKey, aDataDayMode);
	aDataDayMode[VOTES_NUM]++;
	ArraySetArray(g_aDataDayMode, iKey, aDataDayMode);
	SetBit(g_iBitUserDayModeVoted, id);
	jbe_menu_unblock(id);
	client_print_color(0, id, "%s ^3%n ^1проголосовал за ^4%s^1.", g_szChatPrefix, id, aDataDayMode[LANG_MODE]);
	
	// fix чтоб меню не "переоткрылось" таймером
	message_begin(MSG_ONE, SVC_NOP, {0,0,0}, id);
	message_end();
	show_menu(id, 0, "^n");

	return PLUGIN_HANDLED;
}

public jbe_day_mode_start(iDayMode, iAdmin)
{
	new aDataDayMode[DATA_DAY_MODE];
	ArrayGetArray(g_aDataDayMode, iDayMode, aDataDayMode);
	jbe_set_name_mode(aDataDayMode[LANG_MODE]);
	if (aDataDayMode[MODE_TIMER])
	{
		g_iDayModeTimer = aDataDayMode[MODE_TIMER] + 1;
		set_task(1.0, "jbe_day_mode_timer", TASK_DAY_MODE_TIMER, _, _, "a", g_iDayModeTimer);
	}
	if (aDataDayMode[MODE_TRACK])
	{
		jbe_daymode_sound_task((aDataDayMode[MODE_TRACK] - 1) + TASK_DAYMODE_SOUND);
	}
	if (iAdmin)
	{
		g_iFriendlyFire = 0;
		if(g_iDayMode == 2) jbe_free_day_ended();
		else
		{
			g_iBitUserFree = 0;
			g_szFreeNames = "";
			g_iFreeLang = 0;
		}
		g_iDayMode = 3;
		remove_task(TASK_CHIEF_CHOICE_TIME);
		g_iChiefId = 0;
		jbe_set_name_chief();
		g_iChiefStatus = 0;
		g_iBitUserWanted = 0;
		g_szWantedNames = "";
		g_iWantedLang = 0;
		g_iBitBalisong = 0;
		g_iBitBasaballbat = 0;
		g_iBitGuardBaton = 0;
		g_iBitLatchkey = 0;
		g_iBitKokain = 0;
		g_iBitFrostNade = 0;
		g_iBitClothingGuard = 0;
		g_iBitHingJump = 0;
		g_iBitDoubleJump = 0;
		g_iBitAutoBhop = 0;
		g_iBitLongJump = 0;
		g_iBitDoubleDamage = 0;
		for(new iPlayer = 1; iPlayer <= MaxClients; iPlayer++)
		{
			if(IsNotSetBit(g_iBitUserAlive, iPlayer)) continue;
			g_iBitKilledUsers[iPlayer] = 0;
			show_menu(iPlayer, 0, "^n");
			if(g_iBitWeaponStatus && IsSetBit(g_iBitWeaponStatus, iPlayer))
			{
				ClearBit(g_iBitWeaponStatus, iPlayer);
				if(get_user_weapon(iPlayer) == CSW_KNIFE)
				{
					new iActiveItem = get_member(iPlayer, m_pActiveItem);
					if(iActiveItem > 0) ExecuteHamB(Ham_Item_Deploy, iActiveItem);
				}
			}
			if(task_exists(iPlayer+TASK_REMOVE_SYRINGE))
			{
				remove_task(iPlayer+TASK_REMOVE_SYRINGE);
				if(get_user_weapon(iPlayer))
				{
					new iActiveItem = get_member(iPlayer, m_pActiveItem);
					if(iActiveItem > 0) ExecuteHamB(Ham_Item_Deploy, iActiveItem);
				}
			}
			if(pev(iPlayer, pev_renderfx) != kRenderFxNone || pev(iPlayer, pev_rendermode) != kRenderNormal)
			{
				jbe_set_user_rendering(iPlayer, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
				g_eUserRendering[iPlayer][RENDER_STATUS] = false;
			}
			if(g_iBitUserFrozen && IsSetBit(g_iBitUserFrozen, iPlayer))
			{
				ClearBit(g_iBitUserFrozen, iPlayer);
				remove_task(iPlayer+TASK_FROSTNADE_DEFROST);
				set_pev(iPlayer, pev_flags, pev(iPlayer, pev_flags) & ~FL_FROZEN);
				set_member(iPlayer, m_flNextAttack, 0.0);
				emit_sound(iPlayer, CHAN_AUTO, sound_defrost_player, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				new Float:vecOrigin[3]; pev(iPlayer, pev_origin, vecOrigin);
				CREATE_BREAKMODEL(vecOrigin, _, _, 10, g_pModelGlass, 10, 25, 0x01);
			}

			if(g_iBitClothingType && IsSetBit(g_iBitClothingType, iPlayer)) jbe_default_player_model(iPlayer);
			if(g_iBitFastRun && IsSetBit(g_iBitFastRun, iPlayer))
			{
				ClearBit(g_iBitFastRun, iPlayer);
				ExecuteHamB(Ham_Player_ResetMaxSpeed, iPlayer);
			}
			
			if (IsSetBit(g_iBitUserHook, iPlayer) && task_exists(iPlayer+TASK_HOOK_THINK))
			{
				remove_task(iPlayer+TASK_HOOK_THINK);
			}
		}
		if(g_bSoccerStatus) jbe_soccer_disable_all();
		if(g_iBoxingGame) jbe_boxing_disable_all();
	}

	for (new iPlayer = 1; iPlayer <= MaxClients; iPlayer++)
	{
		jbe_hide_user_costumes(iPlayer);
	}
	
	jbe_open_doors();
	
	log_amx("DayMode start: %s", aDataDayMode[LANG_MODE]);
}

public jbe_day_mode_ended(iDayMode, iWinTeam)
{
	new aDataDayMode[DATA_DAY_MODE];
	ArrayGetArray(g_aDataDayMode, iDayMode, aDataDayMode);

	if (aDataDayMode[MODE_TRACK])
	{
		remove_task((aDataDayMode[MODE_TRACK] - 1) + TASK_DAYMODE_SOUND);
	}

	client_cmd(0, "mp3 stop");
	g_iBitOffDayModeSound = 0;

	log_amx("DayMode ended: %s",  aDataDayMode[LANG_MODE]);
}

public jbe_day_mode_timer()
{
	if (--g_iDayModeTimer)
		formatex(g_szDayModeTimer, charsmax(g_szDayModeTimer), "[%i]", g_iDayModeTimer);
	
	else
	{
		g_szDayModeTimer = "";
		ExecuteForward(g_iHookDayModeEnded, g_iReturnDayMode, g_iVoteDayMode, 0);
		g_iVoteDayMode = -1;
	}
}

public jbe_vote_day_mode_start()
{
	g_iDayModeVoteTime = g_eAllCvars[DAY_MODE_VOTE_TIME] + 1;
	new aDataDayMode[DATA_DAY_MODE];
	for (new i; i < g_iDayModeListSize; i++)
	{
		ArrayGetArray(g_aDataDayMode, i, aDataDayMode);
		if (aDataDayMode[MODE_BLOCKED])
			aDataDayMode[MODE_BLOCKED]--;
		aDataDayMode[VOTES_NUM] = 0;
		ArraySetArray(g_aDataDayMode, i, aDataDayMode);
	}
	for (new iPlayer = 1; iPlayer <= MaxClients; iPlayer++)
	{
		if (IsNotSetBit(g_iBitUserAlive, iPlayer)) continue;
		SetBit(g_iBitUserVoteDayMode, iPlayer);
		g_iBitKilledUsers[iPlayer] = 0;
		jbe_menu_block(iPlayer);
		jbe_informer_offset_up(iPlayer);
		Show_DayModeMenu(iPlayer);
		set_pev(iPlayer, pev_flags, pev(iPlayer, pev_flags) | FL_FROZEN);
		set_member(iPlayer, m_flNextAttack, float(g_iDayModeVoteTime));
		UTIL_ScreenFade(iPlayer, 0, 0, 4, 0, 0, 0, 255);
	}
	set_task(1.0, "jbe_vote_day_mode_timer", TASK_VOTE_DAY_MODE_TIMER, _, _, "a", g_iDayModeVoteTime);
}

public jbe_vote_day_mode_timer()
{
	if (!--g_iDayModeVoteTime)
		jbe_vote_day_mode_ended();
	else
	{
		ShowHud_VoteDayMode();
		for (new iPlayer = 1; iPlayer <= MaxClients; iPlayer++)
		{
			if (IsNotSetBit(g_iBitUserVoteDayMode, iPlayer)
			|| IsNotSetBit(g_iBitUserAlive, iPlayer)
			|| IsSetBit(g_iBitUserDayModeVoted, iPlayer))
				continue;
			Show_DayModeMenu(iPlayer);
		}
	}
}

ShowHud_VoteDayMode()
{
	new iLen, szMessage[512];
	new aDataDayMode[DATA_DAY_MODE];
	iLen = formatex(szMessage, charsmax(szMessage), "Результат голосования ^nОкончание через: %d^n^n", g_iDayModeVoteTime);
	for (new i; i < g_iDayModeListSize; i++)
	{
		ArrayGetArray(g_aDataDayMode, i, aDataDayMode);
		iLen += formatex(szMessage[iLen], charsmax(szMessage) - iLen, "%s [%d]^n", aDataDayMode[LANG_MODE], aDataDayMode[VOTES_NUM]);
	}
	set_hudmessage(170, 240, 255, -1.0, 0.20, 0, 0.0, 0.9, 0.1, 0.1, -1);
	show_hudmessage(0, szMessage);
}

jbe_vote_day_mode_ended()
{
	for (new iPlayer = 1; iPlayer <= MaxClients; iPlayer++)
	{
		if (IsNotSetBit(g_iBitUserVoteDayMode, iPlayer)) continue;
		ClearBit(g_iBitUserVoteDayMode, iPlayer);
		if (IsNotSetBit(g_iBitUserDayModeVoted, iPlayer))
		{
			show_menu(iPlayer, 0, "^n");
			jbe_informer_offset_down(iPlayer);
			jbe_menu_unblock(iPlayer);
		}
		ClearBit(g_iBitUserDayModeVoted, iPlayer);
		set_pev(iPlayer, pev_flags, pev(iPlayer, pev_flags) & ~FL_FROZEN);
		set_member(iPlayer, m_flNextAttack, 0.0);
		UTIL_ScreenFade(iPlayer, 512, 512, 0, 0, 0, 0, 255, 1);
	}

	new iNum, iDaysMode[MAX_DAYMODES_NUM];
	new aDataDayMode[DATA_DAY_MODE], iVotesNum; // iVotesNum = 0;
	for (new i; i < g_iDayModeListSize; i++)
	{
		ArrayGetArray(g_aDataDayMode, i, aDataDayMode);
		if (aDataDayMode[MODE_BLOCKED]) continue;
		
		if (aDataDayMode[VOTES_NUM] == iVotesNum)
		{
			iVotesNum = aDataDayMode[VOTES_NUM];
			iDaysMode[iNum++] = i;
		}
		else if (aDataDayMode[VOTES_NUM] > iVotesNum)
		{
			iNum = 0;
			iDaysMode[iNum++] = i;
			iVotesNum = aDataDayMode[VOTES_NUM];
			g_iVoteDayMode = i;
		}
	}
	
	if (iNum > 1)
		g_iVoteDayMode = iDaysMode[random_num(0, iNum - 1)];

	ArrayGetArray(g_aDataDayMode, g_iVoteDayMode, aDataDayMode);
	aDataDayMode[MODE_BLOCKED] = aDataDayMode[MODE_BLOCK_DAYS];
	ArraySetArray(g_aDataDayMode, g_iVoteDayMode, aDataDayMode);

	new szFileName[64];
	ArrayGetString(g_aFileDayMode, g_iVoteDayMode, szFileName, charsmax(szFileName));
	unpause("ac", szFileName);

	ExecuteForward(g_iHookDayModeStart, g_iReturnDayMode, g_iVoteDayMode, 0);
}

public jbe_daymode_sound_task(iTrack)
{
	iTrack -= TASK_DAYMODE_SOUND;

	new aDataDayModeMusic[DATA_DAY_MODE_MUSIC];
	ArrayGetArray(g_aDataGameMusic, iTrack, aDataDayModeMusic);

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserConnected, i) || IsSetBit(g_iBitOffDayModeSound, i))
			continue;

		client_cmd(i, "mp3 play %s", aDataDayModeMusic[DM_FILE_NAME]);
		client_print_color(i, i, "%s ^1Музыка игры: ^4%s", g_szChatPrefix, aDataDayModeMusic[DM_TRACK_NAME]);
	}

	set_task(float(aDataDayModeMusic[DM_TRACK_TIME]), "jbe_daymode_sound_task", iTrack + TASK_DAYMODE_SOUND);
}
/*===== <- Режимы игры <- =====*///}