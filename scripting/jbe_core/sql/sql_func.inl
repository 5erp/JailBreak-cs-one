#include "jbe_core\sql\save_user_mysql.inl"
#include "jbe_core\sql\load_user_mysql.inl"
#include "jbe_core\sql\setting_func.inl"
#include "jbe_core\sql\limits_func.inl"

#if defined ENABLE_CACHE
	#include "jbe_core\sql\cache_func.inl"
#endif

/*===== <- Sql <- =====*///{


public SQL_Handler(iFailState, Handle:sqlQuery, const szError[], iError, const szData[], iDataSize, Float:fQueueTime)
{
	switch (iFailState)
	{
		case TQUERY_CONNECT_FAILED:
		{
			log_amx("[JBE] MySQL connection failed");
			log_amx("[ %d ] %s", iError, szError);
			if (iDataSize)
				log_amx("Query state: %d", szData[0]);
			if (g_bSqlConnected) {
				g_bSqlConnected = false;
				set_task(300.0, "fnReloadMySqlConnect", TASK_RELOAD_MYSQL_CONNECT);
			}
			return PLUGIN_HANDLED;
		}
		case TQUERY_QUERY_FAILED:
		{
			log_amx("[JBE] MySQL query failed");
			log_amx("[ %d ] %s", iError, szError);
			if (iDataSize)
				log_amx("Query state: %d", szData[1]);
			return PLUGIN_HANDLED;
		}
	}
	
	switch (szData[0])
	{
		// Проверяем есть ли запись с игроком.
		case SQL_CHECK:
		{
			new id = find_player("k", szData[1]);
			if (!id || IsNotSetBit(g_iBitUserConnected, id))
				return PLUGIN_HANDLED;
			
			// Есть ли результат запроса
			switch (SQL_NumResults(sqlQuery)) {
				
				// создаем новую запись если нет результата
				case 0: create_user_mysql(id);
				
				// берем данные если есть результат
				default:
				{
					new b_len = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "ban_length"));
					if (b_len)
					{
						static szAuth[MAX_AUTHID_LENGTH];
						static szName[MAX_NAME_LENGTH * 2];
						static szAdmin[MAX_NAME_LENGTH * 2];
						static szReason[MAX_REASON_LENGTH];

						SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "player_id"), szAuth, charsmax(szAuth));
						SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "player_nick"), szName, charsmax(szName));
						SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "admin_nick"), szAdmin, charsmax(szAdmin));
						SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "ban_reason"), szReason, charsmax(szReason));
						new b_time = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "ban_created"));

					#if defined ENABLE_CACHE
						save_user_bans_cache(id, szAuth, szName, szAdmin, szReason, b_time, b_len);
					#endif
					
						BanUserKick(id, szAuth, szName, szAdmin, szReason, b_time, b_len, .bTaskKick = false);
						
						log_amx("SQL_Handler Banned: %n | %f sec", id, fQueueTime);
						return PLUGIN_HANDLED;
					}

					new iSysTime = get_systime();
					
					g_iTimeBlockGrEnd[id] = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "b_end"));
					if (g_iTimeBlockGrEnd[id])
					{
						SetBit(g_iBitUserGrBlock, id);
						SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "admin_name"), g_GrAdmin_Blocker[id], charsmax(g_GrAdmin_Blocker[]));

						if (g_iUserTeam[id] == TEAM_GUARD)
						{
							jbe_set_user_team(id, TEAM_PRISONER);
							client_print_color(id, id, "%s Вам заблокирован вход за охрану! Осталось: %s", g_szGrBlockPrefix, seconds_to_str(g_iTimeBlockGrEnd[id] - iSysTime));
						}

						log_amx("[SQL] load Guard Blocked: %n", id);
					}
					else
					{
						g_GrAdmin_Blocker[id][0] = EOS;
					}
					
					// Если есть другие данные (бана или блока), но нет основных - создаем запись
					g_iSqlUserID[id] = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "id"));
					if (!g_iSqlUserID[id])
					{
						create_user_mysql(id);
						return PLUGIN_HANDLED;
					}

					SetBit(g_iBitUserSqlLoad, id);

					g_iUserExp[id] = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "exp"));
					g_iUserMoney[id] = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "money"));
					
					if (g_iUserMoney[id] > LIMIT_SAVE_MONEY && IsNotSetBit(g_iBitUserVip, id))
						g_iUserMoney[id] = LIMIT_SAVE_MONEY;
						
					jbe_set_user_exp(id, g_iUserExp[id], .bMessage = false);
					jbe_set_user_money(id, g_iUserMoney[id], 0);
					

					new iMonthSave = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "month_save"));
					check_save_month(id, iMonthSave);

					// ---=== Quests ===---

					new iTableID = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "save_id"));
					if (iTableID)
					{
						enum _:DATA_SQL_QUESTS { EXEC, END, NUM };
						static szColum[DATA_SQL_QUESTS][4];
						new iEnd;

						for (new i; i < MAX_EXECUTE_QUEST; i++)
						{
							formatex(szColum[END],	charsmax(szColum[]), "e%d", i+1);
							iEnd = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, szColum[END]));
							if (iEnd < iSysTime) continue;

							formatex(szColum[EXEC],	charsmax(szColum[]), "i%d", i+1);
						//	formatex(szColum[SUB],	charsmax(szColum[]), "s%d", i+1);
							formatex(szColum[NUM],	charsmax(szColum[]), "n%d", i+1);

							user_quest_set(id, 
								.iQuest = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, szColum[EXEC])),
							//	.iSub = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, szColum[SUB])),
								.iNum = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, szColum[NUM])),
								.iEnd = iEnd
							);
						}
					}
					else
						create_user_quests_mysql(id);

				/*
					new i, iNum = IsNotSetBit(g_iBitUserVip, id) ? ALL_QUESTS : ALL_QUESTS + VIP_QUESTS;
					while (SQL_MoreResults(sqlQuery)) {
						
						g_iUserQuestExec[id][i] = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "quest"));
						g_iUserQuestEnd[id][i] = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "end"));
						g_iUserQuestSub[id][i] = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "sub"));
						g_iUserQuestNum[id][i] = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "num"));
						
						if (i++ < iNum)
							SQL_NextRow(sqlQuery);
					}
				*/

					// ---=== Quests ===---

					jbe_set_user_bracelets(0, id,
						.iType = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "br_type")),
						.iTime = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "br_time")) // 
					);
					
					static szSqlColum[128];
					SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "setting"), szSqlColum, charsmax(szSqlColum));
					set_user_setting_sql(id, szSqlColum);

					new szLimits[LIMITS_STR_SIZE];
					SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "limits"), szLimits, charsmax(szLimits));
					set_user_limits_sql(id, szLimits);
				}
			}
		}
		case SQL_INSERT:
		{
			new iPlayer = find_player("k", szData[1]); 

			if (!iPlayer || IsNotSetBit(g_iBitUserConnected, iPlayer))
				return PLUGIN_HANDLED;
			
			g_iSqlUserID[iPlayer] = SQL_GetInsertId(sqlQuery);
			if (g_iSqlUserID[iPlayer])
			{
				SetBit(g_iBitUserSqlLoad, iPlayer);
				create_user_quests_mysql(iPlayer);
			}
		#if defined SQL_DEBUG
			log_amx("SQL_Handler SQL_INSERT | %f sec | id: %d | g_iSqlUserID %d | ", fQueueTime, iPlayer, g_iSqlUserID[iPlayer]);
		#endif
		
		}

		case SQL_RELOAD:
		{
			g_bSqlConnected = true;
			log_amx("SQL_Handler SQL_RELOAD [SQL] Connect OK! | %f sec", fQueueTime);
			for (new i = 1, Float:fTime = 0.0; i <= MaxClients; i++)
			{
				if (IsNotSetBit(g_iBitUserConnected, i)
				|| task_exists(i+TASK_LOAD_PLAYER_SQL)
				|| IsSetBit(g_iBitUserSqlLoad, i))
					continue;
				
				set_task(fTime, "load_user_mysql", i+TASK_LOAD_PLAYER_SQL);
				fTime += 0.5;
			}
		}
	}
	
