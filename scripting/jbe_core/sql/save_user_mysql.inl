public save_user_mysql(id) // сохранить данные пользователя
{
	if (IsNotSetBit(g_iBitUserSqlLoad, id) || !g_bSqlConnected)
		return;

	log_amx("save_user_mysql start %n", id);
	
	new szData[1], iLen;
	
	if (g_iUserMoney[id] > LIMIT_SAVE_MONEY && IsNotSetBit(g_iBitUserVip, id))
		g_iUserMoney[id] = LIMIT_SAVE_MONEY;
	else if (g_iUserMoney[id] > LIMIT_SAVE_MONEY_VIP)
		g_iUserMoney[id] = LIMIT_SAVE_MONEY_VIP;

	// TODO: убрать лишнее проверить работу
	new iSysTime = get_systime();
	new iUserGameTime;

	if (g_iUserTeam[id] == TEAM_PRISONER || g_iUserTeam[id] == TEAM_GUARD)
		iUserGameTime = iSysTime - g_iUserGameTime[id];
	else
		iUserGameTime = 0;

	static szSetting[SETTING_STR_SIZE];
	get_user_setting_sql(id, szSetting, charsmax(szSetting));

	static szLimits[LIMITS_STR_SIZE];
	get_user_limits_sql(id, szLimits, charsmax(szLimits));

	iLen = formatex(g_szSqlQuery, charsmax(g_szSqlQuery), 
		"UPDATE %s s, %s q SET ",
		g_szRankTable, g_szQuestsTable);

	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
		"s.name='%s',s.auth='%s',s.money='%d',s.gametime=s.gametime+'%d'",
			get_user_name_sql(id), fnGetUserAuthID(id),
			g_iUserMoney[id], iUserGameTime);

	if (g_iUserExp[id] > 0)
		iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
			",s.exp='%d'", g_iUserExp[id]);

	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
		",s.ip='%s',s.pass='%s'", fnGetUserIp(id), get_user_pass_sql(id));

	if (g_iUserBraceletsType[id] && g_iUserBraceletsTime[id]) 
		iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
			",s.br_type='%d',s.br_time='%d'",
			g_iUserBraceletsType[id], g_iUserBraceletsTime[id]);

	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
		",s.setting='%s'", szSetting);

	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
		",s.limits='%s'", szLimits);

	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
		",s.timestamp='%d'", iSysTime);

	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
		",s.month_time=s.month_time+'%d'", iUserGameTime);

	/* ---=== Quests ===--- */
	new i, j;
	for (i = 0; i < MAX_EXECUTE_QUEST; i++)
	{
		if (!g_iUserQuest[id][EXEC][i])
			continue;

		if (i >= ALL_QUESTS && IsNotSetBit(g_iBitUserVip, id))
			break;

		j++;
		iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
			",q.i%d='%d',q.n%d='%d',q.e%d='%d'",
				j, g_iUserQuest[id][EXEC][i],
			//	j, g_iUserQuest[id][SUB][i],
				j, g_iUserQuest[id][NUM][i],
				j, g_iUserQuest[id][END][i]);
	}

	if (j < MAX_EXECUTE_QUEST)
	{
		for (i = MAX_EXECUTE_QUEST; i > j; i--)
		{
			iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
				",q.i%d='',q.n%d='',q.e%d=''", i, i, i, i);
		}
	}

/*
	new szKey[(11+4) * MAX_EXECUTE_QUEST], ikLen;

	for (i = 0; i < MAX_EXECUTE_QUEST; i++) {
		if (!g_iUserQuestExec[id][i])
			continue;

		ikLen += formatex(szKey, charsmax(szKey), "%sq.key='%d_%d'",
			ikLen ? "OR " : "", g_iSqlUserID[id], g_iUserQuestExec[id][i]);
	}
	if (ikLen)
		format(szKey, charsmax(szKey), "AND q.save_id=s.id AND (%s)", szKey);

*/
	/* ---=== Quests ===--- */

	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
		"WHERE s.id='%d' AND q.save_id=s.id;", g_iSqlUserID[id]);
		
	szData[0] = SQL_IGNORE;
	SQL_ThreadQuery(g_sqlTuple, "SQL_Handler", g_szSqlQuery, szData, sizeof szData);
	
#if defined SQL_DEBUG
	log_amx("save_user_mysql id: %d name: %n | g_szSqlQuery %s", id, id, g_szSqlQuery);
#endif

}
