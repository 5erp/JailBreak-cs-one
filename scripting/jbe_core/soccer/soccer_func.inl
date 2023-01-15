#include "jbe_core\soccer\soccer_menu.inl"

soccer_init()
{
	DisableHookChain(g_iBallUseEmpty = RegisterHookChain(RG_CBasePlayer_UseEmpty, "CBasePlayer_UseEmpty_Post", .post = 1));
}

/* Команды управления счетом */
public ClCmd_ScoreMenu(id)
{
	if (g_iChiefId == id || IsSetBit(g_iBitUserAdmin, id))
	{
		return Show_SoccerScoreMenu(id);
	} 
	else return ClCmd_InfoBuy(id);
}

/*===== -> Футбол -> =====*///{
jbe_soccer_enable_all()
{
	g_bSoccerStatus = true;
	jbe_set_name_mode("Футбол");
	g_iChiefGameType = GAME_SOCCER;
	for (new i; i < charsmax(g_iHamHookItemDeploy); i++)
		EnableHamForward(g_iHamHookItemDeploy[i]);
}

jbe_soccer_disable_all()
{
	jbe_soccer_remove_ball();
	jbe_set_name_mode("");

	for (new iPlayer = 1; iPlayer <= MaxClients; iPlayer++)
	{
		if (IsSetBit(g_iBitUserSoccer, iPlayer)) {
			ClearBit(g_iBitUserSoccer, iPlayer);
			ExecuteHamB(Ham_Player_ResetMaxSpeed, iPlayer);
			if (IsSetBit(g_iBitClothingGuard, iPlayer) && IsSetBit(g_iBitClothingType, iPlayer))
				jbe_set_user_model(iPlayer, models_player[GUARD]);
			else jbe_default_player_model(iPlayer);
			set_member(iPlayer, m_bloodColor, 247);
			new iActiveItem = get_member(iPlayer, m_pActiveItem);
			if (iActiveItem > 0)
			{
				ExecuteHamB(Ham_Item_Deploy, iActiveItem);
				UTIL_WeaponAnimation(iPlayer, 3);
			}
		}
	}

	if (g_bSoccerGame)
		rg_send_audio(0, sound_whitle_end);
		
	remove_task(TASK_SHOW_SOCCER_SCORE);
	g_iSoccerScore = {0, 0};
	g_bSoccerGame = false;
	g_bSoccerStatus = false;
	g_iChiefGameType = GAME_NONE;
	
	for (new i; i < charsmax(g_iHamHookItemDeploy); i++)
		DisableHamForward(g_iHamHookItemDeploy[i]);
}

jbe_soccer_create_ball(pPlayer)
{
	if (g_iSoccerBall) return g_iSoccerBall;

	g_iBallTouchID = register_touch("ball", "*", "Ball_Touch");
	g_iBallThinkID = register_think("ball", "Ball_Think");
	g_iSoccerImpulseID = register_impulse(100, "ClientImpulse100");
	EnableHookChain(g_iBallUseEmpty);

	g_iSoccerBall = rg_create_entity("func_wall", true);

	if (is_entity(g_iSoccerBall))
	{
		set_entvar(g_iSoccerBall, var_classname, "ball");
		set_entvar(g_iSoccerBall, var_solid, SOLID_TRIGGER);
		set_entvar(g_iSoccerBall, var_movetype, MOVETYPE_BOUNCE);
		entity_set_model(g_iSoccerBall, model_ball);
		entity_set_size(g_iSoccerBall, Float:{-4.0, -4.0, -4.0}, Float:{4.0, 4.0, 4.0});
		set_entvar(g_iSoccerBall, var_skin, g_iBallType);
		set_entvar(g_iSoccerBall, var_framerate, 1.0);
		set_entvar(g_iSoccerBall, var_sequence, 0);
		set_entvar(g_iSoccerBall, var_nextthink, get_gametime() + 0.04);
		get_aiming_position(pPlayer, g_flSoccerBallOrigin);
		entity_set_origin(g_iSoccerBall, g_flSoccerBallOrigin);
		drop_to_floor(g_iSoccerBall);
		return g_iSoccerBall;
	}
	jbe_soccer_remove_ball();

	return 0;
}

