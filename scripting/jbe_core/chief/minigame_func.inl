/* Mini Game Chief */

is_shief_global_game() return (g_iChiefGameType && ((1<<g_iChiefGameType) & CHIEF_ALL_GLOBAL_GAMES));

is_yes_start_minigame(id, iGame)
{
	if ((iGame == GAME_NONE && g_iChiefGameType != GAME_NONE)
	|| (g_iChiefGameType && g_iChiefGameType != iGame))
	{
		client_print_color(id, id, "%s Недоступно, запущенна игра ^4%s^1!", g_szChatPrefix, g_szMiniGameNames[g_iChiefGameType]);
	}

	else return true;

	return false;
}

jbe_minigame_logo(id)
{
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i))
			continue;
		
		set_member(i, m_flNextDecalTime, 0.0);
	}

	client_print_color(0, print_team_blue,
		"%s %s ^4%n ^1Всем ^3обнулил таймер ^1рисования логотипа.",
		g_szChatPrefix, id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix, id);
}

jbe_minigame_friendlyfire(id)
{
	g_iFriendlyFire = !g_iFriendlyFire;
	
	client_print_color(0, g_iFriendlyFire ? print_team_red : print_team_blue,
		"%s %s ^4%n ^3%s ^1огонь по своим.", g_szChatPrefix, 
		id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix,
		id, g_iFriendlyFire ? "включил" : "выключил");
	
	if (g_iFriendlyFire)
	{
		jbe_set_name_mode("Огонь по своим");
		g_iChiefGameType = GAME_FFIRE;
	}
	else
	{
		jbe_set_name_mode("");
		g_iChiefGameType = GAME_NONE;
	}
}

jbe_minigame_random_skin()
{
	for (new i = 1; i <= MaxClients; i++)
	{
		if (g_iUserTeam[i] != TEAM_PRISONER
		|| IsNotSetBit(g_iBitUserAlive, i) || IsSetBit(g_iBitUserFree, i) || IsSetBit(g_iBitUserWanted, i)
		|| IsSetBit(g_iBitUserSoccer, i) || IsSetBit(g_iBitUserBoxing, i) || IsSetBit(g_iBitUserDuel, i))
			continue;
		
		if (g_iPersonalModel[i] > 0)
			jbe_set_player_prisoner_model(i);
			
		set_entvar(i, var_skin, random_num(0, 3));
	}
	client_print_color(0, print_team_blue, "%s ^1Все заключённые переодеты в ^3случайную одежду^1.", g_szChatPrefix);
}

jbe_minigame_deagle()
{
	for (new i = 1; i <= MaxClients; i++)
	{
		if (g_iUserTeam[i] != TEAM_PRISONER || IsNotSetBit(g_iBitUserAlive, i)
		|| IsSetBit(g_iBitUserSoccer, i) || IsSetBit(g_iBitUserBoxing, i)
		|| IsSetBit(g_iBitUserDuel, i) || IsSetBit(g_iBitUserWanted, i)
		|| IsSetBit(g_iBitUserFree, i)) continue;
		
		rg_remove_item(i, "weapon_deagle", .removeAmmo = true);

		new iEntity = rg_give_item(i, "weapon_deagle", .type = GT_APPEND);

		if (iEntity > 0)
		{
			set_member(iEntity, m_Weapon_iClip, -1);
		}
	}
	client_print_color(0, print_team_red, "%s ^1Всем заключённым выдан ^3'^4Desert Eagle^3' ^1без патронов.", g_szChatPrefix);
}

jbe_minigame_random_word()
{
	client_print_color(0, print_team_blue, "%s ^1Слово начальника: ^3%s", g_szChatPrefix,
		g_szMiniGameWord[ random (sizeof g_szMiniGameWord) ]);
}


/**************************************
*	Кое-как глобальные игры Начальника
***************************************/

