/*===== -> 'hamsandwich' события -> =====*///{

hamsandwich_init()
{
	RegisterHam(Ham_Weapon_PrimaryAttack, "weapon_knife", "Ham_KnifePrimaryAttack_Post", true);
	RegisterHam(Ham_Weapon_SecondaryAttack, "weapon_knife", "Ham_KnifeSecondaryAttack_Post", true);
	RegisterHam(Ham_Item_Deploy, "weapon_knife", "Ham_KnifeDeploy_Post", true);
	
	new g_szDoorClass[][] = { "func_door", "func_door_rotating" };
	for (new i; i < sizeof(g_szDoorClass); i++)
	{
		RegisterHam(Ham_Use, g_szDoorClass[i], "Ham_DoorUse", false);
		RegisterHam(Ham_Blocked, g_szDoorClass[i], "Ham_DoorBlocked", false);
	}

	// Soccer and Box
	new szWeaponName[][] = {
		"weapon_p228", "weapon_scout", "weapon_hegrenade", "weapon_xm1014", "weapon_c4", "weapon_mac10", "weapon_aug",
		"weapon_smokegrenade", "weapon_elite", "weapon_fiveseven", "weapon_ump45", "weapon_sg550", "weapon_galil",
		"weapon_famas", "weapon_usp", "weapon_glock18", "weapon_awp", "weapon_mp5navy", "weapon_m249", "weapon_m3",
		"weapon_m4a1", "weapon_tmp", "weapon_g3sg1", "weapon_flashbang", "weapon_deagle", "weapon_sg552", "weapon_ak47", "weapon_p90"
	};
	for (new i; i < sizeof(szWeaponName); i++)
		DisableHamForward(g_iHamHookItemDeploy[i] = RegisterHam(Ham_Item_Deploy, szWeaponName[i], "Ham_ItemDeploy_Post", true));
	
	new szWeaponsDuel[][] = {
		"weapon_deagle",
		"weapon_m3",
		// weapon_hegrenade
		"weapon_m249",
		"weapon_awp",
		// weapon_knife
		"weapon_ak47",
		"weapon_scout"
	};
	for (new i; i < sizeof(szWeaponsDuel); i++) // Duel
		DisableHamForward(g_iHamHookPrimaryAttackDuel[i] = 
			RegisterHam(Ham_Weapon_PrimaryAttack, szWeaponsDuel[i], "Ham_ItemPrimaryAttackDuel_Post", true));

	// Shop
	RegisterHam(Ham_Player_ResetMaxSpeed, "player", "Ham_PlayerResetMaxSpeed_Post", true);
	DisableHamForward(g_iHamHookPlayerJump = RegisterHam(Ham_Player_Jump, "player", "Ham_PlayerJump", false));
	DisableHamForward(g_iHamHookGrenadeTouch = RegisterHam(Ham_Touch, "grenade", "Ham_GrenadeTouch_Post", true));

	for (new i; i <= 8; i++)
		DisableHamForward(g_iHamHookForwards[i] = RegisterHam(Ham_Use, szHamHookEntityBlock[i], "HamHook_EntityBlock", false));
	for (new i = 9; i < sizeof(szHamHookEntityBlock); i++)
		DisableHamForward(g_iHamHookForwards[i] = RegisterHam(Ham_Touch, szHamHookEntityBlock[i], "HamHook_EntityBlock", false));

	RegisterHam(Ham_TraceAttack, "func_button", "Ham_TraceAttack_Button", false);
	RegisterHam(Ham_Use, "func_button", "Ham_Use_Button", false);
}

new g_iStatusHamJump;
ham_jump_check()
{
	//static g_iStatusHamJump;

	if (g_iDayMode == DAY_DAYMODE && !g_iStatusHamJump)
		{}

	else if (!g_iStatusHamJump
	&& (g_iBitHingJump || g_iBitDoubleJump || g_iBitAutoBhop || g_iBitLongJump))
	{
		EnableHamForward(g_iHamHookPlayerJump);
		g_iStatusHamJump = 1;
	}

	else if (g_iStatusHamJump
	&& (!g_iBitHingJump && !g_iBitDoubleJump && !g_iBitAutoBhop && !g_iBitLongJump))
	{
		DisableHamForward(g_iHamHookPlayerJump);
		g_iStatusHamJump = 0;
	}
}

