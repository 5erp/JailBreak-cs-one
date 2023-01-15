jbe_deathmatch_check()
{
	if (!g_bDeathMatchEnabled)
	{
		if (g_eAllCvars[DEATHMATCH_MAX_PLAYERS] && g_iPlayersNum[TEAM_PRISONER] >= g_eAllCvars[DEATHMATCH_MAX_PLAYERS])
			return;
		
		g_bDeathMatchEnabled = true;
		UTIL_DirectorMessage(0, "DeathMatch Включен!", 250, 50, 50, -1.0, 0.20, 1, 3.0, 3.0, 0.1, 0.8);

		if (g_eAllCvars[DEATHMATCH_MAX_PLAYERS])
		{
			set_hudmessage(127, 127, 127, -1.0, 0.25, 0, 6.0, 2.0, 0.1, 0.1, -1);
			ShowSyncHudMsg(0, g_iSyncInfoCenter, "Игроков меньше %d", g_eAllCvars[DEATHMATCH_MAX_PLAYERS]);
		}

		if (g_iDayMode == DAY_DEATHMATCH)
		{
			g_iFriendlyFire = FF_PRISONER;
			for (new i = 1; i <= MaxClients; i++)
			{
				if (IsNotSetBit(g_iBitUserAlive, i)) continue;
				jbe_deathmatch_giveweapon(i);
			}
		}
	}
	else if (g_bDeathMatchEnabled && (g_eAllCvars[DEATHMATCH_MAX_PLAYERS] && g_iPlayersNum[TEAM_PRISONER] > g_eAllCvars[DEATHMATCH_MAX_PLAYERS]))
	{
		g_bDeathMatchEnabled = false;
		UTIL_DirectorMessage(0, "DeathMatch Выключен!", 50, 50, 250, -1.0, 0.20, 1, 3.0, 3.0, 0.1, 0.8);
		set_hudmessage(127, 127, 127, -1.0, 0.25, 0, 6.0, 2.0, 0.1, 0.1, -1);
		ShowSyncHudMsg(0, g_iSyncInfoCenter, "Игроков больше %d", g_eAllCvars[DEATHMATCH_MAX_PLAYERS] - 1);
		if (g_iDayMode == DAY_DEATHMATCH)
		{
			g_iFriendlyFire = FF_OFF;
			for (new i = 1; i <= MaxClients; i++)
			{
				if (IsNotSetBit(g_iBitUserAlive, i)) continue;
				rg_remove_all_items(i);
				rg_give_item(i, "weapon_knife");
			}
		}
	}

	if (g_bDeathMatchEnabled && g_eAllCvars[DEATHMATCH_RADAR] > 0)
	{
		if (g_iPlayersNum[TEAM_PRISONER] >= g_eAllCvars[DEATHMATCH_RADAR] && g_bGameRadarStatus)
			jbe_radar_off();
		else if (g_iPlayersNum[TEAM_PRISONER] < g_eAllCvars[DEATHMATCH_RADAR] && !g_bGameRadarStatus)
			jbe_radar_on();
	}
}

jbe_deathmatch_start()
{
	open_doors_hotfix();
	jbe_deathmatch_check();
	g_iDayMode = DAY_DEATHMATCH;
	jbe_set_name_mode( g_szDeathMatchName[ random( sizeof g_szDeathMatchName ) ] );

	if (g_eAllCvars[DEATHMATCH_RADAR] == -1 || (g_eAllCvars[DEATHMATCH_RADAR] && g_iPlayersNum[TEAM_PRISONER] >= g_eAllCvars[DEATHMATCH_RADAR]))
	{
		jbe_radar_off();
	}

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i)) continue;
	
		if (g_iDeathMatchSpawnsNum >= MIN_SPAWNS_NUM)
			jbe_deathmatch_random_spawn(i);

	//	if (g_bDeathMatchEnabled && !g_bGameRadarStatus)
	//	{
	//		set_score_attrib(i, SCORE_ATTRIB_DEAD);
	//	}

		jbe_reset_user_informer(i);
	}

	set_task(1.0, "jbe_deathmatch_informer", TASK_SHOW_INFORMER, _, _, "b");
	set_task(float(g_eAllCvars[DEATHMATCH_UPDATE_TIME]), "jbe_deathmatch_round", TASK_DEATHMATCH_PLAYER);
	
	if (g_bDeathMatchEnabled)
	{
		set_task(1.0, "jbe_deathmatch_task_ffire", TASK_CHIEF_GAME_START, _, _, "a", g_iDayModeTimer = 11);
	}

	set_cvar_num("mp_item_staytime", g_eAllCvars[DEATHMATCH_REMOVE_GUNS]);

	if (!g_aDeathMatchSpawns)
		g_aDeathMatchSpawns = ArrayCreate(DEATHMATCH_SPAWN_NUM, DEATHMATCH_SPAWNS);
