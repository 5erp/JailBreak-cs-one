/*
	to do
	* immunity levels
	* get_players с флагами, услугами, зареганных и т.д.
	
	
	5.3.5
		* fix установки флагов игрокам до соединения с БД
*/


#include <amxmodx>
#include <amxmisc>
#include <sqlx>
#include <gamecms5>

/*========================== НАСТРОЙКИ ================================*/

#define USE_ADMINS			//использовать загрузку админов с сайта GameCMS
//#define PL_GAMEMONEY		//записывать в БД баланс игровых денег (`users`.`game_money`)
//#define HLTV_IMMUNITY		//выдать флаги HLTV серверу. Закомментировать, если HLTV не используется или не нужны флаги
//#define SERVICES_REMOVAL 	//динамическое удаление услуги (сразу после ее окончания)
//#define ADD_ADMIN			//включить функцию добавления услуг в базу сайта (amx_addadmin)
//#define USE_USERS_GROUPS	//использование групп пользователей для выдачи флагов доступа (gamecms_users_groups.inl)
//#define USE_USERS_SETTINGS	//Использовать сохранение настроек пользователя
//#define STOPPED_ADMIN_PLUGINS //стопить админ плагины admin.amxx admin_sql.amxx ..
#if defined USE_USERS_GROUPS
	#define FROM_ARRAY						// взять данные о флагах группы из массива (иначе, берется из БД сайта)
#endif

/*========================== Дальше настроек НЕТ!! ================================*/

new const PLUGIN[]	= "GameCMS_API";
new const VERSION[]	= "5.3.5";
new const AUTHOR[]	= "zhorzh78";

#if !defined USE_ADMINS
	#if defined SERVICES_REMOVAL
		#undef SERVICES_REMOVAL
	#endif
	#if defined USE_USERS_GROUPS
		#undef USE_USERS_GROUPS
	#endif
	#if defined ADD_ADMIN
		#undef ADD_ADMIN
	#endif
#endif

enum _:eCvars
{
	Host, 
	User, 
	Pass, 
	Db, 
	Url, 
	Debug, 
	#if defined HLTV_IMMUNITY
	HltvFlags,
	#endif
	#if defined USE_USERS_GROUPS
	CmdGroupMenu,
	CmdGroupMenuAccess,
	#endif
	ReservIP 
};

enum _:MFType 
{	
	PluginLoaded = 1,
	#if defined USE_ADMINS
		AdminConnected,
		Admin_Disconnect,
		AdminsLoaded,
	#endif
	BirthdayMember,
	MemberConnected,
	PluginStop,
	SendChatPrefix
};

#if defined PL_GAMEMONEY
	new bool:g_bGameMoneyError;
#endif

new cpCvarsData[eCvars], g_iMFHandles[MFType];
new g_szAuthIDs[MAX_PLAYERS + 1][MAX_STRING_LEN], g_szAuthIDs64[MAX_PLAYERS + 1][MAX_STRING_LEN];
new g_szQueryStr[MAX_QUERY_MEDIUM_LEN];

new Array:g_arhDisconnQuery, Trie:g_trhDisconnUsers, Trie:g_trhPlayerInfo;
new Handle:g_hDbTuple, g_iServerId;
new bool:g_UsePrefix, g_iRetFwdIgnor, g_bitAPIstatus; 

#if defined USE_ADMINS
	new g_szUsersPasswd[MAX_PLAYERS + 1][MAX_STRING_LEN], g_szPassField[MAX_STRING_LEN], g_Data[AdminInfo];
	new cpAmxPasswordField, cpAmxDefaultAccess, cpAmxMode, g_iAdminCount, g_iPurchasedCount;
	new Array:g_arhAllAdminsInfo, Array:g_arhAllPurchServices, Array:g_arrUserServices;
	new Trie:g_trhOnlineAdminsInfo, Trie:g_trhAdminInfo;
	new g_szDefAccess[MAX_INT_LEN], g_bitDefAccess;

	#if defined ADD_ADMIN
		#include gamecms/gamecms_addadmin.inl
	#endif
	#if defined SERVICES_REMOVAL
		#include gamecms/gamecms_admin_remove.inl
	#endif
#endif

#if defined USE_USERS_GROUPS
	#tryinclude gamecms/gamecms_users_groups.inl
#endif

#if defined USE_USERS_SETTINGS
	#tryinclude gamecms/gamecms_user_settings.inl
#endif

#include gamecms/gamecms_natives.inl

public plugin_init()
{
	register_plugin(PLUGIN, VERSION, AUTHOR);
	
	#if defined USE_ADMINS
		cpAmxPasswordField = register_cvar("amx_password_field", "_pw");
		cpAmxDefaultAccess = register_cvar("amx_default_access", "z");
		cpAmxMode = register_cvar("amx_mode", "1");

		register_cvar("amx_vote_ratio", "0.04");
		register_cvar("amx_vote_time", "10");
		register_cvar("amx_vote_answers", "1");
		register_cvar("amx_vote_delay", "60");
		register_cvar("amx_last_voting", "0");
		register_cvar("amx_show_activity", "2");
		register_cvar("amx_votekick_ratio", "0.40");
		register_cvar("amx_voteban_ratio", "0.40");
		register_cvar("amx_votemap_ratio", "0.40");

		register_concmd("amx_reloadadmins", "CmdReloadAdmins", ADMIN_RCON);
	#endif

	register_cvar("gamecms_api", VERSION, FCVAR_SERVER|FCVAR_UNLOGGED);
	register_concmd("cms_reloadusers", "CmdReloadUsers", ADMIN_RCON);

	#if defined ADD_ADMIN
		FnAddAdminConcmd();
	#endif
	
	#if defined _gamecms_groups_included
		FnPrpareUsersGroup();
	#endif
	
	cpCvarsData[Url]		= register_cvar("cms_url", "site.ru");
	cpCvarsData[Debug]		= register_cvar("cms_debug", "0");
	cpCvarsData[Host]		= register_cvar("cms_hostname", "127.0.0.1");
	cpCvarsData[User]		= register_cvar("cms_username", "", FCVAR_PROTECTED);
	cpCvarsData[Pass]		= register_cvar("cms_password", "szPasswd", FCVAR_PROTECTED);
	cpCvarsData[Db]			= register_cvar("cms_dbname", "cmsbase", FCVAR_PROTECTED);
	cpCvarsData[ReservIP]	= register_cvar("cms_game_server", "");
	
	#if defined HLTV_IMMUNITY
		cpCvarsData[HltvFlags]	= register_cvar("cms_hltv_flags", "ab");
	#endif
	
	new szConfigsDir[MAX_NAME_LENGTH];
	get_configsdir(szConfigsDir, charsmax(szConfigsDir));
	
	#if defined AMXX_182
		MaxClients = get_maxplayers();
		server_cmd("exec %s/amxx.cfg", szConfigsDir);
		set_task(4.0, "MapsConfigsLoad", _, szConfigsDir, charsmax(szConfigsDir));
	#endif
	
	server_cmd("exec %s/gamecms/gamecms.cfg", szConfigsDir);
	server_exec();
	
	#if defined _gamecms_groups_included
		FnPrpareUsersGroupPost();
	#endif
}

