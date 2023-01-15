// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

#include "jbe_core\bracelets\bracelets_menu.inl"

bracelets_init()
{
	register_menucmd(register_menuid("Show_BraceletesMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_BraceletesMenu");
	
	register_menucmd(register_menuid("Show_BraceletesTimeMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_BraceletesTimeMenu");

	register_menucmd(register_menuid("Show_BraceletesEditMenu"),
		(1<<0|1<<1|1<<2|1<<8|1<<9), "Handle_BraceletesEditMenu");
	
	register_clcmdex("br", "ClCmd_BraceletesMenu", 1); // say /br
}

public ClCmd_BraceletesMenu(id)
{
	if (IsNotSetBit(g_iBitUserAdmin, id))
		return ClCmd_InfoBuy(id);
		
	return Show_BraceletesMenu(id, _, 1);
}

// учитываем только если начал бунт атаковав КТ 
// (подстава не считается, так же если игрок уже был в розыске не добавляется).
add_user_wanted_num(id)
{
	if (g_iUserBraceletsType[id] || g_iLastPnId || IsSetBit(g_iBitUserRealWanted, id) || is_shief_global_game())
		return;
	
	SetBit(g_iBitUserRealWanted, id);
	
	if (++g_iUserWantedNum[id] > BR_WANTED_LIMIT)
	{
		client_print_color(id, id, "^4[^3Warning^4] ^1Вы бунтуете %d раз за неделю, запрещено бунтовать больше %d-х раз!", g_iUserWantedNum[id], BR_WANTED_LIMIT);
		
		if (!g_iBitUserAdmin)
			return;
		
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAdmin, i) || i == id) continue;
			client_print_color(i, id, "^4[^3Warning^4] ^1Игрок ^3%n ^1бунтует %d ^1раз за неделю (запрещено больше %d раз).", id, g_iUserWantedNum[id], BR_WANTED_LIMIT);
		}
	}
}

/* Для автивных проверок (атака, нажатие кнопки) */
bool:is_user_braceletes(id, iType, bool:bFade = true)
{
	if (!g_iUserBraceletsType[id])
		return false;

	if (g_iLastPnId && g_iLastPnId == id)
		return false;

	if (g_iDayMode == DAY_DEATHMATCH || g_iDayMode == DAY_DAYMODE)
		return false;

	if (is_shief_global_game())
		return false;

	if (iType == BR_CHECK_BUTTON && g_iUserBraceletsType[id] == BR_ATTACK)
		return false;

	if (is_user_active_bracelets(id))
	{
		if (bFade)
			UTIL_ScreenFade(id, (1<<10), (1<<10), 0, 30, 5, 5, 200);

		return true;
	}

	return false;
}

/* Пасивные проверки (в меню, прочее) */
bool:is_user_active_bracelets(id) {

	if (!g_iUserBraceletsType[id])
		return false;

	new iSysTime = get_systime();
	new iDiffTime = iSysTime - g_iUserBrTimeCheck[id];

	if ((g_iUserBraceletsTime[id] -= iDiffTime) > 0)
	{
		g_iUserBrTimeCheck[id] = iSysTime;
		return true;
	}

	g_iUserBraceletsType[id] = BR_NONE;
	g_iUserBraceletsTime[id] = 0;

	set_hudmessage(255, 0, 0, -1.0, 0.75, 0, 6.0, 2.0, 0.1, 0.1, -1);
	ShowSyncHudMsg(id, g_iSyncInfoCenter, "Ваши наручники сняты, не шалите =)");

	if (IsSetBit(g_iBitUserAlive, id) && get_user_weapon(id) == CSW_KNIFE)
		jbe_default_knife_model(id);

	return false;
}