/*	else
	{
		ArrayClear(g_aDeathMatchSpawns);
		g_iDeathMatchSpawnsNum = 0;
	}
*/
}

public jbe_deathmatch_task_ffire()
{
	if (--g_iDayModeTimer)
	{
		client_print_center(0, "DeathMatch через: %d", g_iDayModeTimer);
	}
	else
	{
		client_print_center(0, "Убей их всех!");
		g_iFriendlyFire = FF_PRISONER;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i))
				continue;

			jbe_deathmatch_giveweapon(i);
		}
	}
}

jbe_deathmatch_ended()
{
	if (!g_bGameRadarStatus)
	{
		jbe_radar_on();
	}

	g_iDayMode = DAY_NORMAL; // для regamedll (с убранным event hltv)
	g_bDeathMatchEnabled = false;
	remove_task(TASK_SHOW_INFORMER);
	remove_task(TASK_DEATHMATCH_PLAYER);
	remove_task(TASK_CHIEF_GAME_START);
	set_cvar_num("mp_item_staytime", cvar_mp_item_staytime);
}

jbe_radar_off()
{
	set_msg_block(MsgId_Radar, BLOCK_SET);
	g_msgScoreAttrib = register_message(MsgId_ScoreAttrib, "MessageScoreAttrib");
	g_bGameRadarStatus = false;
	check_player_pre_think();

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsSetBit(g_iBitUserAlive, i))
			set_score_attrib(i, SCORE_ATTRIB_DEAD);
	}
}

jbe_radar_on()
{
	set_msg_block(MsgId_Radar, BLOCK_NOT);
	unregister_message(MsgId_ScoreAttrib, g_msgScoreAttrib);
	g_msgScoreAttrib = 0;
	g_bGameRadarStatus = true;
	check_player_pre_think();

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsSetBit(g_iBitUserAlive, i))
			set_score_attrib(i, SCORE_ATTRIB_NONE);
	}
}

public MessageScoreAttrib(iMsgpPlayer, iMsgDest, iMsgEntity)
{
    enum { pPlayer = 1, iFlag };
    
    if (get_msg_arg_int(pPlayer) == iMsgEntity)
    {
		set_msg_arg_int(iFlag, ARG_BYTE, get_msg_arg_int(iFlag) | SCORE_ATTRIB_DEAD);
    }

    return PLUGIN_CONTINUE;
}

public jbe_deathmatch_round()
{
	UTIL_DirectorMessage(0, "Начисление денег", 255, 0, 0, -1.0, 0.3, 1, 3.0, 3.0, 0.1, 0.8);

	new aDataRoundSound[DATA_ROUND_SOUND], iTrack = random_num(0, g_iRoundSoundSize - 1);
	ArrayGetArray(g_aDataRoundSound, iTrack, aDataRoundSound);

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserConnected, i)) continue;

		for (new j; j < DATA_RANK_BONUS; j++)
			g_iRankBonus[i][j]++;

		if (IsSetBit(g_iBitUserVip, i))
			fn_vip_round_up(i);

		jbe_set_user_money(i, g_iUserMoney[i] + g_eAllCvars[ROUND_FREE_MODEY] + g_eAllCvars[ROUND_ALIVE_MODEY], 1);
		
		if (IsNotSetBit(g_iBitUserRoundSound, i)) continue;
		client_cmd(i, "mp3 play sound/CsOneRu/jb/round_sound/%s.mp3", aDataRoundSound[FILE_NAME]);
	}

	set_task(float(g_eAllCvars[DEATHMATCH_UPDATE_TIME]), "jbe_deathmatch_round", TASK_DEATHMATCH_PLAYER);
	rank_reward_exp_players();
	jbe_check_ready_limits_task();
	jbe_deathmatch_check();
}

