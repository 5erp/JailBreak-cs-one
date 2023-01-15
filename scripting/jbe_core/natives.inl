/*===== -> Нативы -> =====*///{
public plugin_natives()
{
	set_native_filter("native_filter");
	
	register_native("jbe_get_day", "jbe_get_day", 1);
	register_native("jbe_set_day", "jbe_set_day", 1);
	register_native("jbe_get_day_week", "jbe_get_day_week", 1);
	register_native("jbe_set_day_week", "jbe_set_day_week", 1);
	register_native("jbe_get_day_mode", "jbe_get_day_mode", 1);
	register_native("jbe_set_day_mode", "jbe_set_day_mode", 1);
	register_native("jbe_open_doors", "jbe_open_doors", 1);
	register_native("jbe_close_doors", "jbe_close_doors", 1);
	register_native("jbe_get_user_money", "jbe_get_user_money", 1);
	register_native("jbe_set_user_money", "jbe_set_user_money", 1);
	register_native("jbe_get_user_team", "jbe_get_user_team", 1);
	register_native("jbe_set_user_team", "jbe_set_user_team", 1);
	register_native("jbe_get_user_model", "_jbe_get_user_model", 1);
	register_native("jbe_set_user_model", "_jbe_set_user_model", 1);
	register_native("jbe_informer_offset_up", "jbe_informer_offset_up", 1);
	register_native("jbe_informer_offset_down", "jbe_informer_offset_down", 1);
	register_native("jbe_menu_block", "jbe_menu_block", 1);
	register_native("jbe_menu_unblock", "jbe_menu_unblock", 1);
	register_native("jbe_menu_blocked", "jbe_menu_blocked", 1);
	register_native("jbe_is_user_free", "jbe_is_user_free", 1);
	register_native("jbe_add_user_free", "jbe_add_user_free", 1);
	register_native("jbe_add_user_free_next_round", "jbe_add_user_free_next_round", 1);
	register_native("jbe_sub_user_free", "jbe_sub_user_free", 1);
	register_native("jbe_free_day_start", "jbe_free_day_start", 1);
	register_native("jbe_free_day_ended", "jbe_free_day_ended", 1);
	register_native("jbe_is_user_wanted", "jbe_is_user_wanted", 1);
	register_native("jbe_add_user_wanted", "jbe_add_user_wanted", 1);
	register_native("jbe_sub_user_wanted", "jbe_sub_user_wanted", 1);
	register_native("jbe_is_user_chief", "jbe_is_user_chief", 1);
	register_native("jbe_set_user_chief", "jbe_set_user_chief", 1);
	register_native("jbe_get_chief_status", "jbe_get_chief_status", 1);
	register_native("jbe_get_chief_id", "jbe_get_chief_id", 1);
	register_native("jbe_set_user_costumes", "jbe_set_user_costumes", 1);
	register_native("jbe_hide_user_costumes", "jbe_hide_user_costumes", 1);
	register_native("jbe_prisoners_divide_color", "jbe_prisoners_divide_color", 1);
	register_native("jbe_register_day_mode", "jbe_register_day_mode");
	register_native("jbe_get_user_voice", "jbe_get_user_voice", 1);
	register_native("jbe_set_user_voice", "jbe_set_user_voice", 1);
	register_native("jbe_set_user_voice_next_round", "jbe_set_user_voice_next_round", 1);
	register_native("jbe_get_user_rendering", "_jbe_get_user_rendering", 1);
	register_native("jbe_set_user_rendering", "jbe_set_user_rendering", 1);
	register_native("jbe_save_user_rendering", "jbe_save_user_rendering", 1);
	register_native("jbe_is_wanted", "jbe_is_wanted", 1);
	register_native("jbe_set_user_godmode", "jbe_set_user_godmode", 1);
}

public native_filter(const name[], index, trap)
{
	if (!trap)
		return PLUGIN_HANDLED;
	
	return PLUGIN_CONTINUE;
}

public jbe_get_day() return g_iDay;
public jbe_set_day(iDay) g_iDay = iDay;

public jbe_get_day_week() return g_iDayWeek;
public jbe_set_day_week(iWeek) g_iDayWeek = (g_iDayWeek > 7) ? 1 : iWeek;

public jbe_get_day_mode() return g_iDayMode;
public jbe_set_day_mode(iMode)
{
	g_iDayMode = iMode;
	//formatex(g_szDayMode, charsmax(g_szDayMode), g_szGameMode[g_iDayMode]);
}

public jbe_open_doors()
{
	for (new i, iDoor; i < g_iDoorListSize; i++)
	{
		iDoor = ArrayGetCell(g_aDoorList, i);
		dllfunc(DLLFunc_Use, iDoor, 0);
	}
	
	g_bDoorStatus = true;
	g_bOneOpenDoors = true;
}
public jbe_close_doors()
{
	for (new i, iDoor; i < g_iDoorListSize; i++)
	{
		iDoor = ArrayGetCell(g_aDoorList, i);
		dllfunc(DLLFunc_Think, iDoor);
	}
	g_bDoorStatus = false;
}

