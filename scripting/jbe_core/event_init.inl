/*===== -> Игровые события -> =====*///{

event_init()
{
	register_logevent("LogEvent_RestartGame", 2, "1=Game_Commencing", "1&Restart_Round_");
	register_event("HLTV", "Event_HLTV", "a", "1=0", "2=0"); //(?) в regamedll вызывается много раз вместо 1, так что убираем
	register_logevent("LogEvent_RoundStart", 2, "1=Round_Start");
	register_logevent("LogEvent_RoundEnd", 2, "1=Round_End");
	g_iEvent_StatusValueShow = register_event("StatusValue", "Event_StatusValueShow", "be", "1=2", "2!0");
	g_iEvent_StatusValueHide = register_event("StatusValue", "Event_StatusValueHide", "be", "1=1", "2=0");
	register_event("Damage", "Event_Damage", "b", "2!0");
	register_event("30", "Event_ChangeMap", "a");
}

public LogEvent_RestartGame()
{
	LogEvent_RoundEnd();
//	jbe_set_day(0);
	jbe_set_day_week(0);
}

public Event_HLTV()
{	
	g_bRoundEnd = false;

	for (new i; i < sizeof(g_iHamHookForwards); i++)
		DisableHamForward(g_iHamHookForwards[i]);

	if (g_bRestartGame)
	{
		if (task_exists(TASK_RESTART_GAME_TIMER)) return;
		g_iDayModeTimer = g_eAllCvars[RESTART_GAME_TIME] + 1;
		set_task(1.0, "jbe_restart_game_timer", TASK_RESTART_GAME_TIMER, _, _, "a", g_iDayModeTimer);
		return;
	}
		
	for (new i; i < sizeof(g_iHamHookItemDeploy); i++)
		DisableHamForward(g_iHamHookItemDeploy[i]);

	g_fRoundStartTime = get_gametime();

	jbe_set_day(++g_iDay);
	jbe_set_day_week(++g_iDayWeek);
	jbe_set_name_chief();
	
	g_iChiefStatus = 0;
	g_iBitUserFree = 0;
	g_szFreeNames[0] = EOS;
	g_szDayModeTimer[0] = EOS;
	g_szWantedNames[0] = EOS;
	g_iFreeLang = 0;
	g_iBitUserWanted = 0;
	g_iWantedLang = 0;
	g_iLastPnId = 0;
	g_iLastPnType = 0;
	g_iBitBalisong = 0;
	g_iBitBasaballbat = 0;
	g_iBitGuardBaton = 0;
	g_iBitWeaponStatus = 0;
	g_iBitLatchkey = 0;
	g_iBitKokain = 0;
	g_iBitFrostNade = 0;
	g_iBitClothingGuard = 0;
	g_iBitClothingType = 0;
	g_iBitHingJump = 0;
	g_iBitFastRun = 0;
	g_iBitDoubleJump = 0;
	g_iBitAutoBhop = 0;
	g_iBitLongJump = 0;
	g_iBitDoubleDamage = 0;
	g_iBitLotteryTicket = 0;
	g_iBitUserVoice = 0;
	g_iBitFallNoDamage = 0;
	g_bDoorStatus = false;
	g_bOneOpenDoors = false;
	g_iBitRobShop = 0;
	g_bRobShop = false;
	g_iRobShopNum = 0;
	g_iBitUserBet = 0;
	g_iBitUserChangeName = 0;
	g_iChiefGameType = 0;
	g_iBitElectroCostume = 0;
	g_iChiefChoiceTime = 0;
	g_iBitUserGhost = 0;
	g_iBitActiveGhost = 0;
	g_iBitUserBuried = 0;
	g_iBitBuyWeaponsGuard = 0;
	g_iBitPriceGunPn = 0;
	g_iBitUserGodMode = 0;
	g_iBitUser3DView = 0;
	g_iBitOpenShopInfo = 0;
	g_iBitBuyFrostNade = 0;
	g_iBitUserRealWanted = 0;
	g_iBitUsedRankBonus = 0;
	g_iBitBuyRespawn = 0;

	if (g_iBitUseGlobalBlock)
	{
		arrayset(g_iGlobalBlock, 0, eGlobalBlock);
		g_iBitUseGlobalBlock = 0;
	}

	if (g_bGlobalLight) set_lights("#OFF");
	
//	if (g_bIsVenicleMap) fix_venicle_spawn();

	if (g_iDayWeek <= 4 || g_iDayWeek == 7 || !g_iDayModeListSize || g_iPlayersNum[1] < 2 || !g_iPlayersNum[2])
		jbe_set_day_mode(1);
	else
		jbe_set_day_mode(3);

//	if (!g_bGameRadarStatus)
//		jbe_radar_on();
	
	ham_jump_check();
	check_frostnade_fwd();
	spectbot_check();
	check_player_pre_think();
}

