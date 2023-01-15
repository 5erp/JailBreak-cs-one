/*===== -> Остальной хлам -> =====*///{


// hot fix TODO: Сделать норм :D
is_player_model_anims(id) return id;

/*
	Access flags:
//	a - immunity (can't be kicked/baned/slayed/slaped and affected by other commmands) - immun
	b - reservation (can join on reserved slots)
	c - amx_kick command
//	d - amx_ban and amx_unban commands	Admin
	e - amx_slay and amx_slap commands
	f - amx_map command
	g - amx_cvar command (not all cvars will be available)
	h - amx_cfg command
	i - amx_chat and other chat commands
	j - amx_vote and other vote commands
	k - access to sv_password cvar (by amx_cvar command)
	l - access to amx_rcon command and rcon_password cvar (by amx_cvar command)
	m - custom level A	model pack
	n - custom level B	// 
	o - custom level C	// 
//	p - custom level D	Global
//	q - custom level E	Girl
//	r - custom level F	Grab
//	s - custom level G	Hook
//	t - custom level H	VIP
	u - menu access
	z - user (no admin)
*/
jbe_get_user_privilegies(id)
{
	new iFlags = get_user_flags(id);
	if (iFlags & ADMIN_LEVEL_E) /*q*/ SetBit(g_iBitGirl, id);
	if (iFlags & ADMIN_LEVEL_H) /*t*/ 
	{
		SetBit(g_iBitUserVip, id);
		ClearBit(g_iBitUserNightVip, id);
	}
	if (iFlags & ADMIN_BAN)		/*d*/ SetBit(g_iBitUserAdmin, id);
	if (iFlags & ADMIN_LEVEL_G) /*s*/ 
	{
		SetBit(g_iBitUserHook, id);
		if (IsNotSetBit(g_iBitUserSqlLoad, id))
			set_hook_setting_default(id);
	}
	if (iFlags & ADMIN_LEVEL_D) /*p*/ 
	{
		SetBit(g_iBitUserGlobalAdmin, id);
		arrayset(g_iGlobalSkillType[id], 1, sizeof(g_iGlobalSkillType[]));
	}
	if (iFlags & ADMIN_LEVEL_A) /*m*/ SetBit(g_iBitUserModelsPack, id);
	if (iFlags & ADMIN_LEVEL_F) /*r*/
	{
		SetBit(g_iBitUserGrab, id);
		if (IsNotSetBit(g_iBitUserSqlLoad, id))
			g_iGrabMenuType[id] = GRAB_MENU_NORMAL;
	}
	if (iFlags & ADMIN_RCON) /*l*/
		SetBit(g_iBitRconAdmin, id); 

	jbe_get_user_prefix_flag(id);

	get_user_personal_models(id);
	
	if (g_iPersonalModel[id] > 0 && IsSetBit(g_iBitUserAlive, id))
		jbe_default_player_model(id);

	reload_user_voice(id);

	if (IsNotSetBit(g_iBitUserVip, id))
		check_vip_night(id);
}

jbe_clear_user_privilegies(id)
{
	ClearBit(g_iBitUserAdmin, id);
	ClearBit(g_iBitUserVip, id);
	ClearBit(g_iBitVipResAccept, id);
	ClearBit(g_iBitUserNightVip, id);
	ClearBit(g_iBitGirl, id);
	ClearBit(g_iBitUserGrab, id);
	ClearBit(g_iBitRconAdmin, id);

	if (IsSetBit(g_iBitUserHook, id))
	{
		ClearBit(g_iBitUserHook, id);
		ClearBit(g_iBitHookUse, id);
		remove_task(id + TASK_HOOK_THINK);
	}

	if (IsSetBit(g_iBitUserGlobalAdmin, id))
	{
		global_clear_blocks(id);
		ClearBit(g_iBitUserGlobalAdmin, id);
	}

	if (g_iPersonalModel[id] || g_iPersonalModelsUserSize[id]
	|| IsSetBit(g_iBitUserModelsPack, id))
	{
		if (g_aPersonalModelsUser[id])
			ArrayClear(g_aPersonalModelsUser[id]);

		g_iPersonalModel[id] = 0;
		g_iPersonalModelsUserSize[id] = 0;
		ClearBit(g_iBitUserModelsPack, id);

		if (IsSetBit(g_iBitUserAlive, id))
			jbe_default_player_model(id);
	}

	if (IsSetBit(g_iBitUserConnected, id))
		reload_user_voice(id);
}

// Ham Block ;)
eHamBlock:jbe_set_hamblock(any:...)
{
	for (new i; i < numargs(); i++)
	{
		EnableHamForward(g_iHamHookForwards[getarg(i)]);
	}
}

jbe_create_buyzone()
{
	new iEntity = rg_create_entity("func_buyzone", true);
	set_entvar(iEntity, var_iuser1, IUSER1_BUYZONE_KEY);

	// чтоб юзать байзону везде
	dllfunc(DLLFunc_Spawn, iEntity);
	set_entvar(iEntity, var_mins, Float:{-8192.0, -8192.0, -8192.0});
	set_entvar(iEntity, var_maxs, Float:{8192.0, 8192.0, 8192.0});
	engfunc(EngFunc_SetSize, iEntity, Float:{-8192.0, -8192.0, -8192.0}, Float:{8192.0, 8192.0, 8192.0});
	set_entvar(iEntity, var_team, 0);
}

check_adverts_name(id)
{
	new szName[MAX_NAME_LENGTH];
	get_user_name(id, szName, charsmax(szName));

	if (IsAdvertsString(szName))
	{
		new const szNewName[] = "CS-ONE | Bad Name";
		set_entvar(id, var_netname, szNewName);
		set_key_value(engfunc(EngFunc_GetInfoKeyBuffer, id), "name", szNewName);
		rh_update_user_info(id);
	}
}