public jbe_get_user_money(pPlayer) return g_iUserMoney[pPlayer];
public jbe_set_user_money(pPlayer, iNum, iFlash)
{
	g_iUserMoney[pPlayer] = iNum;
	set_member(pPlayer, m_iAccount, iNum);
	
	message_begin(MSG_ONE, MsgId_Money, _, pPlayer);
	write_long(iNum);
	write_byte(iFlash);
	message_end();
}

public jbe_get_user_team(pPlayer)
{
	if (IsNotSetBit(g_iBitUserConnected, pPlayer)) 
		return -1;

	return g_iUserTeam[pPlayer];
}

public jbe_set_user_team(pPlayer, iTeam)
{
	if (IsNotSetBit(g_iBitUserConnected, pPlayer))
		return 0;

	if (g_iUserTeam[pPlayer] == TEAM_UNASSIGNED || g_iUserTeam[pPlayer] == TEAM_SPECTATOR)
	{
		g_fRankRewardExp[pPlayer] = get_gametime();
		g_iUserGameTime[pPlayer] = get_systime();
	}

	if (IsSetBit(g_iBitUserAlive, pPlayer))
		user_kill(pPlayer, 1);

	rg_join_team(pPlayer, TeamName:iTeam);

	if (iTeam != TEAM_SPECTATOR)
	{
		set_member(pPlayer, m_bTeamChanged, false);
		set_member(pPlayer, m_iNumSpawns, 1);
		rg_set_user_team(pPlayer, _:iTeam, .model = MODEL_UNASSIGNED, .send_teaminfo = true, .check_win_conditions = true);
	}

	if (get_member(pPlayer, m_iTeam) != iTeam)
		return 0;

	g_iPlayersNum[g_iUserTeam[pPlayer]]--;
	g_iUserTeam[pPlayer] = iTeam;
	g_iPlayersNum[g_iUserTeam[pPlayer]]++;

	team_menu_check_respawn(pPlayer);

	return iTeam;
}

public _jbe_get_user_model(pPlayer, const szModel[], iLen)
{
	param_convert(2);
	return jbe_get_user_model(pPlayer, szModel, iLen);
}

public jbe_get_user_model(pPlayer, const szModel[], iLen)
{
	return get_member(pPlayer, m_szModel, szModel, iLen);
}

public _jbe_set_user_model(pPlayer, const szModel[])
{
	param_convert(2);
	jbe_set_user_model(pPlayer, szModel);
}

public jbe_set_user_model(pPlayer, const szModel[]) 
{
#define MODEL_CHANGE_TIME	0.2

	copy(g_szUserModel[pPlayer], charsmax(g_szUserModel[]), szModel);
	static Float:fGameTime, Float:fChangeTime;
	fGameTime = get_gametime();
	
	if (fGameTime - fChangeTime > MODEL_CHANGE_TIME)
	{
		jbe_set_user_model_fix(pPlayer + TASK_CHANGE_MODEL);
		fChangeTime = fGameTime;
	}
	else
	{
		set_task((fChangeTime + MODEL_CHANGE_TIME) - fGameTime, "jbe_set_user_model_fix", pPlayer + TASK_CHANGE_MODEL);
		fChangeTime = fChangeTime + MODEL_CHANGE_TIME;
	}
}

public jbe_set_user_model_fix(pPlayer)
{
	pPlayer -= TASK_CHANGE_MODEL;
	rg_set_user_model(pPlayer, g_szUserModel[pPlayer], .update_index = true);
	SetBit(g_iBitUserModel, pPlayer);
}

public jbe_informer_offset_up(pPlayer)
{
	g_fMainInformerPosX[pPlayer] = 0.21;
	g_fMainInformerPosY[pPlayer] = 0.01;

	if (IsSetBit(g_iBitButtonSound, pPlayer))
		client_cmd(pPlayer, "spk UI/buttonclick.wav");

	if (IsSetBit(g_iBitUserAlive, pPlayer) && g_iDayMode != DAY_DEATHMATCH)
	{
		if (g_bMafiaGame)
			jbe_main_mafia_informer(pPlayer + TASK_SHOW_INFORMER);
		else
			jbe_main_informer(pPlayer + TASK_SHOW_INFORMER);

		jbe_reset_user_informer(pPlayer);
	}
}

public jbe_informer_offset_down(pPlayer)
{	
	// При выходе игрока с сервера отрабатывается выход с новых меню если те были открыты
	if (IsNotSetBit(g_iBitUserConnected, pPlayer))
		return;

	ClearBit(g_iBitOpenMainMenu, pPlayer);
	
	// Чтоб на новых многостраничных меню информер не опускался
	new iMenuOld, iMenuNew;
	if (player_menu_info(pPlayer, iMenuOld, iMenuNew) && iMenuNew > -1)
		return;

	if (IsSetBit(g_iBitButtonSound, pPlayer))
		client_cmd(pPlayer, "spk UI/buttonrollover.wav");
	
	g_fMainInformerPosX[pPlayer] = 0.01;
	g_fMainInformerPosY[pPlayer] = 0.27;
}