public plugin_cfg()
{
	FnMForwardsCreate();
//	CheckDBConnection();
	set_task(1.0, "CheckDBConnection");
}

public CheckDBConnection()
{
	new szHostName[MAX_STRING_LEN], szUserName[MAX_STRING_LEN], szPasswd[MAX_STRING_LEN], szDbName[MAX_STRING_LEN];
	get_pcvar_string(cpCvarsData[Host], szHostName, charsmax(szHostName));
	get_pcvar_string(cpCvarsData[User], szUserName, charsmax(szUserName));
	get_pcvar_string(cpCvarsData[Pass], szPasswd, charsmax(szPasswd));
	get_pcvar_string(cpCvarsData[Db], szDbName, charsmax(szDbName));
	get_pcvar_string(cpCvarsData[Url], SiteUrl, charsmax(SiteUrl));

	// делаем так вместо того что было
	g_hDbTuple = SQL_MakeDbTuple(szHostName, szUserName, szPasswd, szDbName, 10);

	new g_ServerAddr[2][24], ServerAddr[24];
	
	if (!get_pcvar_string(cpCvarsData[ReservIP], ServerAddr, charsmax(ServerAddr)))
		get_user_ip(0, ServerAddr, charsmax(ServerAddr));
	
	strtok(ServerAddr, g_ServerAddr[0], charsmax(g_ServerAddr[]), g_ServerAddr[1], charsmax(g_ServerAddr[]), ':');
	
	new szQuery[256], szDataNew[1];
	
	formatex(szQuery, charsmax(szQuery),
		"SELECT `id` FROM `servers` WHERE `servers`.`ip` = '%s' AND `servers`.`port` = '%s';",
			g_ServerAddr[0], g_ServerAddr[1]);
	
	szDataNew[0] = 0;
	SQL_ThreadQuery(g_hDbTuple, "SQL_CheckHandler", szQuery, szDataNew, sizeof szDataNew);

/*	
	new Handle:hSQL_Connection;	
	new iError, szError[MAX_STRING_LEN * 4];

	hSQL_Connection = FnTryConnect(0, hSQL_Connection, iError, szError, charsmax(szError), szUserName, szPasswd, szDbName);

	if(hSQL_Connection == Empty_Handle)
	{
		SendDbConnectionResults(PLUGIN_HANDLED, hSQL_Connection, Empty_Handle);
		return;
	}

	log_amx("Соединение с базой данных GameCMS установлено");
	g_bitAPIstatus |= UseGameCms;

	g_trhPlayerInfo = TrieCreate();
	g_arhDisconnQuery = ArrayCreate(MAX_STRING_LEN);
	g_trhDisconnUsers = TrieCreate();
	
	#if defined _gamecms_user_settings
		g_trhUserSettings  = TrieCreate();
	#endif
	
	FnCheckOnlinePlayers(1);
	FnGetServerId(hSQL_Connection, szError, charsmax(szError));
		
	ExecuteForward(g_iMFHandles[PluginLoaded], g_iRetFwdIgnor, g_hDbTuple, VERSION, g_iServerId);	

	#if defined USE_ADMINS
		#if defined _gamecms_groups_included
			LoadGroups();
		#endif
		
		if(!g_iServerId)
			return;
		
		get_pcvar_string(cpAmxPasswordField, g_szPassField, charsmax(g_szPassField));
		
		g_arhAllPurchServices = ArrayCreate(AdminInfo);
		g_arhAllAdminsInfo = ArrayCreate(AdminInfo);
		g_arrUserServices = ArrayCreate(AdminInfo);
		g_trhOnlineAdminsInfo = TrieCreate();
		g_trhAdminInfo = TrieCreate();

		g_bitAPIstatus |= UseGameCmsAdmins;
		FnLoadAdmins(LOAD_ALL);
	#endif
*/
}

public SQL_CheckHandler(iFailState, Handle:sqlQuery, const szError[], iError, const szData[], iDataSize, Float:fQueueTime)
{
	switch(iFailState)
	{
		case TQUERY_CONNECT_FAILED: 
		{
			log_amx("[] MySQL connection failed");
			log_amx("[ %d ] %s", iError, szError);
			if (iDataSize) log_amx("Query state: %d", szData[0]);
			return PLUGIN_HANDLED;
		}
		case TQUERY_QUERY_FAILED: 
		{
			log_amx("[] MySQL query failed");
			log_amx("[ %d ] %s", iError, szError);
			if (iDataSize) log_amx("Query state: %d", szData[1]);
			return PLUGIN_HANDLED;
		}
	}

	if (SQL_NumResults(sqlQuery))
	{
	//	g_iServerId = SQL_ReadResult(sqlQuery, SQL_FieldNameToNum(sqlQuery, "id"));
		g_iServerId = SQL_ReadResult(sqlQuery, 0);

		ExecuteForward(g_iMFHandles[PluginLoaded], g_iRetFwdIgnor, g_hDbTuple, VERSION, g_iServerId);	
		
		if(!g_iServerId)
			return PLUGIN_HANDLED;
	
		log_amx("Соединение с базой данных GameCMS установлено");

		g_bitAPIstatus |= UseGameCms;
	
		g_trhPlayerInfo = TrieCreate();
		g_arhDisconnQuery = ArrayCreate(MAX_STRING_LEN);
		g_trhDisconnUsers = TrieCreate();
		
		#if defined _gamecms_user_settings
			g_trhUserSettings  = TrieCreate();
		#endif
		
		FnCheckOnlinePlayers(1);
	//	FnGetServerId(hSQL_Connection, szError, charsmax(szError));		
	//	ExecuteForward(g_iMFHandles[PluginLoaded], g_iRetFwdIgnor, g_hDbTuple, VERSION, g_iServerId);	
	
		#if defined USE_ADMINS
			#if defined _gamecms_groups_included
				LoadGroups();
			#endif
			
		//	if(!g_iServerId) return;
			
			get_pcvar_string(cpAmxPasswordField, g_szPassField, charsmax(g_szPassField));
			
			g_arhAllPurchServices = ArrayCreate(AdminInfo);
			g_arhAllAdminsInfo = ArrayCreate(AdminInfo);
			g_arrUserServices = ArrayCreate(AdminInfo);
			g_trhOnlineAdminsInfo = TrieCreate();
			g_trhAdminInfo = TrieCreate();
	
			g_bitAPIstatus |= UseGameCmsAdmins;

			FnLoadAdmins(LOAD_ALL);
		#endif
	}
	else
	{
		log_amx("Server was not found in Database");
	//	static ret_pause; ExecuteForward(MFHandle[PluginStop], ret_pause);
		ExecuteForward(g_iMFHandles[PluginStop], g_iRetFwdIgnor);
		pause("a");
	}

	return PLUGIN_HANDLED;
}