jbe_soccer_remove_ball()
{
	if (g_iSoccerBall)
	{
		if (g_bSoccerBallTrail)
		{
			g_bSoccerBallTrail = false;
			CREATE_KILLBEAM(g_iSoccerBall);
		}
		if (g_iSoccerBallOwner)
		{
			CREATE_KILLPLAYERATTACHMENTS(g_iSoccerBallOwner);
			if (g_bSoccerStatus)
				jbe_set_hand_model(g_iSoccerBallOwner);
			else if (g_bAllUseBall && get_user_weapon(g_iSoccerBallOwner) == CSW_KNIFE)
				jbe_default_weapon_knife(g_iSoccerBallOwner);
		}

		if (is_entity(g_iSoccerBall))
		{
			set_entvar(g_iSoccerBall, var_flags, get_entvar(g_iSoccerBall, var_flags) | FL_KILLME);
		}
			
		g_iSoccerBall = 0;
		g_iSoccerBallOwner = 0;
		g_iSoccerKickOwner = 0;
		g_bSoccerBallTouch = false;
		g_bAllUseBall = false;

		unregister_touch(g_iBallTouchID);
		unregister_think(g_iBallThinkID);
		unregister_impulse(g_iSoccerImpulseID);
		DisableHookChain(g_iBallUseEmpty);
	}
}

jbe_soccer_update_ball()
{
	if (g_iSoccerBall)
	{
		if (is_entity(g_iSoccerBall))
		{
			if (g_bSoccerBallTrail)
			{
				g_bSoccerBallTrail = false;
				CREATE_KILLBEAM(g_iSoccerBall);
			}
			if (g_iSoccerBallOwner)
			{
				CREATE_KILLPLAYERATTACHMENTS(g_iSoccerBallOwner);
				jbe_set_hand_model(g_iSoccerBallOwner);
			}
			set_entvar(g_iSoccerBall, var_velocity, {0.0, 0.0, 0.0});
			set_entvar(g_iSoccerBall, var_solid, SOLID_TRIGGER);
			entity_set_model(g_iSoccerBall, model_ball);
			entity_set_size(g_iSoccerBall, Float:{-4.0, -4.0, -4.0}, Float:{4.0, 4.0, 4.0});
			entity_set_origin(g_iSoccerBall, g_flSoccerBallOrigin);
			set_entvar(g_iSoccerBall, var_skin, g_iBallType);
			drop_to_floor(g_iSoccerBall);
			g_iSoccerBallOwner = 0;
			g_iSoccerKickOwner = 0;
			g_bSoccerBallTouch = false;
		}
		else jbe_soccer_remove_ball();
	}
}

jbe_sub_user_soccer(id)
{
	if (IsSetBit(g_iBitUserSoccer, id) || g_bAllUseBall)
	{
		ClearBit(g_iBitUserSoccer, id);
		if (id == g_iSoccerBallOwner)
		{
			CREATE_KILLPLAYERATTACHMENTS(id);
			set_entvar(g_iSoccerBall, var_solid, SOLID_TRIGGER);
			set_entvar(g_iSoccerBall, var_velocity, {0.0, 0.0, 0.1});
			drop_to_floor(g_iSoccerBall);
			g_iSoccerBallOwner = 0;
		}

		if (g_bSoccerGame && !g_iBitUserSoccer)
			jbe_soccer_disable_all();
	}
}

jbe_soccer_game_start(pPlayer)
{
	new iPlayers;
	for (new iPlayer = 1; iPlayer <= MaxClients; iPlayer++)
		if (IsSetBit(g_iBitUserSoccer, iPlayer)) iPlayers++;

	if (iPlayers < 2)
	{
		client_print_color(pPlayer, print_team_blue, "%s ^1Недостаточно игроков для начала матча, ^4поделите команды^1!", g_szChatPrefix);
		return Show_SoccerTeamMenu(pPlayer); // Если нет футболистов кидаем на меню деления на команды
	}
	else
	{
		rg_send_audio(0, sound_whitle_start);
		g_bSoccerBallTouch = true;
		g_bSoccerGame = true;
		set_task(1.0, "jbe_score_informer", TASK_SHOW_SOCCER_SCORE, _, _, "b");
	}
	return Show_SoccerMenu(pPlayer);
}