public jbe_deathmatch_respawn(id)
{
	if (g_iPlayersNum[TEAM_GUARD])
		return;

	if (id <= MaxClients)
	{
		remove_task(id + TASK_DEATHMATCH_PLAYER);

		if (g_iUserTeam[id] != TEAM_PRISONER)
			return;

		set_task(5.0, "jbe_deathmatch_respawn", id + TASK_DEATHMATCH_PLAYER);
		rg_send_bartime(id, 5);
		
		return;
	}

	id -= TASK_DEATHMATCH_PLAYER;
	
	if (g_iPlayersNum[TEAM_GUARD] || g_iUserTeam[id] != TEAM_PRISONER || g_iDayMode != DAY_DEATHMATCH)
		return;

	if (IsSetBit(g_iBitUserAlive, id))
	{
		if (g_bDeathMatchEnabled)
			jbe_deathmatch_giveweapon(id);
		return;
	}

	rg_round_respawn(id);
	jbe_deathmatch_random_spawn(id);

	if (!g_bGameRadarStatus)
	{
		set_score_attrib(id, SCORE_ATTRIB_DEAD);
	}
	
	if (g_bDeathMatchEnabled)
	{
		set_entvar(id, var_takedamage, DAMAGE_NO);
		jbe_set_user_rendering(id, kRenderFxGlowShell, 0, 255, 0, kRenderNormal, 0);
		UTIL_ScreenFade(id, 0, 0, 4, 0, 150, 0, 100, 0); // делаем экран чуток зеленым пока бессмертен
		set_task(0.5, "jbe_deathmatch_giveweapon", id + TASK_DEATHMATCH_PLAYER);
	}
}