/* Паинт, в pre thinh*/
paint_draw(id)
{
	static iCounter[MAX_PLAYERS + 1];
	static Float:vecOrigin[MAX_PLAYERS + 1][3];

	if (++iCounter[id] > 5)
	{
		if (!is_aiming_at_sky(id))
		{
			static Float:curOrigin[3], Float:fDistance;

			curOrigin = vecOrigin[id];
			
			if (IsNotSetBit(g_iBitPaintHolding, id))
			{
			   fm_get_aim_origin(id, vecOrigin[id]);
			   move_toward_client(id, vecOrigin[id]);
			   SetBit(g_iBitPaintHolding, id);
			   return;
			}
			
			fm_get_aim_origin(id, vecOrigin[id]);
			move_toward_client(id, vecOrigin[id]);
			
			fDistance = get_distance_f(vecOrigin[id], curOrigin);
			
			if (fDistance > 3) // 2
			{
			   draw_line(vecOrigin[id], curOrigin, g_pSpriteLgtning, _, 10, 255, 50,
			      _, random(255), random(255),random(255), 255, _);
			}
		}
		else
			ClearBit(g_iBitPaintHolding, id);

		iCounter[id] = 0;
	}
	return;
}

public jbe_user_defrost(pPlayer)
{
	pPlayer -= TASK_FROSTNADE_DEFROST;
	
	if (IsNotSetBit(g_iBitUserFrozen, pPlayer)) return;
	ClearBit(g_iBitUserFrozen, pPlayer);
	
	if (IsNotSetBit(g_iBitUserAlive, pPlayer)) return;

	set_entvar(pPlayer, var_flags, get_entvar(pPlayer, var_flags) & ~FL_FROZEN);
	set_member(pPlayer, m_flNextAttack, 0.0);
	jbe_reset_user_rendering(pPlayer);
	emit_sound(pPlayer, CHAN_AUTO, sound_defrost_player, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	new Float:vecOrigin[3];
	get_entvar(pPlayer, var_origin, vecOrigin);
	CREATE_BREAKMODEL(vecOrigin, _, _, 10, g_pModelGlass, 10, 25, 0x01);
}

jbe_default_player_model(pPlayer)
{
	new iModel;

	switch (g_iUserTeam[pPlayer])
	{
		case TEAM_PRISONER:
		{
			if (g_iPersonalModel[pPlayer] > 0 && IsNotSetBit(g_iBitUserWanted, pPlayer) && IsNotSetBit(g_iBitUserFree, pPlayer))
				return jbe_personal_model_set(pPlayer);

			iModel = IsSetBit(g_iBitGirl, pPlayer) ? PRISONER_GIRL : PRISONER;

			if (IsSetBit(g_iBitUserWanted, pPlayer)) 
				set_entvar(pPlayer, var_skin, SKIN_WANTED);

			else if (IsSetBit(g_iBitUserFree, pPlayer))
				set_entvar(pPlayer, var_skin, SKIN_FREE);

			else
				set_entvar(pPlayer, var_skin, g_iUserSkin[pPlayer]);
		}
		case TEAM_GUARD:
		{
			if (g_iPersonalModel[pPlayer] > 0)
				return jbe_personal_model_set(pPlayer);
						
			if (g_iChiefId == pPlayer)
				iModel = IsSetBit(g_iBitGirl, pPlayer) ? CHIEF_GIRL : CHIEF;

			else
				iModel = IsSetBit(g_iBitGirl, pPlayer) ? GUARD_GIRL : GUARD;
		}
		default: return PLUGIN_HANDLED;
	}

	jbe_set_user_model(pPlayer, models_player[iModel]);
	set_entvar(pPlayer, var_body, models_player_body[iModel]);

	return PLUGIN_CONTINUE;
}

jbe_personal_model_set(pPlayer)
{
	new aDataPersonalModels[DATA_PERSONAL_MODELS];
	ArrayGetArray(g_aDataPersonalModels, g_iPersonalModel[pPlayer], aDataPersonalModels);
	jbe_set_user_model(pPlayer, aDataPersonalModels[MODEL_PM]);
	new iSubModel = str_to_num(aDataPersonalModels[SUB_MODEL_PM]);
	set_entvar(pPlayer, var_body, iSubModel - 1);
	return PLUGIN_CONTINUE;
}

jbe_set_player_prisoner_model(pPlayer)
{
	new iModel = IsSetBit(g_iBitGirl, pPlayer) ? PRISONER_GIRL : PRISONER;
	jbe_set_user_model(pPlayer, models_player[iModel]);
	set_entvar(pPlayer, var_body, models_player_body[iModel]);
}

jbe_set_player_guard_model(pPlayer)
{
	new iModel = IsSetBit(g_iBitGirl, pPlayer) ? GUARD_GIRL : GUARD;
	jbe_set_user_model(pPlayer, models_player[iModel]);
	set_entvar(pPlayer, var_body, models_player_body[iModel]);
}

jbe_default_knife_model(pPlayer)
{
	switch (g_iUserTeam[pPlayer])
	{
		case TEAM_PRISONER:
		{
			if (g_iUserBraceletsTime[pPlayer])
				jbe_set_bracelets_model(pPlayer);
			else
				jbe_set_hand_model(pPlayer);	
		}
		case TEAM_GUARD: jbe_set_baton_model(pPlayer);
	}
}

jbe_set_hand_model(pPlayer)
{
	set_entvar(pPlayer, var_viewmodel, model_v_hand);
	set_entvar(pPlayer, var_weaponmodel, model_p_hand);
	set_member(pPlayer, m_flNextAttack, 0.75);
}

jbe_set_baton_model(pPlayer)
{
	set_entvar(pPlayer, var_viewmodel, model_v_baton);
	set_entvar(pPlayer, var_weaponmodel, model_p_baton);
	set_member(pPlayer, m_flNextAttack, 0.75);
}

jbe_set_balisong_model(pPlayer)
{
	set_entvar(pPlayer, var_viewmodel, model_v_balisong);
	set_entvar(pPlayer, var_weaponmodel, model_p_balisong);
	set_member(pPlayer, m_flNextAttack, 0.95);
}

jbe_set_baseballbat_model(pPlayer)
{
	set_entvar(pPlayer, var_viewmodel, model_v_baseballbat);
	set_entvar(pPlayer, var_weaponmodel, model_p_baseballbat);
	set_member(pPlayer, m_flNextAttack, 0.95);
}

jbe_set_syringe_model(pPlayer)
{
	set_entvar(pPlayer, var_viewmodel, model_v_syringe);
	
	UTIL_WeaponAnimation(pPlayer, 1);
	UTIL_PlayerAnimation(pPlayer, "crouch_reload_carbine");
//	engclient_cmd(pPlayer, "weapon_knife");
	set_member(pPlayer, m_flNextAttack, 3.0);
	emit_sound(pPlayer, CHAN_AUTO, sound_syringe_use, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
}

public jbe_set_syringe_health(pPlayer)
{
	pPlayer -= TASK_REMOVE_SYRINGE;
	set_entvar(pPlayer, var_health, 200.0);
}

public jbe_remove_syringe_model(pPlayer)
{
	pPlayer -= TASK_REMOVE_SYRINGE;
	new iActiveItem = get_member(pPlayer, m_pActiveItem);
	if(iActiveItem > 0) ExecuteHamB(Ham_Item_Deploy, iActiveItem);
}

jbe_set_bracelets_model(pPlayer)
{
	set_entvar(pPlayer, var_viewmodel, model_v_bracelets);
	set_entvar(pPlayer, var_weaponmodel, model_p_bracelets);
	set_member(pPlayer, m_flNextAttack, 0.75);
}

// Для смены на нож (при буцании мяча или подобной смене рук если есть купленный нож)
jbe_default_weapon_knife(id)
{
	if (g_iBoxingGame && IsSetBit(g_iBitUserBoxing, id))
	{
		jbe_boxing_gloves_model(id, g_iBoxingUserTeam[id]);
		return;
	}

	if (g_iBitWeaponStatus && IsSetBit(g_iBitWeaponStatus, id))
	{
		if (IsSetBit(g_iBitBalisong, id))		jbe_set_balisong_model(id);
		else if (IsSetBit(g_iBitBasaballbat, id))	jbe_set_baseballbat_model(id);
		else if (IsSetBit(g_iBitGuardBaton, id))	jbe_set_baton_model(id);
		else jbe_default_knife_model(id);
	}
	
	else
		jbe_default_knife_model(id);
}

jbe_set_random_skin(id)
{
	if ((IsSetBit(g_iBitUserVip, id) || IsSetBit(g_iBitUserAdmin, id))
	&& IsNotSetBit(g_iBitUserNightVip, id))
		g_iUserSkin[id] = SKIN_BLACK; // вип / админ сразу черный скин.
	else
		g_iUserSkin[id] = random(5); // 0 - 4	
}

jbe_check_last_prisoner()
{
	if (g_iLastPnId || g_iAlivePlayersNum[TEAM_PRISONER] != 1)
		return;

	if (g_iDayMode == DAY_RESTART || g_iDayMode == DAY_DAYMODE || g_iDayMode == DAY_DEATHMATCH)
		return;

	if (is_shief_global_game())
	{
		if (g_iChiefGameType == GAME_MAFIA)
			jbe_mafia_ended();

		else if (g_iChiefGameType == GAME_JIHAD)
			return; //jihad_disable();
			
		else if (g_iChiefGameType == GAME_HUNGER && !g_iDayModeTimer)
			return;

		else if (g_iChiefGameType != GAME_HUNGER)
			return;
	}

	if (g_iPlayersNum[TEAM_PRISONER] == 1)
		g_iLastPnType = LAST_PN_DUEL;

	if (g_bAllUseBall) jbe_soccer_remove_ball();
	if (g_bSoccerStatus) jbe_soccer_disable_all();
	if (g_iBoxingGame) jbe_boxing_disable_all();

	if (!g_iChiefId)
		g_szChiefName[0] = EOS;

	if (g_iDayMode == DAY_FREEDAY)
		jbe_free_day_ended();

	remove_task(TASK_CHIEF_CHOICE_TIME);
	jbe_minigame_stop_all();

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i))
			continue;

		if (jbe_get_user_godmode(i))
			jbe_set_user_godmode(i, 0);

		if (g_iUserTeam[i] != TEAM_PRISONER)
			continue;

		g_iLastPnId = i;
		Show_LastPrisonerMenu(i);
		if (IsNotSetBit(g_iBitUserVoice, i) && IsNotSetBit(g_iBitUserAdmin, i))
		{
			reload_user_voice(i);
			client_print_color(i, i, "%s Вам выдан голос.", g_szChatPrefix);
		}
		if (IsSetBit(g_iBitUserWanted, i))
			jbe_sub_user_wanted(i);
	}

	client_print_color(0, 0, "%s Охрана видит на радаре последнего заключенного, а заключенный охрану!", g_szChatPrefix);
	jbe_open_doors();
	jbe_set_name_mode("Последний заключенный", false);
	jbe_set_name_chief();
	jbe_radar_last_pn();
	set_task(2.0, "jbe_radar_last_pn", TASK_RADAR_LAST_PN, _, _, "b");