#if defined SQL_DEBUG
	log_amx("SQL_Handler | %f sec", fQueueTime);
#endif

	return PLUGIN_HANDLED;
}

public fnReloadMySqlConnect()
{
	log_amx("fnReloadMySqlConnect");
	new szData[1];
	szData[0] = SQL_RELOAD;
	formatex(g_szSqlQuery, charsmax(g_szSqlQuery), "SELECT 1 FROM %s LIMIT 1;", g_szRankTable);
	SQL_ThreadQuery(g_sqlTuple, "SQL_Handler", g_szSqlQuery, szData, sizeof szData);
}

public SQL_HandlerUpdate(iFailState, Handle:sqlQuery, const szError[], iError, const szData[], iDataSize, Float:fQueueTime) {
	if (iFailState)
	{
		log_amx("[SQL ERROR #%d] %s", iError, szError);
		return;
	}
	return;
}

check_save_month(id, iMonthSave)
{
	static iMonth;

	if (!iMonth)
	{
		new szMonth[3];
		get_time("%m", szMonth, charsmax(szMonth));

		iMonth = str_to_num(szMonth);
	}

	if (iMonthSave < iMonth
	|| iMonth == 1 && iMonthSave == 12
	|| iMonth == 1 && iMonthSave < 12 && iMonthSave > 1)
	{
		if (iMonthSave + 1 == iMonth || iMonth == 1 && iMonthSave == 12) // && iMonthSave != 1
		{
			formatex(g_szSqlQuery, charsmax(g_szSqlQuery),
				"UPDATE %s SET \
				month_last = month_time,\
				month_time = '0',\
				month_save = '%d'\
				WHERE id = '%d'",
				g_szRankTable, iMonth, g_iSqlUserID[id]);
		}
		else
		{
			formatex(g_szSqlQuery, charsmax(g_szSqlQuery),
				"UPDATE %s SET \
				month_last = '0',\
				month_time = '0',\
				month_save = '%d'\
				WHERE id = '%d'",
				g_szRankTable, iMonth, g_iSqlUserID[id]);
		}

		new szData[1];
		szData[0] = SQL_IGNORE;
		SQL_ThreadQuery(g_sqlTuple, "SQL_HandlerUpdate", g_szSqlQuery, szData, sizeof szData);
		
		log_amx(g_szSqlQuery);
	}
}