// информер: true - поднят, false - опущен
jbe_get_informer_offset(pPlayer) return g_fMainInformerPosX[pPlayer] == 0.21;


public jbe_menu_block(pPlayer) SetBit(g_iBitBlockMenu, pPlayer);
public jbe_menu_unblock(pPlayer) ClearBit(g_iBitBlockMenu, pPlayer);
public jbe_menu_blocked(pPlayer) return IsSetBit(g_iBitBlockMenu, pPlayer);

public jbe_is_user_free(pPlayer) return IsSetBit(g_iBitUserFree, pPlayer);

public jbe_add_user_free(pPlayer)
{
	if (g_iDayMode != DAY_NORMAL || g_iUserTeam[pPlayer] != TEAM_PRISONER || IsNotSetBit(g_iBitUserAlive, pPlayer)
	|| IsSetBit(g_iBitUserFree, pPlayer) || IsSetBit(g_iBitUserWanted, pPlayer))
		return 0;
	
	new szName[32];
	get_user_name(pPlayer, szName, charsmax(szName));
	format(g_szFreeNames, charsmax(g_szFreeNames), "%s^n%s", g_szFreeNames, szName);
	g_iFreeLang = 1;
	SetBit(g_iBitUserFree, pPlayer);
	
	if (g_bSoccerStatus && IsSetBit(g_iBitUserSoccer, pPlayer))
	{
		ExecuteHamB(Ham_Player_ResetMaxSpeed, pPlayer);
		jbe_set_player_prisoner_model(pPlayer);
		jbe_default_knife_model(pPlayer);
		UTIL_WeaponAnimation(pPlayer, 3);
		set_member(pPlayer, m_bloodColor, 247);

		jbe_sub_user_soccer(pPlayer);
	}

	if (g_iBoxingGame && IsSetBit(g_iBitUserBoxing, pPlayer))
	{
		ClearBit(g_iBitUserBoxing, pPlayer);
		ExecuteHamB(Ham_Player_ResetMaxSpeed, pPlayer);
		jbe_set_hand_model(pPlayer);
		UTIL_WeaponAnimation(pPlayer, 3);
		set_entvar(pPlayer, var_health, 100.0);
		set_member(pPlayer, m_bloodColor, 247);
	}
	
	if (g_iPersonalModel[pPlayer] > 0)
		jbe_default_player_model(pPlayer);
	else
		set_entvar(pPlayer, var_skin, SKIN_FREE);
		
	remove_task(pPlayer + TASK_FREE_DAY_ENDED);
	set_task(float(g_eAllCvars[FREE_DAY_ID]), "jbe_sub_user_free", pPlayer + TASK_FREE_DAY_ENDED);
		
	return 1;
}

public jbe_add_user_free_next_round(pPlayer)
{
	if (g_iUserTeam[pPlayer] != TEAM_PRISONER) return 0;
	SetBit(g_iBitUserFreeNextRound, pPlayer);
	return 1;
}

public jbe_sub_user_free(pPlayer)
{
	if (pPlayer > TASK_FREE_DAY_ENDED)
		pPlayer -= TASK_FREE_DAY_ENDED;
		
	if (IsNotSetBit(g_iBitUserFree, pPlayer)) return 0;

	ClearBit(g_iBitUserFree, pPlayer);
	remove_task(pPlayer + TASK_FREE_DAY_ENDED);

	if (g_szFreeNames[0] != EOS)
	{
		new szName[34];
		get_user_name(pPlayer, szName, charsmax(szName));
		format(szName, charsmax(szName), "^n%s", szName);
		replace(g_szFreeNames, charsmax(g_szFreeNames), szName, "");
		g_iFreeLang = (g_szFreeNames[0] != 0);
	}
	
	if (IsSetBit(g_iBitUserAlive, pPlayer))
	{
		if (g_iPersonalModel[pPlayer] > 0)
			jbe_default_player_model(pPlayer);
		else
			set_pev(pPlayer, pev_skin, g_iUserSkin[pPlayer]);

		set_hudmessage(0, 255, 0, -1.0, 0.8, 0, 6.0, 2.0, 0.1, 0.1, -1);
		ShowSyncHudMsg(pPlayer, g_iSyncInfoCenter, "Ваш выходной окончен");

		if (g_iChiefId)
		{
			set_hudmessage(0, 255, 0, 0.7, 0.35, 0, 6.0, 2.0, 0.1, 0.1, -1);
			ShowSyncHudMsg(g_iChiefId, g_iSyncInfoCenter, "У « %n »^nОкончен выходной", pPlayer);
		}
	}

	return 1;
}