stock FnGetServerId(Handle:hSQL_Connection, szError[], iErrLen)
{
	new g_ServerAddr[2][MAX_STRING_LEN], ServerAddr[MAX_STRING_LEN];
	if(!get_pcvar_string(cpCvarsData[ReservIP], ServerAddr, charsmax(ServerAddr)))
		get_user_ip(0, ServerAddr, charsmax(ServerAddr));

	strtok(ServerAddr, g_ServerAddr[0], charsmax(g_ServerAddr[]), g_ServerAddr[1], charsmax(g_ServerAddr[]), ':');
	
	new Handle:hGetSrvId;
	hGetSrvId = SQL_PrepareQuery(hSQL_Connection, "SELECT `id` FROM `servers` WHERE `servers`.`ip` = '%s' AND `servers`.`port` = '%s';",
		g_ServerAddr[0], g_ServerAddr[1]);

	if(!SQL_Execute(hGetSrvId))
	{
		if(SQL_QueryError(hGetSrvId, szError, iErrLen))
			log_amx("SQL_QueryError: %s", szError);
		
		SQL_GetQueryString(hGetSrvId, szError, iErrLen)
		log_amx("SQL_QueryString: %s", szError);

		SendDbConnectionResults(PLUGIN_HANDLED, hSQL_Connection, hGetSrvId);
	}
	else
	{
		if(SQL_NumResults(hGetSrvId))
			g_iServerId = SQL_ReadResult(hGetSrvId, SQL_FieldNameToNum(hGetSrvId, "id"));
		else
			log_amx("Server ^"%s:%s^" was not found in Database. Admins was not loaded...", g_ServerAddr[0], g_ServerAddr[1]);

		SendDbConnectionResults(PLUGIN_CONTINUE, hSQL_Connection, hGetSrvId);
	}
}
	
Handle:FnTryConnect(try, Handle:dbHandle, iError, szError[], iLen, szUserName[], szPasswd[], szDbName[])
{
	new szHostName[MAX_STRING_LEN];
	switch(try)
	{
		case 0: get_pcvar_string(cpCvarsData[Host], szHostName, charsmax(szHostName));
		case 1: copy(szHostName, charsmax(szHostName), "127.0.0.1");
		case 2: get_pcvar_string(cpCvarsData[Url], szHostName, charsmax(szHostName));
		default : return dbHandle;
	}
	
	g_hDbTuple = SQL_MakeDbTuple(szHostName, szUserName, szPasswd, szDbName, 10);
	dbHandle = SQL_Connect(g_hDbTuple, iError, szError, iLen); // Проверка доступности БД
	
	if(dbHandle == Empty_Handle)
	{
		log_amx("Connection error: %d. Host: %s | Error: %s", try, szHostName, szError);
		FnTryConnect(++try, dbHandle, iError, szError, iLen, szUserName, szPasswd, szDbName);
	}

	return dbHandle;
}

stock SendDbConnectionResults(result, Handle:hndlConn, Handle:hndlQuery)
{
	if(hndlConn != Empty_Handle)
		SQL_FreeHandle(hndlConn);
	
	if(hndlQuery != Empty_Handle)
		SQL_FreeHandle(hndlQuery);
	
	if(result)
	{
		ExecuteForward(g_iMFHandles[PluginStop], g_iRetFwdIgnor);
		pause("a");
	}
}

#if defined USE_ADMINS
public FnLoadAdmins(iAdminId)
{
	if(~g_bitAPIstatus & UseGameCmsAdmins)
		return;

	new loadOne[MAX_STRING_LEN];
	if(iAdminId > 0)
		formatex(loadOne, charsmax(loadOne), " AND `a`.`id`='%d'", iAdminId);
	
	new iLen;
	iLen+= formatex(g_szQueryStr[iLen], charsmax(g_szQueryStr)- iLen,
		"SELECT `a`.`id`,cast(convert(`a`.`name` using utf8)as binary) as `auth`,`pass`,\
		COALESCE(REPLACE(`ad`.`rights_und`,'none',`s`.`rights`),`ad`.`rights_und`) AS `flags`,`a`.`type`,\
		`ending_date` as `expired`,`ad`.`id` AS `admin_service`,`ad`.`service`,`service_time`,`a`.`active`,`a`.`pause`,\
		cast(convert(`s`.`name` using utf8)as binary) as `service_name`,");
	iLen+= formatex(g_szQueryStr[iLen], charsmax(g_szQueryStr)- iLen, 	
		"cast(convert(`cause` using utf8) as binary) as `cause`, UNIX_TIMESTAMP(`ending_date`) as `expunix`, \
		UNIX_TIMESTAMP(NOW()) as `nowunix`	FROM `admins` a LEFT JOIN `%s` ad ON `ad`.`admin_id`=`a`.`id`\
		LEFT JOIN `servers` sr ON `server`=`sr`.`id` LEFT JOIN `%s` s ON `ad`.`service`=`s`.`id`\
		WHERE `sr`.`id` = '%d' AND (UNIX_TIMESTAMP(`ending_date`) > UNIX_TIMESTAMP(NOW()) OR `ending_date` = '0000-00-00 00:00:00')	%s", 
	TABLE_NAMES[admins_services], TABLE_NAMES[services], g_iServerId, loadOne);

	new szData[2];
	szData[0] = iAdminId;

	if(get_pcvar_num(cpCvarsData[Debug]) > 2)
		log_amx(g_szQueryStr);

	SQL_ThreadQuery(g_hDbTuple, "FnLoadAdmins_post", g_szQueryStr, szData, sizeof(szData));
}

