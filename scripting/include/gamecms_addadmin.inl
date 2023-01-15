// ￿
//https://dev-cs.ru/threads/222/page-2#post-30354

#define ADD_ADMIN

FnAddAdminConcmd()
	register_concmd("amx_addadmin", "addadminfn", ADMIN_RCON, "<playername|auth>  [password] <accessflags> [authtype] [days] - add specified player as an admin to Database")
	

/*======== Обработка консольной команды =============*/
//amx_addadmin "счастливое лицо" "пароль" "флаги" "тип авторизации" "время в минутах"
//amx_addadmin "STEAM_0:0:123456" "" "abcd" "ce" "300"
//amx_addadmin "Moi Nick" "paroliwe" "abipr" "ab" "300"
public addadminfn(id, level, cid)
{
	#if !defined USE_ADMINS_FROM_SITE
	server_print("[API-AddAdmin] Использование админов отключено! Добавление админов невозможно!");
	return PLUGIN_HANDLED;
	#endif

	if (ghDbTuple == Empty_Handle)
	{
		server_print("[API-AddAdmin] Нет связи с базой данных. Добавление админов невозможно!");
		return PLUGIN_CONTINUE;
	}
	
	if (!cmd_access(id, level, cid, 3))
		return PLUGIN_HANDLED;

	new AdmInfo[AdminInfo];
	new argsnum = read_argc();
	
	read_argv(1, AdmInfo[AdminAuthId], charsmax(AdmInfo[AdminAuthId]));
	read_argv(2, AdmInfo[AdminPassword], charsmax(AdmInfo[AdminPassword]));
	read_argv(3, AdmInfo[AdminServiceFlags], charsmax(AdmInfo[AdminServiceFlags]));
	if(argsnum > 4)
		read_argv(4, AdmInfo[AdminType], charsmax(AdmInfo[AdminType]));
	if(argsnum > 5)
	{
		new szTime[11];
		read_argv(5, szTime, charsmax(szTime));
		AdmInfo[AdminServiceTime] = str_to_num(szTime);
	}
	//AdmInfo[AdminServiceTime] = read_argv_int(5);	//мммм а чета не работает.. видимо, инклуд АМХ староват

	FnCheckParams(id, AdmInfo);

	return PLUGIN_HANDLED;
}

/** Добаление аккаунтов в базу данных
*	@iClient - индекс игрока
*	@szAuthType[] - тип авторизации (смотри amxconst.inc: Admin authentication behavior flags)
*	@szFlags[] - флаги (уровни) доступа (смотри amxconst.inc: Admin level constants)
*	@iTime - время в минутах, 0- навсегда (если время не указано, значит 0)
*	@szPasswd[] - пароль доступа (если нужен)
*	@iServiceId - номер услуги на сайте
*		//При указании параметра, флаги услуги будут определены автоматически
*		//При отсутствии, номер услуги будет определен по флагам
*	@force_write - проверка введенных данных (true- включить). При отключеной функции все косяки при добавлении- ваши косяки)
*	cmsapi_add_account(id, "a", 180, "parol", "prt", 0, false)
*	(игроку №id с его ником выданы флаги "prt" на 180 минут, пароль- "parol") кикнет его нафиг после добавления в базу)
*/
//native cmsapi_add_account(iClient, szAuthType[], iTime = 0,  szPasswd[] = "", szFlags[] = "", iServiceId = 0, force_write = false)
//
public native_cmsapi_add_account(nId, params)
{
	if (ghDbTuple == Empty_Handle)
	{
		server_print("[API-AddAdmin] Нет связи с базой данных. Добавление админов невозможно!");
		return;
	}

	new AdmInfo[AdminInfo], id;
	
	id = get_param(1);
	get_string(2, AdmInfo[AdminType], charsmax(AdmInfo[AdminType]));
	if(containi(AdmInfo[AdminType], "c") != -1)
		AdmInfo[AdminAuthId] = g_szAuthIDs[id];
	else
		get_user_name(id, AdmInfo[AdminAuthId], charsmax(AdmInfo[AdminAuthId]));
	
	AdmInfo[AdminServiceTime] = get_param(3);
	get_string(4, AdmInfo[AdminPassword], charsmax(AdmInfo[AdminPassword]));
	get_string(5, AdmInfo[AdminServiceFlags], charsmax(AdmInfo[AdminServiceFlags]));
	AdmInfo[AdminServiceId] = get_param(6);		

	if(!bool:get_param(7))
	{
		FnAddAccount(id, AdmInfo);
		return;
	}

	FnCheckParams(id, AdmInfo);
}