public jbe_deathmatch_giveweapon(id)
{
	if (id > MaxClients)
	{
		id -= TASK_DEATHMATCH_PLAYER;
		// 0.8 - чтоб успел сразу выстрелить при снятии бессмертия
		set_task(0.8, "jbe_deathmatch_take_god", id + TASK_DEATHMATCH_PLAYER);
	}

	if (!g_bDeathMatchEnabled)
		return PLUGIN_HANDLED;
		
	if (IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;

	hide_hud_elements(id, HideElement_Timer|HideElement_Flashlight);

	new iRandomWpn = random(sizeof g_szDeathMatchWeapons + 1);
	new szMessage[24];
	
	if (iRandomWpn < sizeof g_szDeathMatchWeapons)
	{
		new iEntity = rg_give_item(id, g_szDeathMatchWeapons[iRandomWpn], .type = GT_REPLACE);

		if (iEntity < 0) return PLUGIN_HANDLED;

		new WeaponIdType:iWpnID = rg_get_weapon_info(g_szDeathMatchWeapons[iRandomWpn], WI_ID);
		rg_set_user_bpammo(id, iWpnID, 250);
		
		formatex(szMessage, charsmax(szMessage), g_szDeathMatchWeapons[iRandomWpn]);
		replace(szMessage, charsmax(szMessage), "weapon_", "");
		strtoupper(szMessage);

		if (iRandomWpn >= DM_WPN_PISTOL_SHIELD && random(5) == 1)
		{
			rg_give_item(id, "weapon_shield", GT_REPLACE);

			format(szMessage, charsmax(szMessage), "%s + Shield", szMessage);
		}
	}
	else // Ножи вместо ганов 
	{
		switch (random(3))
		{
			case 0:
			{
				SetBit(g_iBitBalisong, id);
				copy(szMessage, charsmax(szMessage), "Нож-Бабочка");
			}
			case 1:
			{
				SetBit(g_iBitBasaballbat, id);
				copy(szMessage, charsmax(szMessage), "Бита");
			}
			case 2:
			{
				SetBit(g_iBitGuardBaton, id);
				copy(szMessage, charsmax(szMessage), "Дубинка");
			}
		}
		
		ClearBit(g_iBitWeaponStatus, id);

		if (get_user_weapon(id) == CSW_KNIFE)
		{
			new iActiveItem = get_member(id, m_pActiveItem);
			if (iActiveItem > 0)
			{
				InvertBit(g_iBitWeaponStatus, id);
				ExecuteHamB(Ham_Item_Deploy, iActiveItem);
				UTIL_WeaponAnimation(id, 3);
			}
		}
	}

	switch (random(14))
	{
		case 0:
		{
			set_entvar(id, var_health, 250.0);
			client_print_color(id, id, "%s ^3[DM] ^1Вам выдано: ^4%s ^1и^4 250HP^1.", g_szChatPrefix, szMessage);
		}
		case 1:
		{
			rg_set_user_armor(id, 250, ARMOR_VESTHELM);
			client_print_color(id, id, "%s ^3[DM] ^1Вам выдано: ^4%s ^1и^4 250AP^1.", g_szChatPrefix, szMessage);
		}
		case 2:
		{
			jbe_set_user_maxspeed(id, 450.0, true);
			client_print_color(id, id, "%s ^3[DM] ^1Вам выдано: ^4%s ^1и ^4скорость^1.", g_szChatPrefix, szMessage);
		}
		case 3:
		{
			set_entvar(id, var_gravity, 0.3);
			client_print_color(id, id, "%s ^3[DM] ^1Вам выдано: ^4%s ^1и^4 30％ гравитации^1.", g_szChatPrefix, szMessage);
		}
		case 4:
		{
			SetBit(g_iBitDoubleJump, id);
			ham_jump_check();
			client_print_color(id, id, "%s ^3[DM] ^1Вам выдано: ^4%s ^1и ^4двойной прыжок^1.", g_szChatPrefix, szMessage);
		}
		case 5:
		{
			SetBit(g_iBitDoubleDamage, id);
			client_print_color(id, id, "%s ^3[DM] ^1Вам выдано: ^4%s ^1и ^4двойной урон^1.", g_szChatPrefix, szMessage);
		}
		case 6:
		{
			rg_give_item_ex(id, "weapon_flashbang", GT_REPLACE, 2);
			client_print_color(id, id, "%s ^3[DM] ^1Вам выдано: ^4%s ^1и^4 2 слеповых гранаты^1.", g_szChatPrefix, szMessage);
		}
		case 7:
		{
			rg_give_item_ex(id, "weapon_hegrenade", GT_REPLACE, 10);
			client_print_color(id, id, "%s ^3[DM] ^1Вам выдано: ^4%s ^1и^4 10 взрывных гранат^1.", g_szChatPrefix, szMessage);
		}
		case 8:
		{
			SetBit(g_iBitFrostNade, id);
			check_frostnade_fwd();
			rg_give_item(id, "weapon_smokegrenade", GT_REPLACE);
			client_print_color(id, id, "%s ^3[DM] ^1Вам выдано: ^4%s ^1и ^4граната заморозка^1.", g_szChatPrefix, szMessage);
		}
		case 9:
		{
			SetBit(g_iBitLongJump, id);
			ham_jump_check();
			client_print_color(id, id, "%s ^3[DM] ^1Вам выдано: ^4%s ^1и ^4длиный прыжок^1.", g_szChatPrefix, szMessage);
		}
		case 10:
		{
			SetBit(g_iBitKokain, id);
			client_print_color(id, id, "%s ^3[DM] ^1Вам выдано: ^4%s ^1и ^4кокаин^1.", g_szChatPrefix, szMessage);
		}

		// bad skills
		case 11:
		{
			set_entvar(id, var_health, 50.0);
			client_print_color(id, id, "%s ^3[DM] ^1Вам выдано: ^4%s ^1и^4 -50HP^1.", g_szChatPrefix, szMessage);
		}
		case 12:
		{
			jbe_set_user_maxspeed(id, 200.0, true);
			client_print_color(id, id, "%s ^3[DM] ^1Вам выдано: ^4%s ^1и^4 замедление^1.", g_szChatPrefix, szMessage);
		}
		case 13:
		{
			set_entvar(id, var_gravity, 1.3);
			client_print_color(id, id, "%s ^3[DM] ^1Вам выдано: ^4%s ^1и^4 130％ гравитации^1.", g_szChatPrefix, szMessage);
		}
	}
	
	return PLUGIN_HANDLED;
}

public jbe_deathmatch_take_god(id)
{
	id -= TASK_DEATHMATCH_PLAYER;

	if (IsNotSetBit(g_iBitUserAlive, id))
		return;

	UTIL_ScreenFade(id, 0, 0, 0, 0, 0, 0, 0, 1); // убираем зеленоватый экран
	jbe_set_user_rendering(id, kRenderFxNone, 0, 0, 0, kRenderNormal, 0); // убираем глов зеленый
	
	if (!jbe_get_user_godmode(id))
	{
		set_entvar(id, var_takedamage, DAMAGE_YES);
	}
}

jbe_deathmatch_save_spawn(id)
{
	if (g_iDeathMatchSpawnsNum >= DEATHMATCH_SPAWN_NUM)
		return;

	new iFlag = get_entvar(id, var_flags);

	// если игок в присяде/на лестнице/в воде игнорим
	if ((iFlag) & (FL_DUCKING|FL_FLY|FL_WATERJUMP))
		return;

	// если игрок не на земле игнорим
	if (~iFlag & FL_ONGROUND)
		return;

	static Float:vOrigin[3];
	get_entvar(id, var_origin, vOrigin);

	// если игрок застрял игнорим
	if (!is_hull_vacant(vOrigin, HULL_HUMAN, id))
		return;

	// если дистанция от места где заспанился маленькая игнорим
	if (get_distance_f(vOrigin, g_fTempSpawnOrigin[id]) < MIN_DIST_SPAWN)
		return;
/*
	if (find_sphere_class(id, "info_player_deathmatch", 500.0, "", 1))
	{
		IsSetBit(g_iBitRconAdmin, id) && client_print(id, print_console, "(dm spawn) spawn find_sphere_class info_player_deathmatch");
		return;
	}
*/
	
	static Float:spawn_data[DEATHMATCH_SPAWNS];	

	for (new i; i < g_iDeathMatchSpawnsNum; i++)
	{
		ArrayGetArray(g_aDeathMatchSpawns, i, spawn_data);
		xs_vec_copy(spawn_data[DMS_ORIGIN], g_fTempSpawnOrigin[id]);
		if (get_distance_f(vOrigin, g_fTempSpawnOrigin[id]) < MIN_DIST_SPAWN)
			return;
	}

	xs_vec_copy(vOrigin, spawn_data[DMS_ORIGIN]);
	get_entvar(id, var_angles, spawn_data[DMS_ANGLES]);
	get_entvar(id, var_v_angle, spawn_data[DMS_V_ANGLE]);

	ArrayPushArray(g_aDeathMatchSpawns, spawn_data);

	g_iDeathMatchSpawnsNum++;
}

jbe_deathmatch_random_spawn(id)
{
	if (IsNotSetBit(g_iBitUserAlive, id))
		return;

	if (g_iDeathMatchSpawnsNum < MIN_SPAWNS_NUM)
	{
		get_entvar(id, var_origin, g_fTempSpawnOrigin[id]);
		return;
	}

	static Float:spawn_data[DEATHMATCH_SPAWNS];

	ArrayGetArray(g_aDeathMatchSpawns, random(g_iDeathMatchSpawnsNum), spawn_data);

	set_entvar(id, var_origin, spawn_data[DMS_ORIGIN]);
	set_entvar(id, var_angles, spawn_data[DMS_ANGLES]);
	set_entvar(id, var_v_angle, spawn_data[DMS_V_ANGLE]);

	xs_vec_copy(spawn_data[DMS_ORIGIN], g_fTempSpawnOrigin[id]);

	return;
}
/*
stock jbe_deathmatch_delete_spawn(iSpawn)
{

}

//===================

stock old_jbe_deathmatch_random_spawn(id) // Типо рандомный спавн
{
	if (!g_iDeathMatchSpawnsNum || IsNotSetBit(g_iBitUserAlive, id))
		return;

	new iSpawn = random(g_iDeathMatchSpawnsNum - 1);
	new Float:fSpawnData[3];
	
	fSpawnData[0] = g_fDeathMatchSpawns[iSpawn][SPAWN_ORIGIN_X];
	fSpawnData[1] = g_fDeathMatchSpawns[iSpawn][SPAWN_ORIGIN_Y];
	fSpawnData[2] = g_fDeathMatchSpawns[iSpawn][SPAWN_ORIGIN_Z];

//	engfunc(EngFunc_SetOrigin, id, fSpawnData); // Задаем координаты игрока
	entity_set_origin(id, fSpawnData);
	
	fSpawnData[0] = g_fDeathMatchSpawns[iSpawn][SPAWN_ANGLES_X];
	fSpawnData[1] = g_fDeathMatchSpawns[iSpawn][SPAWN_ANGLES_Y];
	fSpawnData[2] = g_fDeathMatchSpawns[iSpawn][SPAWN_ANGLES_Z];

	// Задаем направление взгляда игрока (куда смотрит игрок)
	set_entvar(id, var_angles, fSpawnData);

	fSpawnData[0] = g_fDeathMatchSpawns[iSpawn][SPAWN_V_ANGLE_X];
	fSpawnData[1] = g_fDeathMatchSpawns[iSpawn][SPAWN_V_ANGLE_Y];
	fSpawnData[2] = g_fDeathMatchSpawns[iSpawn][SPAWN_V_ANGLE_Z];

	// Задаем направленеи взгляда игрока (куда смотрит модель игрока)
	set_entvar(id, var_v_angle, fSpawnData);
	
//	unstuck_user(id);
}


// .cfg spawns
stock load_deathmatch_random_spawns()
{
	return PLUGIN_HANDLED;
	new szCfgDir[64], szCfgFile[128], szMapName[64];
	get_mapname(szMapName, charsmax(szMapName));
	get_localinfo("amxx_configsdir", szCfgDir, charsmax(szCfgDir));
	formatex(szCfgFile, charsmax(szCfgFile), "%s/jb_engine/jbdm_spawns/%s.cfg", szCfgDir, szMapName);

	new fp = fopen(szCfgFile, "rt");
	
	if (!fp)
	{
		log_amx("Not file jbdm_spawns/%s.cfg", szMapName);
		return -1;
	}

	new szBuffer[64], szParse[9][22];

	while (!feof(fp))
	{
		fgets(fp, szBuffer, charsmax(szBuffer));
		trim(szBuffer);
		if (!szBuffer[0] || szBuffer[0] == ';') continue;

		parse(szBuffer,
			szParse[SPAWN_ORIGIN_X],	charsmax(szParse[]),
			szParse[SPAWN_ORIGIN_Y],	charsmax(szParse[]),
			szParse[SPAWN_ORIGIN_Z],	charsmax(szParse[]),
			
			szParse[SPAWN_ANGLES_X],	charsmax(szParse[]),
			szParse[SPAWN_ANGLES_Y],	charsmax(szParse[]),
			szParse[SPAWN_ANGLES_Z],	charsmax(szParse[]),
			
			szParse[SPAWN_V_ANGLE_X],	charsmax(szParse[]),
			szParse[SPAWN_V_ANGLE_Y],	charsmax(szParse[]),
			szParse[SPAWN_V_ANGLE_Z],	charsmax(szParse[])
		);
		
		g_fDeathMatchSpawns[g_iDeathMatchSpawnsNum][SPAWN_ORIGIN_X] = str_to_float(szParse[SPAWN_ORIGIN_X]);
		g_fDeathMatchSpawns[g_iDeathMatchSpawnsNum][SPAWN_ORIGIN_Y] = str_to_float(szParse[SPAWN_ORIGIN_Y]);
		g_fDeathMatchSpawns[g_iDeathMatchSpawnsNum][SPAWN_ORIGIN_Z] = str_to_float(szParse[SPAWN_ORIGIN_Z]);
		
		g_fDeathMatchSpawns[g_iDeathMatchSpawnsNum][SPAWN_ANGLES_X] = str_to_float(szParse[SPAWN_ANGLES_X]);
		g_fDeathMatchSpawns[g_iDeathMatchSpawnsNum][SPAWN_ANGLES_Y] = str_to_float(szParse[SPAWN_ANGLES_Y]);
		g_fDeathMatchSpawns[g_iDeathMatchSpawnsNum][SPAWN_ANGLES_Z] = str_to_float(szParse[SPAWN_ANGLES_Z]);
		
		g_fDeathMatchSpawns[g_iDeathMatchSpawnsNum][SPAWN_V_ANGLE_X] = str_to_float(szParse[SPAWN_V_ANGLE_X]);
		g_fDeathMatchSpawns[g_iDeathMatchSpawnsNum][SPAWN_V_ANGLE_Y] = str_to_float(szParse[SPAWN_V_ANGLE_Y]);
		g_fDeathMatchSpawns[g_iDeathMatchSpawnsNum][SPAWN_V_ANGLE_Z] = str_to_float(szParse[SPAWN_V_ANGLE_Z]);
		
		//g_iDeathMatchSpawnsNum++;
		if (++g_iDeathMatchSpawnsNum >= DEATHMATCH_SPAWN_NUM) break;
	}
	
	return fclose(fp);
}
*/