create_user_mysql(id)
{
	new szDataNew[2];
	new iLen;

	new szMonth[3];
	get_time("%m", szMonth, charsmax(szMonth));
	new iMonth = str_to_num(szMonth);

	iLen = formatex(g_szSqlQuery, charsmax(g_szSqlQuery),
		"INSERT INTO %s SET \
			auth='%s',name='%s',\
			timestamp='%d',month_save='%d'",
	g_szRankTable, fnGetUserAuthID(id), get_user_name_sql(id), get_systime(), iMonth);

#if defined ENABLE_IP_PASS_SAVE
	iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
		",ip='%s',pass='%s'", fnGetUserIp(id), get_user_pass_sql(id));
#endif

	szDataNew[0] = SQL_INSERT;
	szDataNew[1] = get_user_userid(id);

	SQL_ThreadQuery(g_sqlTuple, "SQL_Handler", g_szSqlQuery, szDataNew, sizeof szDataNew);
	
#if defined SQL_DEBUG
	log_amx("create_user_mysql id: %d name: %n | g_szSqlQuery %s |", id, id, g_szSqlQuery);
#endif

}

create_user_quests_mysql(id)
{
	new szDataNew[1];

	formatex(g_szSqlQuery, charsmax(g_szSqlQuery),
		"INSERT INTO %s \
			SET save_id='%d';",
	g_szQuestsTable, g_iSqlUserID[id]);
	
	szDataNew[0] = SQL_IGNORE;

	SQL_ThreadQuery(g_sqlTuple, "SQL_Handler", g_szSqlQuery, szDataNew, sizeof szDataNew);

#if defined SQL_DEBUG
	log_amx("create_user_mysql id: %d name: %n | g_szSqlQuery %s |", id, id, g_szSqlQuery);
#endif

}

