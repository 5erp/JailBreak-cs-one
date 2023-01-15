// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

#include "jbe_core\gr_block\gr_block_menus.inl"

// get_cvar_string("jbe_grblock_table", g_szBlockTable, charsmax(g_szBlockTable));

guard_block_init()
{
	register_menucmd(register_menuid("Show_BlockGuardMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_BlockGuardMenu");

	register_menucmd(register_menuid("Show_BlockGuardReasonMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_BlockGuardReasonMenu");

	register_menucmd(register_menuid("Show_BlockGuardTimeMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_BlockGuardTimeMenu");

	register_clcmd("gr_reason", "ClCmd_GrBlockReason", ADMIN_BAN);
}

jbe_is_user_guard_block(id)
{
	if (IsSetBit(g_iBitUserGrBlock, id))
	{
		if (g_iTimeBlockGrEnd[id] > get_systime())
			return true;
		else
		{
			ClearBit(g_iBitUserGrBlock, id);
			g_iTimeBlockGrEnd[id] = 0;
			client_print_color(id, id, "%s Вам разблокирован вход за охрану.", g_szGrBlockPrefix);

			log_amx("[GR Block] jbe_is_user_guard_block unblock %n", id);
		}
	}

	return false;
}

public ClCmd_GrBlockReason(id, flags)
{
	if (!CmdAccess(id, flags))
		return UTIL_console_print(id, "%L", id, "ACCESS_DENIED_CNSL");

	if (g_iMenuPlayer[id])
	{
		read_argv(1, g_szReason[id], charsmax(g_szReason[]));
		
		return Cmd_BlockGuardTimeMenu(id);
	}

	return PLUGIN_HANDLED;
}

// хз, просто хандлер под кт блок, остался кода был отдельным плагином 
public SQL_Handler_GrBlock(iFailState, Handle:sqlQuery, const szError[], iError, const szData[], iDataSize, Float:fQueueTime)
{
	switch (iFailState) {
		case TQUERY_CONNECT_FAILED: {
			log_amx("[GR BLOCK] MySQL connection failed");
			log_amx("[ %d ] %s", iError, szError);
			if(iDataSize) log_amx("Query state: %d", szData[0]);
			return PLUGIN_HANDLED;
		}
		case TQUERY_QUERY_FAILED: {
			log_amx("[GR BLOCK] MySQL query failed");
			log_amx("[ %d ] %s", iError, szError);
			if (iDataSize) log_amx("Query state: %d", szData[1]);
			return PLUGIN_HANDLED;
		}
	}

	log_amx("[GR BLOCK] MySQL Connect Time: %f", fQueueTime);
	return PLUGIN_HANDLED;
}

#define SECOND_MINUTE 60

fnBlockPlayer(id)
{
	new szAuth[2][32];
	new szName[2][32];
	new szIp[2][32];
	new iTimeStart;
	new iTimeEnd;
	new iLenght;
	new iRemoveType;
	new iBanCount;
	new szDataNew[2];
	new iPlayer = g_iMenuPlayer[id];
	
	get_user_authid(id, szAuth[0], charsmax(szAuth[]));
	get_user_authid(iPlayer, szAuth[1], charsmax(szAuth[]));

	get_user_name(id, szName[0], charsmax(szName[]));
	get_user_name(iPlayer, szName[1], charsmax(szName[]));
	
	get_user_ip(id, szIp[0], charsmax(szIp[]), 1);
	get_user_ip(iPlayer, szIp[1], charsmax(szIp[]), 1);
	
	iTimeStart = get_systime();

	iLenght = g_iszTimeBlock[g_iMenuTime[id]][0] * SECOND_MINUTE;
	iTimeEnd = iTimeStart + iLenght;

	g_iTimeBlockGrEnd[iPlayer] = iTimeEnd;
	SetBit(g_iBitUserGrBlock, iPlayer);

	new szReason[92];

	if (g_iMenuReason[id])
		formatex(szReason, charsmax(szReason), "%s", g_szReasons[g_iMenuReason[id]]);
	else
		formatex(szReason, charsmax(szReason), "%s", g_szReason[id]);
	
	formatex(g_szSqlQuery, charsmax(g_szSqlQuery),
	"SET NAMES`utf8`;\
		INSERT INTO`%s`\
		(\
			`auth`,\
			`ip`,\
			`name`,\
			`time_start`,\
			`time_end`,\
			`length`,\
			`reason`,\
			`admin_auth`,\
			`admin_ip`,\
			`admin_name`,\
			`remove_type`,\
			`ban_count`\
		)\
	VALUES('%s','%s','%s','%d','%d','%d','%s','%s','%s','%s','%d','%d')",
		g_szGrBlockTable, szAuth[1], szIp[1], szName[1], iTimeStart, iTimeEnd, iLenght, 
		szReason, szAuth[0], szIp[0], szName[0], iRemoveType, iBanCount);
	
	szDataNew[0] = SQL_IGNORE;
	szDataNew[1] = id;
	
	SQL_ThreadQuery(g_sqlTuple, "SQL_Handler_GrBlock", g_szSqlQuery, szDataNew, sizeof szDataNew);
	
	client_print_color(0, 0, "%s ^4%s ^1заблокировал ^4%s ^1вход за охрану, на ^4%s ^1причина ^3%s^1.",
		g_szGrBlockPrefix, szName[0], szName[1], g_iszTimeBlock[g_iMenuTime[id]][1], szReason);

	get_user_name(id, g_GrAdmin_Blocker[iPlayer], charsmax(g_GrAdmin_Blocker[]));
}

fnUnBlockPlayer(id, iPlayer)
{
	new szName[MAX_NAME_LENGTH];
	get_user_name(id, szName, charsmax(szName));
	
	if (!equal(szName, g_GrAdmin_Blocker[iPlayer]))
	{
		client_print_color(id, id, "%s Не доступно.", g_szGrBlockPrefix);
		client_print(id, print_console, "save name %s", g_GrAdmin_Blocker[iPlayer]);
		return;
	}

	ClearBit(g_iBitUserGrBlock, iPlayer);
	g_iTimeBlockGrEnd[iPlayer] = 0;

	new szAuth[32];
	new szIp[32];
	new szData[1];
	
	szData[0] = SQL_IGNORE;
	
	get_user_authid(iPlayer, szAuth, charsmax(szAuth));
	get_user_ip(iPlayer, szIp, charsmax(szIp));
	
	formatex(g_szSqlQuery, charsmax(g_szSqlQuery),
		"UPDATE`%s`SET`remove_type`='1'\
			WHERE(`auth`='%s'OR`ip`='%s')\
				AND(`time_end`>UNIX_TIMESTAMP(NOW())AND`remove_type`=0)\
					LIMIT 1;", g_szGrBlockTable, szAuth, szIp);
	
	SQL_ThreadQuery(g_sqlTuple, "SQL_Handler_GrBlock", g_szSqlQuery, szData, sizeof szData);
	
	client_print_color(0, 0, "%s ^4%n ^1разблокировал ^4%n ^1вход за охрану.", g_szGrBlockPrefix, id, iPlayer);
}