jbe_minigame_stop_all()
{
	if (!g_iChiefGameType)
		return;

//	if (g_iChiefGameType == GAME_M9SO)
//		set_cvar_string("mp_round_infinite", cvar_mp_round_infinite);

	remove_task(TASK_CHIEF_GAME_START);
	remove_task(TASK_CHIEF_GAME_START + GAME_BOMBER);
	remove_task(TASK_CHIEF_GAME_START + GAME_GHOST);
	remove_task(TASK_CHIEF_GAME_END);

	jbe_set_name_mode("");
	
	global_clear_blocks();	// убираем глобал блок с функций
	g_iBitUserWanted = 0; // чтоб летали на хуке
	g_szWantedNames[0] = EOS;
	g_szDayModeTimer[0] = EOS;
	g_iFriendlyFire = FF_OFF;
	g_iChiefGameType = GAME_NONE;
}

jbe_minigame_chief(iGame)
{
	if (g_iBitUserWanted)
	{
		client_print_color(g_iChiefId, 0, "%s ^1Нельзя запустить игру если есть розыск!", g_szChatPrefix);
		return;
	}

	if (g_iLastPnId || g_iAlivePlayersNum[TEAM_PRISONER] < 2)
	{
		client_print_color(g_iChiefId, 0, "%s ^1Недоступно, мало игроков.", g_szChatPrefix);
		return;
	}

	if (g_iChiefGlobalGameLimit >= CHIEF_GLOBAL_GAME_LIMIT)
	{
		client_print_color(g_iChiefId, print_team_red,
			"%s ^3Ошибка^1, лимит массовых игр^4 %d ^1на игровую неделю.",
			g_szChatPrefix, CHIEF_GLOBAL_GAME_LIMIT);
		return;
	}

	g_iChiefGlobalGameLimit++;

	if (g_iDuelStatus) jbe_duel_cancel();
	if (g_bAllUseBall) jbe_soccer_remove_ball();

	switch (iGame)
	{
		case GAME_SHTURM:	jbe_set_name_mode("Штурм | Старт", false);
		case GAME_HUNGER:	jbe_set_name_mode("Голодные игры | Старт", false);
		case GAME_M9SO:		jbe_set_name_mode("Мясо | Старт", false);
		case GAME_GHOST:	jbe_set_name_mode("Невидимки | Старт", false);
		case GAME_SNIPER:	jbe_set_name_mode("Снайпера | Старт", false);
		case GAME_BOMBER:	jbe_set_name_mode("Бомбер | Старт", false);
		default: return;
	}

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i)) continue;

		if (IsSetBit(g_iBitUserFree, i))
		{
			jbe_sub_user_free(i);
			SetBit(g_iBitUserFreeNextRound, i);
		}

		// убираем скиллы все
		_delete_user_all_skills(i);
		
		jbe_set_user_godmode(i, 1);
	}

	g_iChiefGameType = iGame;

	set_task(1.0, "jbe_minigame_chief_start", TASK_CHIEF_GAME_START, _, _, "a", g_iDayModeTimer = 11);
}

public jbe_minigame_chief_start()
{
	jbe_open_doors();
	if (--g_iDayModeTimer)
	{
		formatex(g_szDayModeTimer, charsmax(g_szDayModeTimer), "[%i]", g_iDayModeTimer);
		rg_send_audio(0, sounds_countdown[g_iDayModeTimer]);
	}
	else
	{
		g_szDayModeTimer[0] = EOS;

		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i)) continue;

			jbe_set_user_godmode(i, 0);

			if (IsSetBit(g_iBitUserHook, i))
				ClCmd_HookOff(i);
		}

		switch (g_iChiefGameType)
		{
			case GAME_SHTURM:	jbe_minigame_shturm_start();
			case GAME_HUNGER:	jbe_minigame_hunger_start();
			case GAME_M9SO:		jbe_minigame_m9so_start();
			case GAME_GHOST:	jbe_minigame_ghost_start();
			case GAME_SNIPER:	jbe_minigame_sniper_start();
			case GAME_BOMBER:	jbe_minigame_bomber_start();
		}
		set_task(1.0, "jbe_minigame_end_timer", TASK_CHIEF_GAME_END, _, _, "a", g_iDayModeTimer = GAME_DURATION);
	}
}