public Ham_KnifePrimaryAttack_Post(iEntity)
{
	new id = get_member(iEntity, m_pPlayer);
	
	if (g_iSoccerBallOwner && g_iSoccerBallOwner == id)
	{
		jbe_soccer_kick_ball(id, .iKnifeKickPower = 0);
		return;
	}

	if (g_bSoccerStatus && IsSetBit(g_iBitUserSoccer, id))
	{
		set_member(id, m_flNextAttack, 1.0);
		return;
	}

	if (g_iBoxingGame && IsSetBit(g_iBitUserBoxing, id))
	{
		if (pev(id, pev_button) & IN_BACK)
		{
			g_iBoxingTypeKick[id] = 4;
			set_member(id, m_flNextAttack, 1.5);
		}
		else
		{
			g_iBoxingTypeKick[id] = 3;
			set_member(id, m_flNextAttack, 0.9);
		}
		return;
	}
	
	if (g_iBitWeaponStatus && IsSetBit(g_iBitWeaponStatus, id))
	{
		if (IsSetBit(g_iBitBalisong, id))		set_member(id, m_flNextAttack, 0.7);
		else if (IsSetBit(g_iBitBasaballbat, id))	set_member(id, m_flNextAttack, 0.9);
		else if (IsSetBit(g_iBitGuardBaton, id))	set_member(id, m_flNextAttack, 0.7);
		return;
	}

	switch (g_iUserTeam[id])
	{
		case 1: set_member(id, m_flNextAttack, 1.0);
		case 2: set_member(id, m_flNextAttack, 0.5);
	}
}

public Ham_KnifeSecondaryAttack_Post(iEntity)
{
	new id = get_member(iEntity, m_pPlayer);

	if (g_iSoccerBallOwner && g_iSoccerBallOwner == id)
	{
		jbe_soccer_kick_ball(id, .iKnifeKickPower = 1);
		return;
	}

	if (g_bSoccerStatus && IsSetBit(g_iBitUserSoccer, id))
	{
		set_member(id, m_flNextAttack, 1.0);
		return;
	}

	if (g_iBoxingGame && IsSetBit(g_iBitUserBoxing, id))
	{
		if (pev(id, pev_button) & IN_BACK)
		{
			g_iBoxingTypeKick[id] = 2;
			set_member(id, m_flNextAttack, 1.5);
		}
		else
		{
			static iKick; iKick = !iKick;
			g_iBoxingTypeKick[id] = iKick;
			set_member(id, m_flNextAttack, 1.1);
		}
		return;
	}

	if (g_iBitWeaponStatus && IsSetBit(g_iBitWeaponStatus, id))
	{
		if(IsSetBit(g_iBitBalisong, id)) set_member(id, m_flNextAttack, 1.0);
		else if(IsSetBit(g_iBitBasaballbat, id)) set_member(id, m_flNextAttack, 1.5);
		else if(IsSetBit(g_iBitGuardBaton, id)) set_member(id, m_flNextAttack, 1.37);
		return;
	}

	switch (g_iUserTeam[id])
	{
		case 1: set_member(id, m_flNextAttack, 1.0);
		case 2: set_member(id, m_flNextAttack, 1.37);
	}
}

public Ham_KnifeDeploy_Post(iEntity)
{
	new id = get_member(iEntity, m_pPlayer);

	// если есть щит, не ставим модели
	if (rg_get_user_shield(id))
		return;
		
	if ((g_bSoccerStatus || g_bAllUseBall)
	&& (IsSetBit(g_iBitUserSoccer, id) || g_iSoccerBallOwner == id))
	{
		if (g_iSoccerBallOwner == id)
			jbe_soccer_hand_ball_model(id);
		else if (IsSetBit(g_iBitUserSoccer, id))
			jbe_set_hand_model(id);
		else
			jbe_default_weapon_knife(id);
		return;
	}

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
		return;
	}

	jbe_default_knife_model(id);
}

