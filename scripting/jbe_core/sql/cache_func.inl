save_user_data_cache(id)
{
	new aDataUserCache[DATA_USER_CACHE];

	get_user_name(id, aDataUserCache[UC_NAME], charsmax(aDataUserCache[UC_NAME]));
	get_user_authid(id, aDataUserCache[UC_AUTHID], charsmax(aDataUserCache[UC_AUTHID]));
	get_user_ip(id, aDataUserCache[UC_IP], charsmax(aDataUserCache[UC_IP]), 1);
	aDataUserCache[UC_PASS]		= get_user_pass_sql(id);
	aDataUserCache[UC_SQL_ID]	= g_iSqlUserID[id];
	aDataUserCache[UC_EXP]		= g_iUserExp[id];
	aDataUserCache[UC_LEVEL]	= g_iUserLevel[id];
	aDataUserCache[UC_MONEY]	= g_iUserMoney[id];
	aDataUserCache[UC_BRACELETS_TYPE] = g_iUserBraceletsType[id];
	aDataUserCache[UC_BRACELETS_TIME] = g_iUserBraceletsTime[id];
	get_user_setting_sql(id, aDataUserCache[UC_SETTING], charsmax(aDataUserCache[UC_SETTING]));
	get_user_limits_sql(id, aDataUserCache[UC_LIMITS], charsmax(aDataUserCache[UC_LIMITS]));
	aDataUserCache[UC_GR_BLOCK]	= g_iTimeBlockGrEnd[id];
	aDataUserCache[UC_GR_BLOCK_ADMIN]	= g_GrAdmin_Blocker[id];

	for (new i; i < MAX_EXECUTE_QUEST; i++)
	{
		if (!g_iUserQuest[id][EXEC][i]) continue;	
		aDataUserCache[UC_QUESTS][CACHE_QUEST_EXEC][i]	= g_iUserQuest[id][EXEC][i];
		aDataUserCache[UC_QUESTS][CACHE_QUEST_NUM][i]	= g_iUserQuest[id][NUM][i];
		aDataUserCache[UC_QUESTS][CACHE_QUEST_END][i]	= g_iUserQuest[id][END][i];
	}

	aDataUserCache[UC_DISCONECT_TIME] = get_systime();

	if (g_iUserCacheID[id] > -1)
		ArraySetArray(g_aUsersCache, g_iUserCacheID[id], aDataUserCache);
	else
	{
		ArrayPushArray(g_aUsersCache, aDataUserCache, sizeof(aDataUserCache));
		g_iUserCacheSize++;
	}

	g_iUserCacheID[id] = -1;
}

load_user_data_cache(id)
{
	new aDataUserCache[DATA_USER_CACHE];
	new szAuth[MAX_AUTHID_LENGTH];
	new szIp[MAX_USERIP_LENGTH];
	new szPass[MAX_PASS_LENGTH];
	new bool:bIsLoad = false;

	get_user_authid(id, szAuth, charsmax(szAuth));
	get_user_ip(id, szIp, charsmax(szIp), 1);
	szPass = get_user_pass_sql(id);

	for (new i; i < g_iUserCacheSize; i++)
	{
		ArrayGetArray(g_aUsersCache, i, aDataUserCache);
		
		if (equal(aDataUserCache[UC_AUTHID], szAuth)
		|| (equal(aDataUserCache[UC_IP], szIp) && equal(aDataUserCache[UC_PASS], szPass)))
		{
			g_iUserCacheID[id] = i;
			bIsLoad = true;
			break;
		}
	}

	if (bIsLoad)
	{
		g_iSqlUserID[id] = aDataUserCache[UC_SQL_ID];
		if (g_iSqlUserID[id]) SetBit(g_iBitUserSqlLoad, id);
		g_iUserExp[id] = aDataUserCache[UC_EXP];
		g_iUserLevel[id] = aDataUserCache[UC_LEVEL];
		g_iUserMoney[id] = aDataUserCache[UC_MONEY];
		g_iUserBraceletsType[id] = aDataUserCache[UC_BRACELETS_TYPE];
		g_iUserBraceletsTime[id] = aDataUserCache[UC_BRACELETS_TIME];
		set_user_setting_sql(id, aDataUserCache[UC_SETTING]);
		set_user_limits_sql(id, aDataUserCache[UC_LIMITS]);

		if (aDataUserCache[UC_GR_BLOCK] > get_systime())
		{
			g_iTimeBlockGrEnd[id] = aDataUserCache[UC_GR_BLOCK];
			SetBit(g_iBitUserGrBlock, id);
			g_GrAdmin_Blocker[id] = aDataUserCache[UC_GR_BLOCK_ADMIN];
		}

		for (new i; i < MAX_EXECUTE_QUEST; i++)
		{
			user_quest_set(id, 
				.iQuest = aDataUserCache[UC_QUESTS][CACHE_QUEST_EXEC][i],
				.iNum = aDataUserCache[UC_QUESTS][CACHE_QUEST_NUM][i],
				.iEnd = aDataUserCache[UC_QUESTS][CACHE_QUEST_END][i]
			);
		}

		if (g_iUserMoney[id] > LIMIT_SAVE_MONEY && IsNotSetBit(g_iBitUserVip, id))
			g_iUserMoney[id] = LIMIT_SAVE_MONEY;

		else if (g_iUserMoney[id] > LIMIT_SAVE_MONEY_VIP)
			g_iUserMoney[id] = LIMIT_SAVE_MONEY_VIP;
			
	}
	else g_iUserCacheID[id] = -1;

	log_amx("load_user_data_cache(%s) %n", 
		bIsLoad ? "true" : "false", id);

	return bIsLoad;
}