// Сохранение ВСЕХ данных при смене карты (в changelevel)
stock sql_update_changelevel()
{
	new iLen;
	new iSysTime = get_systime();
	new szData[1];
	new iUserGameTime;
	static szSetting[SETTING_STR_SIZE];
	static szLimits[LIMITS_STR_SIZE];

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserSqlLoad, i))
			continue;
			
		if (g_iUserMoney[i] > LIMIT_SAVE_MONEY && IsNotSetBit(g_iBitUserVip, i))
			g_iUserMoney[i] = LIMIT_SAVE_MONEY;
		else if (g_iUserMoney[i] > LIMIT_SAVE_MONEY_VIP)
			g_iUserMoney[i] = LIMIT_SAVE_MONEY_VIP;

		if (g_iUserTeam[i] == TEAM_PRISONER || g_iUserTeam[i] == TEAM_GUARD)
			iUserGameTime = iSysTime - g_iUserGameTime[i];
		else
			iUserGameTime = 0;

		get_user_setting_sql(i, szSetting, charsmax(szSetting));
		get_user_limits_sql(i, szLimits, charsmax(szLimits));

		iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
			"%s('%d','%s','%d','%d','%s','%s','%d','%d','%s','%s','%d','%d')", iLen ? "," : "",
		g_iSqlUserID[i],
		get_user_name_sql(i), // szName,
		g_iUserExp[i],
		g_iUserMoney[i],
		fnGetUserIp(i),
		get_user_pass_sql(i),
		g_iUserBraceletsType[i],
		g_iUserBraceletsTime[i],
		szSetting,
		szLimits,
		iUserGameTime,
		iUserGameTime);
	}
	
	if (!iLen) return; // не выполняем запрос если нет игроков
	
	format(g_szSqlQuery, charsmax(g_szSqlQuery),
		"INSERT INTO %s \
			(id,name,exp,money,ip,pass,\
				br_type,br_time,setting,limits,gametime,month_time) \
		VALUES \
		%s \
		ON DUPLICATE KEY UPDATE \
			name=VALUES(name),\
			exp=VALUES(exp),\
			money=VALUES(money),\
			ip=VALUES(ip),\
			pass=VALUES(pass),\
			br_type=VALUES(br_type),\
			br_time=VALUES(br_time),\
			setting=VALUES(setting),\
			limits=VALUES(limits),\
			gametime=VALUES(gametime)+gametime,\
			month_time=VALUES(month_time)+month_time,\
			timestamp='%d'",
	g_szRankTable, g_szSqlQuery, iSysTime);

	szData[0] = SQL_IGNORE;
	SQL_ThreadQuery(g_sqlTuple, "SQL_Handler", g_szSqlQuery, szData, sizeof szData);

#if defined SQL_DEBUG
	log_amx("sql_update_changelevel() %s", g_szSqlQuery);
#endif

	sql_update_quests_changelevel();
}

// TODO: Не сохранять если нет нужды
sql_update_quests_changelevel()
{
	new iLen;
	new szData[1];
	new i, j, l;

	for (i = 1; i <= MaxClients; i++)
	{
	//	if (!g_iSqlUserID[i])
		if (IsNotSetBit(g_iBitUserSqlLoad, i)) // !g_iUserQuestsNum[i]
			continue;

		iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
			"%s('%d'", iLen ? "," : "", g_iSqlUserID[i]);

		j = 0;

		for (l = 0; l < MAX_EXECUTE_QUEST; l++) {
			if (!g_iUserQuest[i][EXEC][l] //g_iUserQuestExec[i][l]
			|| (l >= MAX_EXECUTE_QUEST
			&& IsNotSetBit(g_iBitUserVip, i)))
				continue;

			j++;
			iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
				",'%d','%d','%d'",
				g_iUserQuest[i][EXEC][l],
			//	g_iUserQuest[i][SUB][l],
				g_iUserQuest[i][NUM][l],
				g_iUserQuest[i][END][l]);
			/*g_iUserQuestExec[i][l],
				g_iUserQuestSub[i][l],
				g_iUserQuestNum[i][l],
				g_iUserQuestEnd[i][l]*/
		}
		
		if (j == MAX_EXECUTE_QUEST)
		{
			iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen, ")");
			continue;
		}
		
		for (l = MAX_EXECUTE_QUEST; l > j; l--)
		{
			iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
				",'0','0','0'");
		}

		iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen, ")");
	}

	if (!iLen) return; // не выполняем запрос если нет игроков

	new iLenHead;
	new iLenValue;
	new szQuestsHead[16 * (MAX_EXECUTE_QUEST + 1)];
	new szQuestsValue[65 * (MAX_EXECUTE_QUEST + 1)];

	for (i = 0; i < MAX_EXECUTE_QUEST; i++)
	{
		iLenHead += formatex(szQuestsHead[iLenHead], charsmax(szQuestsHead) - iLenHead,
			",i%d,n%d,e%d", i+1, i+1, i+1);

		iLenValue += formatex(szQuestsValue[iLenValue], charsmax(szQuestsValue) - iLenValue,
			"%si%d=VALUES(i%d),n%d=VALUES(n%d),e%d=VALUES(e%d)",
		iLenValue ? "," : "", i+1, i+1, i+1, i+1, i+1, i+1);
	}

	format(g_szSqlQuery, charsmax(g_szSqlQuery),
		"INSERT INTO %s \
			(save_id%s) \
		VALUES \
			%s \
		ON DUPLICATE KEY UPDATE \
			%s",
	g_szQuestsTable, szQuestsHead, g_szSqlQuery, szQuestsValue);

	szData[0] = SQL_IGNORE;
	SQL_ThreadQuery(g_sqlTuple, "SQL_Handler", g_szSqlQuery, szData, sizeof szData);