public Ham_DoorUse(iEntity, iCaller, iActivator)
{
	if (iCaller != iActivator && pev(iEntity, pev_iuser1) == IUSER1_DOOR_KEY)
	{
		if (iCaller != g_iChiefId)
			return HAM_SUPERCEDE;
	}
	return HAM_IGNORED;
}

public Ham_DoorBlocked(iBlocked, iBlocker)
{
	if (jbe_is_user_valid(iBlocker) && IsSetBit(g_iBitUserAlive, iBlocker) && pev(iBlocked, pev_iuser1) == IUSER1_DOOR_KEY)
	{
		ExecuteHamB(Ham_TakeDamage, iBlocker, 0, 0, 9999.9, 0);
		client_print_color(iBlocker, 0, "%s Вы убиты за подпирание клетки!", g_szChatPrefix);
		
		if (g_iAlivePlayersNum[TEAM_PRISONER] >= 2)
			set_task(1.0, "jbe_user_respawn_task", iBlocker + TASK_USER_RESPAWN);
		
		return HAM_SUPERCEDE;
	}
	return HAM_IGNORED;
}

public Ham_ItemDeploy_Post(iEntity)
{
	new id = get_member(iEntity, m_pPlayer);

	if (IsSetBit(g_iBitUserSoccer, id) || IsSetBit(g_iBitUserBoxing, id))
		engclient_cmd(id, "weapon_knife");
}

public Ham_ItemPrimaryAttackDuel_Post(iEntity)
{
	if (!g_iDuelStatus)
		return HAM_IGNORED;
	
	new id = get_member(iEntity, m_pPlayer);

	if (IsNotSetBit(g_iBitUserDuel, id))
		return HAM_IGNORED;

	if (~1<<g_iDuelStatus & 1<<DS_RUN && IsNotSetBit(g_iBitGrabUse, id))
		g_iDuelStatus = DS_RUN;
	
	if (g_iDuelType != DT_M249)
	{
		set_member(id, m_flNextAttack, 11.0);
	//	set_member(id, m_Weapon_flNextPrimaryAttack, 11.0);
		remove_task(TASK_DUEL_TIMER_ATTACK);
		set_task(1.0, "jbe_duel_timer_attack", TASK_DUEL_TIMER_ATTACK, _, _, "a", g_iDuelTimerAttack = 11);
	}

	id = g_iDuelUsersId[0] != id ? g_iDuelUsersId[0] : g_iDuelUsersId[1];
	set_member(id, m_flNextAttack, 0.0);
//	set_member(id, m_Weapon_flNextPrimaryAttack, 0.0);
	g_iDuelCurrentAttacer = g_iDuelUsersId[0] != id ? 1 : 0;
	new iItem = get_member(id, m_pActiveItem);

	if (g_iDuelType == DT_AWP) // |1<<DT_M3
		set_member(iItem, m_Weapon_flNextSecondaryAttack, get_gametime() + 11.0);
	
	new iClip = (1<<g_iDuelType & 1<<DT_M249) ? 100 : 1;
	set_member(iItem, m_Weapon_iClip, iClip);

	return HAM_IGNORED;
}