public FnLoadAdmins_post(failstate, Handle:query, szError[], iError, postdata[], lendata)
{
	if(SQL_ErrorAPI(szError, iError, failstate))
		return;

	new iUnix, iAdminId = postdata[0];
	FnDeleteLoadedServices(iAdminId);
	
	if(SQL_NumResults(query)) 
	{
		new iExtraTime = floatround(get_gametime());
		while(SQL_MoreResults(query))
		{
			g_Data[AdminId] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "id"));
			SQL_ReadResult(query, SQL_FieldNameToNum(query, "auth"), g_Data[AdminAuthId], charsmax(g_Data[AdminAuthId]));
			SQL_ReadResult(query, SQL_FieldNameToNum(query, "pass"), g_Data[AdminPassword], charsmax(g_Data[AdminPassword]));
			SQL_ReadResult(query, SQL_FieldNameToNum(query, "flags"), g_Data[AdminServiceFlags], charsmax(g_Data[AdminServiceFlags]));
			SQL_ReadResult(query, SQL_FieldNameToNum(query, "type"), g_Data[AdminType], charsmax(g_Data[AdminType]));
			SQL_ReadResult(query, SQL_FieldNameToNum(query, "expired"), g_Data[AdminExpired], charsmax(g_Data[AdminExpired]));
			g_Data[AdminServiceTime] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "service_time"));		
			iUnix = SQL_ReadResult(query, SQL_FieldNameToNum(query, "expunix"));

			g_Data[AdminService] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "admin_service"));
			g_Data[AdminServiceId] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "service"));

			SQL_ReadResult(query, SQL_FieldNameToNum(query, "service_name"), g_Data[AdminServiceName], charsmax(g_Data[AdminServiceName]));
			g_Data[AdminActive] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "active"));
			g_Data[AdminPaused] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "pause"));
			SQL_ReadResult(query, SQL_FieldNameToNum(query, "cause"), g_Data[AdminReason], charsmax(g_Data[AdminReason]));
				
			if(g_Data[AdminActive] == 1)
			{
				if(iUnix > 0)
					g_Data[AdminActive] = iUnix - SQL_ReadResult(query, SQL_FieldNameToNum(query, "nowunix")) + iExtraTime;
			}

			mysql_escape_string(g_Data[AdminAuthId], charsmax(g_Data[AdminAuthId]));
			mysql_escape_string(g_Data[AdminServiceName], charsmax(g_Data[AdminServiceName]));
			mysql_escape_string(g_Data[AdminReason], charsmax(g_Data[AdminReason]));

			if(containi(g_Data[AdminServiceFlags], "_") != -1)
			{
				#if defined AMXX_182
					ArrayPushArray(g_arhAllPurchServices, g_Data);
				#else
					ArrayPushArray(g_arhAllPurchServices, g_Data, sizeof(g_Data));
				#endif
				g_iPurchasedCount = ArraySize(g_arhAllPurchServices);
			}
			else
			{
				#if defined AMXX_182
					ArrayPushArray(g_arhAllAdminsInfo, g_Data);
				#else
					ArrayPushArray(g_arhAllAdminsInfo, g_Data, sizeof(g_Data));
				#endif
				g_iAdminCount = ArraySize(g_arhAllAdminsInfo);
			}
				
			SQL_NextRow(query);
		}
	}

	log_amx("Загружено из базы данных: услуг по флагам %d шт. / других услуг: %d шт.", g_iAdminCount, g_iPurchasedCount);
	
	if(g_iAdminCount && get_pcvar_num(cpAmxMode))
	{
		if(iAdminId > 0)
		{
			new iClient;
			if(read_flags(g_Data[AdminType]) & FLAG_AUTHID)
				iClient = find_player("c", g_Data[AdminAuthId]);
			else
				iClient = find_player("ab", g_Data[AdminAuthId]);
			
			if(get_pcvar_num(cpCvarsData[Debug]) > 2)
				log_amx("Access after reload  - %s / %d / %d", g_Data[AdminAuthId], iClient, g_Data[AdminId])
			
			if(iClient)
				AuthorizeUser(iClient, .bSilentCheck = false);
		}
		else FnCheckOnlinePlayers(2);
		/*for(new i = 1; i <= MaxClients; i++)
		{
			if(is_user_connected(i))
				AuthorizeUser(i, .bSilentCheck = true);
		}*/
	}

	ExecuteForward(g_iMFHandles[AdminsLoaded], g_iRetFwdIgnor);
}
#endif

public client_authorized(id)
{
	#if defined HLTV_IMMUNITY
	if(is_user_hltv(id))
	{
		new szFlags[MAX_INT_LEN];
		get_pcvar_string(cpCvarsData[HltvFlags], szFlags, charsmax(szFlags));
		set_user_flags(id, read_flags(szFlags));
		return;
	}
	#endif
	
	get_user_authid(id, g_szAuthIDs[id], charsmax(g_szAuthIDs[]));
	StGetUserAuthid64(id, g_szAuthIDs64[id], charsmax(g_szAuthIDs64[]), g_szAuthIDs[id]);

	#if defined USE_ADMINS
	if(get_pcvar_num(cpAmxMode) && g_iAdminCount)
		AuthorizeUser(id, .bSilentCheck = false);
	#endif
}

#define TASK_LOAD_MEMBER_DATA 2345323

public client_putinserver(id)
{
	if(is_user_hltv(id) || is_user_bot(id))
		return;

	//FnLoadMemberData(id);
	set_task(2.5, "task_load_member_data", id + TASK_LOAD_MEMBER_DATA);
}

public client_disconnected(id)
{
	#if defined USE_ADMINS
	if(g_trhOnlineAdminsInfo)
		if(TrieDeleteKey(g_trhOnlineAdminsInfo, get_id_key(id)))
			ExecuteForward(g_iMFHandles[Admin_Disconnect], g_iRetFwdIgnor, id);
	#endif

//	if(TrieKeyExists(g_trhPlayerInfo, g_szAuthIDs[id]))
//		UpdateMemberData(id, 0, false);
}

#if defined USE_ADMINS
public CmdReloadAdmins(id, level, cid)
{
	if(!g_iServerId)
	{
		log_amx("OnReloadAdmins: Server was not found in Database")
		return PLUGIN_HANDLED;
	}
	
	if(cmd_access(id, level, cid, 1))
	{
		new szArg[MAX_INT_LEN];
		read_args(szArg, charsmax(szArg));

		FnLoadAdmins(StGetCmdArgNum(szArg));
	}

	return PLUGIN_HANDLED;
}

AuthorizeUser(id, const szNewName[] = "", const szNewPasswd[] = "", bSilentCheck)
{
	if(~g_bitAPIstatus & UseGameCmsAdmins)
		return PLUGIN_HANDLED;

	if(is_user_bot(id))
		return PLUGIN_HANDLED;

	#if defined HLTV_IMMUNITY
	if(is_user_hltv(id))
		return PLUGIN_HANDLED;
	#endif

	get_pcvar_string(cpAmxDefaultAccess, g_szDefAccess, charsmax(g_szDefAccess));
	g_bitDefAccess = read_flags(strlen(g_szDefAccess)? g_szDefAccess : "z");

	new szPasswd[MAX_STRING_LEN], szUserName[MAX_NAME_LENGTH];
	strlen(szNewName)?
		copy(szUserName, charsmax(szUserName), szNewName):
			get_user_name(id, szUserName, charsmax(szUserName));

	strlen(szNewPasswd)?
		copy(szPasswd, charsmax(szPasswd), szNewPasswd):
			get_user_info(id, g_szPassField, szPasswd, charsmax(szPasswd));

	copy(g_szUsersPasswd[id], charsmax(g_szUsersPasswd[]), szPasswd);
	
	return CheckUserAccess(id, szUserName, g_szAuthIDs[id], szPasswd, bSilentCheck);
}