public jbe_free_day_start()
{
	if (g_iDayMode != DAY_NORMAL) return 0;
	for (new iPlayer = 1; iPlayer <= MaxClients; iPlayer++) {
		if (g_iUserTeam[iPlayer] == TEAM_PRISONER && IsSetBit(g_iBitUserAlive, iPlayer)
		&& IsNotSetBit(g_iBitUserWanted, iPlayer)) {
			if (IsSetBit(g_iBitUserFree, iPlayer))
				remove_task(iPlayer+TASK_FREE_DAY_ENDED);
			else {
				SetBit(g_iBitUserFree, iPlayer);
				
				if (g_bSoccerStatus && IsSetBit(g_iBitUserSoccer, iPlayer))
				{
					ExecuteHamB(Ham_Player_ResetMaxSpeed, iPlayer);
					jbe_set_player_prisoner_model(iPlayer);
					jbe_default_knife_model(iPlayer);
					UTIL_WeaponAnimation(iPlayer, 3);
					set_member(iPlayer, m_bloodColor, 247);
					jbe_sub_user_soccer(iPlayer);
				}
				
				if (g_iBoxingGame && IsSetBit(g_iBitUserBoxing, iPlayer)) {
					ClearBit(g_iBitUserBoxing, iPlayer);
					ExecuteHamB(Ham_Player_ResetMaxSpeed, iPlayer);
					jbe_set_hand_model(iPlayer);
					UTIL_WeaponAnimation(iPlayer, 3);
					set_entvar(iPlayer, var_health, 100.0);
					set_member(iPlayer, m_bloodColor, 247);
				}

				if (!(g_iPersonalModel[iPlayer] > 0))
					set_entvar(iPlayer, var_skin, SKIN_FREE);
			}
		}
	}
	
	if (g_bSoccerStatus) jbe_soccer_disable_all();
	if (g_iBoxingGame) jbe_boxing_disable_all();
	if (g_iDuelStatus && !g_iLastPnId) jbe_duel_cancel();
	
	g_szFreeNames = ""; // g_szFreeNames[0] = EOS;
	g_iFreeLang = 0;
	jbe_open_doors();
	jbe_set_day_mode(2); // DAY_FREEDAY
	jbe_set_name_chief();
	g_iFriendlyFire = FF_OFF;
	g_iChiefGameType = GAME_NONE;
	jbe_set_name_mode("Выходной");
	g_iDayModeTimer = g_eAllCvars[FREE_DAY_ALL] + 1;
	set_task(1.0, "jbe_free_day_ended_task", TASK_FREE_DAY_ENDED, _, _, "a", g_iDayModeTimer);
	return 1;
}
public jbe_free_day_ended_task()
{
	if(--g_iDayModeTimer) formatex(g_szDayModeTimer, charsmax(g_szDayModeTimer), "[%i]", g_iDayModeTimer);
	else jbe_free_day_ended();
}
public jbe_free_day_ended()
{
	if (g_iDayMode != DAY_FREEDAY) return 0;
	g_szDayModeTimer = "";
	remove_task(TASK_FREE_DAY_ENDED);
	for (new iPlayer = 1; iPlayer <= MaxClients; iPlayer++)
	{
		if (IsSetBit(g_iBitUserFree, iPlayer))
		{
			ClearBit(g_iBitUserFree, iPlayer);
			if (g_iPersonalModel[iPlayer] > 0)
				jbe_default_player_model(iPlayer);
			else
				set_pev(iPlayer, pev_skin, g_iUserSkin[iPlayer]);
		}
	}
	if (!g_iLastPnId)
	{
		rg_send_audio(0, sound_freeday_end);
		jbe_set_name_chief(g_iChiefId);
	}
	jbe_set_day_mode(1);
	jbe_set_name_mode("");
	if (g_iChiefId)
		jbe_set_name_chief(g_iChiefId);
	return 1;
}

public jbe_is_user_wanted(pPlayer) return IsSetBit(g_iBitUserWanted, pPlayer);

