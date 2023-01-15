public load_user_mysql(id)
{	
	if (id <= MaxClients)
	{
		log_amx("load_user_mysql task %n", id);
		if (!is_valid_user_auth(id))
		{
			SetBit(g_iBitUnvalidAuthId, id);
			log_amx("!is_valid_user_auth(%d) %n", id, id);
			return;
		}

		set_task(TIME_SQL_LOAD_USER.0, "load_user_mysql", id + TASK_LOAD_PLAYER_SQL);
		g_fRankRewardExp[id] = get_gametime();
		return;
	}

	id -= TASK_LOAD_PLAYER_SQL;

	log_amx("load_user_mysql start %n", id);

	if (IsNotSetBit(g_iBitUserConnected, id) || !g_bSqlConnected)
		return;

	static szAuth[MAX_AUTHID_LENGTH];
	static szIp[MAX_USERIP_LENGTH];
	static szPass[MAX_PASS_LENGTH];

	new szData[2];
	new iLen;
	
	get_user_authid(id, szAuth, charsmax(szAuth));
	get_user_ip(id, szIp, charsmax(szIp), 1);
	get_user_info(id, g_szPassStr, szPass, charsmax(szPass));

	if (szPass[0])
		mysql_escape_string(szPass, charsmax(szPass));

	// a jbe_save
	// b jbe_grblocks
	// c amx_bans
	// q jbe_quests q.* - грузим все данные
	// amx_serverinfo - типо заглушка
	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
		"SELECT \
			a.id,a.exp,a.money,\
			a.br_type,a.br_time,a.setting,\
			a.limits, a.month_save,\
			b.time_end AS b_end,b.admin_name,\
			c.player_id,c.player_nick,c.admin_nick,c.ban_reason,c.ban_created,c.ban_length, \
			q.* \
		FROM amx_serverinfo "); // , load_user_quests_sql()
	
	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
		"LEFT JOIN %s a ON a.auth='%s'",
	g_szRankTable, szAuth);
	
	if (szPass[0]) { // если есть пасс, подрубаем поиск по ip+pass
		iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
			" OR (a.ip='%s' AND a.pass='%s')",
		szIp, szPass);
	}

	// загрузка // old
	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
		"LEFT JOIN %s q ON q.save_id=a.id ", g_szQuestsTable);

	
/*	// new
	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
		"LEFT JOIN %s q \
		ON q.save_id=a.id \
		AND q.time_end<UNIX_TIMESTAMP(NOW()) \
		AND q.ended='0'", 
	g_szQuestsTable);
*/
	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
		"LEFT JOIN %s b ON ((b.auth='%s' OR b.ip='%s') \
			AND(b.time_end>UNIX_TIMESTAMP(NOW())) AND remove_type = '0')",
	g_szGrBlockTable, szAuth, szIp);
	
	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
		"LEFT JOIN %s_bans c \
			ON c.server_ip='%s' \
		AND((c.ban_created+c.ban_length*60)>UNIX_TIMESTAMP(NOW()) \
			OR c.ban_length='0') \
		AND((c.player_ip='%s'AND UNIX_TIMESTAMP(NOW())-c.ban_created<'%d') \
			OR c.player_id='%s')AND c.expired='0'",
	g_szTablePrefix, g_BanCvars[ban_srv_ip], szIp, g_BanCvars[ban_ip_bantime], szAuth);
	
	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen, " LIMIT 1;");

	szData[0] = SQL_CHECK;
	szData[1] = get_user_userid(id); //szData[1] = id;
	
	SQL_ThreadQuery(g_sqlTuple, "SQL_Handler", g_szSqlQuery, szData, sizeof szData);
	
#if defined SQL_DEBUG
	log_amx("load_user_mysql id: %d name: %n | g_szSqlQuery %s", id, id, g_szSqlQuery);
#endif
	
}