public Ham_PlayerJump(id)
{
	// fix over jump
	if (IsSetBit(g_iBitHookUse, id))
		return HAM_SUPERCEDE;

	static iBitUserJump;
	if ((g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY
	|| g_iDayMode == DAY_DEATHMATCH) && IsNotSetBit(g_iBitUserDuel, id)
	&& (IsSetBit(g_iBitHingJump, id) || IsSetBit(g_iBitDoubleJump, id)
	|| IsSetBit(g_iBitAutoBhop, id) || IsSetBit(g_iBitLongJump, id)))
	{
		if (~pev(id, pev_oldbuttons) & IN_JUMP)
		{
			new iFlags = pev(id, pev_flags);
			
			if (IsSetBit(g_iBitLongJump, id))
			{
				if (iFlags & FL_WATERJUMP || pev(id, pev_waterlevel) >= 2)
					return HAM_IGNORED;
		
				static afButtonPressed;
				afButtonPressed = get_member(id, m_afButtonPressed);
				if (!(afButtonPressed & IN_JUMP) || !(iFlags & FL_ONGROUND))
					return HAM_IGNORED;

				if (pev(id, pev_bInDuck) || iFlags & FL_ONGROUND
				&& Float:get_member(id, m_fLongJump)
				&& pev(id, pev_button) & IN_DUCK && pev(id, pev_flDuckTime))
				{
					static Float:fVecTemp[3];
					pev(id, pev_velocity, fVecTemp);
					if (vector_length(fVecTemp) > 50.0)
					{
						pev(id, pev_punchangle, fVecTemp);
						fVecTemp[0] = -5.0;
						set_pev(id, pev_punchangle, fVecTemp);
						get_global_vector(GL_v_forward, fVecTemp);
						fVecTemp[0] *= 560.0;
						fVecTemp[1] *= 560.0;
						fVecTemp[2] = 299.33259094191531084669989858532;
						set_pev(id, pev_velocity, fVecTemp);
						set_member(id, m_Activity, ACT_LEAP);
						set_member(id, m_IdealActivity, ACT_LEAP);
						set_pev(id, pev_oldbuttons, pev(id, pev_oldbuttons) | IN_JUMP);
						set_pev(id, pev_gaitsequence, PLAYER_SUPERJUMP);
						set_pev(id, pev_frame, 0.0);
						set_member(id, m_afButtonPressed, afButtonPressed & ~IN_JUMP);
						return HAM_IGNORED;
					}
				}
			}
			
			if (iFlags & (FL_ONGROUND|FL_CONVEYOR))
			{
				if(IsSetBit(g_iBitHingJump, id))
				{
					new Float:vecVelocity[3];
					pev(id, pev_velocity, vecVelocity);
					vecVelocity[2] = 500.0;
					set_pev(id, pev_velocity, vecVelocity);
				}
				SetBit(iBitUserJump, id);
				return HAM_IGNORED;
			}
			
			if (IsSetBit(iBitUserJump, id) && IsSetBit(g_iBitDoubleJump, id)
			&& ~iFlags & (FL_ONGROUND|FL_CONVEYOR|FL_INWATER))
			{
				new Float:vecVelocity[3];
				pev(id, pev_velocity, vecVelocity);
				vecVelocity[2] = 450.0;
				set_pev(id, pev_velocity, vecVelocity);
				ClearBit(iBitUserJump, id);
			}
		}

		else if (IsSetBit(g_iBitAutoBhop, id)
		&& pev(id, pev_flags) & (FL_ONGROUND|FL_CONVEYOR))
		{
			new Float:vecVelocity[3];
			pev(id, pev_velocity, vecVelocity);
			vecVelocity[2] = 250.0;
			set_pev(id, pev_velocity, vecVelocity);
			set_pev(id, pev_gaitsequence, 6);
		}
	}
	return HAM_IGNORED;
}

public Ham_PlayerResetMaxSpeed_Post(id)
{
	if (g_iDayMode != DAY_DAYMODE
	&& IsSetBit(g_iBitFastRun, id) && IsNotSetBit(g_iBitUserDuel, id)
	&& IsNotSetBit(g_iBitUserSoccer, id) && IsNotSetBit(g_iBitUserBoxing, id))
		if (g_fUserMaxSpeed[id])
			set_pev(id, pev_maxspeed, g_fUserMaxSpeed[id]);
		else
			set_pev(id, pev_maxspeed, 400.0);
}