jbe_soccer_divide_team(iType)
{
	if (!g_bSoccerStatus) jbe_soccer_enable_all();
	new const szLangPlayer[][] = { "красных", "синих" };
	
	for (new iPlayer = 1, iTeam; iPlayer <= MaxClients; iPlayer++)
	{
		if (IsSetBit(g_iBitUserAlive, iPlayer) && IsNotSetBit(g_iBitUserSoccer, iPlayer) && IsNotSetBit(g_iBitUserDuel, iPlayer)
		&& (g_iUserTeam[iPlayer] == TEAM_PRISONER && IsNotSetBit(g_iBitUserFree, iPlayer) && IsNotSetBit(g_iBitUserWanted, iPlayer)
		&& IsNotSetBit(g_iBitUserBoxing, iPlayer) || !iType && g_iUserTeam[iPlayer] == TEAM_GUARD && iPlayer != g_iChiefId)) {
			SetBit(g_iBitUserSoccer, iPlayer);
			ExecuteHamB(Ham_Player_ResetMaxSpeed, iPlayer);

			jbe_set_player_prisoner_model(iPlayer);

			set_entvar(iPlayer, var_skin, iTeam + model_football_offset);

			set_member(iPlayer, m_bloodColor, -1);
			client_print_color(iPlayer, 0, "%s ^1Вы в команде ^4%s^1.", g_szChatPrefix, szLangPlayer[iTeam]);
			g_iSoccerUserTeam[iPlayer] = iTeam;
			if (get_user_weapon(iPlayer) != CSW_KNIFE)
				engclient_cmd(iPlayer, "weapon_knife");
			else
			{
				new iActiveItem = get_member(iPlayer, m_pActiveItem);
				if (iActiveItem > 0)
				{
					ExecuteHamB(Ham_Item_Deploy, iActiveItem);
					UTIL_WeaponAnimation(iPlayer, 3);
				}
			}
			iTeam = !iTeam;
		}
	}
	if (!g_iBitUserSoccer) jbe_soccer_disable_all();
}

