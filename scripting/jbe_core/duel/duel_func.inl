#include "jbe_core\duel\choice_duel_menu.inl"
#include "jbe_core\duel\duel_users_menu.inl"
#include "jbe_core\duel\duel_prize_menu.inl"
#include "jbe_core\duel\duel_prize_target_menu.inl"
#include "jbe_core\duel\duel_custom_prize_menu.inl"
#include "jbe_core\duel\duel_prize_accept.inl"

duel_init()
{
	// choice_duel_menu.inl
	register_menucmd(register_menuid("Show_ChoiceDuelMenu"), (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_ChoiceDuelMenu");
	
	// duel_users_menu.inl
	register_menucmd(register_menuid("Show_DuelUsersMenu"), (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_DuelUsersMenu");
	
	// duel_prize_menu.inl
	register_menucmd(register_menuid("Show_DuelPrizeMenu"), (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<8|1<<9), "Handle_DuelPrizeMenu");
	
	// duel_prize_target_menu.inl
	register_menucmd(register_menuid("Show_DuelPrizeTargetMenu"), (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_DuelPrizeTargetMenu");
	
	// duel_custom_prize_menu.inl
	register_menucmd(register_menuid("Show_DuelCustomPrizeMenu"), (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<9), "Handle_DuelCustomPrizeMenu");
	
	// duel_prize_accept.inl
	register_menucmd(register_menuid("Show_DuelPrizeAccept"), (1<<0|1<<1), "Handle_DuelPrizeAccept");

	register_clcmd("duel_num", "ClCmd_DuelCustomPrize");
}


/*===== -> Дуэль -> =====*///{
jbe_duel_start_ready(pPlayer, pTarget)
{
	g_iDuelStatus = DS_READY;
	rg_remove_all_items(pPlayer);
	rg_remove_all_items(pTarget);
	g_iDuelUsersId[0] = pPlayer;
	g_iDuelUsersId[1] = pTarget;
	SetBit(g_iBitUserDuel, pPlayer);
	SetBit(g_iBitUserDuel, pTarget);
	ExecuteHamB(Ham_Player_ResetMaxSpeed, pPlayer);
	ExecuteHamB(Ham_Player_ResetMaxSpeed, pTarget);
	set_pev(pPlayer, pev_gravity, 1.0);
	set_pev(pTarget, pev_gravity, 1.0);
	if (jbe_get_user_godmode(pPlayer)) jbe_set_user_godmode(pPlayer, 0);
	if (jbe_get_user_godmode(pTarget)) jbe_set_user_godmode(pTarget, 0);
	client_cmd(0, "mp3 play %s", sound_duel_ready);

	for (new i = 0; i < sizeof(g_iHamHookForwards); i++)
		EnableHamForward(g_iHamHookForwards[i]);

	set_task(1.0, "jbe_duel_count_down", TASK_DUEL_COUNT_DOWN, _, _, "a", g_iDuelCountDown = 10 + 1);
	jbe_set_user_rendering(pPlayer, kRenderFxGlowShell, 255, 0, 0, kRenderNormal, 0);
	jbe_save_user_rendering(pPlayer);
	jbe_set_user_rendering(pTarget, kRenderFxGlowShell, 0, 0, 255, kRenderNormal, 0);
	jbe_save_user_rendering(pTarget);

	set_task(0.5, "jbe_duel_bream_cylinder", TASK_DUEL_BEAMCYLINDER, _, _, "b");
	jbe_duel_bets_start();

	if (g_iLastPnId)
	{
		remove_task(TASK_COUNT_DOWN_TIMER);
		if (g_iDuelPrizeType > DUEL_PRIZE_VOICE)
			Show_DuelPrizeAccept(pTarget);
	}
}

public jbe_duel_count_down()
{
	if (--g_iDuelCountDown)
	{
		set_hudmessage(125, 15, 120, -1.0, 0.16, 0, 0.0, 0.9, 0.1, 0.1, -1);
		ShowSyncHudMsg(0, g_iSyncDuelInformer, 
		"Дуэль на '%s'^n\
		%n ~VS~ %n^n\
		Начало через %d секунд!",
		g_iDuelLang[g_iDuelType], g_iDuelUsersId[0], g_iDuelUsersId[1], g_iDuelCountDown);
	}
	else
	{
		jbe_duel_start();
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserBet, i)
			&& (g_iUserTeam[i] == TEAM_PRISONER || g_iUserTeam[i] == TEAM_GUARD)
			&& g_iDuelUsersId[0] != i && g_iDuelUsersId[1] != i)
			{
				show_menu(i, 0, "^n");
				jbe_informer_offset_down(i);
			}
		}
	}
}

jbe_duel_start()
{
	enum _:eDuelTeams { DUEL_PN, DUEL_GR };
	g_iDuelStatus = DS_WAIT;

	switch (g_iDuelType)
	{
		case DT_DEAGLE:
		{
			EnableHamForward(g_iHamHookPrimaryAttackDuel[DUEL_WEAPON_DEAGLE]);

			set_pev(g_iDuelUsersId[0], pev_health, 100.0);
			set_pev(g_iDuelUsersId[1], pev_health, 100.0);

			rg_give_item(g_iDuelUsersId[0], "item_assaultsuit");
			rg_give_item(g_iDuelUsersId[1], "item_assaultsuit");

			set_task(1.0, "jbe_duel_timer_attack_pre", TASK_DUEL_TIMER_ATTACK, _, _, "a", g_iDuelTimerAttack = 21);
			
			new iEntity[eDuelTeams];
			iEntity[DUEL_PN] = rg_give_item(g_iDuelUsersId[DUEL_PN], "weapon_deagle");
			iEntity[DUEL_GR] = rg_give_item(g_iDuelUsersId[DUEL_GR], "weapon_deagle");

			// на случай если оружие не выдалось (уже есть например)
			if (iEntity[DUEL_PN] > 0) set_member(iEntity[DUEL_PN], m_Weapon_iClip, 1);
			if (iEntity[DUEL_GR] > 0) set_member(iEntity[DUEL_GR], m_Weapon_iClip, 1);
		}
		case DT_M3:
		{
			EnableHamForward(g_iHamHookPrimaryAttackDuel[DUEL_WEAPON_M3]);
			new iEntity[eDuelTeams];
			iEntity[DUEL_PN] = rg_give_item(g_iDuelUsersId[DUEL_PN], "weapon_m3");
			iEntity[DUEL_GR] = rg_give_item(g_iDuelUsersId[DUEL_GR], "weapon_m3");
			if (iEntity[DUEL_PN] > 0) set_member(iEntity[DUEL_PN], m_Weapon_iClip, 1);
			if (iEntity[DUEL_GR] > 0) set_member(iEntity[DUEL_GR], m_Weapon_iClip, 1);
			
			set_pev(g_iDuelUsersId[0], pev_health, 100.0);
			set_pev(g_iDuelUsersId[1], pev_health, 100.0);
			
			rg_give_item(g_iDuelUsersId[0], "item_assaultsuit");
			rg_give_item(g_iDuelUsersId[1], "item_assaultsuit");
		
			// вроде не нужно
			set_member(get_member(g_iDuelUsersId[0], m_pActiveItem), m_Weapon_flNextSecondaryAttack, get_gametime() + 21.0);
			set_member(get_member(g_iDuelUsersId[1], m_pActiveItem), m_Weapon_flNextSecondaryAttack, get_gametime() + 21.0);
		
			set_task(1.0, "jbe_duel_timer_attack_pre", TASK_DUEL_TIMER_ATTACK, _, _, "a", g_iDuelTimerAttack = 21);
		}
		case DT_HEGRENADE:
		{
			rg_give_item(g_iDuelUsersId[0], "weapon_hegrenade");
			rg_give_item(g_iDuelUsersId[1], "weapon_hegrenade");

			rg_set_user_bpammo(g_iDuelUsersId[0], WEAPON_HEGRENADE, 100);
			rg_set_user_bpammo(g_iDuelUsersId[1], WEAPON_HEGRENADE, 100);

			set_pev(g_iDuelUsersId[0], pev_health, 100.0);
			set_pev(g_iDuelUsersId[1], pev_health, 100.0);

			rg_give_item(g_iDuelUsersId[0], "item_assaultsuit");
			rg_give_item(g_iDuelUsersId[1], "item_assaultsuit");

			g_iDuelStatus = DS_RUN;

			check_duel_fm_setmodel();
		}
		case DT_M249:
		{
			EnableHamForward(g_iHamHookPrimaryAttackDuel[DUEL_WEAPON_M249]);

			rg_give_item(g_iDuelUsersId[0], "weapon_m249");
			rg_set_user_bpammo(g_iDuelUsersId[0], WEAPON_M249, 200);
			set_pev(g_iDuelUsersId[0], pev_health, 506.0);
			rg_give_item(g_iDuelUsersId[0], "item_assaultsuit");

			rg_give_item(g_iDuelUsersId[1], "weapon_m249");
			rg_set_user_bpammo(g_iDuelUsersId[1], WEAPON_M249, 200);
			set_pev(g_iDuelUsersId[1], pev_health, 506.0);
			rg_give_item(g_iDuelUsersId[1], "item_assaultsuit");
		}
		case DT_AWP:
		{
			EnableHamForward(g_iHamHookPrimaryAttackDuel[DUEL_WEAPON_AWP]);

			new iEntity[eDuelTeams];
			iEntity[DUEL_PN] = rg_give_item(g_iDuelUsersId[DUEL_PN], "weapon_awp");
			iEntity[DUEL_GR] = rg_give_item(g_iDuelUsersId[DUEL_GR], "weapon_awp");
			if (iEntity[DUEL_PN] > 0) set_member(iEntity[DUEL_PN], m_Weapon_iClip, 1);
			if (iEntity[DUEL_GR] > 0) set_member(iEntity[DUEL_GR], m_Weapon_iClip, 1);
			set_pev(g_iDuelUsersId[0], pev_health, 100.0);
			set_pev(g_iDuelUsersId[1], pev_health, 100.0);
			rg_give_item(g_iDuelUsersId[0], "item_assaultsuit");
			rg_give_item(g_iDuelUsersId[1], "item_assaultsuit");
			set_member(get_member(g_iDuelUsersId[0], m_pActiveItem), m_Weapon_flNextSecondaryAttack, get_gametime() + 21.0);
			set_member(get_member(g_iDuelUsersId[1], m_pActiveItem), m_Weapon_flNextSecondaryAttack, get_gametime() + 21.0);
			set_task(1.0, "jbe_duel_timer_attack_pre", TASK_DUEL_TIMER_ATTACK, _, _, "a", g_iDuelTimerAttack = 21);
		}
		case DT_KNIFE:
		{
			rg_give_item(g_iDuelUsersId[0], "weapon_knife");
			set_pev(g_iDuelUsersId[0], pev_health, 150.0);
			rg_give_item(g_iDuelUsersId[0], "item_assaultsuit");
			rg_give_item(g_iDuelUsersId[1], "weapon_knife");
			set_pev(g_iDuelUsersId[1], pev_health, 150.0);
			rg_give_item(g_iDuelUsersId[1], "item_assaultsuit");
			
			SetBit(g_iBitBalisong, g_iDuelUsersId[0]);
			SetBit(g_iBitBalisong, g_iDuelUsersId[1]);
			SetBit(g_iBitWeaponStatus, g_iDuelUsersId[0]);
			SetBit(g_iBitWeaponStatus, g_iDuelUsersId[1]);
			jbe_set_balisong_model(g_iDuelUsersId[0]);
			jbe_set_balisong_model(g_iDuelUsersId[1]);

			g_iDuelStatus = DS_RUN;
		}
		case DT_AK47:
		{
			EnableHamForward(g_iHamHookPrimaryAttackDuel[DUEL_WEAPON_AK47]);
			new iEntity[eDuelTeams];
			iEntity[DUEL_PN] = rg_give_item(g_iDuelUsersId[DUEL_PN], "weapon_ak47");
			iEntity[DUEL_GR] = rg_give_item(g_iDuelUsersId[DUEL_GR], "weapon_ak47");
			if (iEntity[DUEL_PN] > 0) set_member(iEntity[DUEL_PN], m_Weapon_iClip, 1);
			if (iEntity[DUEL_GR] > 0) set_member(iEntity[DUEL_GR], m_Weapon_iClip, 1);
			set_pev(g_iDuelUsersId[0], pev_health, 200.0);
			set_pev(g_iDuelUsersId[1], pev_health, 200.0);
			rg_give_item(g_iDuelUsersId[0], "item_assaultsuit");
			rg_give_item(g_iDuelUsersId[1], "item_assaultsuit");
			set_task(1.0, "jbe_duel_timer_attack_pre", TASK_DUEL_TIMER_ATTACK, _, _, "a", g_iDuelTimerAttack = 21);
		}
		case DT_SCOUT:
		{
			EnableHamForward(g_iHamHookPrimaryAttackDuel[DUEL_WEAPON_SCOUT]);
			new iEntity[eDuelTeams];
			iEntity[DUEL_PN] = rg_give_item(g_iDuelUsersId[DUEL_PN], "weapon_scout");
			iEntity[DUEL_GR] = rg_give_item(g_iDuelUsersId[DUEL_GR], "weapon_scout");
			if (iEntity[DUEL_PN] > 0) set_member(iEntity[DUEL_PN], m_Weapon_iClip, 1);
			if (iEntity[DUEL_GR] > 0) set_member(iEntity[DUEL_GR], m_Weapon_iClip, 1);
			set_pev(g_iDuelUsersId[0], pev_health, 250.0);
			set_pev(g_iDuelUsersId[1], pev_health, 250.0);
			rg_give_item(g_iDuelUsersId[0], "item_assaultsuit");
			rg_give_item(g_iDuelUsersId[1], "item_assaultsuit");
			set_task(1.0, "jbe_duel_timer_attack_pre", TASK_DUEL_TIMER_ATTACK, _, _, "a", g_iDuelTimerAttack = 21);
		}
	}
}

public jbe_duel_timer_attack_pre()
{
	if (--g_iDuelTimerAttack)
	{
		set_hudmessage(125, 15, 120, -1.0, 0.16, 0, 0.0, 0.9, 0.1, 0.1, -1);
		ShowSyncHudMsg(0, g_iSyncDuelInformer, "Кто первый?^n«« %d »»", g_iDuelTimerAttack);
	}
	else
	{
		set_hudmessage(125, 15, 120, -1.0, 0.16, 0, 0.0, 2.0, 0.1, 0.1, -1);
		ShowSyncHudMsg(0, g_iSyncDuelInformer, "Все уснули :c");

		jbe_duel_cancel();
	}
}

public jbe_duel_timer_attack()
{
	if (--g_iDuelTimerAttack)
	{
		set_hudmessage(125, 15, 120, -1.0, 0.16, 0, 0.0, 0.9, 0.1, 0.1, -1);
		ShowSyncHudMsg(0, g_iSyncDuelInformer, "Стреляет %n^n«« %d »»", g_iDuelUsersId[g_iDuelCurrentAttacer], g_iDuelTimerAttack);
	}
	else
	{
		new iActiveItem = get_member(g_iDuelUsersId[g_iDuelCurrentAttacer], m_pActiveItem); 
		if (iActiveItem > 0)
			ExecuteHamB(Ham_Weapon_PrimaryAttack, iActiveItem);
	}
}

public jbe_duel_bream_cylinder()
{
	new Float:vecOrigin[2][3];
	pev(g_iDuelUsersId[0], pev_origin, vecOrigin[0]);
	pev(g_iDuelUsersId[1], pev_origin, vecOrigin[1]);

	if (g_iDuelStatus == DS_READY || g_iDuelType != DT_KNIFE)
	{
		new Float:fDistance = get_distance_f(vecOrigin[0], vecOrigin[1]);

		if (g_iDuelStatus == DS_RUN && g_iDuelType != DT_HEGRENADE)
		{
			if (fDistance < DUEL_DIST_CUR_ATTACER.0) // отталкиваем от текущего стрелка
			{
				duel_push_player(g_iDuelUsersId[g_iDuelCurrentAttacer], DUEL_DMG_CUR_ATTACER.0, DUEL_DIST_CUR_ATTACER.0);
			}

			if (fDistance < DUEL_DIST_READY_ATTACER.0) // отталкиваем от ожидающего игрока
			{
				new iReadyAttacer = g_iDuelCurrentAttacer == 0 ? 1 : 0;
				duel_push_player(g_iDuelUsersId[iReadyAttacer], DUEL_DMG_READY_ATTACER.0, DUEL_DIST_READY_ATTACER.0);
			}
		}
		else if (fDistance < DUEL_DIST_READY.0) // отталкиваем друг от друга
		{
			duel_push_player(g_iDuelUsersId[0], DUEL_DMG_READY.0, DUEL_DIST_READY.0);
			duel_push_player(g_iDuelUsersId[1], DUEL_DMG_READY.0, DUEL_DIST_READY.0);
		}
	}
	
	static bool:bBeamCylinder;
	bBeamCylinder = !bBeamCylinder;
	if (!bBeamCylinder) return;

	if (pev(g_iDuelUsersId[0], pev_flags) & FL_DUCKING) vecOrigin[0][2] -= 15.0;
	else vecOrigin[0][2] -= 33.0;
	CREATE_BEAMCYLINDER(vecOrigin[0], 150, g_pSpriteWave, _, _, 5, 3, _, 255, 0, 0, 255, _);
	
	if (pev(g_iDuelUsersId[1], pev_flags) & FL_DUCKING) vecOrigin[1][2] -= 15.0;
	else vecOrigin[1][2] -= 33.0;
	CREATE_BEAMCYLINDER(vecOrigin[1], 150, g_pSpriteWave, _, _, 5, 3, _, 0, 0, 255, 255, _);
}

// отталкиваем дуэлянтов
duel_push_player(iPlayer, Float:fPushDamage, Float:fPushDistance)
{
	if (!pev_valid(iPlayer) || !is_user_alive(iPlayer))
		return;

	new Float:vecOrigin[3];
	pev(iPlayer, pev_origin, vecOrigin);

	iPlayer = g_iDuelUsersId[0] != iPlayer ? g_iDuelUsersId[0] : g_iDuelUsersId[1];

	if (!pev_valid(iPlayer) || !is_user_alive(iPlayer))
		return;
	
	new Float:fOrigin[3], Float:fDistance, Float:fDamage;

	pev(iPlayer, pev_origin, fOrigin);
	
	fDistance = get_distance_f(fOrigin, vecOrigin);
	fDamage = fPushDamage - floatmul(fPushDamage, floatdiv(fDistance, fPushDistance));
	fDamage *= EstimateDamage(vecOrigin, iPlayer, 0);
	
	if (fDamage < DUEL_MIN_DMG.0)
		return;
	
	CreateKnockBack(iPlayer, vecOrigin, fDamage, fDamage);
}

// iLooser // pPlayer - проигравший
jbe_duel_ended(pPlayer, bool:bWinDuel = true)
{
	g_iBitUserDuel = 0; // обнуляем тут чтоб скорость норм восстановилась в ХАМе
	
	if (IsSetBit(g_iBitUserConnected, pPlayer))
		rg_remove_all_items(pPlayer);
	
	new iPlayer = g_iDuelUsersId[0] != pPlayer ? g_iDuelUsersId[0] : g_iDuelUsersId[1];
	ExecuteHamB(Ham_Player_ResetMaxSpeed, iPlayer);
	
	ClearBit(g_iBitBalisong, iPlayer);
	ClearBit(g_iBitWeaponStatus, iPlayer);
	rg_remove_all_items(iPlayer);
	rg_give_item(iPlayer, "weapon_knife");
	
	if (!bWinDuel)
	{
		client_print_color(0, 0, "%s Дуэль не состоялась.", g_szDuelPrefix);
	}
	
	// если дуэль при ласт зеке
	else if (g_iLastPnId && (g_iDuelStatus == DS_RUN || g_iDuelStatus == DS_WAIT))
	{
		if (g_iDuelPrizeType >= DUEL_PRIZE_MONEY_CUSTOM)
			duel_give_prize(iPlayer, pPlayer);

		else if (g_iPlayersNum[TEAM_PRISONER] + g_iPlayersNum[TEAM_GUARD] >= PRIZE_MIN_PLAYERS)
		{
			switch (g_iUserTeam[iPlayer])
			{
				case TEAM_PRISONER: duel_give_prize(iPlayer, pPlayer);
				case TEAM_GUARD: give_duel_prize_guard(iPlayer);
			}
		}
		else
		{
			client_print_color(iPlayer, 0, "%s ^1Опыт и деньги за победу в дуэли начисляется если больше^4 %d-ти ^1игроков.",
				g_szDuelPrefix, PRIZE_MIN_PLAYERS);
		}
	}

	if (bWinDuel)
	{
		jbe_duel_bets_check(g_iDuelUsersId[0] != pPlayer ? 0 : 1);
		check_user_quest(iPlayer, QUEST_ID_DUELS);	
	}
	else
	{
		jbe_duel_bets_return();
	}
	
	jbe_duel_disable_all();
}

give_duel_prize_guard(iPlayer)
{
	if (g_iUserTeam[iPlayer] != TEAM_GUARD)
		return;

	jbe_set_user_money(iPlayer, g_iUserMoney[iPlayer] + PRIZE_MONEY_NUM, 1);
	jbe_set_user_exp(iPlayer, g_iUserExp[iPlayer] + PRIZE_EXP_NUM);

	client_print_color(iPlayer, 0, "%s ^1Вы получили^4 +%d ^1опыта и^4 %d$ ^1за победу в дуэли.",
		g_szDuelPrefix, PRIZE_EXP_NUM, PRIZE_MONEY_NUM);

	check_user_quest(iPlayer, QUEST_ID_EXP, PRIZE_EXP_NUM);
}

// для отмены дуэли начальником
jbe_duel_cancel()
{
	g_iBitUserDuel = 0;
	
	ExecuteHamB(Ham_Player_ResetMaxSpeed, g_iDuelUsersId[0]);
	ExecuteHamB(Ham_Player_ResetMaxSpeed, g_iDuelUsersId[1]);

	rg_remove_all_items(g_iDuelUsersId[0]);
	rg_remove_all_items(g_iDuelUsersId[1]);

	rg_give_item(g_iDuelUsersId[0], "weapon_knife");
	rg_give_item(g_iDuelUsersId[1], "weapon_knife");
	
	jbe_duel_disable_all();
	jbe_duel_bets_return();
}

jbe_duel_disable_all()
{
	for (new i = 0; i < sizeof(g_iHamHookForwards); i++)
		DisableHamForward(g_iHamHookForwards[i]);

	g_iDuelStatus = DS_NOT; // тут для check_duel_fm_setmodel()
	g_iBitUserDuel = 0; // на всякий, если офнули в конце раунда
	g_iDuelPrizeTarget = 0;
	g_iDuelPrizeType = 0;
	g_iDualPrizeNum = 0;

	switch (g_iDuelType)
	{
		case 1: DisableHamForward(g_iHamHookPrimaryAttackDuel[DUEL_WEAPON_DEAGLE]);
		case 2: DisableHamForward(g_iHamHookPrimaryAttackDuel[DUEL_WEAPON_M3]);
		case 3: check_duel_fm_setmodel();
		case 4: DisableHamForward(g_iHamHookPrimaryAttackDuel[DUEL_WEAPON_M249]);
		case 5: DisableHamForward(g_iHamHookPrimaryAttackDuel[DUEL_WEAPON_AWP]);
		case 7: DisableHamForward(g_iHamHookPrimaryAttackDuel[DUEL_WEAPON_AK47]);
		case 8: DisableHamForward(g_iHamHookPrimaryAttackDuel[DUEL_WEAPON_SCOUT]);
	}
	
	jbe_set_user_rendering(g_iDuelUsersId[0], kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
	jbe_set_user_rendering(g_iDuelUsersId[1], kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
	jbe_save_user_rendering(g_iDuelUsersId[0]);
	jbe_save_user_rendering(g_iDuelUsersId[1]);
	
	remove_task(TASK_DUEL_BEAMCYLINDER);
	remove_task(TASK_DUEL_TIMER_ATTACK);
	
	// обнуление для меню дуэлей начальника
	g_iDuelUsersId[0] = 0;
	g_iDuelUsersId[1] = 0;
	g_iDuelType = DT_NONE; // хз, думаю не нужно)
	
	if (task_exists(TASK_DUEL_COUNT_DOWN))
	{
		remove_task(TASK_DUEL_COUNT_DOWN);
		client_cmd(0, "mp3 stop");
	}
}

/*===== -> Приз в Дуэле -> =====*/
public ClCmd_DuelCustomPrize(id)
{
	if (!g_iLastPnId || g_iLastPnId != id)
		return PLUGIN_HANDLED;

	new szArg[4];
	read_argv(1, szArg, charsmax(szArg));

	if (!is_str_num(szArg)) 
	{
		client_print_color(id, print_team_red, "%s ^1Вы ввели ^3неверные параметры ^1операции, попробуйте ещё раз.", g_szBetPrefix);
		return Show_DuelCustomPrizeMenu(id);		
	}

	g_iDualPrizeNum = str_to_num(szArg);
	
	_duel_custom_prize_check();
	
	return Show_DuelCustomPrizeMenu(id);
}

// +/- к значению кастом приза
duel_custom_prize(bool:bUp)
{
	switch (g_iDuelPrizeType)
	{
		case DUEL_PRIZE_MONEY_CUSTOM:
		{
			if (bUp)
				g_iDualPrizeNum += 5;
			else
				g_iDualPrizeNum -= 5;
		}
		case DUEL_PRIZE_EXP_CUSTOM:
		{
			if (bUp)
				g_iDualPrizeNum += 2;
			else
				g_iDualPrizeNum -= 2;
		}
	}
	_duel_custom_prize_check();
}

// чек лимита для кастом приза дуэли
_duel_custom_prize_check(bMessage = true)
{
	new iLimit;

	switch (g_iDuelPrizeType)
	{
		case DUEL_PRIZE_MONEY_CUSTOM:
		{
			if (g_iUserMoney[g_iLastPnId] < g_iUserMoney[g_iMenuTarget[g_iLastPnId]])
				iLimit = g_iUserMoney[g_iLastPnId];

			else if (g_iUserMoney[g_iMenuTarget[g_iLastPnId]] < g_iUserMoney[g_iLastPnId])
				iLimit = g_iUserMoney[g_iMenuTarget[g_iLastPnId]];

			if (g_iUserMoney[g_iLastPnId] < g_iDualPrizeNum)
				g_iDualPrizeNum = g_iUserMoney[g_iLastPnId];

			else if (g_iUserMoney[g_iMenuTarget[g_iLastPnId]] < g_iDualPrizeNum)
				g_iDualPrizeNum = g_iUserMoney[g_iMenuTarget[g_iLastPnId]];

			if (iLimit > PRIZE_MONEY_LIMIT)
				iLimit = PRIZE_MONEY_LIMIT;
		}

		case DUEL_PRIZE_EXP_CUSTOM:
		{
			if (g_iUserExp[g_iLastPnId] < g_iUserExp[g_iMenuTarget[g_iLastPnId]])
				iLimit = g_iUserExp[g_iLastPnId];
				
			else if (g_iUserExp[g_iMenuTarget[g_iLastPnId]] < g_iUserExp[g_iLastPnId])
				iLimit = g_iUserExp[g_iMenuTarget[g_iLastPnId]];

			if (g_iUserExp[g_iLastPnId] < g_iDualPrizeNum)
				g_iDualPrizeNum = g_iUserExp[g_iLastPnId];

			else if (g_iUserExp[g_iMenuTarget[g_iLastPnId]] < g_iDualPrizeNum)
				g_iDualPrizeNum = g_iUserExp[g_iMenuTarget[g_iLastPnId]];

			if (iLimit > PRIZE_EXP_LIMIT)
				iLimit = PRIZE_EXP_LIMIT;
		}
	}

	if (g_iDualPrizeNum > iLimit)
	{
		g_iDualPrizeNum = iLimit;
		if (bMessage)
			client_print_color(g_iLastPnId, print_team_default,
				"%s Лимит для этой дуэли ^4%d %s^1.", g_szDuelPrefix,
				iLimit, g_iDuelPrizeType == DUEL_PRIZE_MONEY_CUSTOM ? "$" : "опыта");
	}

	else if (g_iDualPrizeNum <= 0)
	{
		switch (g_iDuelPrizeType) {
			case DUEL_PRIZE_MONEY_CUSTOM:
			{
				if (g_iUserMoney[g_iLastPnId] <= 0 || g_iUserMoney[g_iMenuTarget[g_iLastPnId]] <= 0)
					g_iDualPrizeNum = 0;
				else g_iDualPrizeNum = 1;
			}
			case DUEL_PRIZE_EXP_CUSTOM:
			{
				if (g_iUserExp[g_iLastPnId] <= 0 || g_iUserExp[g_iMenuTarget[g_iLastPnId]] <= 0)
					g_iDualPrizeNum = 0;
				else g_iDualPrizeNum = 1;
			}
		}

		if (bMessage)
		{
			if (g_iDualPrizeNum)
				client_print_color(g_iLastPnId, print_team_default,
					"%s Минимальная сумма^4 1 %s^1.", g_szDuelPrefix,
					g_iDuelPrizeType == DUEL_PRIZE_MONEY_CUSTOM ? "$" : "опыта");
			else
				client_print_color(g_iLastPnId, print_team_default,
					"%s У противника или у Вас^4 0 %s^1.", g_szDuelPrefix,
					g_iDuelPrizeType == DUEL_PRIZE_MONEY_CUSTOM ? "$" : "опыта");
		}
	}
}

//  - победитель - проигравший
duel_give_prize(iWinner = 0, iLoser = 0)
{ 
	if (g_iUserTeam[g_iDuelPrizeTarget] != TEAM_PRISONER
	|| IsNotSetBit(g_iBitUserConnected, g_iDuelPrizeTarget))
		return;

	switch (g_iDuelPrizeType)
	{
		case DUEL_PRIZE_MONEY: 
			jbe_set_user_money(g_iDuelPrizeTarget, g_iUserMoney[g_iDuelPrizeTarget] + PRIZE_MONEY_NUM, 1);
		
		case DUEL_PRIZE_FD: 
			SetBit(g_iBitUserFreeNextRound, g_iDuelPrizeTarget);
		
		case DUEL_PRIZE_EXP: 
			jbe_set_user_exp(g_iDuelPrizeTarget, g_iUserExp[g_iDuelPrizeTarget] + PRIZE_EXP_NUM);
		
		case DUEL_PRIZE_VOICE: 
			SetBit(g_iBitUserVoiceNextRound, g_iDuelPrizeTarget);
		
		case DUEL_PRIZE_MONEY_CUSTOM:
		{
			if (g_iDualPrizeNum > 0)
			{
				jbe_set_user_money(iWinner, g_iUserMoney[iWinner] + g_iDualPrizeNum, 1);
				jbe_set_user_money(iLoser, g_iUserMoney[iLoser] - g_iDualPrizeNum, 1);
			}
		}
		
		case DUEL_PRIZE_EXP_CUSTOM:
		{
			if (g_iDualPrizeNum > 0)
			{
				jbe_set_user_exp(iWinner, g_iUserExp[iWinner] + g_iDualPrizeNum);
				jbe_set_user_exp(iLoser, g_iUserExp[iLoser] - g_iDualPrizeNum);
			}
		}
	}
	
	if (g_iDuelPrizeType <= DUEL_PRIZE_VOICE && g_iLastPnId == g_iDuelPrizeTarget)
		client_print_color(0, iWinner, "%s ^3%n ^1выиграл ^4%s^1.", g_szDuelPrefix,
			iWinner, g_szDuelPrizes[g_iDuelPrizeType]);

	else if (g_iDuelPrizeType >= DUEL_PRIZE_MONEY_CUSTOM)
		client_print_color(0, iWinner, "%s ^3%n ^1выиграл ^4%d %s ^1у ^4%n^1.", g_szDuelPrefix,
			iWinner, g_iDualPrizeNum, g_iDuelPrizeType == DUEL_PRIZE_MONEY_CUSTOM ? "$" : "опыта", iLoser);
	
	else
		client_print_color(0, iWinner, "%s ^3%n ^1выиграл ^4%s ^1для ^3%n.", g_szDuelPrefix,
			g_iLastPnId, g_szDuelPrizes[g_iDuelPrizeType], g_iDuelPrizeTarget);
}

/*===== -> Дуэль -> =====*///}