#if defined ENABLE_QUESTS
	check_user_quest(g_iLastPnId, QUEST_ID_LAST_PN);
#endif

}

public jbe_radar_last_pn()
{
	new iCoordsPn[3], iCoordsGr[3];
	if (g_iDuelStatus)
	{
		get_user_origin(g_iDuelUsersId[0], iCoordsPn);
		get_user_origin(g_iDuelUsersId[1], iCoordsGr);
		
		UTIL_ShowRadar(g_iDuelUsersId[1], g_iDuelUsersId[0], iCoordsPn);
		UTIL_ShowRadar(g_iDuelUsersId[0], g_iDuelUsersId[1], iCoordsGr);
	}
	else if (g_iLastPnId)
	{
		if (IsNotSetBit(g_iBitUserAlive, g_iLastPnId))
		{
			remove_task(TASK_RADAR_LAST_PN);
			return;
		}
		
		get_user_origin(g_iLastPnId, iCoordsPn);
		
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || g_iUserTeam[i] != TEAM_GUARD)
				continue;

			get_user_origin(i, iCoordsGr);

			UTIL_ShowRadar(g_iLastPnId, i, iCoordsGr);
			UTIL_ShowRadar(i, g_iLastPnId, iCoordsPn);
		}
	}
}

jbe_set_random_pahan()
{
	new iPlayers[MAX_PLAYERS], iNum;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || g_iUserTeam[i] != TEAM_PRISONER)
			continue;
		iPlayers[iNum++] = i;
	}
	new iPahan = iPlayers[random_num(0, iNum - 1)];
	if (iPahan) // чтоб на id 0 не выдавали
	{
		SetBit(g_iBitBasaballbat, iPahan);
		InvertBit(g_iBitWeaponStatus, iPahan);
		jbe_set_baseballbat_model(iPahan);
	}
}