jbe_soccer_set_team(id, iTeam)
{
	new iTarget, iBody;
	get_user_aiming(id, iTarget, iBody); //get_user_aiming(id, iTarget, iBody, 9999);
	if (iTeam > -1)
	{
		if (jbe_is_user_valid(iTarget) && IsSetBit(g_iBitUserAlive, iTarget) && IsNotSetBit(g_iBitUserDuel, iTarget)
		&& (g_iUserTeam[iTarget] == TEAM_PRISONER && IsNotSetBit(g_iBitUserFree, iTarget) && IsNotSetBit(g_iBitUserWanted, iTarget)
		&& IsNotSetBit(g_iBitUserBoxing, iTarget) || g_iUserTeam[iTarget] == TEAM_GUARD))
		{
			new szLangPlayer[][] = {"красных", "синих"};
			client_print_color(iTarget, 0, "%s ^1Вы в команде ^4%s^1.", g_szChatPrefix, szLangPlayer[iTeam]);
			if (IsNotSetBit(g_iBitUserSoccer, iTarget))
			{
				SetBit(g_iBitUserSoccer, iTarget);
				ExecuteHamB(Ham_Player_ResetMaxSpeed, iTarget);
				jbe_set_user_model(iTarget, models_player[PRISONER]);
				if (get_user_weapon(iTarget) != CSW_KNIFE)
					engclient_cmd(iTarget, "weapon_knife");
				else
				{
					new iActiveItem = get_member(iTarget, m_pActiveItem);
					if(iActiveItem > 0)
					{
						ExecuteHamB(Ham_Item_Deploy, iActiveItem);
						UTIL_WeaponAnimation(iTarget, 3);
					}
				}
				set_member(iTarget, m_bloodColor, -1);
				ClearBit(g_iBitClothingType, iTarget);
			}
			set_entvar(iTarget, var_skin, iTeam + model_football_offset);
			g_iSoccerUserTeam[iTarget] = iTeam;
			if (!g_bSoccerStatus) jbe_soccer_enable_all();
		}
		else client_print_color(id, print_team_blue, "%s ^1Наведите ^4прицел ^1на игрока и ^3повторите попытку^1.", g_szChatPrefix);
	}
	else
	{
		if (jbe_is_user_valid(iTarget) && IsSetBit(g_iBitUserSoccer, iTarget))
		{
			ClearBit(g_iBitUserSoccer, iTarget);
			ExecuteHamB(Ham_Player_ResetMaxSpeed, iTarget);
			if (iTarget == g_iSoccerBallOwner)
			{
				CREATE_KILLPLAYERATTACHMENTS(iTarget);
				set_entvar(g_iSoccerBall, var_solid, SOLID_TRIGGER);
				set_entvar(g_iSoccerBall, var_velocity, {0.0, 0.0, 0.1});
				g_iSoccerBallOwner = 0;
			}
			if (IsSetBit(g_iBitClothingGuard, iTarget) && IsSetBit(g_iBitClothingType, iTarget))
				jbe_set_user_model(iTarget, models_player[GUARD]);
			else
				jbe_default_player_model(iTarget);
			set_member(iTarget, m_bloodColor, 247);
			new iActiveItem = get_member(iTarget, m_pActiveItem);
			if (iActiveItem > 0)
			{
				ExecuteHamB(Ham_Item_Deploy, iActiveItem);
				UTIL_WeaponAnimation(iTarget, 3);
			}
			if (!g_iBitUserSoccer) jbe_soccer_disable_all();
		}
		else client_print_color(id, print_team_blue, "%s ^1Наведите ^4прицел ^1на игрока и ^3повторите попытку^1.", g_szChatPrefix);
	}
}

public jbe_score_informer()
{
	set_hudmessage(102, 69, 0, -1.0, 0.01, 0, 0.0, 0.9, 0.1, 0.1, -1);
	ShowSyncHudMsg(0, g_iSyncSoccerScore, "КРАСНЫЕ %d | %d СИНИЕ", g_iSoccerScore[0], g_iSoccerScore[1]);
}

jbe_soccer_hand_ball_model(pPlayer)
{
	set_entvar(pPlayer, var_viewmodel, model_v_hand_ball);
	set_entvar(pPlayer, var_weaponmodel, model_p_hand);
}

public CBasePlayer_UseEmpty_Post(id)
{
	if (!g_iSoccerBall || g_iSoccerBallOwner != id)
		return HC_CONTINUE;

	if (!is_entity(g_iSoccerBall))
	{
		jbe_soccer_remove_ball();
		return HC_CONTINUE;
	}

	jbe_soccer_kick_ball(id);
	return HC_CONTINUE;
}

jbe_soccer_get_ball_cur_pos(Float:vecOrigin[3])
{
	new Float:vecVelocity[3];
	if (g_iBallType == BT_FOOTBALL)
	{
		velocity_by_aim(g_iSoccerBallOwner, 15, vecVelocity); // 15
	}
	else // if (g_iBallType == BT_VOLLEYBALL || g_iBallType == BT_BASEBALL)
	{
		velocity_by_aim(g_iSoccerBallOwner, 35, vecVelocity); // 30
	}

	get_entvar(g_iSoccerBallOwner, var_origin, vecOrigin);
	vecOrigin[0] += vecVelocity[0];
	vecOrigin[1] += vecVelocity[1];

	if (g_iBallType == BT_FOOTBALL)
	{
		// мяч на уровне ног
		if (get_entvar(g_iSoccerBallOwner, var_flags) & FL_DUCKING)
			vecOrigin[2] -= 18.0;//15.0; 
		else
			vecOrigin[2] -= 36.0;//33.0; 
	}
	else // if (g_iBallType == BT_VOLLEYBALL || g_iBallType == BT_BASEBALL)
	{
		// мяч на уровне рук
		if (get_entvar(g_iSoccerBallOwner, var_flags) & FL_DUCKING)
			vecOrigin[2] -= 6.75;
		else
			vecOrigin[2] -= 9.75;
	}
}