/*======== Обработка параметров добавления =============*/
FnCheckParams(id, AdmInfo[])
{
	switch(AdmInfo[AdminType][0])
	{
		case 'a':
		{
			if(!FnCheckPass(AdmInfo[AdminPassword]))
				return PLUGIN_CONTINUE;
			
			if(AdmInfo[AdminType][1] == 'c')
			{
				if(!FnCheckSteam(AdmInfo[AdminAuthId]))
					return PLUGIN_CONTINUE;
				else
					server_print("[API-AddAdmin] Установлена авторизация по STEAM ID + пароль");
			}
			else
				server_print("[API-AddAdmin] Установлена авторизация по Ник + пароль");
		}
		case 'c':
		{
			if(!FnCheckSteam(AdmInfo[AdminAuthId]))
				return PLUGIN_CONTINUE;
			
			if(AdmInfo[AdminType][1] == 'e')
				server_print("[API-AddAdmin] Установлена авторизация по STEAM ID");
			else
			{
				if(!FnCheckPass(AdmInfo[AdminPassword]))
					return PLUGIN_CONTINUE;
				server_print("[API-AddAdmin] Установлена авторизация по STEAM ID + пароль");
			}
		}
		default:
		{
			server_print("[API-AddAdmin] Не верный тип авторизации");
			return PLUGIN_CONTINUE;
		}
	}

	FnAddAccount(id, AdmInfo);
	
	return PLUGIN_CONTINUE;
}

FnCheckPass(pass[])
{
	if(!(pass[0]))
	{
		server_print("[API-AddAdmin] Необходимо указать пароль");
		return PLUGIN_CONTINUE;
	}
	
	return PLUGIN_HANDLED;
}

FnCheckSteam(steam[])
{
	if(containi(steam, "STEAM_") == -1)
	{
		server_print("[API-AddAdmin] Неверный формат STEAM ID (Пример: STEAM_0:0:0000000)");
		return PLUGIN_CONTINUE;
	}
	
	return PLUGIN_HANDLED;
}

/*======== Добавление админов в БД =============*/
FnAddAccount(id, AdmInfo[])
{
	new b_Time[30], e_Time[30];
	format_time(b_Time, charsmax(b_Time), "%Y-%m-%d %H:%M:%S");

	if(AdmInfo[AdminServiceTime])
		format_time(e_Time, charsmax(e_Time), "%Y-%m-%d %H:%M:%S", (get_systime() + 60*AdmInfo[AdminServiceTime]));
	else
		e_Time = "0000-00-00 00:00:00";
	
	mysql_escape_string(AdmInfo[AdminAuthId], charsmax(AdmInfo[AdminAuthId])*2);
	mysql_escape_string(AdmInfo[AdminPassword], charsmax(AdmInfo[AdminPassword])*2);

	#define szQuery g_szQueryStr
	new len;
	len += formatex(szQuery[len], charsmax(szQuery) - len,
			"INSERT IGNORE INTO admins (name, type, pass, server, user_id) values ('%s', '%s', '%s', '%d', '%d');",
			AdmInfo[AdminAuthId], AdmInfo[AdminType], AdmInfo[AdminPassword], giServerId, cmsapi_is_user_member(id));
			
	if(AdmInfo[AdminServiceId])
		len += formatex(szQuery[len], charsmax(szQuery) - len,
			"INSERT INTO admins_services (rights_und, service_time, bought_date, ending_date, admin_id, service) values \
			((SELECT `rights` FROM `services` WHERE `services`.`id` = '%d' AND `server` = '%d'), '%d', '%s', '%s', LAST_INSERT_ID(), '%d');",
			AdmInfo[AdminServiceId], giServerId, AdmInfo[AdminServiceTime], b_Time, e_Time, AdmInfo[AdminServiceId]);
	else
		len += formatex(szQuery[len], charsmax(szQuery) - len,
			"INSERT INTO admins_services (rights_und, service_time, bought_date, ending_date, admin_id, service) values \
			('%s', '%d', '%s', '%s', LAST_INSERT_ID(), (SELECT `id` FROM `services` WHERE `services`.`rights` = '%s' AND `server` = '%d'));",
			AdmInfo[AdminServiceFlags], AdmInfo[AdminServiceTime], b_Time, e_Time, AdmInfo[AdminServiceFlags], giServerId);
				
	if(get_pcvar_num(g_pCvars[Debug]) > 2)
		log_amx(szQuery);
	
	new szId[2];
	szId[0] = id;
	SQL_ThreadQuery(ghDbTuple, "FnAddAccount_Post", szQuery, szId, sizeof szId);
}

public FnAddAccount_Post(failstate, Handle:query, error[], errornum, data[], datasize)
{
	if(SQL_ErrorAPI(error, errornum, failstate))
		return PLUGIN_CONTINUE;

	new adminId = SQL_GetInsertId(query);
	server_print("[API-AddAdmin] Админ №%d успешно добавлен", adminId);
	
	if(adminId)
	{
		server_cmd("amx_reloadadmins %d", adminId);
		return PLUGIN_HANDLED;
	}
		
	return PLUGIN_CONTINUE;
}