/* Random alive player */
stock fnGetRandomAlive(iNum, iTeam = 0)
{
	new iAlive;
	
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || iTeam && g_iUserTeam[i] != iTeam)
			continue;

		if (++iAlive == iNum)
			return i;
	}

	return 0;
}

#define DEMO_RECORD_TIME 7 // через сколько стартовать запись демо

// CS-One_JB_demo1.dem
public user_demo_record(id)
{
	if (id <= MaxClients)
	{
		remove_task(id + TASK_DEMORECORD_MESSAGE);
		set_task(DEMO_RECORD_TIME.0, "user_demo_record", id + TASK_DEMORECORD_MESSAGE);
		return;
	}

	id -= TASK_DEMORECORD_MESSAGE;
	
	if (IsNotSetBit(g_iBitUserConnected, id))
		return;
	
	if (!g_iUserDemoID[id] || g_iUserDemoID[id] >= 3)
		g_iUserDemoID[id] = 1;
	else
		g_iUserDemoID[id]++;

	new szDemoName[32];
	formatex(szDemoName, charsmax(szDemoName), "%s_%d",
		g_szDemoName, g_iUserDemoID[id]);

	client_cmd(id, "stop;wait;record ^"%s^"", szDemoName);
	
	set_task(3.0, "user_demo_record_info", id + TASK_DEMORECORD_MESSAGE, szDemoName, charsmax(szDemoName));
}

public user_demo_record_info(const szDemoName[], id)
{
	id -= TASK_DEMORECORD_MESSAGE;
	
	if (IsNotSetBit(g_iBitUserConnected, id))
		return;

	new szTime[32];
	get_time("^3%H:%M:%S ^1| ^3%d.%m.%Y", szTime, charsmax(szTime));

	new szName[MAX_NAME_LENGTH];

	if (g_szMemberName[id][0] != EOS)
		szName = g_szMemberName[id];
	else
		get_user_name(id, szName, charsmax(szName));
	
	client_print_color(id, print_team_blue,
		"%s ^1Добро пожаловать в ^4Маленькую тюрьму ^3%s^1.", g_szChatPrefix, szName);
	
	client_print_color(id, print_team_blue,
		"%s ^1Начата запись демо: ^4%s.dem ^1время: ^3%s ^1.",
		g_szChatPrefix, szDemoName, szTime);
}

fnHudAdvertsRead()
{
	new szCfgDir[64], szCfgFile[128];
	get_localinfo("amxx_configsdir", szCfgDir, charsmax(szCfgDir));	
	formatex(szCfgFile, charsmax(szCfgFile), "%s/jb_engine/dhud_adverts.ini", szCfgDir);

	new fp = fopen(szCfgFile, "rt");
	if (!fp) return -1;
	new szBuffer[128];
	g_aHudAdverts = ArrayCreate(128);
	while (!feof(fp))
	{
		fgets(fp, szBuffer, charsmax(szBuffer));
		trim(szBuffer); // удаляем переносы строк
		if (!szBuffer[0] || szBuffer[0] == ';') continue;
		ArrayPushArray(g_aHudAdverts, szBuffer);
	}
	g_iHudAdvertsSize = ArraySize(g_aHudAdverts);
	return fclose(fp);
}

public jbe_user_respawn_task(id)
{
	id -= TASK_USER_RESPAWN;
	
	if (IsSetBit(g_iBitUserConnected, id) && IsNotSetBit(g_iBitUserAlive, id)
	&& (g_iUserTeam[id] == TEAM_PRISONER || g_iUserTeam[id] == TEAM_GUARD)
	&& g_iAlivePlayersNum[TEAM_PRISONER] >= 2)
	{
		rg_round_respawn(id);
	}
}

fix_venicle_spawn_init()
{
//	new bool:g_bIsVenicleMap = false;
	new iEntity;//, vAngles[3];
	while ((iEntity = find_ent_by_class(iEntity, "func_vehicle")) > 0)
	{
	//	pev(iEntity, pev_angles, vAngles);
	//	set_pev(iEntity, pev_vuser4, vAngles);
		RegisterHam(Ham_Use, "func_vehicle", "Ham_VehicleUse", false);
	//	g_iszFuncVehicle = get_entvar(iEntity, var_classname);
		break;
	//	if (g_iszFuncVehicle)
	//		set_pev_string(iEntity, pev_classname, g_iszFuncVehicle);
	//	else
	//		g_iszFuncVehicle = pev(iEntity, pev_classname);
	}

//	if (g_bIsVenicleMap) //(g_iszFuncVehicle)
//	{
//		g_iszTrackTrain = engfunc(EngFunc_AllocString, "tracktrain");
//		RegisterHam(Ham_Use, "func_vehicle", "Ham_VehicleUse", false); // true
//	}
}

stock fix_venicle_spawn()
{
	new iEntity, vAngles[3];
	while ((iEntity = find_ent_by_class(iEntity, "func_vehicle")))
	{
		call_think(iEntity);
		get_entvar(iEntity, var_vuser4, vAngles);
		set_entvar(iEntity, var_angles, vAngles);
	}
}

#if !defined DEVELOPER_OPTIONS

	native cmsgag_is_user_blocked(id);
	/*
		Проверка текущего блока от Админа
		@index - индекс игрока
		0	-	voice + chat
		1	-	chat
		2	-	voice
		-1	- not blocked
	*/
	
reload_user_voice(id) // при смерти / респавне / входе юзаем
{
	new iBlock = cmsgag_is_user_blocked(id);

	if ((((g_iUserTeam[id] == TEAM_GUARD || IsSetBit(g_iBitUserVoice, id) || g_iLastPnId == id) && IsSetBit(g_iBitUserAlive, id))
	|| IsSetBit(g_iBitUserAdmin, id)) && (iBlock != 0 && iBlock != 2))
		VTC_UnmuteClient(id);
	else
		VTC_MuteClient(id);
}