jbe_soccer_kick_ball(id, iKnifeKickPower = 0)
{
	new Float:vecOrigin[3];
//	get_entvar(g_iSoccerBall, var_origin, vecOrigin);
	jbe_soccer_get_ball_cur_pos(vecOrigin);

	new iPointContents = point_contents(vecOrigin);
	if (iPointContents != CONTENTS_EMPTY && iPointContents != CONTENTS_WATER) // чтоб можно было бить мяч в воде
		return;

	new iKickVelocity;

	new Float:vecTempOrigin[3];
	xs_vec_copy(vecOrigin, vecTempOrigin);

	// Удар мяча, установка силы/гравитации
	if (g_iBallType == BT_FOOTBALL)
	{
		new iButton = get_entvar(id, var_button);

		if (iButton & IN_DUCK)
		{
			if (g_bSoccerGame || is_player_model_anims(id))
			{
				if (iButton & IN_FORWARD)	UTIL_PlayerAnimation(id, "soccer_crouchrun");
				else						UTIL_PlayerAnimation(id, "soccer_crouch_idle");
			}

			iKickVelocity = 100;
			g_bSoccerBallTrail = true;
			CREATE_BEAMFOLLOW(g_iSoccerBall, g_pSpriteBeam, 4, 10, 40, 40, 250, 130);
		}
		else
		{
			if (g_bSoccerGame || is_player_model_anims(id))
			{
				if (iButton & IN_FORWARD)
				{
					if (iButton & IN_RUN)	UTIL_PlayerAnimation(id, "soccer_walk");
					else					UTIL_PlayerAnimation(id, "soccer_run");
				}
				else UTIL_PlayerAnimation(id, "soccer_idle");
			}

			iKickVelocity = 60;
		}

		vecOrigin[2] += 5.0; // поднимаем мяч для удара с легким навесом

		if (g_bSoccerGame)
			jbe_set_hand_model(id);

		else if (g_bAllUseBall && get_user_weapon(id) == CSW_KNIFE)
			jbe_default_weapon_knife(id);
	}
	else // if (g_iBallType == BT_VOLLEYBALL || g_iBallType == BT_BASEBALL)
	{
		new iFlags = get_entvar(g_iSoccerBallOwner, var_flags);
	//	set_pev(g_iSoccerBall, pev_movetype, MOVETYPE_BOUNCE);

		if (g_iBallType == BT_VOLLEYBALL)
		{
			set_entvar(g_iSoccerBall, var_gravity, 0.36);

			if (!iKnifeKickPower)
			{
				if (iFlags & FL_DUCKING)	iKickVelocity = 25;
				else if (iFlags & FL_FLY)	iKickVelocity = 35;
				else						iKickVelocity = 30;
			}
			else //if (iKnifeKick == ATTACK2)
			{
				if (iFlags & FL_DUCKING)	iKickVelocity = 40;
				else if (iFlags & FL_FLY)	iKickVelocity = 50;
				else						iKickVelocity = 45;
				
				g_bSoccerBallTrail = true;
				CREATE_BEAMFOLLOW(g_iSoccerBall, g_pSpriteBeam, 4, 10, 40, 40, 250, 130);
			}

			vecOrigin[2] += 33.0; // поднимаем мяч для броска
		}
		else // if (g_iBallType == BT_BASEBALL)
		{
			set_entvar(g_iSoccerBall, var_gravity, 0.4);

			if (!iKnifeKickPower)
			{
				if (iFlags & FL_DUCKING)	iKickVelocity = 30;
				else if (iFlags & FL_FLY)	iKickVelocity = 40;
				else						iKickVelocity = 35;
			}
			else //if (iKnifeKick == ATTACK2)
			{
				if (iFlags & FL_DUCKING)	iKickVelocity = 45;
				else if (iFlags & FL_FLY)	iKickVelocity = 60;
				else						iKickVelocity = 50;

				g_bSoccerBallTrail = true;
				CREATE_BEAMFOLLOW(g_iSoccerBall, g_pSpriteBeam, 4, 10, 40, 40, 250, 130);
			}

			vecOrigin[2] += 38.0; // поднимаем мяч для броска
		}
	}

	// если после смены высоты !iPointContents возвращаем прежнее значение
	iPointContents = point_contents(vecOrigin);
	if (iPointContents != CONTENTS_EMPTY && iPointContents != CONTENTS_WATER)
		xs_vec_copy(vecTempOrigin, vecOrigin);

	set_entvar(g_iSoccerBall, var_origin, vecOrigin);

	new Float:vecVelocity[3];
	velocity_by_aim(id, iKickVelocity * g_iBallSpeed, vecVelocity);

	set_entvar(g_iSoccerBall, var_solid, SOLID_TRIGGER); // откл для теста
	set_entvar(g_iSoccerBall, var_velocity, vecVelocity);

	// fix: когда мяч пролетал мимо игрока
	set_entvar(g_iSoccerBall, var_movetype, MOVETYPE_BOUNCE);
	entity_set_size(g_iSoccerBall, Float:{-4.0, -4.0, -4.0}, Float:{4.0, 4.0, 4.0});

	emit_sound(id, CHAN_AUTO, sound_kick_ball, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	CREATE_KILLPLAYERATTACHMENTS(id);
	
	g_iSoccerBallOwner = 0;
	g_iSoccerKickOwner = id;
}

public Ball_Think(iEntity)
{
	if (iEntity != g_iSoccerBall)
		return;

	if (!is_entity(iEntity))
	{
		jbe_soccer_remove_ball();
		return;
	}

	set_entvar(iEntity, var_nextthink, get_gametime() + 0.04);

	// не давать анимку мячу в руках
	new Float:vecVelocity[3], Float:fVectorLength;
	get_entvar(iEntity, var_velocity, vecVelocity);
	fVectorLength = vector_length(vecVelocity);
//	if (fVectorLength > 20.0)
	if ((!g_iSoccerBallOwner || g_iBallType == BT_FOOTBALL) && fVectorLength > 20.0)
	{
		new Float:fAngles[3];
		vector_to_angle(vecVelocity, fAngles);
		fAngles[0] = 0.0;
		set_entvar(iEntity, var_angles, fAngles);
		set_entvar(iEntity, var_sequence, 1);
	}
	else if (g_iSoccerBallOwner)
	{
		new Float:fAngles[3];
		get_entvar(g_iSoccerBallOwner, var_v_angle, fAngles);
		set_entvar(iEntity, var_angles, fAngles);
		set_entvar(iEntity, var_sequence, 0); // dell?
	}
	else set_entvar(iEntity, var_sequence, 0);

	if (g_iSoccerBallOwner)
	{
	//	if (fVectorLength > 300.0)
	//		set_entvar(iEntity, var_velocity, {0.0, 0.0, 0.0});

		new Float:vecOrigin[3];
		jbe_soccer_get_ball_cur_pos(vecOrigin);

		new Float:fOrigin[3]; 
		get_entvar(iEntity, var_origin, fOrigin);
		new Float:fDistance = get_distance_f(vecOrigin, fOrigin);

		new iPointContents = point_contents(vecOrigin);
		if (fDistance > 50.0 || fVectorLength > 1000.0 || !(iPointContents == CONTENTS_EMPTY || iPointContents == CONTENTS_WATER)) // || !trace_normal(g_iSoccerBallOwner, fOrigin, vecOrigin, fOrigin))// || // || fDistance < 1.0) 
		{
			set_entvar(iEntity, var_origin, vecOrigin);
			set_entvar(iEntity, var_velocity, {0.0, 0.0, 0.0});

		//	if (IsSetBit(g_iBitUserAdmin, g_iSoccerBallOwner))
		//		client_print(g_iSoccerBallOwner, print_chat, "fVectorLength %f", fVectorLength);
		}
		else if (fDistance > 0.0)
		{
			fDistance *= 2.0;
			if (fDistance > 36.0) fDistance = 36.0;

			new Float:fVector[3];
			xs_vec_sub(vecOrigin, fOrigin, fVector);
			xs_vec_mul_scalar(fVector, fDistance, fVector);
			set_entvar(iEntity, var_velocity, fVector);
		}
	}
	else
	{
		if (g_bSoccerBallTrail)
		{
			if (g_iBallType == BT_FOOTBALL && fVectorLength < 600.0)
			{
				g_bSoccerBallTrail = false;
				CREATE_KILLBEAM(iEntity);
			}
			else if (fVectorLength < 100.0)
			{
				g_bSoccerBallTrail = false;
				CREATE_KILLBEAM(iEntity);
			}
		}

		if (g_iSoccerKickOwner)
		{
			new Float:fBallOrigin[3], Float:fOwnerOrigin[3];
			get_entvar(iEntity, var_origin, fBallOrigin);
			get_entvar(g_iSoccerKickOwner, var_origin, fOwnerOrigin);
			fBallOrigin[2] = 0.0;
			fOwnerOrigin[2] = 0.0;

			if (get_distance_f(fBallOrigin, fOwnerOrigin) > 24.0)
			{
				g_iSoccerKickOwner = 0;
				set_entvar(iEntity, var_solid, SOLID_BBOX);
			}
			else if (fVectorLength < 20.0)
			{
				g_iSoccerKickOwner = 0;
				set_entvar(iEntity, var_solid, SOLID_TRIGGER);
			}
		}
	}
}

public Ball_Touch(iTouched, iToucher)
{
	if (!g_iSoccerBall || iTouched != g_iSoccerBall || g_iSoccerBallOwner)
		return;

	if (!is_entity(iTouched))
	{
		jbe_soccer_remove_ball();
		return;
	}

	if (jbe_is_user_valid(iToucher) && g_iSoccerKickOwner != iToucher && ((g_bSoccerBallTouch && IsSetBit(g_iBitUserSoccer, iToucher)) || g_bAllUseBall))
	{
		if (g_iBallType == BT_VOLLEYBALL || g_iBallType == BT_BASEBALL)
		{
			g_iBallTouchNum = 0;
			// чтоб мяч не "дрыгался" когда держит игрок
		//	set_entvar(g_iSoccerBall, var_movetype, MOVETYPE_FLY);
		}

		set_entvar(g_iSoccerBall, var_movetype, MOVETYPE_FLY); // для теста сюда

		g_iSoccerBallOwner = iToucher;
		set_entvar(iTouched, var_solid, SOLID_NOT);
		set_entvar(iTouched, var_velocity, Float:{0.0, 0.0, 0.0});
		emit_sound(iToucher, CHAN_AUTO, sound_grab_ball, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		if (g_bSoccerBallTrail)
		{
			g_bSoccerBallTrail = false;
			CREATE_KILLBEAM(iTouched);
		}
		CREATE_PLAYERATTACHMENT(iToucher, _, g_pSpriteBall, 3000);
		
		if (g_bSoccerGame || (g_bAllUseBall && g_iBallType == BT_FOOTBALL && get_user_weapon(iToucher) == CSW_KNIFE))
			jbe_soccer_hand_ball_model(iToucher);
	}
	// сброшен ударяющий или тач не с ним
	else if (!g_iSoccerKickOwner || g_iSoccerKickOwner != iToucher)
	{
		new Float:iDelay = get_gametime();
		static Float:iDelayOld;
		if ((iDelayOld + 0.15) <= iDelay)
		{
			new Float:vecVelocity[3];
			get_entvar(iTouched, var_velocity, vecVelocity);
			if (vector_length(vecVelocity) > 20.0)
			{
				if (g_iBallType == BT_FOOTBALL)
				{
					vecVelocity[0] *= 0.85;
					vecVelocity[1] *= 0.85;
					vecVelocity[2] *= 0.75;
				}
				else if (g_iBallType == BT_VOLLEYBALL) // -15%
				{
					vecVelocity[0] *= 0.722;
					vecVelocity[1] *= 0.722;
					vecVelocity[2] *= 0.637;
				}
				else // if (g_iBallType == BT_BASEBALL) // -50%
				{
					if (g_iBallTouchNum)
					{
						vecVelocity[0] *= 0.85;
						vecVelocity[1] *= 0.85;
						vecVelocity[2] *= 0.75;
					}
					else // если 1 тач с миром, сильно уменьшаем velocity 
					{
						vecVelocity[0] *= 0.425;
						vecVelocity[1] *= 0.425;
						vecVelocity[2] *= 0.375;
						g_iBallTouchNum++;
					}
				}

				set_entvar(iTouched, var_velocity, vecVelocity);
				if ((iDelayOld + 0.22) <= iDelay)
					emit_sound(iTouched, CHAN_AUTO, sound_bounce_ball, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				iDelayOld = iDelay;

				new Float:vecOrigin[3];
				get_entvar(iTouched, var_origin, vecOrigin);
				new iPointContents = point_contents(vecOrigin);
				if (iPointContents == CONTENTS_EMPTY || iPointContents == CONTENTS_WATER) // чтоб можно было бить мяч в воде
				{
					/* 1 pev_solid не помогает. проверка _solid != SOLID_TRIGGER тоже
					TODO: проверить по отдельности другие свойства, убрать лишнее*/
					set_entvar(iTouched, var_solid, SOLID_TRIGGER);
					set_entvar(iTouched, var_movetype, MOVETYPE_BOUNCE);
					entity_set_size(iTouched, Float:{-4.0, -4.0, -4.0}, Float:{4.0, 4.0, 4.0});
				}
			}
		}

		if (g_iBallType == BT_VOLLEYBALL || g_iBallType == BT_BASEBALL)
		{
			set_entvar(iTouched, var_gravity, 1.0);
		}
	}
}

public ClientImpulse100(id)
{
	if ((g_bAllUseBall || g_bSoccerStatus) && g_iSoccerBall)
	{
		if (IsSetBit(g_iBitUserSoccer, id) || (g_bAllUseBall && IsSetBit(g_iBitUserAlive, id)))
		{
			if (g_iSoccerBallOwner && g_iSoccerBallOwner != id
			&& ((g_iSoccerUserTeam[g_iSoccerBallOwner] != g_iSoccerUserTeam[id]) || g_bAllUseBall))
			{
				new Float:fEntityOrigin[3], Float:fPlayerOrigin[3], Float:fDistance;
				get_entvar(g_iSoccerBall, var_origin, fEntityOrigin);
				get_entvar(id, var_origin, fPlayerOrigin);
				fDistance = get_distance_f(fEntityOrigin, fPlayerOrigin);
				if (fDistance < 60.0)
				{
					CREATE_KILLPLAYERATTACHMENTS(g_iSoccerBallOwner);
					
					if (g_bSoccerStatus)
						jbe_set_hand_model(g_iSoccerBallOwner);
					else if (g_iBallType == BT_FOOTBALL && g_bAllUseBall && get_user_weapon(g_iSoccerBallOwner) == CSW_KNIFE)
						jbe_default_weapon_knife(g_iSoccerBallOwner);
					
					g_iSoccerBallOwner = id;
					emit_sound(id, CHAN_AUTO, sound_grab_ball, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
					CREATE_PLAYERATTACHMENT(id, _, g_pSpriteBall, 3000);
					
					if (g_bSoccerStatus || (g_iBallType == BT_FOOTBALL && g_bAllUseBall && get_user_weapon(id) == CSW_KNIFE))
						jbe_soccer_hand_ball_model(id);

					// чтоб мяч не "дрыгался"
					if (g_iBallType == BT_VOLLEYBALL || g_iBallType == BT_BASEBALL)
						set_entvar(g_iSoccerBall, var_movetype, MOVETYPE_FLY);
				}
			}
			return PLUGIN_HANDLED;
		}
	}
	return PLUGIN_CONTINUE;
}

stock jbe_soccer_show_sprite_ball(pPlayer)
{
	SendWeaponList(pPlayer, "csone/ball", -1, -1, -1, -1, 0, 1, 1, 0);
	SendItemPickup(pPlayer, 1);
}

/*===== <- Футбол <- =====*///}