public LogEvent_RoundStart()
{
	//Event_HLTV(); // тут из-за regamedll
	
	if (g_bRestartGame)
	{
		restart_deathmatch(.bRoundStart = true);
		return;
	}
	
	jbe_set_name_mode("");

	if (!g_iPlayersNum[TEAM_GUARD])
	{
		jbe_deathmatch_start();
	}
	else if (g_iDayWeek == 7)
	{
		jbe_check_last_prisoner();
		jbe_free_day_start();
		open_doors_hotfix();
		g_iChiefIdOld = 0;
		g_iChiefGlobalGameLimit = 0;

		arrayset(g_iUserWantedNum, 0, sizeof(g_iUserWantedNum));
	}
	else if (g_iDayWeek <= 4 || !g_iDayModeListSize || g_iAlivePlayersNum[TEAM_PRISONER] < 2)
	{
		if (!g_iChiefStatus)
		{
			g_iChiefChoiceTime = 40 + 1;
			set_task(1.0, "jbe_chief_choice_timer", TASK_CHIEF_CHOICE_TIME, _, _, "a", g_iChiefChoiceTime);
		}

		for (new i = 1; i <= MaxClients; i++)
		{
			if (g_iUserTeam[i] == TEAM_PRISONER)
			{
				if (g_iDayWeek != 7 && IsSetBit(g_iBitUserFreeNextRound, i))
				{
					jbe_add_user_free(i);
					ClearBit(g_iBitUserFreeNextRound, i);
				}
				if (IsSetBit(g_iBitUserVoiceNextRound, i))
				{
					SetBit(g_iBitUserVoice, i);
					ClearBit(g_iBitUserVoiceNextRound, i);
					reload_user_voice(i);				
				}

				jbe_print_prisons_reason(i);
			}
			g_iBitKilledUsers[i] = 0;
			
			is_user_active_bracelets(i);
		}
		jbe_set_random_pahan();
		jbe_check_last_prisoner();
	}
	else
	{
		jbe_vote_day_mode_start();
	}
	
	jbe_set_name_chief();
	rank_reward_exp_players();
//	g_fRoundStartTime = get_gametime();

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserConnected, i))
			continue;
		
		for (new j; j < DATA_RANK_BONUS; j++)
			g_iRankBonus[i][j]++;
		
		if (IsSetBit(g_iBitUserVip, i))
			fn_vip_round_up(i);
	}

	jbe_check_ready_limits_task();
}

public jbe_chief_choice_timer()
{
	if (--g_iChiefChoiceTime)
	{
		if (g_iChiefChoiceTime == 30)
			g_iChiefIdOld = 0;

		formatex(g_szChiefName, charsmax(g_szChiefName),
			"^nНачальник: Ждем [%i]", g_iChiefChoiceTime);
	}
	else
	{
		jbe_set_name_chief();
		jbe_free_day_start();
	}
}

public LogEvent_RoundEnd()
{
	if (!task_exists(TASK_ROUND_END))
		set_task(0.1, "LogEvent_RoundEndTask", TASK_ROUND_END);
}