public jbe_add_user_wanted(pPlayer)
{
	if (g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY || g_iUserTeam[pPlayer] != TEAM_PRISONER
	|| IsNotSetBit(g_iBitUserAlive, pPlayer) || IsSetBit(g_iBitUserWanted, pPlayer) || g_iLastPnId || is_shief_global_game())
		return 0;

	new szName[34];
	get_user_name(pPlayer, szName, charsmax(szName));
	format(szName, charsmax(szName), "^n%s", szName);
	format(g_szWantedNames, charsmax(g_szWantedNames), "%s%s", g_szWantedNames, szName);
	SetBit(g_iBitUserWanted, pPlayer);
	g_iWantedLang = 1;

	if (IsSetBit(g_iBitUserFree, pPlayer))
	{
		ClearBit(g_iBitUserFree, pPlayer);
		if (g_szFreeNames[0] != 0)
		{
			replace(g_szFreeNames, charsmax(g_szFreeNames), szName, "");
			g_iFreeLang = (g_szFreeNames[0] != 0);
		}

		if (g_iDayMode == DAY_NORMAL && task_exists(pPlayer+TASK_FREE_DAY_ENDED))
			remove_task(pPlayer + TASK_FREE_DAY_ENDED);
	}

	if (IsSetBit(g_iBitUserSoccer, pPlayer))
	{
		ClearBit(g_iBitUserSoccer, pPlayer);
		ExecuteHamB(Ham_Player_ResetMaxSpeed, pPlayer);
		jbe_default_knife_model(pPlayer);
		UTIL_WeaponAnimation(pPlayer, 3);
		set_member(pPlayer, m_bloodColor, 247);
		if (pPlayer == g_iSoccerBallOwner)
		{
			CREATE_KILLPLAYERATTACHMENTS(pPlayer);
			set_entvar(g_iSoccerBall, var_solid, SOLID_TRIGGER);
			set_entvar(g_iSoccerBall, var_velocity, {0.0, 0.0, 0.1});
			g_iSoccerBallOwner = 0;
		}
		//if(g_bSoccerGame) remove_task(pPlayer+TASK_SHOW_SOCCER_SCORE);
	}
	if (IsSetBit(g_iBitUserBoxing, pPlayer))
	{
		ClearBit(g_iBitUserBoxing, pPlayer);
		ExecuteHamB(Ham_Player_ResetMaxSpeed, pPlayer);
		jbe_set_hand_model(pPlayer);
		UTIL_WeaponAnimation(pPlayer, 3);
		set_entvar(pPlayer, var_health, 100.0);
		set_member(pPlayer, m_bloodColor, 247);
	}
	
	jbe_default_player_model(pPlayer);
	
	return 1;
}

public jbe_sub_user_wanted(pPlayer)
{
	if (IsNotSetBit(g_iBitUserWanted, pPlayer)) return 0;
	ClearBit(g_iBitUserWanted, pPlayer);
	ClearBit(g_iBitUserRealWanted, pPlayer);
	if (g_szWantedNames[0] != 0)
	{
		new szName[34];
		get_user_name(pPlayer, szName, charsmax(szName));
		format(szName, charsmax(szName), "^n%s", szName);
		replace(g_szWantedNames, charsmax(g_szWantedNames), szName, "");
		g_iWantedLang = (g_szWantedNames[0] != 0);
	}
	if (IsSetBit(g_iBitUserAlive, pPlayer))
	{
		if (g_iDayMode == DAY_FREEDAY)
		{
			SetBit(g_iBitUserFree, pPlayer);
			if (g_iPersonalModel[pPlayer])
				jbe_default_player_model(pPlayer);
			else
				set_entvar(pPlayer, var_skin, SKIN_FREE);
		}
		else
		{
			if (g_iPersonalModel[pPlayer])
				jbe_default_player_model(pPlayer);
			else
				set_entvar(pPlayer, var_skin, g_iUserSkin[pPlayer]);
		}
	}
	return 1;
}

public jbe_is_user_chief(pPlayer) return (pPlayer == g_iChiefId);

public jbe_set_user_chief(pPlayer)
{
	if (g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY
	|| g_iUserTeam[pPlayer] != TEAM_GUARD || IsNotSetBit(g_iBitUserAlive, pPlayer))
		return 0;

	new iOldChief = g_iChiefId;
	g_iChiefId = pPlayer;

	if (g_iChiefStatus == CHIEF_ALIVE)
	{
		jbe_default_player_model(iOldChief);

		if (~(1<<g_iChiefGameType) & (1<<GAME_SHTURM|1<<GAME_M9SO|1<<GAME_GHOST|1<<GAME_SNIPER))
		{
			if (jbe_get_user_godmode(iOldChief))
				jbe_set_user_godmode(iOldChief, 0);
		
			if (get_user_health(iOldChief) > 100)
				set_entvar(iOldChief, var_health, 100.0);
		
			if (get_user_armor(iOldChief) > 100)
				set_entvar(iOldChief, var_armorvalue, 100.0);
		}
	}

	remove_task(TASK_CHIEF_CHOICE_TIME);
	g_iChiefStatus = CHIEF_ALIVE;
	
	if (!g_iLastPnId)
		jbe_set_name_chief(pPlayer);

	client_print_color(pPlayer, print_team_blue, "%s ^1%s^1.", g_szChatPrefix,
		g_szInfoChief[ random (sizeof g_szInfoChief) ]);
	
	if (IsNotSetBit(g_iBitUserDuel, pPlayer)
	&& (~(1<<g_iChiefGameType) & (1<<GAME_SHTURM|1<<GAME_M9SO|1<<GAME_GHOST|1<<GAME_SNIPER)))
	{
		set_entvar(pPlayer, var_health, 250.0);
		rg_set_user_armor(pPlayer, 250, ARMOR_VESTHELM);
	}

	jbe_default_player_model(pPlayer);
	
	if (g_bSoccerStatus)
	{
		if (IsSetBit(g_iBitUserSoccer, pPlayer))
		{
			ClearBit(g_iBitUserSoccer, pPlayer);
			ExecuteHamB(Ham_Player_ResetMaxSpeed, pPlayer);
			jbe_set_baton_model(pPlayer);
			UTIL_WeaponAnimation(pPlayer, 3);
			set_member(pPlayer, m_bloodColor, 247);
			if (pPlayer == g_iSoccerBallOwner)
			{
				CREATE_KILLPLAYERATTACHMENTS(pPlayer);
				set_entvar(g_iSoccerBall, var_solid, SOLID_TRIGGER);
				set_entvar(g_iSoccerBall, var_velocity, {0.0, 0.0, 0.1});
				g_iSoccerBallOwner = 0;
			}
		}
	}
	return 1;
}