#else 
	reload_user_voice(id) return id;
#endif

stock jbe_set_name_mode(szDayName[] = EOS, bool:bMod = true)
{
	if (szDayName[0] != EOS)
	{
		if (bMod && g_iDayMode == DAY_NORMAL)
			formatex(g_szDayMode, charsmax(g_szDayMode), "^nРежим: %s ", szDayName);
		else 
			formatex(g_szDayMode, charsmax(g_szDayMode), "^n%s ", szDayName);
	}
	else
		g_szDayMode = "";
}


// fWhiteTime - как часто можно юзать функцию, если чаще то + к варнам
// fBlockTime - время блока при достижении лимита варнов
// iType - DATA_CMD_FLOOD
stock bool:is_user_cmd_flood(id, iType = FLOOD_ALL, Float:fWhiteTime = 1.0, Float:fBlockTime = 2.0, bool:bMessage = true)
{
#define WARN_CMD_LIMIT		2	// Лимит варнов, если больше то блок
#define CLEAN_WARN_CMDTIME	2	// Множитель для fBlockTime для обнуления варнов

	static Float:fTimeFlood[MAX_PLAYERS + 1][DATA_CMD_FLOOD];
	static iFloodWarn[MAX_PLAYERS + 1][DATA_CMD_FLOOD];
	static Float:fGameTime;
	static Float:fLastTime[MAX_PLAYERS][DATA_CMD_FLOOD];

	fGameTime = get_gametime();

	// тихий блок
	if (fLastTime[id][iType] > fGameTime)
		return true;

	fLastTime[id][iType] = fGameTime + 0.2;

	if (fTimeFlood[id][iType] > fGameTime)
	{
		if (iFloodWarn[id][iType] > WARN_CMD_LIMIT
		|| ++iFloodWarn[id][iType] > WARN_CMD_LIMIT)
		{
			if (bMessage)
			{
				client_print_color(id, print_team_default,
					"^4*** %s ^1Прекратите флудить! Ждите %0.f сек. ^4***",
					g_szChatPrefix, fTimeFlood[id][iType] - fGameTime);

				client_cmd(id, "spk buttons/blip2.wav");
			}

			return true;
		}
	}

	else if (iFloodWarn[id][iType])
	{
		if ((fTimeFlood[id][iType] + fBlockTime * CLEAN_WARN_CMDTIME) < fGameTime)
			iFloodWarn[id][iType] = 0;
		else
			iFloodWarn[id][iType]--;
	}
	
	if (iFloodWarn[id][iType] == WARN_CMD_LIMIT)
		fTimeFlood[id][iType] = fGameTime + fBlockTime;
	else
		fTimeFlood[id][iType] = fGameTime + fWhiteTime;

	return false;
}

jbe_give_guard_weapond(id, iWeapons)
{
	new szWeapon[][] = {
		"weapon_ak47",
		"weapon_m4a1",
		"weapon_awp",
		"weapon_xm1014",
		"weapon_m249",
		"weapon_sg552",
		"weapon_g3sg1",
		"weapon_sg550",

		"weapon_deagle",
		"weapon_usp",
		"weapon_p228",
		"weapon_elite",
		"weapon_glock18",
		"weapon_fiveseven"
	};
	
	if (iWeapons == 7)
	{
		if (IsNotSetBit(g_iBitUserVip, id))
			return Show_WeaponsGuardMenu(id);

		rg_give_item_ex(id, szWeapon[1], GT_REPLACE, 250);
		rg_give_item_ex(id, szWeapon[7], GT_APPEND, 250);
		
		for (new i = 8; i < sizeof(szWeapon); i++)
		{
			rg_give_item_ex(id, szWeapon[i], GT_APPEND, 250);
		}
	}
	else
	{
		rg_give_item_ex(id, szWeapon[iWeapons], GT_REPLACE, 250);
		rg_give_item_ex(id, szWeapon[8], GT_REPLACE, 250);
	}
	return PLUGIN_HANDLED;
}

// restart
public jbe_restart_game_timer()
{
	if (--g_iDayModeTimer)
	{
		jbe_open_doors();
		formatex(g_szDayModeTimer, charsmax(g_szDayModeTimer), "[%i]", g_iDayModeTimer);
		restart_deathmatch();
	}
	else
	{
		g_szDayModeTimer = "";
		g_bRestartGame = false;
		rg_round_end(5.0, WINSTATUS_DRAW, ROUND_GAME_RESTART);

		set_cvar_num("sv_gravity", cvar_sv_gravity);
		
		g_iBitFastRun = 0; // обнуляем всем скорость (нада же?)
		
		// забираем хук всем кому не положен он :)
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserConnected, i))
				continue;

			if (!(get_user_flags(i) & ADMIN_LEVEL_G))
			{
				ClCmd_HookOff(i);
				ClearBit(g_iBitUserHook, i);
				CREATE_KILLBEAM(i); // убираем спрайт за игроком
			}			
		}
	}
}

// Небольшой ДМ в рестарт :)
restart_deathmatch(bool:bRoundStart = false)
{
	if (bRoundStart)
	{
		set_cvar_num("sv_gravity", 200);
	}

	g_iFriendlyFire = FF_OVERALL;
	
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserConnected, i)
		|| g_iUserTeam[i] == TEAM_UNASSIGNED || g_iUserTeam[i] == TEAM_SPECTATOR)
			continue;
		
		if (IsNotSetBit(g_iBitUserAlive, i))
		{
			rg_round_respawn(i);
		}

		if (IsNotSetBit(g_iBitUserHook, i))
		{
			SetBit(g_iBitUserHook, i); // даем хук на время ресстарта
			set_hook_setting_default(i); // а че
			client_print_color(i, print_team_blue, "%s ^1В рестарт ^4всем ^1доступен hook ^3(в консоль: bind v +hook)^1.", g_szChatPrefix);
		}
	}
}