public LogEvent_RoundEndTask()
{
	if (g_iDayMode != DAY_DAYMODE)
	{
		g_iFriendlyFire = 0;
	
		remove_task(TASK_COUNT_DOWN_TIMER);
		remove_task(TASK_RADAR_LAST_PN);
		remove_task(TASK_CHIEF_CHOICE_TIME);
		remove_task(TASK_SHOW_SOCCER_SCORE);

		jbe_minigame_stop_all();

		if (g_bSoccerStatus) jbe_soccer_disable_all();
		if (g_bAllUseBall) jbe_soccer_remove_ball();
		if (g_iBoxingGame) jbe_boxing_disable_all();
		if (g_iDuelStatus) jbe_duel_disable_all();
		if (g_bMafiaGame) jbe_mafia_ended();
		if (g_bJihadGame) jihad_disable();

		if (g_iDayMode == DAY_FREEDAY)
			jbe_free_day_ended();

		for (new i = 1; i <= MaxClients; i++)
		{			
			if (IsNotSetBit(g_iBitUserAlive, i))
				continue;
		
			if (task_exists(i+TASK_REMOVE_SYRINGE))
			{
				remove_task(i+TASK_REMOVE_SYRINGE);
				if (get_user_weapon(i))
				{
					new iActiveItem = get_member(i, m_pActiveItem);
					if (iActiveItem > 0) ExecuteHamB(Ham_Item_Deploy, iActiveItem);
				}
			}

			if (g_iBitUserFrozen && IsSetBit(g_iBitUserFrozen, i))
			{
				ClearBit(g_iBitUserFrozen, i);
				remove_task(i+TASK_FROSTNADE_DEFROST);
				set_pev(i, pev_flags, pev(i, pev_flags) & ~FL_FROZEN);
				set_member(i, m_flNextAttack, 0.0);
				emit_sound(i, CHAN_AUTO, sound_defrost_player, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				new Float:vecOrigin[3]; pev(i, pev_origin, vecOrigin);
				CREATE_BREAKMODEL(vecOrigin, _, _, 10, g_pModelGlass, 10, 25, 0x01);
			}

			if (IsSetBit(g_iBitUserFree, i))
				remove_task(i + TASK_FREE_DAY_ENDED);

			if (IsSetBit(g_iBitUser3DView, i))
				jbe_set_user_view(i, 0);
				
		#if defined ENABLE_QUESTS
			if (g_iChiefId == i && g_iUserTeam[i] == TEAM_GUARD)
				check_user_quest(i, QUEST_ID_CHIEF_GAME);

			if (g_iPlayersNum[TEAM_GUARD] && g_iPlayersNum[TEAM_PRISONER])
				check_user_quest(i, QUEST_ID_LIFER);
		#endif

		}

		g_iChiefId = 0; // для квеста обнуляем позже
		
		if (g_iDuelStatus)
		{
			g_iBitUserDuel = 0;
			if (task_exists(TASK_DUEL_COUNT_DOWN))
			{
				remove_task(TASK_DUEL_COUNT_DOWN);
				client_cmd(0, "mp3 stop");
			}
		}

		if (g_iDayMode == DAY_DEATHMATCH)
			jbe_deathmatch_ended();

	}
	else // DAY_DAYMODE
	{
		// Если еще идет голосование ЗА игру
		if (task_exists(TASK_VOTE_DAY_MODE_TIMER))
		{
			remove_task(TASK_VOTE_DAY_MODE_TIMER);
			for (new i = 1; i <= MaxClients; i++)
			{
				if (IsNotSetBit(g_iBitUserVoteDayMode, i))
					continue;
				
				ClearBit(g_iBitUserVoteDayMode, i);
				ClearBit(g_iBitUserDayModeVoted, i);
				show_menu(i, 0, "^n");
				jbe_informer_offset_down(i);
				jbe_menu_unblock(i);
				set_pev(i, pev_flags, pev(i, pev_flags) & ~FL_FROZEN);
				set_member(i, m_flNextAttack, 0.0);
				UTIL_ScreenFade(i, 512, 512, 0, 0, 0, 0, 255, 1);
			}
		}

		if (g_iVoteDayMode != -1)
		{
			remove_task(TASK_DAY_MODE_TIMER);
			g_szDayModeTimer = "";
			ExecuteForward(g_iHookDayModeEnded, g_iReturnDayMode, g_iVoteDayMode, g_iAlivePlayersNum[1] ? 1 : 2);
			g_iVoteDayMode = -1;
		}
	}

	for (new i; i < sizeof(g_iHamHookForwards); i++)
		EnableHamForward(g_iHamHookForwards[i]);
	
	g_bRoundEnd = true;
	if (g_iRoundSoundSize && !g_bRestartGame)
	{
		new aDataRoundSound[DATA_ROUND_SOUND], iTrack = random_num(0, g_iRoundSoundSize - 1);
		ArrayGetArray(g_aDataRoundSound, iTrack, aDataRoundSound);
		new szAnim[24];
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserConnected, i) || IsNotSetBit(g_iBitUserRoundSound, i)) continue;
			client_cmd(i, "mp3 play sound/CsOneRu/jb/round_sound/%s.mp3", aDataRoundSound[FILE_NAME]);
			client_print_color(i, 0, "%s Сейчас играет: ^4%s", g_szChatPrefix, aDataRoundSound[TRACK_NAME]);
			if (IsNotSetBit(g_iBitUserAlive, i)) continue;
			
			static iszViewModel = 0;
			if (iszViewModel || (iszViewModel = engfunc(EngFunc_AllocString, model_v_hand)))
				set_pev_string(i, pev_viewmodel2, iszViewModel);
			set_member(i, m_flNextAttack, 5.0);
			UTIL_WeaponAnimation(i, 12); // 8
			
			// Зеки танцуют
			if (is_player_model_anims(i))
			{
				formatex(szAnim, charsmax(szAnim), "animation_%d", random_num(1, 6));
				UTIL_PlayerAnimation(i, szAnim);
			}
		}
	}

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserConnected, i))
			continue;

		// Оффаем граб на всех
		grab_off_to_user(i);
		
		// Оффаем хук у кого есть (чтоб с клеток не вылетали нечайно)
		if (IsSetBit(g_iBitUserHook, i))
			ClCmd_HookOff(i);

		// Выдаем деньги в конце раунда вместо каждого спавна
		if (g_iDayMode == DAY_DEATHMATCH || g_iDayMode == DAY_RESTART)
			continue;
			
		if (IsSetBit(g_iBitUserAlive, i))
		{
			jbe_set_user_money(i, g_iUserMoney[i] + g_eAllCvars[ROUND_FREE_MODEY] + g_eAllCvars[ROUND_ALIVE_MODEY], 0);
			check_user_quest(i, QUEST_ID_ROBS, g_eAllCvars[ROUND_FREE_MODEY] + g_eAllCvars[ROUND_ALIVE_MODEY]);
		}
		else
		{
			jbe_set_user_money(i, g_iUserMoney[i] + g_eAllCvars[ROUND_FREE_MODEY], 0);
			check_user_quest(i, QUEST_ID_ROBS, g_eAllCvars[ROUND_FREE_MODEY]);
		}

		if (IsNotSetBit(g_iBitUserAlive, i))
			continue;

		if (g_eUserRendering[i][RENDER_STATUS])