#if defined SQL_DEBUG
	log_amx("sql_update_quests_changelevel() %s", g_szSqlQuery);
#endif

}

// Сохранение основных данных в конце раунда (не чаше чем раз в 5 мин)
sql_update_round()
{
	new iLen;
	new iSysTime = get_systime();
	new szData[1];
	new iMoney;
	new iUserGameTime;

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserSqlLoad, i))
			continue;
	
		iMoney = g_iUserMoney[i];
		if (iMoney > LIMIT_SAVE_MONEY && IsNotSetBit(g_iBitUserVip, i))
			iMoney = LIMIT_SAVE_MONEY;
		else if (iMoney > LIMIT_SAVE_MONEY_VIP)
			iMoney = LIMIT_SAVE_MONEY_VIP;

		if (g_iUserTeam[i] == TEAM_PRISONER || g_iUserTeam[i] == TEAM_GUARD)
			iUserGameTime = iSysTime - g_iUserGameTime[i];
		else
			iUserGameTime = 0;

		iLen += formatex(g_szSqlQuery[iLen], charsmax(g_szSqlQuery) - iLen,
			"%s('%d','%d','%d','%d','%d')", iLen ? "," : "",
				g_iSqlUserID[i],
				g_iUserExp[i],
				iMoney,
				iUserGameTime,
				iUserGameTime);
			
		g_iUserGameTime[i] = iSysTime; // ставим текущее время как время коннекта
	}
	
	if (!iLen) return; // не выполняем запрос если нет игроков
	
	format(g_szSqlQuery, charsmax(g_szSqlQuery),
		"INSERT INTO %s \
			(id,exp,money,gametime,month_time) \
		VALUES \
			%s \
		ON DUPLICATE KEY UPDATE \
			exp=VALUES(exp),\
			money=VALUES(money),\
			gametime=VALUES(gametime)+gametime,\
			month_time=VALUES(month_time)+month_time,\
			timestamp='%d'",
	g_szRankTable, g_szSqlQuery, iSysTime);

	szData[0] = SQL_IGNORE;
	SQL_ThreadQuery(g_sqlTuple, "SQL_Handler", g_szSqlQuery, szData, sizeof szData);
	
	g_iSqlLastUpdateTime = iSysTime;
	
#if defined SQL_DEBUG
	log_amx("sql_update_round() %s", g_szSqlQuery);
#endif

}

// Для записи в БД сразу с обработкой строки
stock get_user_name_sql(id)
{
	static szName[MAX_NAME_LENGTH * 2];
	get_user_name(id, szName, charsmax(szName));
	mysql_escape_string(szName, charsmax(szName));
	return szName;
}

/* При сохранении одного игрока bCreate = false
 * При сохранении всех в changelevel bCreate = true (создаем пасс если нет)
 * Тут же обрабатываем пасс  */
stock get_user_pass_sql(id)
{
	static szPass[MAX_PASS_LENGTH];
	get_user_info(id, g_szPassStr, szPass, charsmax(szPass));
	
	if (szPass[0] != EOS)
		mysql_escape_string(szPass, charsmax(szPass));

	return szPass;
}

/*===== <- Sql <- =====*///}


/*
"CREATE TABLE IF NOT EXISTS `%s` \
(\
	`id` int(11) NOT NULL AUTO_INCREMENT, \
	`name` varchar(32) NOT NULL, \
	`auth` varchar(32) NOT NULL, \
//	`ip` varchar(32) NOT NULL, \
	`exp` int(11) DEFAULT 0, \
	`money` int(11) DEFAULT 0, \
	`quest`,
	`bracelets` int(11) DEFAULT 0, \
	`setting` VARCHAR( 64 ) NULL DEFAULT NULL ,\
	`timestamp` int(11) DEFAULT 0, \
	PRIMARY KEY (`id`)\
)", g_szRankTable);

*/