jbe_set_user_bracelets(id = 0, iTarget, iType, iTime = 0)
{
	if (IsNotSetBit(g_iBitUserConnected, iTarget))
		return PLUGIN_HANDLED;

	if (!iType)
	{
		if (!g_iUserBraceletsType[iTarget] && !g_iUserBraceletsTime[iTarget])
			return PLUGIN_HANDLED;

		if (id && id == iTarget)
		{
			client_print_color(id, id, "%s Нельзя снимать с себя наручники!", g_szChatPrefix);
			return Show_BraceletesMenu(id, g_iMenuPosition[id]);
		}

		g_iUserBraceletsType[iTarget] = BR_NONE;
		g_iUserBraceletsTime[iTarget] = 0;

		if (IsSetBit(g_iBitUserAlive, iTarget) && get_user_weapon(iTarget) == CSW_KNIFE)
			jbe_default_weapon_knife(iTarget);

		if (!id) return PLUGIN_HANDLED;

		client_print_color(0, iTarget, "%s %s ^4%n ^1снял наручники с ^3%n^1.",	
			g_szChatPrefix, g_szAdminPrefix, id, iTarget);
			
		log_amx("[Наручники] %n снял наручники с %n", id, iTarget);
	}
	else
	{
		if (id && IsSetBit(g_iBitUserWanted, iTarget))
			return client_print_color(id, id, "%s Нельзя надеть наручники на бунтующего.", g_szChatPrefix);

		if (id && id == iTarget && g_iUserBraceletsType[iTarget])
		{
			client_print_color(id, id, "%s Нельзя редактировать себе наручники!", g_szChatPrefix);
			return Show_BraceletesMenu(id, g_iMenuPosition[id]);
		}

		if (id)
		{
			client_print_color(0, iTarget, "%s %s ^4%n ^1%s наручники ^3%n ^1на ^4%s ^1(%s).",
				g_szChatPrefix, g_szAdminPrefix, id, 
				g_iUserBraceletsType[iTarget] ? "изменил" : "надел",
				iTarget, seconds_to_str(iTime), g_szBrTypes[iType]);
			
			new const szType[][] = {
				"",
				"атаковать охрану",
				"нажимать кнопки",
				"атаковать охрану и нажимать кнопки"
			};
	
			client_print_color(iTarget, iTarget, "%s В наручниках Вы не можете %s.", 
				g_szChatPrefix, szType[iType]);
	
			log_amx("[Наручники] %n надел наручники на %n на %s", id, iTarget, 
				seconds_to_str(g_iUserBraceletsTime[iTarget]));	
		}

		g_iUserBraceletsType[iTarget] = iType;
		g_iUserBraceletsTime[iTarget] = iTime;
		g_iUserBrTimeCheck[iTarget] = get_systime();

		if (IsSetBit(g_iBitUserAlive, iTarget) && get_user_weapon(iTarget) == CSW_KNIFE)
			jbe_set_bracelets_model(iTarget);
	}

	return PLUGIN_HANDLED;
}

/* g_iMenuTarget[id] += MaxClients - при выборе игрока
 * iTarget - id g_aUsersCache */
stock jbe_set_user_bracelets_offline(id, iTarget, iType, iTime)
{
	iTarget -= MaxClients;
	new aDataUserCache[DATA_USER_CACHE];
	ArrayGetArray(g_aUsersCache, iTarget, aDataUserCache);
	aDataUserCache[BRACELETS_TYPE] = iType;
	aDataUserCache[BRACELETS_TIME] = iTime;
	ArraySetArray(g_aUsersCache, iTarget, aDataUserCache);

	client_print_color(0, print_team_blue, "%s %s ^4%n ^1надел наручники на ^4%s ^1на ^3%s ^1(%s).", g_szChatPrefix, g_szAdminPrefix,
		id, aDataUserCache[NAME], g_iszBraceletesTime[iTime][BRACELETES_NAME], g_szBrTypes[iType]);

	formatex(g_szSqlQuery, charsmax(g_szSqlQuery),
		"UPDATE %s \
		SET br_type = '%d', br_time = '%d'\
		WHERE id = '%d';",
		g_szRankTable, iType, iTime*60, aDataUserCache[SQL_ID]);

	new szData[1];
	szData[0] = SQL_IGNORE;
	SQL_ThreadQuery(g_sqlTuple, "SQL_Handler", g_szSqlQuery, szData, sizeof szData);
}