public jbe_get_chief_status() return g_iChiefStatus;

public jbe_get_chief_id() return g_iChiefId;

public jbe_set_user_costumes(pPlayer, iCostumes)
{
	if (g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY && g_iDayMode != DAY_DEATHMATCH)
		return 0;
	
	if (iCostumes)
	{
		new aDataCostumes[DATA_COSTUMES_FILE];
		ArrayGetArray(g_aDataCostumes, iCostumes, aDataCostumes);

		if (!g_eUserCostumes[pPlayer][ENTITY])
		{
			g_eUserCostumes[pPlayer][ENTITY] = rg_create_entity("func_wall", .useHashTable = true);
			set_entvar(g_eUserCostumes[pPlayer][ENTITY], var_movetype, MOVETYPE_FOLLOW);
			set_entvar(g_eUserCostumes[pPlayer][ENTITY], var_aiment, pPlayer);
			entity_set_model(g_eUserCostumes[pPlayer][ENTITY], aDataCostumes[COSTUME_FILE]);
			set_entvar(g_eUserCostumes[pPlayer][ENTITY], var_body, iCostumes - 1);
			set_entvar(g_eUserCostumes[pPlayer][ENTITY], var_sequence, 0);
			set_entvar(g_eUserCostumes[pPlayer][ENTITY], var_animtime, get_gametime());
			set_entvar(g_eUserCostumes[pPlayer][ENTITY], var_framerate, 1.0);
		}
		else
		{
			entity_set_model(g_eUserCostumes[pPlayer][ENTITY], aDataCostumes[COSTUME_FILE]);
			set_entvar(g_eUserCostumes[pPlayer][ENTITY], var_body, iCostumes - 1);
		}

		g_eUserCostumes[pPlayer][HIDE] = false;
		g_eUserCostumes[pPlayer][COSTUMES] = iCostumes;

		set_entvar(g_eUserCostumes[pPlayer][ENTITY], var_groupinfo, GI_COSTUMES);

		return 1;
	}
	else if (g_eUserCostumes[pPlayer][COSTUMES])
	{
		if (g_eUserCostumes[pPlayer][ENTITY])
		{
			set_entvar(g_eUserCostumes[pPlayer][ENTITY], var_flags, FL_KILLME);
		}

		g_eUserCostumes[pPlayer][ENTITY] = 0;
		g_eUserCostumes[pPlayer][HIDE] = false;
		g_eUserCostumes[pPlayer][COSTUMES] = 0;
		return 1;
	}
	return 0;
}

public jbe_hide_user_costumes(pPlayer)
{
	if (g_eUserCostumes[pPlayer][ENTITY])
	{
		set_entvar(g_eUserCostumes[pPlayer][ENTITY], var_flags, FL_KILLME);

		g_eUserCostumes[pPlayer][ENTITY] = 0;
		g_eUserCostumes[pPlayer][HIDE] = true;
		return 1;
	}
	return 0;
}

public jbe_prisoners_divide_color(iTeam) // , iGame = 0
{
	if (g_iDayMode != DAY_NORMAL || g_iAlivePlayersNum[TEAM_PRISONER] < 2 || iTeam < 2 || iTeam > 6)
		return 0;
	
	new const szLangPlayer[][] = { "оранжевых", "серых", "жёлтых", "синих", "коричневых", "черных"/*, "зеленых", "красных"*/}; // new +2
	
	// случайная сортировка командам
	new iTeamsID[6] = { 0, 1, 2, 3, 4, 5 };
	SortIntegers(iTeamsID, sizeof iTeamsID, Sort_Random);

	for (new iPlayer = 1, iColor; iPlayer <= MaxClients; iPlayer++)
	{
		if (g_iUserTeam[iPlayer] != TEAM_PRISONER || IsNotSetBit(g_iBitUserAlive, iPlayer) || IsSetBit(g_iBitUserFree, iPlayer)
		|| IsSetBit(g_iBitUserWanted, iPlayer) || IsSetBit(g_iBitUserSoccer, iPlayer) || IsSetBit(g_iBitUserBoxing, iPlayer)
		|| IsSetBit(g_iBitUserDuel, iPlayer)) continue;
		
		client_print_color(iPlayer, 0, "%s ^1Вы в команде ^4%s^1.", g_szChatPrefix, szLangPlayer[iTeamsID[iColor]]);
		
		if (g_iPersonalModel[iPlayer] > 0)
			jbe_set_player_prisoner_model(iPlayer);

		set_entvar(iPlayer, var_skin, iTeamsID[iColor]);

		g_iUserGameTeam[iPlayer] = iTeamsID[iColor];
		
		if (++iColor >= iTeam)
			iColor = 0;
	}
	return 1;
}