jbe_check_ready_limits_task()
{
	if (g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY || g_iDayMode == DAY_DEATHMATCH)
	{
		remove_task(TASK_READY_LIMITS);
		set_task(15.0, "jbe_check_ready_limits", TASK_READY_LIMITS);
	}
}

// уведомление о доступности нового пункта в вип меню и ранг бонусе
public jbe_check_ready_limits()
{
	new bool:bIsRank;
	new bool:bIsVip;

	new szMessage[180];
	new iLen;

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserConnected, i)
		|| g_iUserTeam[i] != TEAM_PRISONER && g_iUserTeam[i] != TEAM_GUARD)
			continue;

		bIsRank = false;
		bIsVip = false;

		for (new j; j < DATA_RANK_BONUS; j++)
		{
			if (g_iRankBonus[i][j] == g_eRankBonusRound[j])
			{
				bIsRank = true;
				break;
			}
		}
	
		if (IsSetBit(g_iBitUserVip, i)
		&& (g_iVip[i][DV_MONEY]		== g_eAllCvars[VIP_MONEY_ROUND]
		|| g_iVip[i][DV_INVISIBLE]	== g_eAllCvars[VIP_INVISIBLE]
		|| g_iVip[i][DV_HP_AP]		== g_eAllCvars[VIP_HP_AP_ROUND]
		|| g_iVip[i][DV_GHOST]		== g_eAllCvars[VIP_GHOST_ROUND]
		|| g_iVip[i][DV_GOD]		== g_eAllCvars[VIP_GOD_ROUND]
		|| g_iVip[i][DV_FOOTSTEPS]	== g_eAllCvars[VIP_FOOTSTEPS_ROUND]))
		{
			bIsVip = true;
		}
	
		if (bIsRank || bIsVip)
		{
			iLen = formatex(szMessage, charsmax(szMessage),
				"%s ^1Открылся доступ к новому пункту", g_szChatPrefix);
	
			if (bIsRank)
				iLen += formatex(szMessage[iLen], charsmax(szMessage) - iLen,
					" в ^4ранг бонусе ^1(^3M ^1-^3 7^1)");
	
			if (bIsVip)
				iLen += formatex(szMessage[iLen], charsmax(szMessage) - iLen,
					"%s ^4vip меню ^1(^3M ^1-^3 8 ^1-^3 1^1)", bIsRank ? " и" : " в");
	
			iLen += formatex(szMessage[iLen], charsmax(szMessage) - iLen, ".");
		}
	
		client_print_color(i, print_team_blue, szMessage);
	}
}

// Пишем причину отсидки
jbe_print_prisons_reason(pPlayer)
{
	client_print_color(pPlayer, print_team_red,
		"^4 * ^1[^3Малява^1] ^1Вас посадили за ^4%s %s^1.",
		g_szPrisonReason1[random_num(0, g_iSizePrisonReason1)],
		g_szPrisonReason2[random_num(0, g_iSizePrisonReason2)]);
}

/**
 * Сделать бытуты не активными
 * если под батутом яма игрок будет просто проваливаться
 */
stock toggle_trigger_push(bool:bStatus)
{
	new iEntity; // = MaxClients + 1;
	while ((iEntity = engfunc(EngFunc_FindEntityByString, iEntity, "classname", "trigger_push")))
	{
		if (!pev_valid(iEntity)) continue;
		set_pev(iEntity, pev_solid, bStatus ? SOLID_TRIGGER : SOLID_NOT);
	}
}

/************************************************
 * set spawn effects - vip menu & green ticket
 ***********************************************/

// время выдачи еффектов при спавне через вип / билет (свечение + наручники)
#define SPAWN_EFFECTS_TIME 30 

// тип наручников до выдачи таковых за респав
new g_iSpawnBraceletsResetType[MAX_PLAYERS + 1];

// bSpawnChief - спавнить возле начальника
respawm_set_effects(id, bool:bSpawnChief = true)
{
	if (g_iDayMode == DAY_DEATHMATCH) return;
	
	jbe_set_user_rendering(id,
		.iRenderFx = kRenderFxGlowShell,
		.iRed = 40,
		.iGreen = 40,
		.iBlue = 200,
		.iRenderMode = kRenderNormal,
		.iRenderAmt = 20);
	jbe_save_user_rendering(id);

	set_task(SPAWN_EFFECTS_TIME.0 + 1.0, "respawn_clear_effects", id + TASK_SPAWN_EFFECTS);

	// если реснулся за кт, а потом стал зеком сохраним тут значение
	g_iSpawnBraceletsResetType[id] = g_iUserBraceletsType[id];

	if (g_iUserTeam[id] != TEAM_PRISONER)
		return;

	if (!g_iUserBraceletsTime[id])
	{
		jbe_set_user_bracelets(0, id, BR_ATTACK, SPAWN_EFFECTS_TIME);
	}
	else
	{
		if (g_iUserBraceletsType[id] == BR_ATTACK)
			g_iUserBraceletsType[id] = BR_ATTACK_BUTTON;

		if (g_iUserBraceletsTime[id] < SPAWN_EFFECTS_TIME)
			g_iUserBraceletsTime[id] = SPAWN_EFFECTS_TIME;
	}

	client_print_color(id, print_team_blue,
		"^4[^3ReSpawn^4] ^1Вам надеты наручники на^4 %d секунд \
		^1(вы не сможете атаковать охрану).", SPAWN_EFFECTS_TIME);

	// teleport to chief
	if (g_bOneOpenDoors && bSpawnChief && g_iChiefId && g_iDayMode == DAY_NORMAL)
	{
		new Float:vecOrigin[3], Float:fVecAngle[3], Float:vecVelocity[3];

		get_entvar(g_iChiefId, var_origin, vecOrigin);
		get_entvar(g_iChiefId, var_v_angle, fVecAngle);
		velocity_by_aim(g_iChiefId, 45, vecVelocity);

		vecOrigin[0] += vecVelocity[0]; // 0.0; //
		vecOrigin[1] += vecVelocity[1];
		vecOrigin[2] += 36.0; //72

		set_entvar(id, var_origin, vecOrigin);
		set_entvar(id, var_angles, fVecAngle);
		set_entvar(id, var_fixangle, 1);

	//	drop_to_floor(id);
	//	unstuck_user(g_iChiefId);
	}
}