//		&& pev(i, pev_renderfx) != kRenderFxNone || pev(i, pev_rendermode) != kRenderNormal)
		{
			jbe_set_user_rendering(i, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
			g_eUserRendering[i][RENDER_STATUS] = false;
		}
		
		if (g_iDayMode == DAY_DAYMODE)
			check_user_quest(i, QUEST_ID_GOOD_GAMER);	
	}
	
	jbe_set_name_mode("");
	jbe_set_name_chief();
	check_vip_night();

	if (g_iSqlLastUpdateTime + 300 < get_systime() && g_bSqlConnected)
		sql_update_round();
}

public Event_StatusValueShow(id)
{
	static iTarget;
	new const szTeam[][] = {"", "Заключённый", "Охранник", ""};
	iTarget = read_data(2);
	set_hudmessage(102, 69, 0, -1.0, 0.8, 0, 0.0, 10.0, 0.0, 0.0, -1);
	ShowSyncHudMsg(id, g_iSyncStatusText,
		"%s: %n^n\
		HP: %d | AP: %d | %d$^n\
		%s: %d|%d",
		szTeam[g_iUserTeam[iTarget]], iTarget,
		get_user_health(iTarget), get_user_armor(iTarget), g_iUserMoney[iTarget],
		g_iszRankExpName[g_iUserLevel[iTarget]][RANK_NAME], g_iUserExp[iTarget],
		g_iszRankExpName[g_iUserLevel[iTarget]+1][RANK_EXP]);
}

public Event_StatusValueHide(id) ClearSyncHud(id, g_iSyncStatusText);

public Event_Damage(iVictim)
{
	static iAttacker;
	iAttacker = get_user_attacker(iVictim);
	
	if (!jbe_is_user_valid(iAttacker) || iVictim == iAttacker || g_iDayMode == DAY_DAYMODE)
		return;
	
	static iDamage;
	iDamage = read_data(2);
	
	set_hudmessage(0, 100, 200, -1.0, 0.55, 2, 0.1, 2.0, 0.02, 0.02, -1);
	ShowSyncHudMsg(iAttacker, g_iSyncDamageAttacker, "%d", iDamage);
	
	set_hudmessage(255, 0, 0, 0.53, 0.49, 2, 0.1, 2.0, 0.02, 0.02, -1);
	ShowSyncHudMsg(iVictim, g_iSyncDamageVictim, "%d", iDamage);
}

public Event_ChangeMap()
{
	log_amx("Event_ChangeMap() start");
	
	if (g_iGameMusicSize)
	{
		new aDataDayModeMusic[DATA_DAY_MODE_MUSIC];
		new iTrack = random_num(0, g_iGameMusicSize - 1);
		ArrayGetArray(g_aDataGameMusic, iTrack, aDataDayModeMusic);
		client_cmd(0, "mp3 play %s", aDataDayModeMusic[FILE_NAME]);
	}
	
	g_iBitBlockMenu = -1;		// -1 это полные 32 бита
	
	new iTask = get_cvar_num("mp_chattime");
	set_task(float(iTask - 4), "_all_clear_bit_connect");	

	if (g_bSqlConnected)
	{
		g_bSqlConnected = false;	// чтоб не грузить если кто-то зайдет
		sql_update_changelevel();	// сохраняем всех разом :D
	}

	log_amx("Event_ChangeMap() ended");
}

public _all_clear_bit_connect()
{
	g_iBitUserConnected = 0;
	g_iBitUserAlive = 0;

	spectbot_check(.bBotsKick = true);
}


/*===== <- Игровые события <- =====*///}