//native jbe_register_day_mode(szLang[32], iBlock, iTime, szSoundFile[128], szSoundName[64], iSoundTime);
public jbe_register_day_mode(iPluginId, iNumParams)
{
	new aDataDayMode[DATA_DAY_MODE];
	get_string(1, aDataDayMode[LANG_MODE], charsmax(aDataDayMode[LANG_MODE]));
	aDataDayMode[MODE_BLOCK_DAYS] = get_param(2);
	aDataDayMode[MODE_TIMER] = get_param(3);

	// game sound
	new aDataDayModeMusic[DATA_DAY_MODE_MUSIC];
	get_string(4, aDataDayModeMusic[DM_FILE_NAME], charsmax(aDataDayModeMusic[DM_FILE_NAME]));
	if (aDataDayModeMusic[DM_FILE_NAME][0])
	{
		get_string(5, aDataDayModeMusic[DM_TRACK_NAME], charsmax(aDataDayModeMusic[DM_TRACK_NAME]));
		aDataDayModeMusic[DM_TRACK_TIME] = get_param(6);
		ArrayPushArray(g_aDataGameMusic, aDataDayModeMusic);
		g_iGameMusicSize++;
		aDataDayMode[MODE_TRACK] = g_iGameMusicSize;
	}

	ArrayPushArray(g_aDataDayMode, aDataDayMode);

	// Pause Game Mode plugin after registering
	new szFileName[64];
	get_plugin(iPluginId, szFileName, charsmax(szFileName));
	ArrayPushString(g_aFileDayMode, szFileName);
	pause("ac", szFileName); 

	g_iDayModeListSize++;
	return g_iDayModeListSize - 1;
}

public jbe_get_user_voice(pPlayer) return IsSetBit(g_iBitUserVoice, pPlayer);
public jbe_set_user_voice(pPlayer)
{
	if(g_iDayMode != 1 && g_iDayMode != 2 || g_iUserTeam[pPlayer] != 1 || IsNotSetBit(g_iBitUserAlive, pPlayer)) return 0;
	SetBit(g_iBitUserVoice, pPlayer);
	
	reload_user_voice(pPlayer);

	return 1;
}
public jbe_set_user_voice_next_round(pPlayer)
{
	if(g_iUserTeam[pPlayer] != 1) return 0;
	SetBit(g_iBitUserVoiceNextRound, pPlayer);
	return 1;
}

public _jbe_get_user_rendering(pPlayer, &iRenderFx, &iRed, &iGreen, &iBlue, &iRenderMode, &iRenderAmt)
{
	for(new i = 2; i <= 7; i++) param_convert(i);
	jbe_get_user_rendering(pPlayer, iRenderFx, iRed, iGreen, iBlue, iRenderMode, iRenderAmt);
}

public jbe_get_user_rendering(pPlayer, &iRenderFx, &iRed, &iGreen, &iBlue, &iRenderMode, &iRenderAmt)
{
	new Float:fRenderColor[3];
	iRenderFx = get_entvar(pPlayer, var_renderfx);
	get_entvar(pPlayer, var_rendercolor, fRenderColor);
	iRed = floatround(fRenderColor[0]);
	iGreen = floatround(fRenderColor[1]);
	iBlue = floatround(fRenderColor[2]);
	iRenderMode = get_entvar(pPlayer, var_rendermode);
	new Float:fRenderAmt;
	get_entvar(pPlayer, var_renderamt, fRenderAmt);
	iRenderAmt = floatround(fRenderAmt);
}

public jbe_set_user_rendering(pPlayer, iRenderFx, iRed, iGreen, iBlue, iRenderMode, iRenderAmt)
{
	new Float:flRenderColor[3];

	flRenderColor[0] = float(iRed);
	flRenderColor[1] = float(iGreen);
	flRenderColor[2] = float(iBlue);

	set_entvar(pPlayer, var_renderfx, iRenderFx);
	set_entvar(pPlayer, var_rendercolor, flRenderColor);
	set_entvar(pPlayer, var_rendermode, iRenderMode);
	set_entvar(pPlayer, var_renderamt, float(iRenderAmt));
}

