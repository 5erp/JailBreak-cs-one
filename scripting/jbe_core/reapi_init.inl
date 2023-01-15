reapi_init()
{
	RegisterHookChain(RG_CBasePlayer_Spawn, "CBasePlayer_Spawn_Post", .post = 1);
	RegisterHookChain(RG_CBasePlayer_Killed, "CBasePlayer_Killed", .post = 0);
	RegisterHookChain(RG_CBasePlayer_Killed, "CBasePlayer_Killed_Post", .post = 1);
	RegisterHookChain(RG_CBasePlayer_TraceAttack, "CBasePlayer_TraceAttack", .post = 0);
	RegisterHookChain(RG_CBasePlayer_TakeDamage, "CBasePlayer_TakeDamage", .post = 0);
	RegisterHookChain(RG_CBasePlayer_SetClientUserInfoName, "CBasePlayer_SetClientUserInfoName", .post = 0);

	RegisterHookChain(RH_SV_StartSound, "SV_StartSound", .post = 0);

	DisableHookChain(HookChain_PreThink = RegisterHookChain(RG_CBasePlayer_PreThink, "CBasePlayer_PreThink", .post = 0));
}

public CBasePlayer_Spawn_Post(id)
{
#if defined DEVELOPER_OPTIONS
	if (!g_iUserTeam[id]) return HC_BREAK;
#endif
	if (is_user_alive(id))
	{
		if (IsNotSetBit(g_iBitUserAlive, id))
		{
			if (IsSetBit(g_iBitActiveGhost, id)) // заюзан призрак
				return spawn_ghost_user(id);

			SetBit(g_iBitUserAlive, id);
			g_iAlivePlayersNum[g_iUserTeam[id]]++;

			ClearBit(g_iBitUserGhost, id); // вдруг мы были призраком
			jbe_set_user_maxspeed(id); //ClearBit(g_iBitFastRun, id); // хз зачем -_-
		}
	//	else // IsSetBit(g_iBitUserAlive, id)
	//	{
		rg_remove_all_items(id, .removeSuit = false);
		rg_give_item(id, "weapon_knife");
		rg_set_user_armor(id, 0, ARMOR_NONE);
	//	}

		jbe_default_player_model(id);
		jbe_reset_user_informer(id);
		jbe_open_menu_spawn(id);
		spawn_user_fix(id, 0.2);
		jbe_reset_user_groupinfo(id);
	}

	return HC_CONTINUE;
}

new g_iUserEntityUse[MAX_PLAYERS + 1];

