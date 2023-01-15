#include "jbe_core\vip\vip_menu.inl"
#include "jbe_core\vip\vip_menu_respawn.inl"
#include "jbe_core\vip\accept_res_menu.inl"

vip_menu_init()
{
	register_menucmd(register_menuid("Show_VipMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_VipMenu");

	register_menucmd(register_menuid("Show_VipRespawnMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_VipRespawnMenu");

	register_menucmd(register_menuid("Show_UsersAcceptResMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_UsersAcceptResMenu");

	register_menucmd(register_menuid("Show_AcceptResMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_AcceptResMenu");

	register_clcmdex("vipmenu", "ClCmd_VipMenu", 1); // say /vipmenu
	register_clcmdex("vmenu", "ClCmd_VipMenu", 1); // say /vmenu
}

public ClCmd_VipMenu(id)
{
	if (is_not_vip_menu(id))
		return PLUGIN_HANDLED;
	
	return Show_VipMenu(id);
}

// Обновление данных каждый раунд
fn_vip_round_up(id)
{
	g_iVip[id][DV_RESPAWN] = g_eAllCvars[VIP_RESPAWN_NUM];
	g_iVip[id][DV_HEALTH] = g_eAllCvars[VIP_HEALTH_NUM];
	g_iVip[id][DV_MONEY]++;
	g_iVip[id][DV_INVISIBLE]++;
	g_iVip[id][DV_HP_AP]++;
	g_iVip[id][DV_GHOST]++;
	g_iVip[id][DV_GOD]++;
	g_iVip[id][DV_FOOTSTEPS]++;
	g_iVip[id][DV_RES_PLAYERS] = g_eAllCvars[VIP_RESPAWN_PLAYERS];
}

// чек доступности вип меню игроку
is_not_vip_menu(id) // if (is_not_vip_menu(id)) return PLUGIN_HANDLED;
{
	if (IsNotSetBit(g_iBitUserVip, id))
		ClCmd_InfoBuy(id);
	
	else if (g_iGlobalBlock[GB_VIP_MENU])
		client_print_color(id, id, "%s ^1VIP Меню ^4заблокированно %s!",
			g_szChatPrefix, global_get_name_block(GB_VIP_MENU));
	
	else if (g_iDayMode == DAY_DAYMODE || g_iDayMode == DAY_RESTART
		|| IsSetBit(g_iBitUserDuel, id) || IsSetBit(g_iBitBlockMenu, id))
		client_print_color(id, id, "%s Сейчас ^4VIP Меню ^1недоступно!", g_szChatPrefix);
	
	else if (g_iUserTeam[id] == TEAM_SPECTATOR || g_iUserTeam[id] == TEAM_UNASSIGNED)
		client_print_color(id, id, "%s Наблюдателям ^4VIP Меню ^1недоступно!", g_szChatPrefix);
	
	else
		return false;
	
	return true;
}

// обнуление данных при выходе / смене ника
stock jbe_vip_reset(id)
{
	ClearBit(g_iBitUserVip, id);
	ClearBit(g_iBitUserNightVip, id);
	ClearBit(g_iBitVipResAccept, id);
	g_iVip[id][DV_RESPAWN] = 0;
	g_iVip[id][DV_HEALTH] = 0;
	g_iVip[id][DV_MONEY] = 0;
	g_iVip[id][DV_INVISIBLE] = 0;
	g_iVip[id][DV_HP_AP] = 0;
	g_iVip[id][DV_GHOST] = 0;
	g_iVip[id][DV_GOD] = 0;
	g_iVip[id][DV_FOOTSTEPS] = 0;
	g_iVip[id][DV_RES_PLAYERS] = 0;
}

stock check_vip_night(pPlayer = 0) {
	static bool:bFreeVipNight;

	if (pPlayer) { // в таск записи демо или при входе
		if (bFreeVipNight && IsNotSetBit(g_iBitUserVip, pPlayer))
			user_night_vip(pPlayer, .bGive = true);
		return;
	}
	
	new iHour;
	time(iHour);
	
	if (
		g_eAllCvars[VIP_NIGHT_START] >= g_eAllCvars[VIP_NIGHT_ENDED]
		&& (iHour >= g_eAllCvars[VIP_NIGHT_START] || iHour <= g_eAllCvars[VIP_NIGHT_ENDED])

		|| g_eAllCvars[VIP_NIGHT_START] < g_eAllCvars[VIP_NIGHT_ENDED]
		&& (iHour >= g_eAllCvars[VIP_NIGHT_START] && iHour <= g_eAllCvars[VIP_NIGHT_ENDED])
	)
	{
		for (pPlayer = 1; pPlayer <= MaxClients; pPlayer++) {
			if (IsNotSetBit(g_iBitUserConnected, pPlayer)
			|| (get_user_flags(pPlayer) & ADMIN_LEVEL_H))
				continue;

			user_night_vip(pPlayer, .bGive = true);
		}
		bFreeVipNight = true;
		set_member_game(m_GameDesc, fmt(" | %s | ", g_eAllCvars[GAME_NAME_NIGHT]));
	}
	else if (bFreeVipNight)
	{
		for (pPlayer = 1; pPlayer <= MaxClients; pPlayer++)
		{
			if (IsNotSetBit(g_iBitUserConnected, pPlayer))
				continue;
			
			if (IsSetBit(g_iBitUserNightVip, pPlayer))
				user_night_vip(pPlayer, .bGive = false);
		}
		bFreeVipNight = false;
		set_member_game(m_GameDesc, fmt(" | %s | ", g_eAllCvars[GAME_NAME]));
	}
}

user_night_vip(id, bool:bGive)
{	
	if (bGive)
	{
		SetBit(g_iBitUserVip, id);
		SetBit(g_iBitUserNightVip, id);
	
		if (g_szPrefix[id][0] == EOS)
			formatex(g_szPrefix[id], charsmax(g_szPrefix[]), "^1[^4%s^1]", g_eAllCvars[VIP_NIGHT_PREFIX]);
	
		client_print_color(id, print_team_blue,
			"%s ^1Вам выдан ночной ^4VIP ^3( ^1открыть: ^4/vmenu ^1или ^4M ^1-^4 8 ^1-^4 1 ^3)^1.",
			g_szChatPrefix);
	}
	else
	{
		ClearBit(g_iBitUserVip, id);
		ClearBit(g_iBitUserNightVip, id);
		
		if (equal(g_szPrefix[id], g_eAllCvars[VIP_NIGHT_PREFIX]))
			g_szPrefix[id][0] = EOS;
		
		client_print_color(id, id, "%s Действие ночного VIP закончилось.", g_szChatPrefix);
	}
}

vip_respawn(id)//, iTarget = 0)
{
	if (IsSetBit(g_iBitVipResAccept, id))
	{
		ClearBit(g_iBitVipResAccept, id);
		// time[id] = 0
	}

	rg_round_respawn(id);

	g_iVip[id][DV_RESPAWN]--;

	respawm_set_effects(id);
}

/*
jbe_is_access_vip_item(id, iItem, iType = VM_TYPE_ALL, szReason[VIP_ITEM_REASON_STR])
{
	if (iType)
	{
		switch (iType)
		{
			case RB_TYPE_PN: 
				if (g_iUserTeam[id] != TEAM_PRISONER)
					formatex(szReason, charsmax(szReason), "prisoner");
				else iType = 0;

			case RB_TYPE_GR: 
				if (g_iUserTeam[id] != TEAM_GUARD)
					formatex(szReason, charsmax(szReason), "guard");
				else iType = 0;

			case RB_TYPE_CHIEF: 
				if (id != g_iChiefId)
					formatex(szReason, charsmax(szReason), "simon");
				else iType = 0;
		}
	}

	if (!iType && g_iVip[id][iItem] >= g_eRankBonusRound[iItem])
		return true;

	formatex(szReason, charsmax(szReason), "%d round",
		g_eRankBonusRound[iItem] - g_iRankBonus[id][iItem]);

	return false;
}

jbe_is_acces_vip_item_handler(id, iItem, iType = RB_TYPE_ALL)
{
	if (iType)
	{
		switch (iType)
		{
			case RB_TYPE_PN: 
				if (g_iUserTeam[id] != TEAM_PRISONER)
					client_print_color(id, print_team_red, "%s ^1Доступно только ^3заключенным^1!", g_szRankPrefix);
				else iType = 0;

			case RB_TYPE_GR: 
				if (g_iUserTeam[id] != TEAM_GUARD)
					client_print_color(id, print_team_blue, "%s ^1Доступно только ^3охране^1!", g_szRankPrefix);
				else iType = 0;

			case RB_TYPE_CHIEF: 
				if (id != g_iChiefId)
					client_print_color(id, print_team_grey, "%s ^1Доступно только ^3начальнику^1!", g_szRankPrefix);
				else iType = 0;
		}
	}

	if (g_iUserExp[id] >= g_iRankBonusExp[iItem] && !iType
	&& g_iRankBonus[id][iItem] >= g_eRankBonusRound[iItem])
	{
		g_iRankBonus[id][iItem] = 0;
		return true;
	}

	if (!iType)
	{
		if (g_iUserExp[id] < g_iRankBonusExp[iItem])
			client_print_color(id, print_team_red, "%s ^1Для доступа нужно ^3%d+ ^1опыта ^1(не хватает ^3%d^1).", 
				g_szRankPrefix, g_iRankBonusExp[iItem], g_iRankBonusExp[iItem] - g_iUserExp[id]);
	
		else
			client_print_color(id, print_team_red, "%s ^1Будет доступно через ^3%d ^1раундов.",
				g_szRankPrefix, g_eRankBonusRound[iItem] - g_iRankBonus[id][iItem]);
	}

	return false;
}

*/