// Сохраняем текущее свечение (чтоб если что вернуть).
public jbe_save_user_rendering(pPlayer)
{
	jbe_get_user_rendering(pPlayer,
		g_eUserRendering[pPlayer][RENDER_FX],
		g_eUserRendering[pPlayer][RENDER_RED],
		g_eUserRendering[pPlayer][RENDER_GREEN],
		g_eUserRendering[pPlayer][RENDER_BLUE],
		g_eUserRendering[pPlayer][RENDER_MODE],
		g_eUserRendering[pPlayer][RENDER_AMT]);

	if (g_eUserRendering[pPlayer][RENDER_FX] != kRenderFxNone || g_eUserRendering[pPlayer][RENDER_MODE] != kRenderNormal)
		g_eUserRendering[pPlayer][RENDER_STATUS] = true;
	else
		g_eUserRendering[pPlayer][RENDER_STATUS] = false;
}

// Возвращаем исходное свечение (не удаляем).
jbe_reset_user_rendering(pPlayer)
{
	if (g_eUserRendering[pPlayer][RENDER_STATUS])
	{
		jbe_set_user_rendering(pPlayer,
			g_eUserRendering[pPlayer][RENDER_FX],
			g_eUserRendering[pPlayer][RENDER_RED],
			g_eUserRendering[pPlayer][RENDER_GREEN],
			g_eUserRendering[pPlayer][RENDER_BLUE],
			g_eUserRendering[pPlayer][RENDER_MODE],
			g_eUserRendering[pPlayer][RENDER_AMT]);
	}
	else
		jbe_set_user_rendering(pPlayer, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
}

jbe_set_user_maxspeed(id, Float:fSpeed = 0.0, bool:bResetSpeed = false)
{
	g_fUserMaxSpeed[id] = fSpeed;

	fSpeed ? SetBit(g_iBitFastRun, id) : ClearBit(g_iBitFastRun, id);

	if (bResetSpeed)
		ExecuteHamB(Ham_Player_ResetMaxSpeed, id);
}

public jbe_is_wanted() return g_iBitUserWanted ? true : false;

jbe_is_team_balance()
{
	new Float:fPnNum;
	fPnNum += g_iPlayersNum[TEAM_PRISONER] - 1;
	
	new Float:fNum;

	// + 0.50 (будет минус один зек для баланса) // + 0.25 (+ 1 зек к балансу).
	fNum = fPnNum / g_eAllCvars[TEAM_BALANCE] + 0.50;
	
	new iNum = floatround(fNum, floatround_floor); // floatround(fNum, floatround_ceil)	
	if (!iNum) iNum = 1;
	if (iNum > g_iPlayersNum[TEAM_GUARD])
		return true;

	return false;
}

bool:jbe_user_buried(iTarget, bool:bSet = true)
{
	if (IsNotSetBit(g_iBitUserAlive, iTarget))
		return false;

	switch (bSet)
	{
		case true:
		{
			if (IsSetBit(g_iBitUserBuried, iTarget))
				return true;

			// не стоит на земле
			if (!(pev(iTarget, pev_flags) & FL_ONGROUND)) 
			{ 
				// пытаемся опустить на землю
				engfunc(EngFunc_DropToFloor, iTarget); 
				if (!(pev(iTarget, pev_flags) & FL_ONGROUND))
					return false;
			}

			new Float:vOrigin[3];
			pev(iTarget, pev_origin, vOrigin);
			vOrigin[2] -= 10;
			set_pev(iTarget, pev_origin, vOrigin);

			SetBit(g_iBitUserBuried, iTarget);

			return true;
		}
		case false:
		{
			if (IsNotSetBit(g_iBitUserBuried, iTarget))
				return true;

			new Float:vOrigin[3];
			pev(iTarget, pev_origin, vOrigin);
			vOrigin[2] += 10;
			set_pev(iTarget, pev_origin, vOrigin);

			ClearBit(g_iBitUserBuried, iTarget);

			return true;
		}
	}
	return false;
}

public jbe_set_user_godmode(id, iType)
{
	set_entvar(id, var_takedamage, iType ? DAMAGE_NO : DAMAGE_YES);
	SetBitBool(g_iBitUserGodMode, id, iType);
}

jbe_get_user_godmode(id) return IsSetBit(g_iBitUserGodMode, id);

jbe_set_user_view(id, iType)
{
	if (iType)
	{
		SetBit(g_iBitUser3DView, id);
		set_view(id, CAMERA_3RDPERSON);
		client_cmd(id, "stopsound"); // fix voice
	}
	else
	{
		ClearBit(g_iBitUser3DView, id);

		// если речь об отключенном
		if (IsSetBit(g_iBitUserConnected, id))
		{
			set_view(id, CAMERA_NONE);
			client_cmd(id, "stopsound"); // fix voice
		}
	}
}

jbe_reset_user_groupinfo(id)
{
	if (IsNotSetBit(g_iBitOffCostumes, id))
		set_entvar(id, var_groupinfo, GI_PLAYER|GI_COSTUMES);
	else
		set_entvar(id, var_groupinfo, GI_PLAYER);
}

/*===== <- Нативы <- =====*///}