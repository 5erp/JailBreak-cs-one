//

get_user_limits_sql(id, szLimits[], iLen)
{
	formatex(szLimits, iLen,
		"%d %d %d %d %d %d %d %d \
		%d %d %d %d %d %d",

		g_iRankBonus[id][RB_MONEY],
		g_iRankBonus[id][RB_HEALTH],
		g_iRankBonus[id][RB_GRENADES],
		g_iRankBonus[id][RB_FREEDAY],
		g_iRankBonus[id][RB_GLOCK],
		g_iRankBonus[id][RB_EL_COSTUME],
		g_iRankBonus[id][RB_KOKAIN],
		g_iRankBonus[id][RB_GOD],

		g_iVip[id][DV_MONEY],
		g_iVip[id][DV_INVISIBLE],
		g_iVip[id][DV_HP_AP],
		g_iVip[id][DV_GHOST],
		g_iVip[id][DV_GOD],
		g_iVip[id][DV_FOOTSTEPS]
	);
}

set_user_limits_sql(id, const szLimits[])
{
	if (!szLimits[0] || szLimits[0] == EOS) return;
		
	static szParse[DATA_SAVE_LIMITS][3];

	parse(szLimits, 
		szParse[LIMIT_RANK_MONEY],		charsmax(szParse),
		szParse[LIMIT_RANK_HEALTH],		charsmax(szParse),
		szParse[LIMIT_RANK_GRENADES],	charsmax(szParse),
		szParse[LIMIT_RANK_FREEDAY],	charsmax(szParse),
		szParse[LIMIT_RANK_GLOCK],		charsmax(szParse),
		szParse[LIMIT_RANK_EL_COSTUME],	charsmax(szParse),
		szParse[LIMIT_RANK_KOKAIN],		charsmax(szParse),
		szParse[LIMIT_RANK_GOD],		charsmax(szParse),

		szParse[LIMIT_VIP_MONEY],		charsmax(szParse),
		szParse[LIMIT_VIP_INVISIBLE],	charsmax(szParse),
		szParse[LIMIT_VIP_HP_AP],		charsmax(szParse),
		szParse[LIMIT_VIP_GHOST],		charsmax(szParse),
		szParse[LIMIT_VIP_GOD],			charsmax(szParse),
		szParse[LIMIT_VIP_FOOTSTEPS],	charsmax(szParse)
	);

	g_iRankBonus[id][RB_MONEY]		= str_to_num(szParse[LIMIT_RANK_MONEY]);
	g_iRankBonus[id][RB_HEALTH]		= str_to_num(szParse[LIMIT_RANK_HEALTH]);
	g_iRankBonus[id][RB_GRENADES]	= str_to_num(szParse[LIMIT_RANK_GRENADES]);
	g_iRankBonus[id][RB_FREEDAY]	= str_to_num(szParse[LIMIT_RANK_FREEDAY]);
	g_iRankBonus[id][RB_GLOCK]		= str_to_num(szParse[LIMIT_RANK_GLOCK]);
	g_iRankBonus[id][RB_EL_COSTUME]	= str_to_num(szParse[LIMIT_RANK_EL_COSTUME]);
	g_iRankBonus[id][RB_KOKAIN]		= str_to_num(szParse[LIMIT_RANK_KOKAIN]);
	g_iRankBonus[id][RB_GOD]		= str_to_num(szParse[LIMIT_RANK_GOD]);

	g_iVip[id][DV_MONEY]			= str_to_num(szParse[LIMIT_VIP_MONEY]);
	g_iVip[id][DV_INVISIBLE]		= str_to_num(szParse[LIMIT_VIP_INVISIBLE]);
	g_iVip[id][DV_HP_AP]			= str_to_num(szParse[LIMIT_VIP_HP_AP]);
	g_iVip[id][DV_GHOST]			= str_to_num(szParse[LIMIT_VIP_GHOST]);
	g_iVip[id][DV_GOD]				= str_to_num(szParse[LIMIT_VIP_GOD]);
	g_iVip[id][DV_FOOTSTEPS]		= str_to_num(szParse[LIMIT_VIP_FOOTSTEPS]);

	fix_user_limits_size(id);
}

fix_user_limits_size(id)
{
	for (new i; i < DATA_RANK_BONUS; i++)
	{
		if (g_iRankBonus[id][i] > g_eRankBonusRound[i] || g_iRankBonus[id][i] < 0)
			g_iRankBonus[id][i] = g_eRankBonusRound[i];
	}

	// vip section
	if (g_iVip[id][DV_MONEY] > g_eAllCvars[VIP_MONEY_ROUND] || g_iVip[id][DV_MONEY] < 0)
		g_iVip[id][DV_MONEY] = g_eAllCvars[VIP_MONEY_ROUND];
	
	if (g_iVip[id][DV_INVISIBLE] > g_eAllCvars[VIP_INVISIBLE] || g_iVip[id][DV_INVISIBLE] < 0)
		g_iVip[id][DV_INVISIBLE] = g_eAllCvars[VIP_INVISIBLE];
	
	if (g_iVip[id][DV_HP_AP] > g_eAllCvars[VIP_HP_AP_ROUND] || g_iVip[id][DV_HP_AP] < 0)
		g_iVip[id][DV_HP_AP] = g_eAllCvars[VIP_HP_AP_ROUND];
	
	if (g_iVip[id][DV_GHOST] > g_eAllCvars[VIP_GHOST_ROUND] || g_iVip[id][DV_GHOST] < 0)
		g_iVip[id][DV_GHOST] = g_eAllCvars[VIP_GHOST_ROUND];
	
	if (g_iVip[id][DV_GOD] > g_eAllCvars[VIP_GOD_ROUND] || g_iVip[id][DV_GOD] < 0)
		g_iVip[id][DV_GOD] = g_eAllCvars[VIP_GOD_ROUND];
	
	if (g_iVip[id][DV_FOOTSTEPS] > g_eAllCvars[VIP_FOOTSTEPS_ROUND] || g_iVip[id][DV_FOOTSTEPS] < 0)
		g_iVip[id][DV_FOOTSTEPS] = g_eAllCvars[VIP_FOOTSTEPS_ROUND];
}

/* При входе делаем все доступным
 * при загрузке лимиты встанут в нужну позицию
*/
set_max_user_limits(id)//, iLastJoin, bool:bRank = true, bool:bVip = true)
{
	for (new i; i < DATA_RANK_BONUS; i++)
		g_iRankBonus[id][i] = g_eRankBonusRound[i] - 1;

	g_iVip[id][DV_MONEY]		= g_eAllCvars[VIP_MONEY_ROUND] - 1;
	g_iVip[id][DV_INVISIBLE]	= g_eAllCvars[VIP_INVISIBLE] - 1;
	g_iVip[id][DV_HP_AP]		= g_eAllCvars[VIP_HP_AP_ROUND] - 1;
	g_iVip[id][DV_GHOST]		= g_eAllCvars[VIP_GHOST_ROUND] - 1;
	g_iVip[id][DV_GOD]			= g_eAllCvars[VIP_GOD_ROUND] - 1;
	g_iVip[id][DV_FOOTSTEPS]	= g_eAllCvars[VIP_FOOTSTEPS_ROUND] - 1;
}