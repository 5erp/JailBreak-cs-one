// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

#include "jbe_core\rank\rank_bonus_menu.inl"

/*===== <- Погоняла <- =====*///{


rank_menus_init()
{
	// rank_bonus_menu
	register_menucmd(register_menuid("Show_RankBonusMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_RankBonusMenu");

	// access exp bonus
	g_iRankBonusExp[RB_MONEY]		= 500;
	g_iRankBonusExp[RB_HEALTH]		= 1250;
	g_iRankBonusExp[RB_GRENADES]	= 1000;
	g_iRankBonusExp[RB_FREEDAY]		= 2000;
	g_iRankBonusExp[RB_GLOCK]		= 2500;
	g_iRankBonusExp[RB_EL_COSTUME]	= 1500;
	g_iRankBonusExp[RB_KOKAIN]		= 2000;
	g_iRankBonusExp[RB_GOD]			= 2500;
}

is_not_rankbonus_menu(id) // if (is_not_rankbonus_menu(id)) return PLUGIN_HANDLED;
{
	if (IsNotSetBit(g_iBitUserAlive, id))
		client_print_color(id, print_team_blue, "%s Мертвым ^4Ранг бонус ^1недоступен!", g_szChatPrefix);
	
	else if (g_iGlobalBlock[GB_RANK_BONUS])
		client_print_color(id, print_team_blue, "%s ^1Ранг бонус ^4заблокирован %s^1!",
			g_szChatPrefix, global_get_name_block(GB_RANK_BONUS));	
	
	else if (g_iDayMode == DAY_RESTART || g_iDayMode == DAY_DAYMODE || IsSetBit(g_iBitUserDuel, id) || IsSetBit(g_iBitUserBoxing, id))
		client_print_color(id, print_team_blue, "%s Сейчас ^4Ранг бонус ^1недоступен!", g_szChatPrefix);

#if !defined DEVELOPER_OPTIONS
	else if (g_iUserLevel[id] >= RANK_REG_LEVEL && !cmsapi_is_user_member(id))
		client_print_color(id, print_team_blue, "%s С ^3%d ^1уровня для доступа к ранг бонусу требуется регистрация на сайте! Подробнее: ^4cs-one.ru/bonus", g_szChatPrefix, RANK_REG_LEVEL);
#endif

	else
		return false;
	
	return true;
}

jbe_set_user_exp(id, iExp, bool:bMessage = true)
{
	g_iUserExp[id] = iExp;

	new iCurrentLevel = jbe_get_user_level(id);
	
	if (g_iUserLevel[id] != iCurrentLevel)
		jbe_set_user_level(id, iCurrentLevel, bMessage);

	rank_user_exp_percent_reset(id);
}

jbe_set_user_level(id, iLevel, bool:bMessage = true)
{
	if (iLevel > g_iRankMaxLevel)
		iLevel = g_iRankMaxLevel;
	
	g_iUserLevel[id] = iLevel;
	
	if (bMessage)
	{
		client_print_color(0, print_team_blue,
			"%s %s ^1Поздравляем ^4%n ^1с получением погоняла ^3%s^1.",
			g_szChatPrefix, g_szRankPrefix, id, g_iszRankExpName[iLevel][RANK_NAME]);
	}
}

jbe_get_user_level(id)
{
	new iCurrentLevel; // new i; минус
	
	for (new i; i < g_iRankMaxLevel; i++) {
		if (g_iUserExp[id] >= g_iszRankExpName[i][RANK_EXP]
		&& g_iUserExp[id] < g_iszRankExpName[i + 1][RANK_EXP]) {
			iCurrentLevel = i; // minus
			break;
		}
	}
	
	return iCurrentLevel; // return i;
}

rank_reward_exp_players()
{
	new iExp;
	new Float:fRewardExp;
	new Float:fGameTime = get_gametime(); // 1000
	
	// (1000 - 400) / 60 / 5 = 2
	new Float:fZalupa;
	for(new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserConnected, i)) continue;
		
		if (g_iUserTeam[i] == TEAM_SPECTATOR || g_iUserTeam[i] == TEAM_UNASSIGNED)
		{
			client_print_color(i, print_team_default,
				"%s Выберите команду чтобы получать опыт!", g_szRankPrefix);
			continue;
		}

		fRewardExp = fGameTime - g_fRankRewardExp[i]; // 1000 - 400 = 600
		if (fRewardExp <= 0) continue;

	#if defined ENABLE_QUESTS
		if (is_user_quest(i, QUEST_ID_TIMER))
			check_user_quest(i, QUEST_ID_TIMER, floatround(fRewardExp / 60));
	#endif

		iExp = floatround(((fRewardExp / 60) / RANK_EXP_MINUTES), floatround_floor); // iExp = (600 / 60) / (10|5) = (1|2)
		if (iExp <= 0) continue;
		jbe_set_user_exp(i, g_iUserExp[i] + iExp);

	#if defined ENABLE_QUESTS
		check_user_quest(i, QUEST_ID_EXP, iExp);
	#endif

		fZalupa = (fRewardExp - (iExp * RANK_EXP_MINUTES));
		fRewardExp -= fZalupa;
		g_fRankRewardExp[i] = fGameTime - fRewardExp;
		
		new szMessage[16];
		formatex(szMessage, charsmax(szMessage), "+%d опыта", iExp);
		UTIL_DirectorMessage(i, szMessage, 255, 212, 42, 0.7, 0.6, 1, 3.0, 3.0, 0.1, 0.8);
	}
}

// Получаем процент уровня
rank_user_exp_percent_reset(id)
{
	// 150 - 100 = 50
	new iExpNextLvl = g_iszRankExpName[g_iUserLevel[id] + 1][RANK_EXP] - g_iszRankExpName[g_iUserLevel[id]][RANK_EXP];
	
	// 110 - 100 = 10
	new iCurrentExp = g_iUserExp[id] - g_iszRankExpName[g_iUserLevel[id]][RANK_EXP];

	// 10 / (50 / 100) = 20%
	g_iUserExpPercent[id] = floatround(float(iCurrentExp) / (float(iExpNextLvl) / 100), floatround_floor);
}


/* show menu bonus */
jbe_is_access_rank_item(id, iItem, iType = RB_TYPE_ALL, szReason[RANK_ITEM_REASON_STR])
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

	if (g_iUserExp[id] >= g_iRankBonusExp[iItem] && !iType
	&& g_iRankBonus[id][iItem] >= g_eRankBonusRound[iItem])
		return true;

	if (!iType)
	{
		if (g_iUserExp[id] < g_iRankBonusExp[iItem])
			formatex(szReason, charsmax(szReason), "%d+ exp",
				g_iRankBonusExp[iItem]);
	
		else
			formatex(szReason, charsmax(szReason), "%d round",
				g_eRankBonusRound[iItem] - g_iRankBonus[id][iItem]);
	}

	return false;
}

/* handler menu bonus */
is_access_rank_item_nandler(id, iItem, iType = RB_TYPE_ALL)
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

	if (IsSetBit(g_iBitUsedRankBonus, id) && g_iDayMode != DAY_DEATHMATCH)
	{
		client_print_color(id, print_team_blue, "%s Вы уже использовали ранг бонус в этом раунде.", g_szChatPrefix);
		return false;
	}

	if (g_iUserExp[id] >= g_iRankBonusExp[iItem] && !iType
	&& g_iRankBonus[id][iItem] >= g_eRankBonusRound[iItem])
	{
		g_iRankBonus[id][iItem] = 0;
		SetBit(g_iBitUsedRankBonus, id);
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


/*===== <- Погоняла <- =====*///}