/* Продолжительность игры начальника, потом убиваем */
public jbe_minigame_end_timer()
{
	if (--g_iDayModeTimer)
	{
		formatex(g_szDayModeTimer, charsmax(g_szDayModeTimer), "[%i]", g_iDayModeTimer);
	}
	else
	{
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i)) continue;

			switch (g_iChiefGameType)
			{
				// Всех убиваем
				case GAME_M9SO, GAME_GHOST:
					user_kill(i, 1);
				
				// Зеков убиваем
				case GAME_SHTURM, GAME_HUNGER:
					if (g_iUserTeam[i] == TEAM_PRISONER)
						user_kill(i, 1);
				
				// Охрану убиваем
				case GAME_BOMBER, GAME_SNIPER:
					if (g_iUserTeam[i] == TEAM_GUARD)
						user_kill(i, 1);
				
				default: return;
			}
		}

		jbe_minigame_stop_all();
	}
}

/* Штурм */
jbe_minigame_shturm_start()
{
	jbe_set_name_mode("Штурм");

	global_set_blocks_server(GB_VIP_MENU, GB_HOOK);

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i)) continue;

		rg_remove_all_items(i);
		rg_give_item(i, "weapon_knife");

		rg_give_item(i, "weapon_ak47");
		rg_set_user_bpammo(i, WEAPON_AK47, 250);
		rg_give_item(i, "weapon_deagle");
		rg_set_user_bpammo(i, WEAPON_DEAGLE, 250);

		switch (g_iUserTeam[i])
		{
			case TEAM_PRISONER:
			{
				set_entvar(i, var_health, 100.0);
				rg_set_user_armor(i, 100, ARMOR_VESTHELM);
			}
			case TEAM_GUARD:
			{
				rg_give_item(i, "weapon_m4a1");
				rg_set_user_bpammo(i, WEAPON_M4A1, 250);

				set_entvar(i, var_health, 250.0);
				rg_set_user_armor(i, 250, ARMOR_VESTHELM);
			}
		}
	}

	jbe_set_hamblock(game_player_hurt, player_weaponstrip, trigger_hurt, armoury_entity, weapon_shield);

	client_print_color(0, 0, "%s Начальник сказал ^4Штурм^1!", g_szChatPrefix);
}

/* Голодные игры */
jbe_minigame_hunger_start()
{
	g_iFriendlyFire = FF_PRISONER;
	jbe_set_name_mode("Голодные игры");
	global_set_blocks_server(GB_VIP_MENU, GB_HOOK);

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i)) continue;

		switch(g_iUserTeam[i])
		{
			case 1:
			{
				rg_remove_all_items(i);
				rg_give_item(i, "weapon_knife");
				rg_give_item(i, "weapon_deagle");
				rg_set_user_bpammo(i, WEAPON_DEAGLE, 250);
				set_entvar(i, var_health, 250.0);
				rg_set_user_armor(i, 250, ARMOR_VESTHELM);
			}
			case 2:
			{
				jbe_set_user_godmode(i, 1);
			}
		}
	}

	jbe_set_hamblock(game_player_hurt, game_player_equip, weaponbox, player_weaponstrip, trigger_hurt,
		armoury_entity, weapon_shield);

	client_print_color(0, 0, "%s Начальник сказал ^4Голодные игры^1, охрана бессмертна и ей запрещено вмешиваться!", g_szChatPrefix);
}

