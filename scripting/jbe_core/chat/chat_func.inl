// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8


/*===== -> Чат -> =====*///{

chat_init()
{
	// chat init 
	register_clcmdex("rus", "LangCtrlRus", 1); // say /rus
	register_clcmdex("eng", "LangCtrlEng", 1); // say /eng

	// вниз чтоб не перебивал команды с '!' и '.'
	register_clcmd("say", "HandlerSay"); // say /say
	register_clcmd("say_team", "HandlerSayTeam"); // say /say_team

	set_task_ex(1.0, "fnReadChatPrefixes", _, _, _, SetTask_AfterMapStart);
}

#if defined LT_PREFIXES
public fnReadChatPrefixes()
{
	new szCfgDir[64], szCfgFile[128];
	get_localinfo("amxx_configsdir", szCfgDir, charsmax(szCfgDir));
	formatex(szCfgFile, charsmax(szCfgFile), "%s/jb_engine/chat_prefixes.ini", szCfgDir);
	
	new fp = fopen(szCfgFile, "rt");
	if (!fp)
	{
		log_amx("File ^"%s/jb_engine/chat_prefixes.ini^" not found", szCfgDir);
		return PLUGIN_HANDLED;
	}

	g_aDataChat = ArrayCreate(DATA_PREFIXES);
	new aDataChatPrefix[DATA_PREFIXES], szBuffer[256];

	while (!feof(fp))
	{
		fgets(fp, szBuffer, charsmax(szBuffer));
		if (szBuffer[0] && szBuffer[0] != ';' &&
		parse(szBuffer,
		aDataChatPrefix[USER_FLAGS], charsmax(aDataChatPrefix[USER_FLAGS]),
		aDataChatPrefix[PREFIX], charsmax(aDataChatPrefix[PREFIX])))
			ArrayPushArray(g_aDataChat, aDataChatPrefix);
	}

	g_aDataChatSize = ArraySize(g_aDataChat);
	return fclose(fp);
}
#endif

public LangCtrlRus(id)
{
	if (IsSetBit(g_iBitUserRusChat, id))
		client_print_color(id, id, "^4*** %s ^1Русский язык УЖЕ включен! ^4***", g_szChatPrefix);
	else
	{
		client_print_color(id, id, "^4*** %s ^1Русский язык активирован! ^4***", g_szChatPrefix);
		SetBit(g_iBitUserRusChat, id);
		client_cmd(id, "spk buttons/blip1.wav");
	}
	return PLUGIN_HANDLED;
}

public LangCtrlEng(id)
{
	if (IsNotSetBit(g_iBitUserRusChat, id))
		client_print_color(id, id, "^4*** %s ^1Английский язык УЖЕ включен! ^4***", g_szChatPrefix);
	else
	{
		client_print_color(id, id, "^4*** %s ^1Английский язык активирован! ^4***", g_szChatPrefix);
		ClearBit(g_iBitUserRusChat, id);
		client_cmd(id, "spk buttons/blip1.wav");
	}
	return PLUGIN_HANDLED;
}

public HandlerSay(id) return FormatMsg(id, false);
public HandlerSayTeam(id) return FormatMsg(id, true);