public CBasePlayer_Killed(iVictim, iKiller, iGib)
{
	if (IsSetBit(g_iBitUserVoteDayMode, iVictim) || IsSetBit(g_iBitUserFrozen, iVictim))
		set_entvar(iVictim, var_flags, get_entvar(iVictim, var_flags) & ~FL_FROZEN);

	if (g_eUserRendering[iVictim][RENDER_STATUS])
	{
		jbe_set_user_rendering(iVictim, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
		g_eUserRendering[iVictim][RENDER_STATUS] = false;
	}
	
//	if (iKiller > MaxClients && g_iDayMode != DAY_DAYMODE && is_entity(iKiller) && pev(iKiller, pev_classname) == g_iszFuncVehicle)
	if (iKiller > MaxClients && g_iDayMode != DAY_DAYMODE && is_entity(iKiller))// && get_entvar(iKiller, var_classname) == g_iszFuncVehicle)
	{
		static szClassName[32];
		get_entvar(iKiller, var_classname, szClassName, charsmax(szClassName));

		if (equal(szClassName, "func_vehicle"))
		{
			new iDriver = get_entvar(iKiller, var_iuser4);
			g_bVehicleKilled = true;

		//	set_pev_string(iKiller, pev_classname, g_iszTrackTrain); 
			set_entvar(iKiller, var_classname, "tracktrain");

			if (iDriver && IsSetBit(g_iBitUserConnected, iDriver))
			{
				SetHookChainArg(2, ATYPE_INTEGER, iDriver);
				g_iUserEntityUse[iDriver] = iKiller;

				if (g_iUserTeam[iDriver] == TEAM_PRISONER && g_iUserTeam[iVictim] == TEAM_GUARD)
				{
					jbe_prisoner_riot_start(iDriver);
					jbe_add_user_wanted(iDriver);
				}
			}
			else
			{
				set_entvar(iKiller, var_iuser4, 0);
			}

			SetHookChainArg(3, ATYPE_INTEGER, GIB_ALWAYS);
		}
	}

	if (g_iDayMode == DAY_DEATHMATCH && jbe_is_user_valid(iKiller)) //  || g_iDayMode == DAY_RESTART
		jbe_deathmatch_save_spawn(iVictim);
}

public CBasePlayer_Killed_Post(iVictim, iKiller, iGib)
{
	if (IsNotSetBit(g_iBitUserAlive, iVictim)) return;

	g_iAlivePlayersNum[g_iUserTeam[iVictim]]--;
	ClearBit(g_iBitUserAlive, iVictim);

	jbe_reset_user_informer(iVictim);
	reload_user_voice(iVictim);
	grab_off_to_user(iVictim);

	switch (g_iDayMode)
	{
		case DAY_NORMAL, DAY_FREEDAY:
		{
			if (g_iUserTeam[iVictim] == TEAM_PRISONER)
			{
				if (g_iDayMode == DAY_FREEDAY && g_iAlivePlayersNum[TEAM_PRISONER] == 2)
					jbe_free_day_ended();

				jbe_check_last_prisoner();

				if (IsSetBit(g_iBitUserFree, iVictim))
					jbe_sub_user_free(iVictim);
		
				else if (IsSetBit(g_iBitUserWanted, iVictim))
				{
					jbe_sub_user_wanted(iVictim);
					if (jbe_is_user_valid(iKiller) && g_iUserTeam[iKiller] == TEAM_GUARD)
					{
						jbe_set_user_money(iKiller, g_iUserMoney[iKiller] + g_eAllCvars[KILLED_WANTED_MODEY], 1);
						check_user_quest(iKiller, QUEST_ID_KILL_WANTED);					
					}
				}
		
				else if (IsSetBit(g_iBitUserBoxing, iVictim))
				{
					ClearBit(g_iBitUserBoxing, iVictim);
					
					if (jbe_is_user_valid(iKiller) && IsSetBit(g_iBitUserBoxing, iKiller))
						check_user_quest(iKiller, QUEST_ID_BOXING);

					if (!g_iBitUserBoxing)
						jbe_boxing_disable_all();
				}

				if (jbe_is_user_valid(iKiller) && iKiller != iVictim)
				{
					jbe_check_teammate_kill(iVictim, iKiller);

					if (g_iUserTeam[iKiller] == TEAM_PRISONER)
						check_user_quest(iKiller, QUEST_ID_PN_KILL_PN);

					else if (g_iUserTeam[iKiller] == TEAM_GUARD)
					{
						check_user_quest(iKiller, QUEST_ID_GR_KILL_PN);
						jbe_add_user_killed_reasons(iKiller, iVictim);
					}
				}
			}

			else if (g_iUserTeam[iVictim] == TEAM_GUARD)
			{
				if (iVictim == g_iChiefId)
				{
					g_iChiefStatus = CHIEF_DEAD;
					g_iChiefId = 0;
					jbe_set_name_chief();
						
					if (jbe_is_user_valid(iKiller))
					{
						if (g_iUserTeam[iKiller] == TEAM_PRISONER && g_iPlayersNum[TEAM_PRISONER] >= 5)
							jbe_set_user_money(iKiller, g_iUserMoney[iKiller] + g_eAllCvars[KILLED_CHIEF_MODEY], 1);
						
						check_user_quest(iKiller, QUEST_ID_KILLER_CHIEF);
					}
				}

				if (jbe_is_user_valid(iKiller) && g_iUserTeam[iKiller] == TEAM_PRISONER)
				{
					if (IsNotSetBit(g_iBitUserWanted, iKiller))
					{
						jbe_prisoner_riot_start(iKiller);
						jbe_add_user_wanted(iKiller);
					}
					add_user_wanted_num(iKiller);

					check_user_quest(iKiller, QUEST_ID_PN_KILL_GR);

					if (g_iPlayersNum[TEAM_PRISONER] >= 5)
						jbe_set_user_money(iKiller, g_iUserMoney[iKiller] + g_eAllCvars[KILLED_GUARD_MODEY], 1);
					
					if (is_shief_global_game() && !g_iAlivePlayersNum[TEAM_GUARD] && g_iAlivePlayersNum[TEAM_PRISONER] == 1)
					{
						if (g_iPlayersNum[TEAM_PRISONER] >= 8)
							SetBit(g_iBitUserFreeNextRound, iKiller);
						else
							jbe_set_user_money(iKiller, g_iUserMoney[iKiller] + g_eAllCvars[LAST_PRISONER_MODEY], 1);

						client_print_color(0, iKiller, "%s ^1Последний герой ^3%n ^1получит ^4%s^1.", g_szChatPrefix,
							iKiller, g_iPlayersNum[TEAM_PRISONER] >= 8 ? "Выходной" : fmt("%d$", g_eAllCvars[LAST_PRISONER_MODEY]));
					}
				}
			}

			jbe_clear_user_shop(iVictim);
			jbe_sub_user_soccer(iVictim);

			if (g_bJihadGame && jbe_is_user_valid(iKiller))
			{
				if (g_iJihadRole[iKiller] == DJR_MARTIAN)
				{
					new Float:fHealth = get_entvar(iKiller, var_health);
					if (fHealth > 0.0)
						set_entvar(iKiller, var_health, fHealth + 50.0);
				}

				else if (g_iJihadRole[iKiller] == DJR_SMUGGLER)
				{
					new Float:fHealth = get_entvar(iKiller, var_health);
					if (fHealth > 0.0)
						set_entvar(iKiller, var_health, fHealth + 100.0);
				}
			}

			if (g_iDuelStatus && IsSetBit(g_iBitUserDuel, iVictim))
			{
				if (iVictim != iKiller && jbe_is_user_valid(iKiller) && IsSetBit(g_iBitUserDuel, iKiller))
					jbe_duel_ended(iVictim);

				else if (g_iDuelType == DT_HEGRENADE && jbe_is_user_valid(iKiller) && IsSetBit(g_iBitUserDuel, iKiller))
				{
					new iInflictor = get_entvar(iVictim, var_dmg_inflictor);
					if (is_entity(iInflictor))
					{
						new szClassName[32];
						get_entvar(iInflictor, var_classname, szClassName, charsmax(szClassName));
						if (equali(szClassName, "grenade"))
							jbe_duel_ended(iVictim);
					}
				}
				 
				if (g_iDuelStatus)
					jbe_duel_ended(iVictim, .bWinDuel = false);
			}

			if (IsSetBit(g_iBitShowDeathMenu, iVictim) && jbe_get_players_num(NUM_ALIVE_ALL))
				Show_DeathUserMenu(iVictim);

			if (IsSetBit(g_iBitUserHook, iVictim))
				remove_task(iVictim + TASK_HOOK_THINK);
		}
		
		case DAY_DAYMODE:
		{
			if (IsSetBit(g_iBitUserVoteDayMode, iVictim))
			{
				ClearBit(g_iBitUserVoteDayMode, iVictim);
				ClearBit(g_iBitUserDayModeVoted, iVictim);
				show_menu(iVictim, 0, "^n");
				jbe_informer_offset_down(iVictim);
				jbe_menu_unblock(iVictim);
				UTIL_ScreenFade(iVictim, 512, 512, 0, 0, 0, 0, 255, 1);
			}
		}
		
		case DAY_DEATHMATCH:
		{
			jbe_deathmatch_respawn(iVictim);
			jbe_clear_user_shop(iVictim);
			jbe_sub_user_soccer(iVictim);

			if (jbe_is_user_valid(iKiller))
			{
				jbe_check_teammate_kill(iVictim, iKiller);
				check_user_quest(iKiller, QUEST_ID_KING_DM, .bMessage = true, .bGameDay = true);
			}
		}
	}

	if (g_bVehicleKilled)
	{
		g_bVehicleKilled = false;
		if (iKiller <= MaxClients)
			iKiller = g_iUserEntityUse[iKiller];
	//	set_pev_string(iKiller, pev_classname, g_iszFuncVehicle);
		set_entvar(iKiller, var_classname, "func_vehicle");

		if (g_iDayMode == DAY_FREEDAY)
		{
			spawn_user_fix(iVictim, .fTimeRespawn = 3.0);
			rg_send_bartime(iVictim, 3);
		}
	}
}

public CBasePlayer_TraceAttack(iVictim, iAttacker, Float:fDamage, Float:fDeriction[3], iTraceHandle, iBitDamage)
{
	if (jbe_is_user_valid(iAttacker))
	{
		static Float:fDamageOld; fDamageOld = fDamage;
		
		if (g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY)
		{
			if (g_bSoccerStatus && IsSetBit(g_iBitUserSoccer, iAttacker))
			{
				if (IsSetBit(g_iBitUserSoccer, iVictim))
				{
					if (g_iSoccerUserTeam[iVictim] == g_iSoccerUserTeam[iAttacker])
						return HC_BREAK;

					SetHookChainArg(3, ATYPE_FLOAT, 0.0);
					return HC_BREAK; // Skip all forwards and real function, use my return value
				}
				return HC_BREAK;
			}
			
			if (g_iBoxingGame && IsSetBit(g_iBitUserBoxing, iAttacker))
			{
				if (g_iBoxingGame != BOXING_WAIT && IsSetBit(g_iBitUserBoxing, iVictim))
				{
					// блочим атаку с оружия взятого через кнопку в боксе
					if (get_user_weapon(iAttacker) != CSW_KNIFE || (g_iBoxingGame == BOXING_RUN_TEAM && g_iBoxingUserTeam[iVictim] == g_iBoxingUserTeam[iAttacker]))
						return HC_BREAK;
					
					switch (g_iBoxingTypeKick[iAttacker])
					{
						case 2:
						{
							if (get_member(iVictim, m_LastHitGroup) == HIT_HEAD)
							{
								fDamage = 22.0;
								UTIL_ScreenShake(iVictim, (1<<15), (1<<14), (1<<15));
								UTIL_ScreenFade(iVictim, (1<<13), (1<<13), 0, 0, 0, 0, 245);
								emit_sound(iVictim, CHAN_AUTO, sound_super_hit, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
							}
							else fDamage = 15.0;
						}
						case 3:
						{
							if (get_member(iVictim, m_LastHitGroup) == HIT_HEAD)
							{
								fDamage = 9.0;
								UTIL_ScreenShake(iVictim, (1<<12), (1<<12), (1<<12));
								UTIL_ScreenFade(iVictim, (1<<10), (1<<10), 0, 50, 0, 0, 200);
							}
							else fDamage = 6.0;
						}
						case 4:
						{
							if (get_member(iVictim, m_LastHitGroup) == HIT_HEAD)
							{
								fDamage = 18.0;
								UTIL_ScreenShake(iVictim, (1<<15), (1<<14), (1<<15));
								UTIL_ScreenFade(iVictim, (1<<13), (1<<13), 0, 0, 0, 0, 245);
								emit_sound(iVictim, CHAN_AUTO, sound_super_hit, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
							}
							else fDamage = 12.0;
						}
						default:
						{
							if (get_member(iVictim, m_LastHitGroup) == HIT_HEAD)
							{
								fDamage = 15.0;
								UTIL_ScreenShake(iVictim, (1<<12), (1<<12), (1<<12));
								UTIL_ScreenFade(iVictim, (1<<10), (1<<10), 0, 50, 0, 0, 200);
							}
							else fDamage = 9.0;
						}
					}
					SetHookChainArg(3, ATYPE_FLOAT, fDamage);
					return HC_CONTINUE;
				}
				return HC_BREAK;
			}
			
			if (g_iDuelStatus)
			{
				if (g_iDuelStatus == DS_READY && IsSetBit(g_iBitUserDuel, iVictim))
					return HC_BREAK;

				if (g_iDuelStatus == DS_RUN)
				{
					if (IsSetBit(g_iBitUserDuel, iVictim) || IsSetBit(g_iBitUserDuel, iAttacker))
					{
						if (IsSetBit(g_iBitUserDuel, iVictim) && IsSetBit(g_iBitUserDuel, iAttacker) && get_user_weapon(iAttacker) == g_iDuelAllWeapons[g_iDuelType])
						{
							// Для дуэли 'зек с зеком' и 'кт с кт' (чтоб не резало дамаг)
							if (g_iUserTeam[iAttacker] == g_iUserTeam[iVictim])
							{
								fDamage = (fDamage / 0.35);
								if (fDamageOld != fDamage)
									SetHookChainArg(3, ATYPE_FLOAT, fDamage);
							}
							return HC_CONTINUE;
						}
						return HC_BREAK;
					}
				}
			}
			
			if (g_iUserTeam[iAttacker] == TEAM_PRISONER)
			{
				if (g_iUserTeam[iVictim] == TEAM_GUARD)
				{
					if (g_iBitElectroCostume && IsSetBit(g_iBitElectroCostume, iVictim) && get_user_weapon(iAttacker) == CSW_KNIFE)
					{
						fDamage = (fDamage * 0.3);
						jbe_user_frost_electro_costum(iAttacker);
					}

					if (fDamage > 12.0)
					{
						if (IsNotSetBit(g_iBitUserWanted, iAttacker))
						{
							jbe_prisoner_riot_start(iAttacker);
							jbe_add_user_wanted(iAttacker);
						}
						add_user_wanted_num(iAttacker);
					}
					
					if (g_iBitUserFrozen && IsSetBit(g_iBitUserFrozen, iVictim))
						return HC_BREAK;
					
					// в наручниках за удар дается розыск и зек замораживается от элл.костюма
					if (is_user_braceletes(iAttacker, BR_CHECK_ATTACK))
					{
						client_print_color(iAttacker, print_team_red, "%s На вас ^3надеты наручники^1, вы не можете атаковать.", g_szChatPrefix);
						SetHookChainArg(3, ATYPE_FLOAT, 0.0);
						return HC_CONTINUE;
					}
				}
			}
		}

		// Бессмертные не могут атаковать в ДМ режиме
		if (g_iDayMode == DAY_DEATHMATCH && jbe_get_user_godmode(iAttacker))
			return HC_BREAK;

		// Множитель дамага для ножей
		if (g_iBitWeaponStatus && IsSetBit(g_iBitWeaponStatus, iAttacker) && get_user_weapon(iAttacker) == CSW_KNIFE)
		{
			if (IsSetBit(g_iBitBalisong, iAttacker))
				fDamage = (fDamage * 1.7);
				
			else if (IsSetBit(g_iBitBasaballbat, iAttacker))
				fDamage = (fDamage * 2.0);
				
			else if (IsSetBit(g_iBitGuardBaton, iAttacker))
				fDamage = (fDamage * 1.6);
		}

		// Уменьшенный урон жертвы
		if (g_iBitKokain && IsSetBit(g_iBitKokain, iVictim))
			fDamage = (fDamage * 0.5);
		
		// Двойной урон атакера
		if (g_iBitDoubleDamage && IsSetBit(g_iBitDoubleDamage, iAttacker))
			fDamage = (fDamage * 2.0);
		
		if (g_iUserTeam[iVictim] == g_iUserTeam[iAttacker])
		{
			switch (g_iFriendlyFire)
			{
				case FF_OFF: return HC_BREAK; // Выкл
				case FF_PRISONER:
				{	// Междку зеками
					if (g_iUserTeam[iVictim] == TEAM_PRISONER)
					{
						// FD не участвует
						if (IsSetBit(g_iBitUserFree, iVictim) || IsSetBit(g_iBitUserFree, iAttacker))
							return HC_BREAK;

						fDamage = (fDamage / 0.35);
					}
					else return HC_BREAK;
				}
				case FF_GUARD:
				{
					if (g_iUserTeam[iVictim] == TEAM_GUARD)
						fDamage = (fDamage / 0.35);
					else
						return HC_BREAK;
				}
				case FF_OVERALL: fDamage = (fDamage / 0.35);	// общий огонь
				case FF_TEAM:
				{
					if (g_iUserTeam[iVictim] == TEAM_PRISONER)
					{
						if (g_iUserGameTeam[iVictim] == g_iUserGameTeam[iAttacker])
							return HC_BREAK;

						fDamage = (fDamage / 0.35);
					}
					else return HC_BREAK;
				}
			}
		}
		
		if (fDamageOld != fDamage)
			SetHookChainArg(3, ATYPE_FLOAT, fDamage);

		if (!jbe_get_user_godmode(iVictim))
			check_user_quest(iAttacker, QUEST_ID_DAMAGER, floatround(fDamage, floatround_floor), .bMessage = false);
	}
	return HC_CONTINUE;
}

public CBasePlayer_TakeDamage(iVictim, iInflictor, iAttacker, Float:fDamage, iBitDamage)
{
	if (g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY)
	{
		if (g_iDuelStatus)
		{
			if ((IsSetBit(g_iBitUserDuel, iVictim) && IsNotSetBit(g_iBitUserDuel, iAttacker))
			|| (IsNotSetBit(g_iBitUserDuel, iVictim) && IsSetBit(g_iBitUserDuel, iAttacker)))
			{
				SetHookChainReturn(ATYPE_INTEGER, 0);
				return HC_BREAK;
			}
			
			else if (IsSetBit(g_iBitUserDuel, iVictim) && IsSetBit(g_iBitUserDuel, iAttacker))
				return HC_CONTINUE;
		}

		if (jbe_is_user_valid(iAttacker) && iBitDamage & DMG_GRENADE) // (1<<24)
		{
			if (is_user_braceletes(iAttacker, BR_CHECK_ATTACK))
			{
				client_print_color(iAttacker, print_team_red, "%s На вас ^3надеты наручники^1, вы не можете атаковать.", g_szChatPrefix);
				SetHookChainReturn(ATYPE_INTEGER, 0);
				return HC_BREAK;
			}

			if (g_iUserTeam[iVictim] == TEAM_GUARD && g_iUserTeam[iAttacker] == TEAM_PRISONER && !g_iDuelStatus && fDamage > 12.0) // && IsNotSetBit(g_iBitUserDuel, iAttacker)
			{
				if (IsNotSetBit(g_iBitUserWanted, iAttacker))
				{
					jbe_prisoner_riot_start(iAttacker);
					jbe_add_user_wanted(iAttacker);
				}
				add_user_wanted_num(iAttacker);
			}

			if (g_iUserTeam[iVictim] == g_iUserTeam[iAttacker])
			{
				switch (g_iFriendlyFire)
				{
					case 0: 
					{
						SetHookChainReturn(ATYPE_INTEGER, 0);
						return HC_BREAK;
					}
					case 1:
					{
						if (IsSetBit(g_iBitUserFree, iVictim) || IsSetBit(g_iBitUserFree, iAttacker))
						{
							SetHookChainReturn(ATYPE_INTEGER, 0);
							return HC_BREAK;
						}

						if (g_iUserTeam[iVictim] == TEAM_PRISONER)
							fDamage = (fDamage / 0.35);
						else
						{
							SetHookChainReturn(ATYPE_INTEGER, 0);
							return HC_BREAK;
						}
					}
					case 2:
					{
						if (g_iUserTeam[iVictim] == TEAM_GUARD)
							fDamage = (fDamage / 0.35);
						else
						{
							SetHookChainReturn(ATYPE_INTEGER, 0);
							return HC_BREAK;
						}
					}
					case 3: fDamage = (fDamage / 0.35);
				}
				SetHookChainArg(4, ATYPE_FLOAT, fDamage);
			}
		}
	}

	if ((iBitDamage & DMG_FALL) && (IsSetBit(g_iBitHookUse, iVictim) || IsSetBit(g_iBitFallNoDamage, iVictim)))
	{
		SetHookChainReturn(ATYPE_INTEGER, 0);
		return HC_BREAK;
	}

	// Бессмертные не могут атаковать в ДМ режиме
	if (g_iDayMode == DAY_DEATHMATCH && jbe_is_user_valid(iAttacker) && jbe_get_user_godmode(iAttacker))
	{
		SetHookChainReturn(ATYPE_INTEGER, 0);
		return HC_BREAK;
	}

	return HC_CONTINUE;
}

check_player_pre_think()
{
	static bool:bActiveThink = false;

	if ((g_iBitGrabUse || g_iBitUsePaint || g_msgScoreAttrib) && !bActiveThink)
	{
		EnableHookChain(HookChain_PreThink);
		bActiveThink = true;
	}

	else if (!g_iBitGrabUse && !g_iBitUsePaint && !g_msgScoreAttrib && bActiveThink)
	{
		DisableHookChain(HookChain_PreThink);
		bActiveThink = false;
	}
}

public CBasePlayer_PreThink(id)
{
	// синк граба
	grab_player_think(id);

	if (IsNotSetBit(g_iBitUserAlive, id))
		return;

	// рисуем паинтом
	if (IsSetBit(g_iBitUsePaint, id))
		paint_draw(id);

	if (!g_bGameRadarStatus)
	{
		new iButton = get_entvar(id, var_button);
		new iButtonOld = get_entvar(id, var_oldbuttons);

		if ((iButton & IN_SCORE) && !(iButtonOld & IN_SCORE))
		{
			set_score_attrib(id, SCORE_ATTRIB_NONE);

			new HideElemenentFlags:iBits = HideElement_Radar_Health_Armor|HideElement_Flashlight;
			if (g_iDayMode == DAY_DEATHMATCH)
				iBits |= HideElement_Timer;

			hide_hud_elements(id, iBits);
		}
		else if ((iButtonOld & IN_SCORE) && !(iButton & IN_SCORE))
		{
			set_score_attrib(id, SCORE_ATTRIB_DEAD);

			new HideElemenentFlags:iBits = HideElement_Flashlight;
			if (g_iDayMode == DAY_DEATHMATCH)
				iBits |= HideElement_Timer;

			hide_hud_elements(id, iBits);
		}
	}
}
/*
radar_control_think(id)
{

}
*/
public SV_StartSound(const recipients, const id, const iChannel, const szSample[], Float:fVolume, Float:fAttn, const iFlag, const iPitch)
{
	if (jbe_is_user_valid(id))
	{
		if (g_iBitUserGhost && IsSetBit(g_iBitUserGhost, id))
			return HC_BREAK;

		if (szSample[0] == 'c' && szSample[5] == 'n' && szSample[7] == 'w' && szSample[11] == 'd') // common/wpn_denyselect.wav (+use)
			return HC_BREAK;
		
		if (szSample[8] == 'k' && szSample[9] == 'n' && szSample[10] == 'i' && szSample[11] == 'f' && szSample[12] == 'e')
		{
			if (g_iSoccerBall && g_iSoccerBallOwner == id)
				return HC_BREAK;
				
			if (g_bSoccerStatus && IsSetBit(g_iBitUserSoccer, id))
			{
				switch (szSample[17])
				{
					case 'l': SetHookChainArg(4, ATYPE_STRING, sound_hand_deploy); // knife_deploy1.wav
					case 'w': SetHookChainArg(4, ATYPE_STRING, sound_hand_hit); // knife_hitwall1.wav
					case 's': SetHookChainArg(4, ATYPE_STRING, sound_hand_slash); // knife_slash(1-2).wav
					case 'b': SetHookChainArg(4, ATYPE_STRING, sound_hand_hit); // knife_stab.wav
					default: SetHookChainArg(4, ATYPE_STRING, sound_hand_hit); // knife_hit(1-4).wav
				}
				return HC_CONTINUE;
			}

			if (g_iBoxingGame && IsSetBit(g_iBitUserBoxing, id))
			{
				switch (szSample[17])
				{
					case 'l': SetHookChainArg(4, ATYPE_STRING, sound_hand_deploy); // knife_deploy1.wav
					case 'w': SetHookChainArg(4, ATYPE_STRING, sound_gloves_hit); // knife_hitwall1.wav
					case 's': SetHookChainArg(4, ATYPE_STRING, sound_hand_slash); // knife_slash(1-2).wav
					case 'b': SetHookChainArg(4, ATYPE_STRING, sound_gloves_hit); // knife_stab.wav
					default: SetHookChainArg(4, ATYPE_STRING, sound_gloves_hit); // knife_hit(1-4).wav
				}
				return HC_CONTINUE;
			}

			if (g_iBitWeaponStatus && IsSetBit(g_iBitWeaponStatus, id))
			{
				switch (szSample[17])
				{
					case 'l':
					{
						if(IsSetBit(g_iBitBalisong, id)) SetHookChainArg(4, ATYPE_STRING, sound_balisong_deploy); // knife_deploy1.wav
						else if(IsSetBit(g_iBitBasaballbat, id)) SetHookChainArg(4, ATYPE_STRING, sound_baton_deploy); // knife_deploy1.wav
						else if(IsSetBit(g_iBitGuardBaton, id)) SetHookChainArg(4, ATYPE_STRING, sound_baton_deploy); // knife_deploy1.wav
					}
					case 'w':
					{
						if(IsSetBit(g_iBitBalisong, id)) SetHookChainArg(4, ATYPE_STRING, sound_balisong_hitwall); // knife_hitwall1.wav
						else if(IsSetBit(g_iBitBasaballbat, id)) SetHookChainArg(4, ATYPE_STRING, sound_baton_hitwall); // knife_hitwall1.wav
						else if(IsSetBit(g_iBitGuardBaton, id)) SetHookChainArg(4, ATYPE_STRING, sound_baton_hitwall); // knife_hitwall1.wav
					}
					case 's':
					{
						if(IsSetBit(g_iBitBalisong, id)) SetHookChainArg(4, ATYPE_STRING, sound_balisong_slash); // knife_slash(1-2).wav
						else if(IsSetBit(g_iBitBasaballbat, id)) SetHookChainArg(4, ATYPE_STRING, sound_baton_slash); // knife_slash(1-2).wav
						else if(IsSetBit(g_iBitGuardBaton, id)) SetHookChainArg(4, ATYPE_STRING, sound_baton_slash); // knife_slash(1-2).wav
					}
					case 'b':
					{
						if(IsSetBit(g_iBitBalisong, id)) SetHookChainArg(4, ATYPE_STRING, sound_balisong_hit); // knife_stab.wav
						else if(IsSetBit(g_iBitBasaballbat, id)) SetHookChainArg(4, ATYPE_STRING, sound_baton_stab); // knife_stab.wav
						else if(IsSetBit(g_iBitGuardBaton, id)) SetHookChainArg(4, ATYPE_STRING, sound_baton_stab); // knife_stab.wav
					}
					default:
					{
						if(IsSetBit(g_iBitBalisong, id)) SetHookChainArg(4, ATYPE_STRING, sound_balisong_hit); // knife_hit(1-4).wav
						else if(IsSetBit(g_iBitBasaballbat, id)) SetHookChainArg(4, ATYPE_STRING, sound_baton_hit); // knife_hit(1-4).wav
						else if(IsSetBit(g_iBitGuardBaton, id)) SetHookChainArg(4, ATYPE_STRING, sound_baton_hit); // knife_hit(1-4).wav
					}
				}
				return HC_CONTINUE;
			}

			switch (g_iUserTeam[id])
			{
				case TEAM_PRISONER:
				{
					switch (szSample[17])
					{
						case 'l': SetHookChainArg(4, ATYPE_STRING, sound_hand_deploy); // knife_deploy1.wav
						case 'w': SetHookChainArg(4, ATYPE_STRING, sound_hand_hit); // knife_hitwall1.wav
						case 's': SetHookChainArg(4, ATYPE_STRING, sound_hand_slash); // knife_slash(1-2).wav
						case 'b': SetHookChainArg(4, ATYPE_STRING, sound_hand_hit); // knife_stab.wav
						default: SetHookChainArg(4, ATYPE_STRING, sound_hand_hit); // knife_hit(1-4).wav
					}
				}
				case TEAM_GUARD:
				{
					switch (szSample[17])
					{
						case 'l': SetHookChainArg(4, ATYPE_STRING, sound_baton_deploy); // knife_deploy1.wav
						case 'w': SetHookChainArg(4, ATYPE_STRING, sound_baton_hitwall); // knife_hitwall1.wav
						case 's': SetHookChainArg(4, ATYPE_STRING, sound_baton_slash); // knife_slash(1-2).wav
						case 'b': SetHookChainArg(4, ATYPE_STRING, sound_baton_stab); // knife_stab.wav
						default: SetHookChainArg(4, ATYPE_STRING, sound_baton_hit); // knife_hit(1-4).wav
					}
				}
			}
			
			return HC_CONTINUE;
		}
	}
	
	return HC_CONTINUE;
}

public CBasePlayer_SetClientUserInfoName(const id, infobuffer[], szNewName[])
{
	if (IsNotSetBit(g_iBitUserAlive, id)) return HC_CONTINUE;

	new szOldName[32];
	get_entvar(id, var_netname, szOldName, charsmax(szOldName));

	if (IsSetBit(g_iBitUserChangeName, id))
	{
		client_print_color(id, print_team_blue, "%s ^1Смена ^3имени ^1будет доступна в следующем раунде.", g_szChatPrefix);
		SetHookChainReturn(ATYPE_BOOL, false);
		return HC_SUPERCEDE;
	}
	else if (IsAdvertsString(szNewName))
	{
		client_print_color(id, id, "%s Реклама запрещена!", g_szChatPrefix);
		SetHookChainArg(3, ATYPE_STRING, "CS-ONE | Bad Name");
	}

	else if (IsSetBit(g_iBitUserWanted, id))
		replace(g_szWantedNames, charsmax(g_szWantedNames), szOldName, szNewName);
	
	else if (IsSetBit(g_iBitUserFree, id))
		replace(g_szFreeNames, charsmax(g_szFreeNames), szOldName, szNewName);
	
	else if (id == g_iChiefId)
		replace(g_szChiefName, charsmax(g_szChiefName), szOldName, szNewName);

	SetBit(g_iBitUserChangeName, id);
	set_task(0.1, "reload_flags", id + TASK_RELOAD_FLAGS);

	return HC_CONTINUE;
}