/* Мясо */
public jbe_minigame_m9so_start()
{
	jbe_set_name_mode("Мясо");
	g_iFriendlyFire = FF_OVERALL;
	UTIL_DirectorMessage(0, "Убей всех!", 255, 0, 0, -1.0, 0.25, 1, 3.0, 3.0, 0.1, 0.8);
	client_print_color(0, 0, "%s Начальник сказал ^4Мясо!", g_szChatPrefix);

	global_set_blocks_server(GB_VIP_MENU, GB_HOOK);
//	set_cvar_string("mp_round_infinite", "af");

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i)) continue;

		rg_remove_all_items(i);
		rg_give_item(i, "weapon_knife");
		rg_give_item_ex(i, "weapon_awp", GT_APPEND, 250);
		rg_give_item_ex(i, "weapon_ak47", GT_APPEND, 250);
		rg_give_item_ex(i, "weapon_m249", GT_APPEND, 250);
		rg_give_item_ex(i, "weapon_deagle", GT_APPEND, 250);

		switch (g_iUserTeam[i])
		{
			case TEAM_PRISONER:
			{
				set_entvar(i, var_health, 150.0);
				rg_set_user_armor(i, 150, ARMOR_VESTHELM);
			}
			case TEAM_GUARD:
			{
				set_entvar(i, var_health, 300.0);
				rg_set_user_armor(i, 300, ARMOR_VESTHELM);
			}
		}
	}

	jbe_set_hamblock(game_player_hurt, player_weaponstrip, trigger_hurt, armoury_entity, weapon_shield);
}

/* Невидимки */
jbe_minigame_ghost_start()
{
	jbe_set_name_mode("Невидимки");
	client_print_color(0, print_team_blue,
		"%s Начальник сказал ^4Невидимки^1! ^3(смотри на радар)", g_szChatPrefix);

	global_set_blocks_server(GB_SHOP, GB_RANK_BONUS, GB_VIP_MENU, GB_HOOK);

	g_iBitFallNoDamage = -1;
	
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i))
			continue;
		
		set_entvar(i, var_effects, get_entvar(i, var_effects) | EF_NODRAW);
		
		if (g_eUserCostumes[i][COSTUMES])
			jbe_hide_user_costumes(i);

		set_entvar(i, var_health, 1.0);
		set_entvar(i, var_max_health, 1.0);
		rg_set_user_armor(i, 0, ArmorType:ARMOR_NONE);

		rg_remove_all_items(i);
		rg_give_item(i, "weapon_knife");
		rg_give_item_ex(i, "weapon_usp", GT_REPLACE, 900);
			
		// Эффект невидимости
		UTIL_ScreenFade(i, (1<<12), (1<<12), (0x0004), 190, 190, 190, 50, 1);

		if (IsSetBit(g_iBitUserHook, i))
			CREATE_KILLBEAM(i);
	}

	jbe_set_hamblock(game_player_hurt, func_recharge, func_healthcharger, game_player_equip, player_weaponstrip,
		trigger_hurt, trigger_gravity, armoury_entity, weapon_shield);

	set_task(5.0, "jbe_minigame_ghost_radar", TASK_CHIEF_GAME_START + GAME_GHOST, _, _, "a", GAME_DURATION / 5);
}

public jbe_minigame_ghost_radar()
{
	if (!g_iAlivePlayersNum[TEAM_PRISONER] || !g_iAlivePlayersNum[TEAM_GUARD])
		return;

	new iCoord[MAX_PLAYERS + 1][3], i, j;

	for (i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i))
			continue;

		get_user_origin(i, iCoord[i]);
	}

	for (i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i))
			continue;

		for (j = 1; j <= MaxClients; j++)
		{
			if (IsNotSetBit(g_iBitUserAlive, j))
				continue;

			if (g_iUserTeam[i] != g_iUserTeam[j])
				UTIL_ShowRadar(i, j, iCoord[j]);
		}
	}
}

/* Снайпера */
jbe_minigame_sniper_start()
{
	jbe_set_name_mode("Снайпера");
	client_print_color(0, 0, "%s Начальник сказал ^4Снайпера^1!", g_szChatPrefix);

	global_set_blocks_server(GB_SHOP, GB_RANK_BONUS, GB_VIP_MENU, GB_HOOK);

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i))
			continue;

		rg_remove_all_items(i);
		rg_set_user_armor(i, 0, ArmorType:ARMOR_NONE);

		switch (g_iUserTeam[i])
		{
			case TEAM_PRISONER:
			{
				SetBit(g_iBitDoubleDamage, i);
				set_entvar(i, var_health, 10.0);
				rg_give_item(i, "weapon_knife");
				jbe_set_user_maxspeed(i, 500.0, true);
			}
			case TEAM_GUARD:
			{
				set_entvar(i, var_health, 50.0);
				rg_give_item_ex(i, "weapon_awp", GT_REPLACE, 900);
			}
		}
	}

	jbe_set_hamblock(game_player_hurt, func_recharge, func_healthcharger, game_player_equip, player_weaponstrip,
		trigger_hurt, trigger_gravity, armoury_entity, weapon_shield, weaponbox);
}