CheckUserAccess(const id, szName[], szAuthId[], szPasswd[], bSilentCheck)
{
	static userKey[MAX_INT_LEN];
	userKey = get_id_key(id);

	remove_user_flags(id);
	TrieDeleteKey(g_trhOnlineAdminsInfo, userKey);

	for (new index = 0; index < ArraySize(g_arhAllAdminsInfo); ++index)
	{
		#if defined AMXX_182
			ArrayGetArray(g_arhAllAdminsInfo, index, g_Data);
		#else
			ArrayGetArray(g_arhAllAdminsInfo, index, g_Data, sizeof(g_Data));
		#endif

		if(g_Data[AdminActive] == 0 || g_Data[AdminActive] == 2 || g_Data[AdminPaused])
			continue;
		
		new iAuthType = read_flags(g_Data[AdminType]);
		new bool:bStatus;

		if(iAuthType & FLAG_AUTHID && equal(szAuthId, g_Data[AdminAuthId]))
			bStatus = true;
		else if(iAuthType & FLAG_TAG && containi(szName, g_Data[AdminAuthId]) != -1) 
			bStatus = true;
		else if(iAuthType & FLAG_KICK && equali(szName, g_Data[AdminAuthId]))
			bStatus = true;
	
		if(bStatus)
		{	
			new iCheckAccess = StCheckUserPassword(szPasswd, g_Data[AdminPassword], iAuthType);

			if(!iCheckAccess)
				continue;

			if(iCheckAccess == 1)
			{
				if(TrieKeyExists(g_trhOnlineAdminsInfo, userKey))
				{
					new tempData[AdminInfo];
					TrieGetArray(g_trhOnlineAdminsInfo, userKey, tempData, sizeof tempData);

					if((equal(tempData[AdminType], g_Data[AdminType]) 
						&& equal(tempData[AdminPassword], g_Data[AdminPassword])) || (read_flags(tempData[AdminType]) & FLAG_NOPASS || iAuthType & FLAG_NOPASS))
					{
						if(get_pcvar_num(cpCvarsData[Debug]))
							log_amx("Найдена еще услуга по флагам: ^"<%s><%s>^" (аккаунт ^"%s^": %d) (флаги ^"%s^") (услуга ^"%s^") (истекает ^"%s^")", 
							szName, szAuthId, g_Data[AdminAuthId], g_Data[AdminId], g_Data[AdminServiceFlags], g_Data[AdminServiceName], g_Data[AdminExpired]);
							
						add(g_Data[AdminServiceFlags], charsmax(g_Data[AdminServiceFlags]), tempData[AdminServiceFlags]);
						//суммирование имен услуг
						formatex(g_Data[AdminServiceName], charsmax(g_Data[AdminServiceName]), "%s + %s", g_Data[AdminServiceName], tempData[AdminServiceName]);
					}
				}
				else
				{
					if(get_pcvar_num(cpCvarsData[Debug]))
						log_amx("Поиск услуг по флагам... ^"<%s><%s>^" (аккаунт ^"%s^": %d) (флаги ^"%s^") (услуга ^"%s^") (истекает ^"%s^")", 
							szName, szAuthId, g_Data[AdminAuthId], g_Data[AdminId], g_Data[AdminServiceFlags], g_Data[AdminServiceName], g_Data[AdminExpired]);
				}

				StReplaceDuplChar(g_Data[AdminServiceFlags]);
				TrieSetArray(g_trhOnlineAdminsInfo, userKey, g_Data, sizeof g_Data);
					
				#if defined SERVICES_REMOVAL
					CheckRemoveTime(g_Data);
				#endif
			}
			else
			{
				log_amx("Логин: ^"<%s><%s>^" использовал неверный пароль (логин ^"%s^") (введенный пароль ^"%s^" (^"%s^")", 
					szName, szAuthId, g_Data[AdminAuthId], szPasswd, g_Data[AdminPassword]);
				
				engclient_cmd(id, "echo ^"* Неверный пароль!^"");
				server_cmd("kick #%d Доступ запрещен! Проверьте пароль или обратитесь к администратору", get_user_userid(id));
				
				return PLUGIN_CONTINUE;
			}
		}
	}

	if(FnSetUserAuthorized(id, szName, szAuthId, bSilentCheck))
		return PLUGIN_HANDLED;

	return PLUGIN_CONTINUE;
}

public client_infochanged(id)
{
	if(!is_user_connected(id) || !get_pcvar_num(cpAmxMode) || (~g_bitAPIstatus & UseGameCmsAdmins))
		return;
 
	new szNewName[MAX_NAME_LENGTH], szOldName[MAX_NAME_LENGTH], szNewPasswd[MAX_STRING_LEN];
    
	get_user_name(id, szOldName, charsmax(szOldName));
	get_user_info(id, "name", szNewName, charsmax(szNewName));
	get_user_info(id, g_szPassField, szNewPasswd, charsmax(szNewPasswd));
	
	if(!strcmp(szNewName, szOldName))
		if(!strcmp(szNewPasswd, g_szUsersPasswd[id]))
			return;

	AuthorizeUser(id, szNewName, szNewPasswd, false);
}
#endif
//--------------   Форум  ------------------------------//

public CmdReloadUsers(id, level, cid)
{
	if(!cmd_access(id, level, cid, 1))
		return PLUGIN_HANDLED;

	new szArg[MAX_INT_LEN];
	read_args(szArg, charsmax(szArg));
	
	new iTarget = StGetCmdArgNum(szArg);
	if(0 < iTarget <= MaxClients)
	{
		FnReloadUserData(iTarget);
		return PLUGIN_CONTINUE;
	}

	return PLUGIN_HANDLED;
}

FnReloadUserData(iTarget)
{
	UpdateMemberData(iTarget, 0, true);
	FnLoadMemberData(iTarget, UPDATE);
}

public task_load_member_data(id)
{
	id -= TASK_LOAD_MEMBER_DATA;
	FnLoadMemberData(id);
}

FnLoadMemberData(id, bStatus = LOAD)
{
	if(~g_bitAPIstatus & UseGameCms)
		return;

	new szData[2], iLen;
	szData[0] = id;
	szData[1] = bStatus;
	
	new db_pl_game_money[MAX_STRING_LEN] = "";
	#if defined PL_GAMEMONEY
		if(!g_bGameMoneyError)
			db_pl_game_money = ", `game_money`";
	#endif

	iLen += formatex(g_szQueryStr[iLen], charsmax(g_szQueryStr)-iLen, 
		"SELECT `users`.`id`, cast(convert(`users`.`login` using utf8) as binary) as `login`, cast(convert(`users`.`name` using utf8) as binary)\
		as `name`, `birth`, `regdate`, `thanks`, `answers`, `proc`, `last_activity`, `reit`,`users`.`shilings`, \
		cast(convert(`users`.`nick` using utf8) as binary) as `nick`, `game_time`, cast(convert(`users`.`prefix` using utf8) as binary) as `prefix` %s ", 
		db_pl_game_money);
	#if defined _gamecms_user_settings
		if(!g_bSettingsError)
			iLen += add(g_szQueryStr[iLen], charsmax(g_szQueryStr)-iLen, ", `plugins_settings` ");
	#endif
	iLen += formatex(g_szQueryStr[iLen], charsmax(g_szQueryStr)-iLen,
		",(SELECT COUNT(*) FROM `%s` WHERE (`user_id1`=`users`.`id` or `user_id2`=`users`.`id`) AND (`new`=`users`.`id` or\
		`new`='r'+`users`.`id`)) AS `new_messages`, cast(convert(`%s`.`name` using utf8) as binary) as `group_name`, `users`.`rights`\
		FROM `users` LEFT JOIN `%s` ON `users`.`rights`=`%s`.`id` WHERE (`users`.`steam_id` = '%s' OR `users`.`steam_id` = '%s' OR `users`.`steam_api` = '%s');"
		, TABLE_NAMES[dialogs], TABLE_NAMES[users_groups], TABLE_NAMES[users_groups], TABLE_NAMES[users_groups], 
		g_szAuthIDs[id], g_szAuthIDs64[id], g_szAuthIDs64[id]);

	if(get_pcvar_num(cpCvarsData[Debug]) > 3)
		log_amx("Searching user..- %s", g_szQueryStr);

	SQL_ThreadQuery(g_hDbTuple, "ForumData_Handler", g_szQueryStr, szData, sizeof(szData));
}