save_user_bans_cache(id, szAuth[MAX_AUTHID_LENGTH], szName[MAX_NAME_LENGTH * 2],
	szAdmin[MAX_NAME_LENGTH * 2], szReason[MAX_REASON_LENGTH], iCreated, iLength)
{
	new aDataBansCache[DATA_BANS_CACHE];

	aDataBansCache[BAN_PLAYER_ID] = szAuth;
	aDataBansCache[BAN_PLAYER_NICK] = szName;
	aDataBansCache[BAN_ADMIN_NICK] = szAdmin;

	copy(aDataBansCache[BAN_REASON], charsmax(aDataBansCache[BAN_REASON]), szReason);

	aDataBansCache[BAN_CREATED] = iCreated;
	aDataBansCache[BAN_LENGTH] = iLength;

	get_user_ip(id, aDataBansCache[BAN_PLAYER_IP], charsmax(aDataBansCache[BAN_PLAYER_IP]), 1);
	aDataBansCache[BAN_DISCONECT_TIME] = get_systime();

	ArrayPushArray(g_aBansCache, aDataBansCache, sizeof(aDataBansCache));

	g_iBansCacheSize++;

	log_amx("save_user_bans_cache %n", id);
}

bool:load_user_bans_cache(id)
{
	new aDataBansCache[DATA_BANS_CACHE];
	new szAuth[MAX_AUTHID_LENGTH];
	new szIp[MAX_USERIP_LENGTH];
//	new iSysTime = get_systime();

	get_user_authid(id, szAuth, charsmax(szAuth));
	get_user_ip(id, szIp, charsmax(szIp), 1);


	for (new i; i < g_iBansCacheSize; i++)
	{
		ArrayGetArray(g_aBansCache, i, aDataBansCache);

		if (equal(aDataBansCache[BAN_PLAYER_ID], szAuth)
		|| equal(aDataBansCache[BAN_PLAYER_IP], szIp))
		{
		//	if (aDataBansCache[BAN_DISCONECT_TIME] + 600 < iSysTime)
		//		continue;
			// if length > sys_time

			BanUserKick(id, 
				aDataBansCache[BAN_PLAYER_ID],
				aDataBansCache[BAN_PLAYER_NICK],
				aDataBansCache[BAN_ADMIN_NICK],
				aDataBansCache[BAN_REASON],
				aDataBansCache[BAN_CREATED],
				aDataBansCache[BAN_LENGTH],
				.bTaskKick = false
			);
			
			log_amx("load_user_bans_cache(true) %n", id);

			return true;
		}
	}

	log_amx("load_user_bans_cache(false) %n", id);
	
	return false;
}