public respawn_clear_effects(id)
{
	id -= TASK_SPAWN_EFFECTS;

	if (IsNotSetBit(g_iBitUserConnected, id))
		return;

	if (is_user_active_bracelets(id))
		g_iUserBraceletsType[id] = g_iSpawnBraceletsResetType[id];

	if (IsNotSetBit(g_iBitUserAlive, id))
		return;

	// TODO: додумать, если держат грабом или выдали другое свечение	
	jbe_set_user_rendering(id, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
	jbe_save_user_rendering(id);
}

jbe_user_frost_electro_costum(id)
{
	client_print_color(id, id, "%s Вас ударило током, вы оглушены на две секунды!", g_szChatPrefix);

	set_entvar(id, var_flags, get_entvar(id, var_flags) | FL_FROZEN);
	set_member(id, m_flNextAttack, 2.0);
	
	jbe_set_user_rendering(id, kRenderFxGlowShell, 0, 110, 255, kRenderNormal, 0);
	
	emit_sound(id, CHAN_AUTO, sound_freeze_player, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	SetBit(g_iBitUserFrozen, id);
	set_task(2.0, "jbe_user_defrost", id+TASK_FROSTNADE_DEFROST);
}

jbe_add_user_killed_reasons(iKiller, iVictim)
{
	if ((is_shief_global_game() && g_iChiefGameType != GAME_HUNGER)
	|| IsSetBit(g_iBitUserDuel, iKiller))
		return;

	if (g_iBitKilledUsers[iKiller])
	{
		SetBit(g_iBitKilledUsers[iKiller], iVictim);
	}
	else
	{
		g_iMenuTarget[iKiller] = iVictim;
		SetBit(g_iBitKilledUsers[iKiller], iVictim);
		Show_KillReasonsMenu(iKiller, iVictim);
	}
}

jbe_check_teammate_kill(iVictim, iKiller)
{
	if (jbe_is_user_valid(iKiller) && (iVictim != iKiller) && (g_iUserTeam[iVictim] == g_iUserTeam[iKiller]))
	{
		new iFrags = get_user_frags(iKiller) + 2;
		set_entvar(iKiller, var_frags, float(iFrags));

		UTIL_UserScoreMessage(iKiller, iFrags, get_user_deaths(iKiller));
	} 
}

jbe_prisoner_riot_start(id)
{
	if (jbe_is_wanted() || g_iLastPnId || is_shief_global_game())
		return;
	
	if (g_iPlayersNum[TEAM_PRISONER] >= 5)
	{
		jbe_set_user_money(id, g_iUserMoney[id] + g_eAllCvars[RIOT_START_MODEY], 1);
		check_user_quest(id, QUEST_ID_ROBS, g_eAllCvars[RIOT_START_MODEY]);
	}

	rg_send_audio(0, sound_prison_riot);
	set_hudmessage(255, 0, 0, -1.0, 0.7, 0, 6.0, 2.0, 0.1, 0.1, -1);
	show_hudmessage(0, "%n начал бунт!", id);
}

// при смене ника 
public reload_flags(id)
{
	id -= TASK_RELOAD_FLAGS;
	if (IsNotSetBit(g_iBitUserConnected, id)) return;
	
	jbe_clear_user_privilegies(id);
	jbe_get_user_privilegies(id);
}

// открываем меню квестов, ставим костюм, даем голос
jbe_open_menu_spawn(id)
{
	if (IsSetBit(g_iBitUserSteam, id) || IsSetBit(g_iBitEternalVoice, id)
	|| (IsSetBit(g_iBitUserVip, id) && IsNotSetBit(g_iBitUserNightVip, id)))
		SetBit(g_iBitUserVoice, id);
		
	reload_user_voice(id);

	if (g_iDayMode == DAY_RESTART || g_iDayMode == DAY_DAYMODE)
		return;
	
	if (g_eUserCostumes[id][HIDE])
		jbe_set_user_costumes(id, g_eUserCostumes[id][COSTUMES]);

	if (g_iDayMode == DAY_DEATHMATCH)
		return;

	switch (g_iUserTeam[id])
	{
		case TEAM_PRISONER:
		{
			if (!g_iUserQuest[id][ALL_NUM])
				Cmd_QuestMenu(id);		
		}

		case TEAM_GUARD:
		{
			if (IsSetBit(g_iBitAutoWeapons, id))
			{
				client_print_color(id, id, "%s ^1Включен Авто-выбор оружия ^3(открыть меню оружия в чат: ^4/weapon^3)^1.", g_szChatPrefix);
			
				jbe_give_guard_weapond(id, g_iUserAutoWeapon[id]);
			
				// если нет квеста и авто-выбор оружия выводим меню при спавне
				if (!g_iUserQuest[id][ALL_NUM])
					Cmd_QuestMenu(id);			
			}
			else
				Show_WeaponsGuardMenu(id);
		}
	}
}

public spawn_user_fix(id, Float:fTimeRespawn) //  = 0.2
{
	if (id <= MaxClients)
	{
		remove_task(id + TASK_SPAWN_FIX);
		set_task(fTimeRespawn, "spawn_user_fix", id + TASK_SPAWN_FIX);//, "1", 2
		return;
	}

	id -= TASK_SPAWN_FIX;
	
	if (IsNotSetBit(g_iBitUserConnected, id))
		return;
	
	if (g_iUserTeam[id] != TEAM_PRISONER && g_iUserTeam[id] != TEAM_GUARD)
		return;

	if (IsNotSetBit(g_iBitUserAlive, id))
		rg_round_respawn(id);
	else
		unstuck_user(id);
}

spawn_ghost_user(id)
{
	ClearBit(g_iBitActiveGhost, id);
	
	message_begin(MSG_ALL, MsgId_ScoreAttrib);
	write_byte(id);
	write_byte((1<<0));
	message_end();

	/* срабатывает респавн(?)
	 * некоректно ведет себя модель если включена видимость через фуллпак */
	set_entvar(id, var_deadflag, DEAD_RESPAWNABLE);
	rg_set_user_footsteps(id, true);
	set_entvar(id, var_solid, SOLID_NOT);
	set_entity_visibility(id, 0);

	rg_remove_all_items(id);
	rg_give_item(id, "weapon_knife");

	jbe_reset_user_informer(id);
	
	// фикс на видимость своих денег (пишет 0 и сверхку + 0)
	jbe_set_user_money(id, g_iUserMoney[id], 0);

	// удаляем траил
	if (IsSetBit(g_iBitUserHook, id))
		CREATE_KILLBEAM(id);

	return PLUGIN_HANDLED;
}

team_menu_check_respawn(id)
{
	if (g_iDayMode == DAY_DEATHMATCH && !g_iPlayersNum[TEAM_GUARD])
	{
		jbe_deathmatch_respawn(id);
		client_print_color(id, print_team_red, "%s ^3[DM] ^1 Вы будете возрождены через^4 5 ^1секунд.", g_szChatPrefix);
	}

	else if (((g_iDayMode == DAY_NORMAL && !g_bOneOpenDoors) || g_iDayMode == DAY_FREEDAY)
	&& g_fRoundStartTime + 40.0 >= get_gametime() && g_iAlivePlayersNum[TEAM_PRISONER] >= 2)
	{
		set_task(1.0, "jbe_user_respawn_task", id + TASK_USER_RESPAWN);
	}
}

jbe_clear_user_shop(id)
{
	remove_task(id + TASK_REMOVE_SYRINGE);
	remove_task(id + TASK_CHANGE_MODEL);
	remove_task(id + TASK_SPAWN_EFFECTS);

	ClearBit(g_iBitKokain, id);
	ClearBit(g_iBitUserBuried, id);
	ClearBit(g_iBitUserGodMode, id);
	ClearBit(g_iBitDoubleDamage, id);
	ClearBit(g_iBitLotteryTicket, id);
	ClearBit(g_iBitFallNoDamage, id);
	ClearBit(g_iBitFastRun, id);
	ClearBit(g_iBitDoubleJump, id);
	ClearBit(g_iBitAutoBhop, id);
	ClearBit(g_iBitHingJump, id);
	ClearBit(g_iBitLongJump, id);
	ham_jump_check();

	g_fUserMaxSpeed[id] = 0.0;
	
	if (IsSetBit(g_iBitUsePaint, id))
	{
		ClearBit(g_iBitUsePaint, id);
		ClearBit(g_iBitPaintHolding, id);
		check_player_pre_think();
	}

	if (IsSetBit(g_iBitUserFrozen, id))
	{
		ClearBit(g_iBitUserFrozen, id);
		remove_task(id + TASK_FROSTNADE_DEFROST);
	}

	if (IsSetBit(g_iBitUser3DView, id))
	{
		jbe_set_user_view(id, 0);
	}

	if (g_iUserTeam[id] == TEAM_PRISONER)
	{
		ClearBit(g_iBitBalisong, id);
		ClearBit(g_iBitBasaballbat, id);
		ClearBit(g_iBitGuardBaton, id);
		ClearBit(g_iBitWeaponStatus, id);
		ClearBit(g_iBitClothingGuard, id);
		ClearBit(g_iBitClothingType, id);
		ClearBit(g_iBitLatchkey, id);
		ClearBit(g_iBitUserRealWanted, id);	// реальный бунт под наручники

		if (IsSetBit(g_iBitFrostNade, id))
		{
			ClearBit(g_iBitFrostNade, id);
			check_frostnade_fwd();
		}
	}

	else if (g_iUserTeam[id] == TEAM_GUARD)
	{
		ClearBit(g_iBitElectroCostume, id);
		ClearBit(g_iBitBuyWeaponsGuard, id);
	}
}

jbe_clear_user_disconnect(id)
{
	remove_task(id + TASK_SHOW_INFORMER);
	remove_task(id + TASK_LOAD_PLAYER_SQL);
	remove_task(id + TASK_DEATHMATCH_PLAYER);

	g_iUserTeam[id] = TEAM_UNASSIGNED;
	g_iSqlUserID[id] = 0;
	g_iUserMoney[id] = 0;
	g_iUserExp[id] = 0;
	g_iUserLevel[id] = 0;
	g_iUserSkin[id] = 0;
	g_iUserAutoWeapon[id] = 0;
	g_iUserBraceletsType[id] = BR_NONE;
	g_iUserBraceletsTime[id] = 0;
	g_iUserWantedNum[id] = 0;
	g_iTimeBlockGrEnd[id] = 0;
	g_iBitKilledUsers[id] = 0;
	g_iBitBlockedPM[id] = 0;
	g_iUserChangeTeamTime[id] = 0;
	g_szMemberName[id][0] = EOS;
	g_szUserModel[id][0] = EOS;
	g_iUserDemoID[id] = 0;
	g_iUserAfkWarns[id] = 0;
	g_iUserGameTime[id] = 0;

	ClearBit(g_iBitUserModel, id);
	ClearBit(g_iBitUserFreeNextRound, id);
	ClearBit(g_iBitUserVoiceNextRound, id);
	ClearBit(g_iBitBlockMenu, id);
	ClearBit(g_iBitUserVoteDayMode, id);
	ClearBit(g_iBitUserDayModeVoted, id);
	ClearBit(g_iBitPriceGunPn, id);
	ClearBit(g_iBitUserSqlLoad, id);
	ClearBit(g_iBitAutoWeapons, id);
	ClearBit(g_iBitUserOffPM, id);
	ClearBit(g_iBitUserGrBlock, id);
	ClearBit(g_iBitUser3DView, id);
//	ClearBit(g_iBit3DViewUse, id);
	ClearBit(g_iBitOffCostumes, id);
	ClearBit(g_iBitUserGhost, id);
	ClearBit(g_iBitActiveGhost, id);
	ClearBit(g_iBitOffDayModeSound, id);
	ClearBit(g_iBitUserVoice, id);
	ClearBit(g_iBitBuyFrostNade, id);
	ClearBit(g_iBitEternalVoice, id);
	ClearBit(g_iBitUsedRankBonus, id);

	arrayset(g_PM_LastTargets[id], 0, sizeof g_PM_LastTargets[]);
}

/*===== <- Остальной хлам <- =====*///}