public ForumData_Handler(failstate, Handle:query, szError[], iError, szData[], datasize)
{
	if(SQL_ErrorAPI(szError, iError, failstate))
		return PLUGIN_CONTINUE;

	new bStatus = szData[1];
	if(bStatus == SAVE)
		return PLUGIN_CONTINUE;
	
	new id = szData[0];
	if(!id || !SQL_NumResults(query))
		return PLUGIN_CONTINUE;

	new szUserData[userDataID];
	TrieGetArray(g_trhPlayerInfo, g_szAuthIDs[id], szUserData, sizeof(szUserData));
	
	SQL_ReadResult(query, SQL_FieldNameToNum(query, "regdate"), szUserData[MemberRegDate], charsmax(szUserData[MemberRegDate]));
	SQL_ReadResult(query, SQL_FieldNameToNum(query, "login"), szUserData[MemberLogin], charsmax(szUserData[MemberLogin]));
	SQL_ReadResult(query, SQL_FieldNameToNum(query, "birth"), szUserData[MemberBirth], charsmax(szUserData[MemberBirth]));
	SQL_ReadResult(query, SQL_FieldNameToNum(query, "name"), szUserData[MemberName], charsmax(szUserData[MemberName]));
	SQL_ReadResult(query, SQL_FieldNameToNum(query, "nick"), szUserData[MemberNick], charsmax(szUserData[MemberNick]));
	SQL_ReadResult(query, SQL_FieldNameToNum(query, "last_activity"), szUserData[MemberLastActivity], charsmax(szUserData[MemberLastActivity]));
	SQL_ReadResult(query, SQL_FieldNameToNum(query, "group_name"), szUserData[MemberGroupName], charsmax(szUserData[MemberGroupName]));
	SQL_ReadResult(query, SQL_FieldNameToNum(query, "shilings"), szUserData[MemberMoney]);
	
	szUserData[MemberId] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "id"));
	szUserData[MemberGameTime] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "game_time"));
	
	szUserData[MemberGameTime] += szUserData[MemberGameTimeCurr];
	szUserData[MemberMoney] += szUserData[MemberMoneyCurr];
	
	#if defined AMXX_182
		szUserData[MemberMoneyOld] = _:szUserData[MemberMoney];
	#else
		szUserData[MemberMoneyOld] = szUserData[MemberMoney];
	#endif

	szUserData[MemberDiscount] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "proc"));
	szUserData[MemberThanks] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "thanks"));
	szUserData[MemberAnswers] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "answers"));
	szUserData[MemberRaiting] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "reit"));
	szUserData[MemberGroup] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "rights"));
	szUserData[MemberMessages] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "new_messages"));

	#if defined _gamecms_user_settings
		if(!g_bSettingsError)
		{
			new szUserSettings[MAX_CMS_SETTINGS_LENGTH];
			SQL_ReadResult(query, SQL_FieldNameToNum(query, "plugins_settings"), szUserSettings, charsmax(szUserSettings));
			if(szUserSettings[0])
				TrieSetString(g_trhUserSettings, g_szAuthIDs[id], szUserSettings);
		}
	#endif

	SQL_ReadResult(query, SQL_FieldNameToNum(query, "prefix"), szUserData[MemberPrefix], charsmax(szUserData[MemberPrefix]));
	mysql_escape_string(szUserData[MemberGroupName], charsmax(szUserData[MemberGroupName]));
	mysql_escape_string(szUserData[MemberPrefix], charsmax(szUserData[MemberPrefix]));
	
	if(g_UsePrefix)
	{
		if(strlen(szUserData[MemberPrefix]))
			ExecuteForward(g_iMFHandles[SendChatPrefix], g_iRetFwdIgnor, id, szUserData[MemberPrefix], 1);

		if(strlen(szUserData[MemberGroupName]))
			ExecuteForward(g_iMFHandles[SendChatPrefix], g_iRetFwdIgnor, id, szUserData[MemberGroupName], 2);
	}

	#if defined PL_GAMEMONEY
	if(!g_bGameMoneyError)
	{
		szUserData[MemberGameMoney] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "game_money"));
		szUserData[MemberGameMoney] += szUserData[MemberGameMoneyCurr];
		szUserData[MemberGameMoneyOld] = szUserData[MemberGameMoney];
	}
	#endif

	mysql_escape_string(szUserData[MemberLogin], charsmax(szUserData[MemberLogin])*2);
	mysql_escape_string(szUserData[MemberName], charsmax(szUserData[MemberName])*2);
	mysql_escape_string(szUserData[MemberNick], charsmax(szUserData[MemberNick])*2);

	TrieSetArray(g_trhPlayerInfo, g_szAuthIDs[id], szUserData, sizeof szUserData);

	#if defined _gamecms_groups_included
	if(bStatus == UPDATE)
		AuthorizeUser(id, .bSilentCheck = true);
	else
		FnCheckUserGroup(id);
	#endif

	static CurrentTime[MAX_INT_LEN], szDate[2][MAX_INT_LEN];
	get_time("%m-%d", CurrentTime, charsmax(CurrentTime)) ;

	strtok(szUserData[MemberBirth], szDate[0], charsmax(szDate[]), szDate[1], charsmax(szDate[]), '-');

	if(!strcmp(CurrentTime, szDate[1]) && strcmp("1900", szDate[0]))
		ExecuteForward(g_iMFHandles[BirthdayMember], g_iRetFwdIgnor, id, szUserData[MemberName], SiteUrl);

	ExecuteForward(g_iMFHandles[MemberConnected], g_iRetFwdIgnor, id, szUserData[MemberId], szUserData[MemberNick]);	
	
	if(TrieDeleteKey(g_trhDisconnUsers, g_szAuthIDs[id]))
	{
		new exist, str[MAX_STRING_LEN];
		for(exist = 0; exist < ArraySize(g_arhDisconnQuery); exist++)
		{
			ArrayGetString(g_arhDisconnQuery, exist, str, charsmax(str));
			if(!strcmp(str, g_szAuthIDs[id]))
				ArrayDeleteItem(g_arhDisconnQuery, exist);
		}
	}
	
	return PLUGIN_HANDLED;
}