FormatMsg(id, bool:IsTeam)
{
	if (IsNotSetBit(g_iBitUserConnected, id))
		return PLUGIN_HANDLED;
		
	// afk control
	afk_user_clear_warns(id);
	
#define MAX_BYTES 180 // 191
	static sMessage[MAX_CHAT_LENGTH];
	static sTags[MAX_CHAT_LENGTH];
	static sConvertStr[charsmax(sMessage) * 2 + 1];
	static iLen;
	
	read_args(sMessage, charsmax(sMessage));
	remove_quotes(sMessage);
	trim(sMessage);
		
	if (sMessage[0] == EOS)// || sMessage[0] == '/')
		return PLUGIN_HANDLED; //PLUGIN_HANDLED_MAIN;

	if (is_user_cmd_flood(id, FLOOD_CHAT, 0.75, 3.75))
		return PLUGIN_HANDLED_MAIN;

	if (sMessage[0] == '@' && IsSetBit(g_iBitUserAdmin, id) && !IsTeam)
	{
		admin_say_hud(id, sMessage, charsmax(sMessage));
		return PLUGIN_HANDLED;
	}

	// private message
	new iTarget;

	if (
		(sMessage[0] == '@' && IsTeam)
	|| (
		(sMessage[1] == 'p' && sMessage[2] == 'm' && !IsTeam)
	&& 
		(sMessage[0] == '/' || sMessage[0] == '.' || sMessage[0] == '!')
		)
	)
	{
		if (g_iGlobalBlock[GB_PM])
		{
			client_print_color(id, print_team_red, "%s ^1Приватные сообщения ^3заблокированы %s^1.", g_szChatPrefix, global_get_name_block(GB_PM));
			return PLUGIN_HANDLED;
		}
		
		sTags[0] = EOS, iLen = 0;
		switch (IsTeam)
		{
			case true:
			{
				replace(sMessage, charsmax(sMessage), "@", "");
				replace(sMessage, charsmax(sMessage), "^"", "");
				
				if (sMessage[0] == EOS)
				{
					client_print_color(id, id, "[ADMIN PM] Введите сообщение, формат: '@ Сообщение'.");
					return PLUGIN_HANDLED;
				}
				
				iTarget = true;
			}
			case false:
			{
				new szPars[2], szTarget[32];
				replace(sMessage, charsmax(sMessage), "/pm", "");
				replace(sMessage, charsmax(sMessage), ".pm", "");
				replace(sMessage, charsmax(sMessage), "!pm", "");

				parse(sMessage,
					szTarget,	charsmax(szTarget),	// кому мессага
					szPars,		charsmax(szPars));	// первый символ месаги
				
				if (szPars[0] == EOS)
				{
					client_print_color(id, id, "[PM] Введите сообщение, формат: '/pm Player_Name Сообщение'.");
					return PLUGIN_HANDLED;
				}
				
				iTarget = pm_target(id, szTarget); // если ввести короткое и будет подобное то выбъет ошибку
				if (!iTarget) return PLUGIN_HANDLED;

				// убрать же ?
				if (IsSetBit(g_iBitUserOffPM, iTarget))
				{
					client_print_color(id, id, "[PM] Игрок отключил приватные сообщения.");
					return PLUGIN_HANDLED;
				}

				g_PM_CurrentTarget[id] = iTarget;

				if (check_not_pm_access(id))
					return PLUGIN_HANDLED;
				
				replace(sMessage, charsmax(sMessage), szTarget, "");
				replace_all(sMessage, charsmax(sMessage), "^"", "");
			}
		}
	}

	if (sMessage[0] == '/' && !iTarget)
		return PLUGIN_HANDLED_MAIN;

	if (!iTarget) // если сообщение не приватное
	{
		// Global block chat all	
		if (g_iGlobalBlock[GB_CHAT_ALL])
		{
			client_print_color(id, print_team_red, "%s ^1Всем игрокам ^4чат ^3заблокирован %s^1.",
				g_szChatPrefix, global_get_name_block(GB_CHAT_ALL));
			
			return PLUGIN_HANDLED;
		}
	
		switch (g_iUserTeam[id])
		{
			case TEAM_PRISONER:
				iLen = formatex(sTags, charsmax(sTags), "%s%s",
					IsSetBit(g_iBitUserAlive, id) ? "^3" : "^1*^3Труп^1* ", IsTeam ? "^1(^3Заключенный^1) " : "");
			
			case TEAM_GUARD:
				iLen = formatex(sTags, charsmax(sTags), "%s%s",
					IsSetBit(g_iBitUserAlive, id) ? "^3" : "^1*^3Труп^1* ", IsTeam ? "^1(^3Охранник^1) " : "");
			
			default: iLen = formatex(sTags, charsmax(sTags), "^1*^3Spec^1* ");
		}
		
		if (g_szPrefix[id][0])
			iLen += formatex(sTags[iLen], charsmax(sTags) - iLen, "%s ", g_szPrefix[id]);
		iLen += formatex(sTags[iLen], charsmax(sTags) - iLen, "^3%n %s", id, IsSetBit(g_iBitUserAdmin, id) ? "^4:  ^1" : "^1:  ");
	}
	
	if (IsSetBit(g_iBitUserRusChat, id))
	{
		new nLen;
		for (new n; sMessage[n] != EOS; n++)
		{
			if ('!' <= sMessage[n] <= '~')
			{
				sConvertStr[nLen++] = g_chConvert{(sMessage[n] - '!') * 2};
				if (g_chConvert{(sMessage[n] - '!') * 2 + 1} != EOS)
				{
					sConvertStr[nLen++] = g_chConvert{(sMessage[n] - '!') * 2 + 1};
				}
			}
			else sConvertStr[nLen++] = sMessage[n];
		}
		sConvertStr[nLen] = EOS;

	}
	else sConvertStr = sMessage;

	new iByteLimit = MAX_BYTES;	
	
	while (iLen + strlen(sConvertStr) > MAX_BYTES)
		sConvertStr[iByteLimit -= 10] = 0;
	
	iLen += formatex(sTags[iLen], charsmax(sTags) - iLen, sConvertStr);

	if (iTarget)
	{
		switch (IsTeam)
		{
			case true:
			{
				new iAdmNum;
				
				formatex(sMessage, charsmax(sMessage), "^1[ADMIN PM] ^3%n ^1> : %s", id, sTags);
				
				for (new i = 1; i <= MaxClients; i++)
				{
					if (IsNotSetBit(g_iBitUserAdmin, i) || i == id)
						continue;
					
					client_print_color(i, id, sMessage);
					iAdmNum++;
				}
				
				formatex(sMessage, charsmax(sMessage), "^1[ADMIN PM | %d Admins] > ^3%n ^1 : %s", iAdmNum, id, sTags);
				
				client_print_color(id, id, sMessage);
			}
			case false:
			{
				if (g_iGlobalBlock[GB_CHAT_DEAD] && IsNotSetBit(g_iBitUserAlive, id) && IsSetBit(g_iBitUserAlive, iTarget))
				{
					client_print_color(id, id, "%s Чат мертвых заблокирован %s^1.", g_szChatPrefix, global_get_name_block(GB_CHAT_DEAD));
					client_print_color(id, id, "%s Вы не можете написать живому игроку.", g_szChatPrefix);
				}
				else
				{
					if (IsAdvertsString(sMessage))
					{
						client_print_color(id, id, "%s Реклама запрещена!", g_szChatPrefix);
						return PLUGIN_HANDLED;
					}

					formatex(sMessage, charsmax(sMessage), "^1[PM] ^3%n ^1> : %s", id, sTags);
					client_print_color(iTarget, id, sMessage);
					
					formatex(sMessage, charsmax(sMessage), "^1[PM] > ^3%n ^1: %s", iTarget, sTags);
					client_print_color(id, iTarget, sMessage);
					
					client_print(iTarget, print_console, sMessage); // дублируем в консоль получателю

				// звук входящего PM
					client_cmd(iTarget, "spk weapons/mine_activate.wav"); // spk buttons/button9.wav					
				}
			}
		}
		return PLUGIN_HANDLED;
	}

	if (IsAdvertsString(sMessage))
	{
		client_print_color(id, id, "%s Реклама запрещена!", g_szChatPrefix);
		return PLUGIN_HANDLED;
	}

	switch (IsTeam)
	{
		case true:
		{
			if (g_iGlobalBlock[GB_CHAT_DEAD] && IsNotSetBit(g_iBitUserAlive, id)) 
			{
				for (new i = 1; i <= MaxClients; i++)
					if (IsNotSetBit(g_iBitUserAlive, i) && g_iUserTeam[id] == g_iUserTeam[i])
						client_print_color(i, id, sTags);
			} 
			else 
			{
				for (new i = 1; i <= MaxClients; i++)
					if (g_iUserTeam[id] == g_iUserTeam[i])
						client_print_color(i, id, sTags);
			}
		}
		case false: 
		{
			if (g_iGlobalBlock[GB_CHAT_DEAD] && IsNotSetBit(g_iBitUserAlive, id)) 
			{
				for (new i = 1; i <= MaxClients; i++)
					if (IsSetBit(g_iBitUserConnected, i) && IsNotSetBit(g_iBitUserAlive, i))
						client_print_color(i, id, sTags);
			}
			else 
			{
				for (new i = 1; i <= MaxClients; i++)
					if (IsSetBit(g_iBitUserConnected, i))
						client_print_color(i, id, sTags);
			}
		}
	}
	return PLUGIN_HANDLED;
}


#if !defined DEVELOPER_OPTIONS

public OnAPISendChatPrefix(id, prefix[], type)
{
	if (type != 1)
		return PLUGIN_HANDLED;

	new AdminReason[33];
	if (!cmsapi_is_admin_active(id, AdminReason, charsmax(AdminReason), _))
		return PLUGIN_HANDLED;

	if (cmsapi_get_user_services(id, "", "_prefix", 0))
	{
		if (prefix[0] && prefix[0] != '0')
			return formatex(g_szPrefix[id], charsmax(g_szPrefix[]), "^1[^4%s^1]", prefix);
		else
			return formatex(g_szPrefix[id], charsmax(g_szPrefix[]), "^1[^4 CS-One ^1]", prefix);
	}
	return PLUGIN_CONTINUE; //	return g_szPrefix[id][0] = EOS;
}
#endif

jbe_get_user_prefix_flag(id)
{
	new iFlags = get_user_flags(id);
	new aDataChatPrefix[DATA_PREFIXES];
	for (new i = 0; i < g_aDataChatSize; i++)
	{
		ArrayGetArray(g_aDataChat, i, aDataChatPrefix);
		if (iFlags & read_flags(aDataChatPrefix[USER_FLAGS]))
			return formatex(g_szPrefix[id], charsmax(g_szPrefix[]), "^1[^4%s^1]", aDataChatPrefix[PREFIX]);
	}
	return g_szPrefix[id][0] = EOS;
}

// проверка строки на наличие запрещенных ключей
bool:IsAdvertsString(szString[])
{
	for (new i; i < sizeof g_szWhileKeysChat; i++)
	{
		if (containi(szString, g_szWhileKeysChat[i]) != -1)
			return false;
	}

	for (new i; i < sizeof g_szBadKeysChat; i++)
	{
		if (containi(szString, g_szBadKeysChat[i]) != -1)
			return true;
	}
	
	return false;	
}
/*===== <- Чат <- =====*///{