public Ham_GrenadeTouch_Post(iTouched)
{
	if ((g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY || g_iDayMode == DAY_DEATHMATCH)
	&& pev(iTouched, pev_iuser1) == IUSER1_FROSTNADE_KEY)
	{
		new Float:vecOrigin[3], id;
		pev(iTouched, pev_origin, vecOrigin);
		CREATE_BEAMCYLINDER(vecOrigin, 150, g_pSpriteWave, _, _, 4, 60, _, 0, 110, 255, 255, _);
		
		while ((id = find_ent_in_sphere(id, vecOrigin, 150.0)))
		{
			if (jbe_is_user_valid(id) && (g_iUserTeam[id] == TEAM_GUARD
			|| (g_iUserTeam[id] == TEAM_PRISONER && g_iFriendlyFire)))
			{
				set_pev(id, pev_flags, pev(id, pev_flags) | FL_FROZEN);
				set_member(id, m_flNextAttack, 6.0);
				jbe_set_user_rendering(id, kRenderFxGlowShell, 0, 110, 255, kRenderNormal, 0);
				emit_sound(iTouched, CHAN_AUTO, sound_freeze_player, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				SetBit(g_iBitUserFrozen, id);

				if (task_exists(id + TASK_FROSTNADE_DEFROST))
					change_task(id + TASK_FROSTNADE_DEFROST, 6.0);
				else
					set_task(6.0, "jbe_user_defrost", id + TASK_FROSTNADE_DEFROST);
			}
		}
		emit_sound(iTouched, CHAN_AUTO, sound_grenade_frost_explosion, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		engfunc(EngFunc_RemoveEntity, iTouched);

		check_frostnade_fwd();
	}
}

public HamHook_EntityBlock(iEntity, id)
{
	if (g_bRoundEnd || IsSetBit(g_iBitUserDuel, id) || is_shief_global_game()
	|| g_iBitGlobalEntBlock || g_iDayMode == DAY_DAYMODE)
		return HAM_SUPERCEDE;

	return HAM_IGNORED;
}

public Ham_TraceAttack_Button(iEntity, iAttacker)
{
	#define USE_SET 2

	if (!iEntity) return;

	static Float:fNextUse;
	new Float:fGameTime = get_gametime();
	
	if (fGameTime > fNextUse)
	{
		if (jbe_is_user_valid(iAttacker) && (g_iUserTeam[iAttacker] == TEAM_GUARD
		|| g_iDayMode == DAY_DEATHMATCH) && pev_valid(iEntity))
		{
			//ExecuteHamB(Ham_Use, iEntity, iAttacker, 0, USE_SET, 1.0);
			ExecuteHamB(Ham_Use, iEntity, 0, iAttacker, USE_SET, 1.0);
			set_pev(iEntity, pev_frame, 0.0);
			fNextUse = fGameTime + 0.3;
			
			#if defined DEVELOPER_OPTIONS
			client_print(0, print_chat, "[TraceAttack_Button] Used | iEntity: %d | iAttacker %d", iEntity, iAttacker);
			#endif
		}
	}
}

public Ham_Use_Button(iEntity, id, iActivator, iUseType, Float:fValue)
{
	if (iUseType != USE_SET || !jbe_is_user_valid(iActivator))
		return HAM_IGNORED;
	
	if (g_iUserTeam[iActivator] == TEAM_PRISONER && is_user_braceletes(iActivator, BR_CHECK_BUTTON))
	{
		client_print_color(iActivator, print_team_red,
			"%s На вас ^3надеты наручники^1, вы не можете нажимать кнопки.", g_szChatPrefix);
		return HAM_SUPERCEDE;
	}
	else if (g_iDayMode == DAY_NORMAL)
	{
		client_print(0, print_console, " ** [Кнопки] %n нажал на кнопку (Кнопка: %d)", iActivator, iEntity);
	}

	return HAM_IGNORED;
}

public Ham_VehicleUse(iEntity, id, iActivator, iUseType, Float:flValue )
{
	if (g_iUserTeam[id] == TEAM_PRISONER && is_user_braceletes(id, BR_CHECK_BUTTON))
	{
		client_print_color(id, 0, "%s На вас ^3надеты наручники^1, вы не можете использовать транспорт.", g_szChatPrefix);
		return HAM_SUPERCEDE;
	}
	else if (iUseType == USE_SET && IsSetBit(g_iBitUserAlive, id))
	{
		set_entvar(iEntity, var_iuser4, id);
	}
	return HAM_IGNORED;
}

/*===== <- 'hamsandwich' события <- =====*///}