public UpdateMemberData(id, money, bool:send)
{	
	if(~g_bitAPIstatus & UseGameCms)
		return PLUGIN_HANDLED;

	new szUserData[userDataID];
	TrieGetArray(g_trhPlayerInfo, g_szAuthIDs[id], szUserData, sizeof(szUserData));

	if(money)
	{
		new Float:flMoney = float(money);
		szUserData[MemberMoney] += flMoney;
		szUserData[MemberMoneyCurr] += flMoney;
		szUserData[MemberMoneyOld] += flMoney;

		if(!send)
		{
			TrieSetArray(g_trhPlayerInfo, g_szAuthIDs[id], szUserData, sizeof(szUserData));
			return PLUGIN_CONTINUE;
		}
	}
	else
	{
		szUserData[MemberMoneyCurr] += (szUserData[MemberMoney]- szUserData[MemberMoneyOld]);
	}

	#define szSaveQuery g_szQueryStr
	new iLen;
	iLen += formatex(szSaveQuery[iLen], charsmax(szSaveQuery) - iLen, "UPDATE users SET game_time=game_time+'%d'", szUserData[MemberGameTimeCurr] += get_user_time(id));
	
	if(szUserData[MemberMoneyCurr] != 0.00)
		iLen += formatex(szSaveQuery[iLen], charsmax(szSaveQuery) - iLen, ", shilings = shilings+'%.2f'", szUserData[MemberMoneyCurr]);
	
	#if defined PL_GAMEMONEY
		if(!g_bGameMoneyError)
		{
			szUserData[MemberGameMoneyCurr] += (szUserData[MemberGameMoney]- szUserData[MemberGameMoneyOld]);
			if(szUserData[MemberGameMoneyCurr] != 0)
				iLen += formatex(szSaveQuery[iLen], charsmax(szSaveQuery) - iLen, ", game_money=game_money+'%d'", szUserData[MemberGameMoneyCurr]);
		}
	#endif
	
	#if defined _gamecms_user_settings
		if(!g_bSettingsError)
		{
			new szUserSettings[MAX_CMS_SETTINGS_LENGTH];
			if(TrieGetString(g_trhUserSettings, g_szAuthIDs[id], szUserSettings, charsmax(szUserSettings)))
			{
				if(szUserSettings[0] != EOS)
					iLen += formatex(szSaveQuery[iLen], charsmax(szSaveQuery) - iLen, ", plugins_settings = '%s'", szUserSettings);
			}
		}
	#endif

	iLen += formatex(szSaveQuery[iLen], charsmax(szSaveQuery) - iLen, " WHERE id = '%d';", szUserData[MemberId]);	

	if(!send)
	{
		new push = ArrayPushString(g_arhDisconnQuery, g_szAuthIDs[id]);
		TrieSetString(g_trhDisconnUsers, g_szAuthIDs[id], szSaveQuery);

		if(get_pcvar_num(cpCvarsData[Debug]) > 3)
			log_amx("Add on Disconnect- %s / %s / push index- %d", g_szAuthIDs[id], szSaveQuery, push);
			
		TrieSetArray(g_trhPlayerInfo, g_szAuthIDs[id], szUserData, sizeof szUserData);

		return PLUGIN_CONTINUE;
	}

	if(get_pcvar_num(cpCvarsData[Debug]) > 3)
		log_amx("Update on Reload User- %s / %d / %s", g_szAuthIDs[id], id, szSaveQuery);

	new szData[2];
	szData[0] = id;
	szData[1] = UPDATE;

	szUserData[MemberMoneyOld] = _:szUserData[MemberMoney];
	szUserData[MemberMoneyCurr] = _:0.0;
	
	TrieSetArray(g_trhPlayerInfo, g_szAuthIDs[id], szUserData, sizeof szUserData);

	SQL_ThreadQuery(g_hDbTuple, "ForumData_Handler", szSaveQuery, szData, sizeof(szData));
	
	return PLUGIN_HANDLED;
}

//--------------  не Форум )  ------------------------------//

public plugin_end() 
{
	//QuerySaveUsersData();
	
	if(g_hDbTuple != Empty_Handle) 
		SQL_FreeHandle(g_hDbTuple);
	
	ArrayDestroy(g_arhDisconnQuery);
	
	#if defined USE_ADMINS
		ArrayDestroy(g_arhAllAdminsInfo);
		ArrayDestroy(g_arhAllPurchServices);
		ArrayDestroy(g_arrUserServices);
	#endif
	
	#if defined _gamecms_groups_included
		if(g_GroupInfo)
			ArrayDestroy(g_GroupInfo);
	#endif
}

FnCheckOnlinePlayers(type)
{
	//if(!get_playersnum())
	//	return;
	
	if(type == 1)
	{
		for(new i = 1; i <= MaxClients; i++)
			if(is_user_connected(i))
				FnLoadMemberData(i);
	}
	#if defined USE_ADMINS
	else
	{
		for(new i = 1; i <= MaxClients; i++)
			if(is_user_connected(i) || is_user_connecting(i))
				AuthorizeUser(i, .bSilentCheck = true);
	}
	#endif
}

stock QuerySaveUsersData()
{
	#define QueryString g_szQueryStr
	QueryString[0]= EOS;
	
	new tmpQueryString[MAX_QUERY_SMALL_LEN], lenF, iLen;
	new arrsize = ArraySize(g_arhDisconnQuery), str[MAX_STRING_LEN];
	new szData[2];
	szData[1] = SAVE;
	
	for(new i; i < arrsize; i++)
	{
		ArrayGetString(g_arhDisconnQuery, i, str, sizeof(str));
		#if defined AMXX_182
			TrieGetString(g_trhDisconnUsers, str, tmpQueryString, sizeof(tmpQueryString));
			iLen = strlen(tmpQueryString);
		#else
			TrieGetString(g_trhDisconnUsers, str, tmpQueryString, sizeof(tmpQueryString), iLen);
		#endif
	
		if((lenF + iLen)  > sizeof(QueryString) - 10)
		{
			SQL_ThreadQuery(g_hDbTuple, "ForumData_Handler", QueryString, szData, sizeof(szData));
			QueryString[0]= EOS;
			lenF = 0;
		}
		
		lenF +=formatex(QueryString[lenF], charsmax(QueryString)- lenF, "%s", tmpQueryString);
	}

	#if defined PL_GAMEMONEY
	if(g_bGameMoneyError)
		lenF +=formatex(QueryString[lenF], charsmax(QueryString)- lenF, "ALTER TABLE `users` ADD `game_money` INT( 11 ) NOT NULL DEFAULT '0'");
	#endif
	
	#if defined _gamecms_user_settings
	if(g_bSettingsError)
		lenF +=formatex(QueryString[lenF], charsmax(QueryString)- lenF, "ALTER TABLE `users` ADD `plugins_settings` VARCHAR( 1024 ) NOT NULL");
	#endif

	if(lenF)
		SQL_ThreadQuery(g_hDbTuple, "ForumData_Handler", QueryString, szData, sizeof(szData));
}