/* Бомбер */
jbe_minigame_bomber_start()
{
	jbe_set_name_mode("Бомбер");
	client_print_color(0, 0, "%s Начальник сказал ^4Бомбер^1!", g_szChatPrefix);

	global_set_blocks_server(GB_SHOP, GB_RANK_BONUS, GB_VIP_MENU, GB_HOOK);

	g_bCGameBomberType = true;

	for (new i = 1; i <= MaxClients; i++) {
		if (IsNotSetBit(g_iBitUserAlive, i))
			continue;

		rg_remove_all_items(i);
		rg_give_item(i, "weapon_knife");
		rg_set_user_armor(i, 0, ArmorType:ARMOR_NONE);
		set_entvar(i, var_health, 100.0);

		if (g_iUserTeam[i] == TEAM_GUARD)
		{
			jbe_set_user_godmode(i, 0);
			UTIL_ScreenFade(i, 0, 0, 4, 0, 0, 0, 255, 1);
			SetBit(g_iBitUserFrozen, i);
			set_entvar(i, var_flags, get_entvar(i, var_flags) | FL_FROZEN);
			rg_set_user_noclip(i, 1);
		}
		else
		{
			rg_set_user_footsteps(i, true);
			if (g_eUserCostumes[i][COSTUMES])
				jbe_hide_user_costumes(i);
		}
	}

	jbe_set_hamblock(game_player_equip, func_recharge, func_healthcharger, game_player_equip, player_weaponstrip, trigger_hurt,
		trigger_gravity, armoury_entity, weapon_shield);

	set_task(15.0, "jbe_minigame_bomber_timer", TASK_CHIEF_GAME_START + GAME_BOMBER, _, _, "a", GAME_DURATION / 15);
}

public jbe_minigame_bomber_timer() {
	if (g_bCGameBomberType) {	// Ход охраны
		g_bCGameBomberType = false;
		client_print_center(0, "Ход охраны");
		for (new i = 1; i <= MaxClients; i++) {
			if (IsNotSetBit(g_iBitUserAlive, i))
				continue;

			switch (g_iUserTeam[i])
			{
				case TEAM_PRISONER:
				{
					SetBit(g_iBitUserFrozen, i);
					set_entvar(i, var_flags, get_entvar(i, var_flags) | FL_FROZEN);
					jbe_set_user_rendering(i, kRenderFxGlowShell, 0, 0, 0, kRenderTransAlpha, 0);
					jbe_save_user_rendering(i);
				}
				case TEAM_GUARD:
				{
					UTIL_ScreenFade(i, 512, 512, 0, 0, 0, 0, 255, 1);
					ClearBit(g_iBitUserFrozen, i);
					set_entvar(i, var_flags, get_entvar(i, var_flags) & ~FL_FROZEN);
					rg_give_item_ex(i, "weapon_hegrenade", GT_REPLACE, 100);
				}
			}
		}
	}
	else	// Ход зеков
	{
		g_bCGameBomberType = true;
		client_print_center(0, "Ход зеков");
		for (new i = 1; i <= MaxClients; i++) {
			if (IsNotSetBit(g_iBitUserAlive, i))
				continue;

			switch (g_iUserTeam[i]) {
				case TEAM_PRISONER: {
					ClearBit(g_iBitUserFrozen, i);
					set_pev(i, pev_flags, pev(i, pev_flags) & ~FL_FROZEN);
					jbe_set_user_rendering(i, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
					jbe_save_user_rendering(i);
				}
				case TEAM_GUARD: {
					UTIL_ScreenFade(i, 0, 0, 4, 0, 0, 0, 255, 1);
					SetBit(g_iBitUserFrozen, i);
					set_pev(i, pev_flags, pev(i, pev_flags) | FL_FROZEN);
				}
			}
		}
	}
}