//--------------------------------------------------//
		
SQL_ErrorAPI(const szError[], iError, failstate)
{

	if(failstate == TQUERY_CONNECT_FAILED)
	{
		log_amx("[Error] Connection szError: %s (%d)", szError, iError);
		return PLUGIN_HANDLED;
	}

	if(failstate == TQUERY_QUERY_FAILED)
	{
		if(iError != DUPLICATE_ENTRY)
			log_amx("[Error] Query szError: %s (%d)", szError, iError);

		#if defined PL_GAMEMONEY	
		if(containi(szError, "game_money") != -1)
			g_bGameMoneyError = true;
		#endif
		
		#if defined _gamecms_user_settings
		if(containi(szError, "plugins_settings") != -1)
			g_bSettingsError = true;
		#endif
	}
	
	return PLUGIN_CONTINUE;
}

FnMForwardsCreate() 
{
	g_iMFHandles[PluginLoaded] = CreateMultiForward("OnAPIPluginLoaded", ET_IGNORE, FP_CELL, FP_STRING, FP_CELL);
	//Подключение именинника (для плагина GameCMS Birthday)
	g_iMFHandles[BirthdayMember] = CreateMultiForward("OnAPIBirthdayMember", ET_IGNORE, FP_CELL, FP_STRING, FP_STRING);
	//Подключение зарегистрированного пользователя
	g_iMFHandles[MemberConnected] = CreateMultiForward("OnAPIMemberConnected", ET_IGNORE, FP_CELL, FP_CELL, FP_STRING);
	//ошибка загрузки API
	g_iMFHandles[PluginStop] = CreateMultiForward("OnAPIPluginStop", ET_IGNORE);
	#if defined USE_ADMINS
		//выполняется после успешной загрузки админов
		g_iMFHandles[AdminsLoaded] = CreateMultiForward("OnAPIAdminsLoaded", ET_IGNORE);
		//админ зашел на сервер
		g_iMFHandles[AdminConnected]=CreateMultiForward("OnAPIAdminConnected", ET_IGNORE, FP_CELL, FP_STRING, FP_CELL, FP_CELL);
		//админ вышел с сервера
		g_iMFHandles[Admin_Disconnect]=CreateMultiForward("OnAPIAdminDisconnected", ET_IGNORE, FP_CELL);
	#endif
	
	if(FindPluginFunction("OnAPISendChatPrefix"))
	{
		//установка префикса в чат. требуется доработка плагина чата
		g_iMFHandles[SendChatPrefix] = CreateMultiForward("OnAPISendChatPrefix", ET_IGNORE, FP_CELL, FP_STRING, FP_CELL);
		g_UsePrefix = true;
	}
}

#if defined USE_ADMINS
	#if defined AMXX_182
		public MapsConfigsLoad(szConfigsDir[])
		{
			new curMap[MAX_STRING_LEN], mapPrefix[MAX_STRING_LEN], mapName[MAX_STRING_LEN], configsFile[MAX_STRING_LEN * 2];
			
			get_mapname(curMap, charsmax(curMap));
			formatex(configsFile, charsmax(configsFile), "%s/maps/%s.cfg", szConfigsDir, curMap);
			if (!file_exists(configsFile))
			{
				strtok(curMap, mapPrefix, charsmax(mapPrefix), mapName, charsmax(mapName), '_');
				formatex(configsFile, charsmax(configsFile), "%s/maps/prefix_%s.cfg", szConfigsDir, mapPrefix);
			}

			if(file_exists(configsFile))
				server_cmd("exec %s", configsFile);
		}
	#endif

FnSetUserAuthorized(id, szName[] = "", szAuthId[] = "", bSilentCheck)
{
	new flags;
	if(TrieGetArray(g_trhOnlineAdminsInfo, get_id_key(id), g_Data, sizeof g_Data))
	{
		client_cmd(id, "echo ^"*** Права доступа к услугам предоставлены! ***^"");
		
		if((g_bitDefAccess & ADMIN_USER) != g_bitDefAccess)
			StAddExtraFlags(g_szDefAccess, g_Data[AdminServiceFlags], charsmax(g_Data[AdminServiceFlags]));
			
		flags = read_flags(g_Data[AdminServiceFlags]);

		log_amx("Авторизация... ^"<%s><%s>^" (аккаунт ^"%s^" / id= %d) (флаги ^"%s^") (услуга ^"%s^")", 
			szName, szAuthId, g_Data[AdminAuthId], g_Data[AdminId], g_Data[AdminServiceFlags], g_Data[AdminServiceName]);
	}
	else
	{
		flags = g_bitDefAccess;
		if(get_pcvar_num(cpAmxMode) == 2)
		{
			engclient_cmd(id, "echo ^"* Доступ только Авторизованным!^"");
			server_cmd("kick #%d Доступ разрешен только Авторизованным игрокам!", get_user_userid(id));
			
			return PLUGIN_HANDLED;
		}
	}

	if((g_bitDefAccess & ADMIN_USER) != g_bitDefAccess)
		remove_user_flags(id, ADMIN_USER);

	set_user_flags(id, flags);

	if(!bSilentCheck)
		ExecuteForward(g_iMFHandles[AdminConnected], g_iRetFwdIgnor, id, szName, g_Data[AdminId], flags);

	return PLUGIN_CONTINUE;
}

FnDeleteLoadedServices(iAdminId)
{
	if(!iAdminId)
		TrieClear(g_trhOnlineAdminsInfo);

	FnDeleteLoadedServicesIter(g_arhAllPurchServices, iAdminId);
	FnDeleteLoadedServicesIter(g_arhAllAdminsInfo, iAdminId);
	
	g_iPurchasedCount = ArraySize(g_arhAllPurchServices);
	g_iAdminCount = ArraySize(g_arhAllAdminsInfo);
}

FnDeleteLoadedServicesIter(Array:arhSource, iAdminId)
{
	new index = ArraySize(arhSource);
	if(index)
	{
		do
		{
			#if defined AMXX_182
			ArrayGetArray(arhSource, --index, g_Data);
			#else
			ArrayGetArray(arhSource, --index, g_Data, sizeof(g_Data));
			#endif
			if((iAdminId && g_Data[AdminId] == iAdminId) || (!iAdminId && g_Data[AdminId] > 0) || g_Data[AdminActive] == 0)
			{
				ArrayDeleteItem(arhSource, index);
				
				if(task_exists(g_Data[AdminService]))
					remove_task(g_Data[AdminService]);
			}
		}
		while(index > 0)
	}
}
#endif
