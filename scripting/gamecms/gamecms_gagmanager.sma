/*
	
	
	todo
		https://cs-games.club/index.php?threads/gamecms-gagmanager.8/page-10#post-3495
	
	
	нативы для снятия блока
	нативы для временных блоков через trie

	14.05.19
		* изменение натива cmsgag_add_user_block
			Добавлен параметр *	@printChat	Оповестить в чат об операции, false- нет, true- стандартное оповещение о блокировке/разблокировке
		* соответствующие изменения в коде для правильной работы данного натива

*/

#include <amxmodx>
#include <amxmisc>
#include <sqlx>
#include <gamecms5>
#include <time>

new const PLUGIN[] = "GameCMS_GagManager";
new const VERSION[] = "2.2.3";
new const AUTHOR[] = "zhorzh78";

#define REAPI

#if defined REAPI
	#include <reapi>
	#define VTC_OnClientStartSpeak(iSender)	VTC_OnClientStartSpeak(const iSender)
	#define VTC_OnClientStopSpeak(iSender)	VTC_OnClientStopSpeak(const iSender)
	#define FMRES_IGNORED	HC_CONTINUE
	#define FMRES_SUPERCEDE	HC_SUPERCEDE
#else
	#include <VtcApi>
	#include <hamsandwich>
	#include <fakemeta>	
	#include <engine>
	new const g_szSpamEntClassname[] = "SpamEntClassname";
#endif

#define	GetBit(%1,%2)			((%1) & (1 << (%2 & 31)))
#define	SetBit(%1,%2)			((%1) |= (1 << (%2 & 31)))
#define	ResetBit(%1,%2)			((%1) &= ~(1 << (%2 & 31)))
#define IsEnemy(%1,%2)			(g_iPlayerTeam[%1] != g_iPlayerTeam[%2])
#define IsPlayerMuted(%1,%2)	(GetBit(g_bitPlayerMuted[%1], %2))
#define	MENU_DATA_LENGTH		8
#define	MENU_ITEM_LENGTH		100
#define	MAX_REASON_LENGTH		100
#define TIME_NOT_EXPIRING		0
#define TIME_MAP_END_EXPIRING	-1
#define EMPTY_DATA				-2
#define MENU_TIME				't'
#define MENU_REASON				'r'
#define MENU_REASON_SELF		's'
#define MENU_TYPE				'b'
#define MENU_PLAYERS			'p'
#define MENU_ITEM_BLOCK_ADD		'a'
#define MENU_ITEM_BLOCK_REMOVE	'd'
#define MAX_CHAT_MSG_LENGTH		190
#define STATIC_ITEM_HANDLER		-4
#define STATIC_ITEM_POSITION	6

#define MAX_IMMUNITY_LEVELS		5	//максимальное количество уровней доступа "cms_gag_immunity_lvl"
#define MAX_IMMUNITY_LEVEL_LEN	5	//максимальное количество флагов в одном уровне доступа
#define MAX_BLOCK_TYPES			3	//типов блокировки (все, чат, голос)


enum _:BlockInfo
{
	GBid,							//comms.GBid BanID
	GBlockType,						//comms.type
	GExpired,						//comms.GExpired
	GAdminId,						//comms.admin_id
	GCreated,						//comms.GCreated
	GBlockTime,						//comms.length
	GAuthId[MAX_AUTHID_LENGTH],		//comms.GAuthId
	GName[MAX_NAME_LENGTH],			//comms.GName
	GBlockReason[MAX_REASON_LENGTH],//comms.reason
	GAdminNick[MAX_NAME_LENGTH],	//comms.admin_nick
	GModifiedBy[MAX_NAME_LENGTH],	//comms.modified_by
	bool:GModifiedBlocked,
	GTargetID
};

enum _:eReasonData
{
	REASON_TEXT[MAX_REASON_LENGTH],
	REASON_TIME,
	REASON_TYPE
};

new const sqlTable[] = "comms";
new const g_szSoundFilePath[] = "../valve/sound/buttons/blip2.wav";
new Trie:g_trhWhiteListCmdArray, Trie:g_trhBlockedUsers;
new Array:g_arhBlockTime, Array:g_arhBlockReasons;
new Handle:g_hSqlHandle;

new szQuery[MAX_QUERY_SMALL_LEN*2], plBlockInfo[MAX_PLAYERS+1][BlockInfo], g_szPlayerMuteType[MAX_PLAYERS + 1];
new g_bitPlayerMuted[MAX_PLAYERS+1], CsTeams:g_iPlayerTeam[MAX_PLAYERS+1], bool:g_isAlivePlayer[MAX_PLAYERS+1], g_iImmunLevel[MAX_PLAYERS+1];
new g_szBlockType[MAX_BLOCK_TYPES][MAX_NAME_LENGTH], g_bitAccessLvl[MAX_IMMUNITY_LEVELS];
new g_iEntSpamChecker[MAX_PLAYERS+1], Float:g_flPlayerSpamTimer[MAX_PLAYERS+1];
new g_bitBlockedVoice, g_bitBlockedChat, g_bitBlockedFully, g_bitUseTeamVoice;
new g_iTimeMenu, g_iReasonMenuId, g_iTypeMenu, g_iStaticMenuCallback;
new g_iAddTime, g_iAddLastId, g_iMenuAccessNull, g_iImmunLevelsNum;
new g_bitApiAllow, g_iCmdGagMenu, g_iServerId, bool:g_bRoundEnd;

//cvars
new cpTime, cpImmunityLvl, cpWhiteCmds, cpDeadAdmTalk, cpAmxShowActivity, cpAllTalk, cpBlockVoiceSpam;
new cv_iDeadAdmTalk, cv_iAllTalk, cv_iBlockSpam, cv_iMuteTypeChoose, Float:cv_flBlockSpamTimer, Float:cv_flTimeForInfo;

public plugin_init()
{
	register_plugin(PLUGIN, VERSION, AUTHOR);
	register_cvar("gamecms_gagmanager", VERSION, FCVAR_SERVER);
	
	register_dictionary("time.txt");
	register_dictionary("common.txt");
	register_dictionary("gamecms_gagmanager.txt");
	
	register_clcmd("say /mute",			"MuteMenu");
	register_clcmd("say_team /mute",	"MuteMenu");
	register_clcmd("+teamvoice",		"CmdTeamVoiceOn");
	register_clcmd("-teamvoice",		"CmdTeamVoiceOff");
	register_clcmd("say",				"HookSayChat");
	register_clcmd("say_team",			"HookSayChat");

	#if defined _reapi_included
		RegisterHookChain(RG_CBasePlayer_Spawn, "CBasePlayerSpawn", true);
		RegisterHookChain(RG_CBasePlayer_Killed, "CBasePlayerKilled", true);
		RegisterHookChain(RG_CSGameRules_CanPlayerHearPlayer, "CanPlayerHearPlayer", false);
		RegisterHookChain(RG_CSGameRules_RestartRound, "Rlog_Round_Start", true);
		RegisterHookChain(RG_RoundEnd, "Rlog_Round_End", true);
	#else
		RegisterHam(Ham_Spawn, "player", "CBasePlayerSpawn", true);
		RegisterHam(Ham_Killed, "player", "CBasePlayerKilled", true);
		register_forward(FM_Voice_SetClientListening, "CanPlayerHearPlayer", false);
		register_logevent("Rlog_Round_End", 2, "1=Round_End");
		register_logevent("Rlog_Round_Start", 2, "1=Round_Start");
	#endif
	
	//квары
	cpTime = register_cvar("cms_gag_time", "-1, 10, 60, 1440, 0");
	cpImmunityLvl = register_cvar("cms_gag_immunity_lvl", "a, b, c, d");
	cpWhiteCmds = register_cvar("cms_gag_whitecmds", "");
	cpDeadAdmTalk = register_cvar("cms_gag_deadadmtalk", "0");
	cpBlockVoiceSpam = register_cvar("cms_gag_block_voice_spam", "1");

	new cpBlockSpamTimer = register_cvar("cms_gag_block_spam_timer", "10.0");
	new cpMuteTypeChoose = register_cvar("cms_gag_mute_type_choose", "0");
	
	cpAmxShowActivity = get_cvar_pointer("amx_show_activity");
	cpAllTalk = get_cvar_pointer("sv_alltalk");
	hook_cvar_change(cpAllTalk, "HookCvarChanged");
	bind_pcvar_float(register_cvar("cms_gag_time_for_info", "0.0"), cv_flTimeForInfo);
	
	g_trhBlockedUsers = TrieCreate();
	g_trhWhiteListCmdArray = TrieCreate();
	g_arhBlockTime = ArrayCreate();
	
	//меню причин, чтобы загрузить их из конфига(ов)
	FnReasonMenuCreate();
	
	new configsFile[MAX_NAME_LENGTH*2];
	get_configsdir(configsFile, charsmax(configsFile));
	format(configsFile, charsmax(configsFile), "%s/gamecms/gamecms_gagmanager.cfg", configsFile);
	
	server_cmd("exec %s", configsFile);
	server_exec();

	cv_iMuteTypeChoose = get_pcvar_num(cpMuteTypeChoose);
	cv_flBlockSpamTimer = get_pcvar_float(cpBlockSpamTimer);

	if(cv_iMuteTypeChoose)
		register_message(get_user_msgid("SayText"), "MessageSayText");

	new tmpCvarStr[MAX_FMT_LENGTH];
	if(get_pcvar_string(cpImmunityLvl, tmpCvarStr, charsmax(tmpCvarStr)))
	{
		new tmpOut[MAX_IMMUNITY_LEVELS][MAX_IMMUNITY_LEVEL_LEN];
		g_iImmunLevelsNum = explode_string(tmpCvarStr, ",", tmpOut, sizeof(tmpOut), sizeof(tmpOut[]));
		
		if(!g_iImmunLevelsNum)
			return;
		else if(g_iImmunLevelsNum > MAX_IMMUNITY_LEVELS)
			g_iImmunLevelsNum = MAX_IMMUNITY_LEVELS;

		for(new i; i < g_iImmunLevelsNum; i++)
		{
			trim(tmpOut[i]);
			g_bitAccessLvl[i] = read_flags(tmpOut[i]);
		}

		new iMenuFlag = g_bitAccessLvl[1];
		g_iCmdGagMenu = register_clcmd("amx_gagmenu", "AdminPlayersMenu", iMenuFlag, "", 1);
		register_clcmd("say /gag", "AdminPlayersMenu", iMenuFlag, "", 1);
	}
}

public plugin_precache()
{
	if(file_exists(g_szSoundFilePath))
		precache_sound(g_szSoundFilePath);
}

public Rlog_Round_End()
{
	g_bRoundEnd = true;
}

public Rlog_Round_Start()
{
	g_bRoundEnd = false;
}

public OnConfigsExecuted()
{
	new tmpCvarStr[512];
	//время
	if(get_pcvar_string(cpTime, tmpCvarStr, charsmax(tmpCvarStr)))
		CvarsToArray(g_arhBlockTime, tmpCvarStr, ',', "t");
	
	//разрешенные команды
	if(get_pcvar_string(cpWhiteCmds, tmpCvarStr, charsmax(tmpCvarStr)))
		CvarsToArray(g_trhWhiteListCmdArray, tmpCvarStr, ',', "c");
	
	cv_iDeadAdmTalk = get_pcvar_num(cpDeadAdmTalk);
	cv_iAllTalk = get_pcvar_num(cpAllTalk);
	cv_iBlockSpam = get_pcvar_num(cpBlockVoiceSpam);
	
	if(cv_iBlockSpam != BLOCK_STATUS_NONE && cv_flBlockSpamTimer > 0.0)
	{
		for(new iClient = 1; iClient <= MaxClients; iClient++)
		{
		#if defined _reapi_included
			g_iEntSpamChecker[iClient] = rg_create_entity("info_target", true);
		}
		#else
			g_iEntSpamChecker[iClient] = create_entity("info_target");
			if(g_iEntSpamChecker[iClient])
			{
				entity_set_string(g_iEntSpamChecker[iClient], EV_SZ_classname, g_szSpamEntClassname);
				entity_set_edict(g_iEntSpamChecker[iClient], EV_ENT_owner, iClient);
			}
		}
		
		register_think(g_szSpamEntClassname, "SpamEntThinkCallback");
		#endif
	}
	
	FnCreateStaticMenu();
}

FnReasonMenuCreate()
{
	register_concmd("cms_gag_reason", "CmdAddGagReason");
	g_arhBlockReasons = ArrayCreate(eReasonData);
	g_iStaticMenuCallback = menu_makecallback("StaticMenuCallback");
	g_iReasonMenuId = menu_create(fmt("%L", LANG_PLAYER, "CMSGAG_MENU_REASON_HEADER"), "StaticMenuHandler");
	menu_additem(g_iReasonMenuId, fmt("%L", LANG_PLAYER, "CMSGAG_MENU_REASON_CUSTOM"), "rs");
	menu_setprop(g_iReasonMenuId, MPROP_EXITNAME, fmt("%L", LANG_PLAYER, "EXIT"));
}

public HookCvarChanged(pcvar, const old_value[], const new_value[])
{
	if(pcvar == cpAllTalk)
		cv_iAllTalk = str_to_num(new_value);
}

/*======== Загрузка причины =========*/
public CmdAddGagReason(id, level, cid)
{
	if(!cmd_access(id, g_bitAccessLvl[1], cid, 0))
		return PLUGIN_HANDLED;

	new szReason[MENU_ITEM_LENGTH];
	read_args(szReason, charsmax(szReason));

	if(szReason[0])
	{
		new szReasonInfo[eReasonData];
		if(!id)
		{
			new iItems, szTime[MENU_DATA_LENGTH], szType[MENU_DATA_LENGTH];
			iItems = parse(szReason, szReasonInfo[REASON_TEXT], charsmax(szReasonInfo), szTime, charsmax(szTime), szType, charsmax(szType));

			if(iItems > 1)
			{
				szReasonInfo[REASON_TIME] = szTime[0] ? str_to_num(szTime) : EMPTY_DATA;
				switch(szType[0])
				{
					case 'a': szReasonInfo[REASON_TYPE] = BLOCK_STATUS_ALL;
					case 'c': szReasonInfo[REASON_TYPE] = BLOCK_STATUS_CHAT;
					case 'v': szReasonInfo[REASON_TYPE] = BLOCK_STATUS_VOICE;
					default: szReasonInfo[REASON_TYPE] = EMPTY_DATA;
				}
			}
			else
				szReasonInfo[REASON_TIME] = szReasonInfo[REASON_TYPE] = EMPTY_DATA;

			ArrayPushArray(g_arhBlockReasons, szReasonInfo, sizeof(szReasonInfo));	
		}
		else
		{	
			remove_quotes(szReason);
			trim(szReason);
			copy(plBlockInfo[id][GBlockReason], charsmax(plBlockInfo[]), szReason);
			return plBlockInfo[id][GCreated] ? GagInfoMenu(id) : menu_display(id, g_iTimeMenu, 0);
		}
	}
	else
		menu_display(id, g_iReasonMenuId, 0);

	return PLUGIN_HANDLED;
}

	
/*======== Ловим соединение с БД от API =========*/
public OnAPIPluginLoaded(Handle:sqlTuple) 
{
	g_hSqlHandle = sqlTuple;
	g_bitApiAllow |= UseGameCms;
	g_iServerId = cmsapi_get_server_id();
	
	SQL_SetCharset(g_hSqlHandle, "utf8");

	if(get_playersnum()) //Если игроки зашли ДО соединения с БД
	{
		new szAuthid[MAX_AUTHID_LENGTH];
		for(new i = 1; i <= MaxClients; i++)
		{
			if(!is_user_connected(i))
				continue;

			get_user_authid(i, szAuthid, charsmax(szAuthid));
			FnClientConnected(i, szAuthid);
		}
	}
}

public OnAPIAdminsLoaded()
	g_bitApiAllow |= UseGameCmsAdmins;

/*======== Статические меню (время, тип) =========*/

public FnCreateStaticMenu()
{
	new iSize = ArraySize(g_arhBlockReasons);
	new szReasonInfo[eReasonData];
	for(new i; i < iSize; i++)
	{
		ArrayGetArray(g_arhBlockReasons, i, szReasonInfo, sizeof(szReasonInfo));
		
		if(szReasonInfo[REASON_TIME] != EMPTY_DATA)
			format(szReasonInfo[REASON_TEXT], charsmax(szReasonInfo[REASON_TEXT]), "%s \y[%s]", szReasonInfo[REASON_TEXT], 
				FormatBlockTime(szReasonInfo[REASON_TIME], 1, timeunit_minutes));
		
		if(szReasonInfo[REASON_TYPE] != EMPTY_DATA)
			format(szReasonInfo[REASON_TEXT], charsmax(szReasonInfo[REASON_TEXT]), "%s \y[%L]", szReasonInfo[REASON_TEXT], 
				LANG_PLAYER, fmt("CMSGAG_BLOCK_TYPE_%d", szReasonInfo[REASON_TYPE]));

		menu_additem(g_iReasonMenuId, szReasonInfo[REASON_TEXT], "r", 0, g_iStaticMenuCallback);
	}

/* ===========	Меню выбора времени*/
	g_iTimeMenu = menu_create(fmt("%L", LANG_PLAYER, "CMSGAG_MENU_TIME_HEADER"), "StaticMenuHandler", 1);
	iSize = ArraySize(g_arhBlockTime);
	for (new i = 0; i < iSize; ++i)
		menu_additem(g_iTimeMenu, FormatBlockTime(ArrayGetCell(g_arhBlockTime, i), 1, timeunit_minutes), "t", 0, g_iStaticMenuCallback);

	menu_setprop(g_iTimeMenu, MPROP_EXITNAME, fmt("%L", LANG_PLAYER, "EXIT"));
	
/* ===========	Меню выбора типа блокировки	*/

	g_iTypeMenu = menu_create(fmt("%L", LANG_PLAYER, "CMSGAG_MENU_TYPE_HEADER"), "StaticMenuHandler", 1);
	for (new i = 0; i < MAX_BLOCK_TYPES; ++i)
	{
		formatex(g_szBlockType[i], charsmax(g_szBlockType[]), "%L", LANG_PLAYER, fmt("CMSGAG_BLOCK_TYPE_%d", i));
		menu_additem(g_iTypeMenu, g_szBlockType[i], "b", 0, g_iStaticMenuCallback);
	}

	menu_setprop(g_iTypeMenu, MPROP_EXITNAME, fmt("%L", LANG_PLAYER, "EXIT"));
}

public StaticMenuCallback(id, menu, item)
{
	new szData[MENU_DATA_LENGTH], szName[MENU_ITEM_LENGTH];
	menu_item_getinfo(menu, item, g_iMenuAccessNull, szData, charsmax(szData), szName, charsmax(szName));

	new itemStatus;
	switch(szData[0])
	{
		case MENU_TIME:
		{
			new iTime = ArrayGetCell(g_arhBlockTime, item);
			///?????? get_timeleft на ксдм
			itemStatus = (iTime == plBlockInfo[id][GBlockTime] && plBlockInfo[id][GCreated]) ||
			!g_bitApiAllow && (iTime > get_timeleft()/60 || iTime == TIME_NOT_EXPIRING) ? ITEM_DISABLED : ITEM_IGNORE;
		}
		case MENU_REASON: itemStatus = strcmp(szName, plBlockInfo[id][GBlockReason]) == 0 ? ITEM_DISABLED : ITEM_IGNORE;
		case MENU_TYPE: itemStatus = item == plBlockInfo[id][GBlockType] && plBlockInfo[id][GCreated] ? ITEM_DISABLED : ITEM_IGNORE;
	}

	return itemStatus;
 }
 
public StaticMenuHandler(id, menu, item)
{
	if(item == MENU_EXIT)
	{
		if(plBlockInfo[id][GCreated])
			GagInfoMenu(id);
		else
			arrayset(plBlockInfo[id], 0, BlockInfo);
		return PLUGIN_HANDLED;
	}
		
	new szData[MENU_DATA_LENGTH], szName[MENU_ITEM_LENGTH];
	menu_item_getinfo(menu, item, g_iMenuAccessNull, szData, charsmax(szData), szName, charsmax(szName));
		
	switch(szData[0])
	{
		case MENU_TIME:	//time
		{
			plBlockInfo[id][GBlockTime] = ArrayGetCell(g_arhBlockTime, item);
			if(plBlockInfo[id][GCreated])
			{
				GagInfoMenu(id);
				return PLUGIN_HANDLED;
			}

			if(plBlockInfo[id][GBlockType] == EMPTY_DATA)
				menu_display(id, g_iTypeMenu, 0);
			else
				GagInfoMenu(id);
		}
		case MENU_REASON:	//reason
		{
			switch(szData[1])
			{
				case MENU_REASON_SELF:
				{
					client_cmd(id,"messagemode cms_gag_reason");
					return PLUGIN_HANDLED;
				}
				default:
				{
					new szReasonInfo[eReasonData];
					ArrayGetArray(g_arhBlockReasons, item - 1, szReasonInfo, sizeof(szReasonInfo));

					copy(plBlockInfo[id][GBlockReason], charsmax(plBlockInfo[]), szReasonInfo[REASON_TEXT]);

					if(plBlockInfo[id][GCreated])
					{
						GagInfoMenu(id);
						return PLUGIN_HANDLED;
					}
					plBlockInfo[id][GBlockTime] = szReasonInfo[REASON_TIME];
					plBlockInfo[id][GBlockType] = szReasonInfo[REASON_TYPE];
					
					if(plBlockInfo[id][GBlockTime] == EMPTY_DATA)
						menu_display(id, g_iTimeMenu, 0);
					else if(plBlockInfo[id][GBlockType] == EMPTY_DATA)
						menu_display(id, g_iTypeMenu, 0);
					else
						GagInfoMenu(id);
				}
			}
		}
		case MENU_TYPE:	//type
		{
			plBlockInfo[id][GBlockType] = item;
			GagInfoMenu(id);
		}
	}
			
	return PLUGIN_HANDLED;
}

/*======== Меню игроков (админ) =========*/
public AdminPlayersMenu(id)
{
	if(!cmd_access(id, g_bitAccessLvl[1], g_iCmdGagMenu, 0))
		return PLUGIN_HANDLED;

	arrayset(plBlockInfo[id], 0, BlockInfo);
		
	static iAdminMenuCallback;
	if(!iAdminMenuCallback)
		iAdminMenuCallback = menu_makecallback("AdminPlayersMenuCallback");
	new iPlayersMenu = menu_create(fmt("%L", LANG_PLAYER, "CMSGAG_MENU_BLOCK_HEADER"), "AdminPlayersMenuHandler");

	new iTargetClient, szFmtName[MENU_ITEM_LENGTH], szPlayersIds[MAX_PLAYERS], iPlayersCount;
	get_players_ex(szPlayersIds, iPlayersCount, (GetPlayers_ExcludeBots | GetPlayers_ExcludeHLTV));

	for (new i; i < iPlayersCount; i++)
	{
		iTargetClient = szPlayersIds[i];
		if(id == iTargetClient)
			continue;

		if(cmsgag_is_user_blocked(iTargetClient) != BLOCK_STATUS_NONE)
			formatex(szFmtName, charsmax(szFmtName), "\w%n %L", iTargetClient, LANG_PLAYER, "CMSGAG_ITEM_PLAYER_BLOCKED");
		else
			formatex(szFmtName, charsmax(szFmtName), "\w%n", iTargetClient);

		menu_additem(iPlayersMenu, szFmtName, fmt("%d", iTargetClient), 0, iAdminMenuCallback);
	}

	menu_setprop(iPlayersMenu, MPROP_BACKNAME, fmt("%L", LANG_PLAYER, "BACK"));
	menu_setprop(iPlayersMenu, MPROP_NEXTNAME, fmt("%L", LANG_PLAYER, "MORE"));
	menu_setprop(iPlayersMenu, MPROP_EXITNAME, fmt("%L", LANG_PLAYER, "EXIT"));

	menu_display(id, iPlayersMenu, 0);
	
	return PLUGIN_HANDLED;
}

public AdminPlayersMenuCallback(id, menu, item)
{
	new szPlayerId[MENU_DATA_LENGTH];
	menu_item_getinfo(menu, item, g_iMenuAccessNull, szPlayerId, charsmax(szPlayerId));

	return FnAboveImmunity(id, str_to_num(szPlayerId)) ? ITEM_ENABLED : ITEM_DISABLED;
 }
 
public AdminPlayersMenuHandler(id, menu, item)
{
	if(item == MENU_EXIT)
	{
		menu_destroy(menu);
		return PLUGIN_HANDLED;
	}

	new szPlayerId[MENU_DATA_LENGTH];
	menu_item_getinfo(menu, item, g_iMenuAccessNull, szPlayerId, charsmax(szPlayerId));
	plBlockInfo[id][GTargetID] = str_to_num(szPlayerId);
	
	menu_destroy(menu);
	
	get_user_name(plBlockInfo[id][GTargetID], plBlockInfo[id][GName], charsmax(plBlockInfo[]));
	get_user_authid(plBlockInfo[id][GTargetID], plBlockInfo[id][GAuthId], charsmax(plBlockInfo[]));

	new iBlockOwner;
	if(g_bitApiAllow & UseGameCmsAdmins)
		iBlockOwner = cmsapi_get_admin_ident(id);

	if(!TrieGetArray(g_trhBlockedUsers,  plBlockInfo[id][GAuthId], plBlockInfo[id], charsmax(plBlockInfo[])))
	{
		plBlockInfo[id][GAdminId] = iBlockOwner;

		menu_display(id, g_iReasonMenuId, 0);
		return PLUGIN_HANDLED;
	}

	//если игрок заблокирован, найдем админа
	if(g_bitApiAllow & UseGameCmsAdmins)
	{
		#if !defined API54
		new iAdminId = plBlockInfo[id][GAdminId];
		if(iAdminId != iBlockOwner)
		{
			new Trie:trhAdminInfo = cmsapi_get_admin_info(iAdminId);
			if(trhAdminInfo)
			{
				new adminData[AdminInfo];
				TrieGetArray(trhAdminInfo, fmt("%d", iAdminId), adminData, sizeof adminData);
				plBlockInfo[id][GModifiedBlocked] = FnAboveImmunity(get_user_flags(id), read_flags(adminData[AdminServiceFlags]), true) ? false : true;
				//TrieDestroy(trhAdminInfo);
			}
		}
		#else
		if(plBlockInfo[id][GAdminId] != iBlockOwner)
		{
			new adminData[AdminInfo];
			if(cmsapi_get_info_by_admin_id(plBlockInfo[id][GAdminId], adminData))
				plBlockInfo[id][GModifiedBlocked] = FnAboveImmunity(get_user_flags(id), read_flags(adminData[AdminServiceFlags]), true) ? false : true;
		}
		#endif
	}

	GagInfoMenu(id);

	return PLUGIN_HANDLED;
}

/*======== Меню сохранения / удаления блока =========*/
 public GagInfoMenu(id)
{
	new tmpName[MENU_ITEM_LENGTH];
	new tmpTypeNum = plBlockInfo[id][GBlockType];
	new iGagInfoMenu = menu_create(fmt("%L", LANG_PLAYER, "CMSGAG_MENU_INFO_HEADER"), "GagInfoMenuHandler");	
	static iGagInfoMenuCallback;
	if(!iGagInfoMenuCallback)
		iGagInfoMenuCallback = menu_makecallback("GagInfoMenuCallback");
	new bool:existKey = TrieKeyExists(g_trhBlockedUsers, plBlockInfo[id][GAuthId]);
	
	/*if(!plBlockInfo[id][GCreated])
		plBlockInfo[id][GCreated] = get_systime();
	*/
	if(existKey)
		formatex(tmpName, charsmax(tmpName), "%L %s %L", LANG_PLAYER,"CMSGAG_ITEM_PLAYER_INFO_NAME", plBlockInfo[id][GName], LANG_PLAYER,"CMSGAG_ITEM_PLAYER_BLOCKED");
	else
		formatex(tmpName, charsmax(tmpName), "%L %s", LANG_PLAYER,"CMSGAG_ITEM_PLAYER_INFO_NAME", plBlockInfo[id][GName]);
	menu_additem(iGagInfoMenu, tmpName, "p", 0);
	
	formatex(tmpName, charsmax(tmpName), "%L %s", LANG_PLAYER, "CMSGAG_ITEM_PLAYER_INFO_TIME", FormatBlockTime(plBlockInfo[id][GBlockTime], 1, timeunit_minutes));
	menu_additem(iGagInfoMenu, tmpName, "t", 0, iGagInfoMenuCallback);
	
	formatex(tmpName, charsmax(tmpName), "%L %s", LANG_PLAYER, "CMSGAG_ITEM_PLAYER_INFO_REASON", plBlockInfo[id][GBlockReason]);
	menu_additem(iGagInfoMenu, tmpName, "r", 0, iGagInfoMenuCallback);
	
	formatex(tmpName, charsmax(tmpName), "%L %s", LANG_PLAYER, "CMSGAG_ITEM_PLAYER_INFO_TYPE", g_szBlockType[tmpTypeNum]);
	menu_additem(iGagInfoMenu, tmpName, "b", 0, iGagInfoMenuCallback);
	
	if(existKey)
	{
		static iTime;
		iTime = plBlockInfo[id][GExpired] > 0 ? (plBlockInfo[id][GExpired] - get_systime())/60 : plBlockInfo[id][GExpired];
		formatex(tmpName, charsmax(tmpName), "%L %s", LANG_PLAYER, "CMSGAG_ITEM_PLAYER_INFO_EXPIRED", FormatBlockTime(iTime, 2, timeunit_minutes));
		menu_addtext(iGagInfoMenu, tmpName, 0);
	}
	if(strlen(plBlockInfo[id][GAdminNick]))
	{
		formatex(tmpName, charsmax(tmpName), "%L %s", LANG_PLAYER, "CMSGAG_ITEM_PLAYER_INFO_ADMIN_1", plBlockInfo[id][GAdminNick]);
		menu_addtext(iGagInfoMenu, tmpName, 0);
	}
	if(strlen(plBlockInfo[id][GModifiedBy]))
	{
		formatex(tmpName, charsmax(tmpName), "%L %s", LANG_PLAYER, "CMSGAG_ITEM_PLAYER_INFO_ADMIN_2", plBlockInfo[id][GModifiedBy]);
		menu_addtext(iGagInfoMenu, tmpName, 0);
	}

	menu_addblank2(iGagInfoMenu);

	menu_additem(iGagInfoMenu, fmt("%L", LANG_PLAYER, existKey ? "CMSGAG_ITEM_PLAYER_INFO_SAVE" : "CMSGAG_ITEM_PLAYER_INFO_SET"), "a", 0, iGagInfoMenuCallback);
	
	if(existKey)
		menu_additem(iGagInfoMenu, fmt("%L", LANG_PLAYER, "CMSGAG_ITEM_PLAYER_INFO_REMOVE"), "d", 0, iGagInfoMenuCallback);
	
	menu_setprop(iGagInfoMenu, MPROP_EXITNAME, fmt("%L", LANG_PLAYER, "EXIT"));
	menu_setprop(iGagInfoMenu, MPROP_EXIT, MEXIT_ALL);
	menu_display(id, iGagInfoMenu, 0);
	
	return PLUGIN_HANDLED;
}

public GagInfoMenuCallback(id, menu, item)
{
	return (plBlockInfo[id][GModifiedBlocked]/* || cmsgag_is_user_blocked(id) != BLOCK_STATUS_NONE*/) ? ITEM_DISABLED : ITEM_ENABLED;
}

public GagInfoMenuHandler(id, menu, item)
{
	if(item == MENU_EXIT)
	{
		arrayset(plBlockInfo[id], 0, BlockInfo);
	}
	else
	{
		if(!plBlockInfo[id][GCreated])
			plBlockInfo[id][GCreated] = get_systime();

		new szData[MENU_DATA_LENGTH];
		menu_item_getinfo(menu, item, g_iMenuAccessNull, szData, charsmax(szData));
		
		switch(szData[0])
		{
			case MENU_TIME:		menu_display(id, g_iTimeMenu, 0);				//time
			case MENU_REASON:	menu_display(id, g_iReasonMenuId, 0);			//reason
			case MENU_TYPE:		menu_display(id, g_iTypeMenu, 0);				//type
			case MENU_PLAYERS:	AdminPlayersMenu(id);							//szPlayersIds menu
			case MENU_ITEM_BLOCK_ADD:		BlockQueryFunc(id, BLOCK_FUNC_ADD);		//add/ save block
			case MENU_ITEM_BLOCK_REMOVE:	BlockQueryFunc(id, BLOCK_FUNC_REMOVE);	//remove block
		}
	}

	menu_destroy(menu);
	return PLUGIN_HANDLED;
}

/*======== Меню игроков (игрок) =========*/
public MuteMenu(id)
{
	static g_iMuteMenuCB;
	if(!g_iMuteMenuCB)
		g_iMuteMenuCB = menu_makecallback("MuteMenuCallback");
	new iMuteMenu = menu_create(fmt("%L", LANG_PLAYER, "CMSGAG_MENU_MUTE_HEADER"), "MuteMenuHandler", 1);

	new szPlayersIds[MAX_PLAYERS], iPlayersCount;
	get_players_ex(szPlayersIds, iPlayersCount, (GetPlayers_ExcludeBots | GetPlayers_ExcludeHLTV));

	for(new i; i < iPlayersCount; i++)
	{
		if(cv_iMuteTypeChoose)
			menu_additem_static(iMuteMenu, "", fmt("%d", szPlayersIds[i]), i, iPlayersCount-1, true, 0, g_iMuteMenuCB);
		else
			menu_additem(iMuteMenu, "", fmt("%d", szPlayersIds[i]), 0, g_iMuteMenuCB);
	}
		
	menu_setprop(iMuteMenu, MPROP_BACKNAME, fmt("%L", LANG_PLAYER, "BACK"));
	menu_setprop(iMuteMenu, MPROP_NEXTNAME, fmt("%L", LANG_PLAYER, "MORE"));
	menu_setprop(iMuteMenu, MPROP_EXITNAME, fmt("%L", LANG_PLAYER, "EXIT"));

	menu_display(id, iMuteMenu, 0);
	return PLUGIN_HANDLED;
}

stock menu_additem_static(iMenu, itemName[], itemInfo[], index, lastItemPos, bool:shift = true, paccess = 0, iMenuCallback = -1)
{
	new staticItem = index % STATIC_ITEM_POSITION;
	menu_additem(iMenu, itemName, itemInfo, paccess, iMenuCallback);	
	
	if(index == lastItemPos)
	{
		if(shift)
		{
			new addBlank = STATIC_ITEM_POSITION - staticItem - 1;
			for(new i; i < addBlank; i++)
				menu_addblank2(iMenu);
		}

		staticItem = STATIC_ITEM_POSITION - 1;
	}

	if(staticItem == STATIC_ITEM_POSITION - 1)
	{
		if(shift)
			menu_addblank(iMenu, 0);
		
		menu_additem(iMenu, "", fmt("%d", STATIC_ITEM_HANDLER), paccess, iMenuCallback);
	}
}

public MuteMenuCallback(id, menu, item)
{
	new szPlayerId[MENU_DATA_LENGTH], iTargetClient, szFmtName[MAX_NAME_LENGTH*2];
	menu_item_getinfo(menu, item, g_iMenuAccessNull, szPlayerId, charsmax(szPlayerId));

	static iItemStatus;
	iItemStatus = ITEM_IGNORE;
	iTargetClient = str_to_num(szPlayerId);
	
	if(iTargetClient != STATIC_ITEM_HANDLER && is_user_connected(iTargetClient))
	{
		formatex(szFmtName, charsmax(szFmtName), "\w%n", iTargetClient);
		if(iTargetClient == id)
			iItemStatus = ITEM_DISABLED;
		else if(GetBit(g_bitBlockedFully, iTargetClient))
		{
			format(szFmtName, charsmax(szFmtName), "%s %L", szFmtName, LANG_PLAYER, "CMSGAG_ITEM_BLOCKED_BY_ADMIN");
			iItemStatus = ITEM_DISABLED;
		}
		else if(IsPlayerMuted(id, iTargetClient))
			format(szFmtName, charsmax(szFmtName), "%s %L", szFmtName, LANG_PLAYER, "CMSGAG_ITEM_PLAYER_MUTED");
	}
	else
	{
		formatex(szFmtName, charsmax(szFmtName), "%L %L", LANG_PLAYER, "CMSGAG_ITEM_PLAYER_INFO_TYPE", 
			LANG_PLAYER, fmt("CMSGAG_BLOCK_TYPE_%d", g_szPlayerMuteType[id]));
	}
	
	menu_item_setname(menu, item, szFmtName);
	return iItemStatus;
}
 
public MuteMenuHandler(id, menu, item)
{
	if (item == MENU_EXIT)
	{
		menu_destroy(menu);
		return PLUGIN_HANDLED;
	}

	new szPlayerId[MENU_DATA_LENGTH];
	menu_item_getinfo(menu, item, g_iMenuAccessNull, szPlayerId, charsmax(szPlayerId));
	
	new iTargetClient = str_to_num(szPlayerId);
	
	if(iTargetClient != STATIC_ITEM_HANDLER)
	{
		if(!is_user_connected(iTargetClient))
		{
			client_print_color(id, iTargetClient, "%L %L", LANG_PLAYER, "CMSGAG_CHAT_PREFIX", LANG_PLAYER, "CMSGAG_CHAT_NOPLAYER");
			menu_destroy(menu);
			return PLUGIN_HANDLED;
		}

		if(!IsPlayerMuted(id, iTargetClient))
		{
			if(GetBit(g_bitBlockedFully, iTargetClient))
			{
				client_print_color(id, iTargetClient, "%L %L", LANG_PLAYER, "CMSGAG_CHAT_PREFIX", LANG_PLAYER, "CMSGAG_CHAT_ALREADY_BLOCKED");
				menu_destroy(menu);
				return PLUGIN_HANDLED;
			}
			
			FnMutePlayer(id, iTargetClient, true);
			client_print_color(id, iTargetClient, "%L %L", LANG_PLAYER, "CMSGAG_CHAT_PREFIX", LANG_PLAYER, "CMSGAG_CHAT_SET_MUTE", iTargetClient);
		}
		else
		{
			FnMutePlayer(id, iTargetClient, false);
			client_print_color(id, iTargetClient, "%L %L", LANG_PLAYER, "CMSGAG_CHAT_PREFIX", LANG_PLAYER, "CMSGAG_CHAT_RESET_MUTE", iTargetClient);
		}
	}
	else
	{
		if(++g_szPlayerMuteType[id] > BLOCK_STATUS_VOICE)
			g_szPlayerMuteType[id] = BLOCK_STATUS_ALL;
	}

	new imenu, newmenu, menupage;
	player_menu_info(id, imenu, newmenu, menupage);
	
	menu_display(id, menu, menupage);
	return PLUGIN_HANDLED;
}


FnMutePlayer(id, iTargetClient, setMute)
{
	if(setMute)
		SetBit(g_bitPlayerMuted[id], iTargetClient);
	else
		ResetBit(g_bitPlayerMuted[id], iTargetClient);
}

public client_authorized(id, const authid[])
{
	FnClientConnected(id, authid);
}

FnClientConnected(id, const authid[])
{
	if(is_user_bot(id) || is_user_hltv(id))
		return;

	g_bitPlayerMuted[id] = 0;
	g_szPlayerMuteType[id] = 0;
	g_isAlivePlayer[id] = false;
	g_iImmunLevel[id] = FnImmunityLevel(get_user_flags(id));

	FnSpamTimerReset(id);
	
	ResetBit(g_bitUseTeamVoice, id);
	FnCheckBlockAvailability(id, authid);
}

public OnAPIAdminConnected(id, const szName[], adminID, Flags)
{
	g_iImmunLevel[id] = FnImmunityLevel(Flags);
}

public client_disconnected(id)
{
	FnResetBlock(id);

	for(new i = 1; i < MaxClients; i++)
		FnMutePlayer(i, id, false);
}

/*======== Получение данных из БД =========*/
public FnCheckBlockAvailability(id, const szAuthid[])
{
	if(TrieGetArray(g_trhBlockedUsers, szAuthid, plBlockInfo[id], charsmax(plBlockInfo[])))
	{
		FnChangeBlock(id, plBlockInfo[id][GBlockType]);
		return;
	}

	if(!g_bitApiAllow || !g_hSqlHandle)
		return;

	formatex(szQuery, charsmax(szQuery), "SELECT * FROM `%s` WHERE `server_id` = '%d' AND `authid` = '%s'\
	AND ((`expired` > UNIX_TIMESTAMP(NOW()) OR `expired` = '0' ) AND `length` != '-1' AND `expired` != '-1') LIMIT 1;",
	sqlTable, g_iServerId, szAuthid);

	static Data[2];
	Data[0] = id;
	SQL_ThreadQuery(g_hSqlHandle, "FnCheckBlockAvailability_post", szQuery, Data, sizeof(Data));
}

public FnCheckBlockAvailability_post(failstate, Handle:query, const error[], errornum, const postData[], DataSize)
{
	if(SQL_Error(error, errornum, failstate))
		return;

	if(!SQL_NumResults(query))
		return;
	
	static id;
	id = postData[0];

	plBlockInfo[id][GBid] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "bid"));
	plBlockInfo[id][GBlockType] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "type"));
	plBlockInfo[id][GExpired] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "expired"));
	//plBlockInfo[id][GCreated] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "created"));
	plBlockInfo[id][GBlockTime] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "length"));
	plBlockInfo[id][GAdminId] = SQL_ReadResult(query, SQL_FieldNameToNum(query, "admin_id"));
		
	SQL_ReadResult(query, SQL_FieldNameToNum(query, "authid"), plBlockInfo[id][GAuthId], charsmax(plBlockInfo[][GAuthId]));
	SQL_ReadResult(query, SQL_FieldNameToNum(query, "name"), plBlockInfo[id][GName], charsmax(plBlockInfo[][GName]));
	SQL_ReadResult(query, SQL_FieldNameToNum(query, "reason"), plBlockInfo[id][GBlockReason], charsmax(plBlockInfo[][GBlockReason]));
	SQL_ReadResult(query, SQL_FieldNameToNum(query, "admin_nick"), plBlockInfo[id][GAdminNick], charsmax(plBlockInfo[][GAdminNick]));
	SQL_ReadResult(query, SQL_FieldNameToNum(query, "modified_by"), plBlockInfo[id][GModifiedBy], charsmax(plBlockInfo[][GModifiedBy]));

	mysql_escape_string(plBlockInfo[id][GName], charsmax(plBlockInfo[][GName])*2);
	mysql_escape_string(plBlockInfo[id][GBlockReason], charsmax(plBlockInfo[][GBlockReason]));
	mysql_escape_string(plBlockInfo[id][GAdminNick], charsmax(plBlockInfo[][GAdminNick])*2);
	mysql_escape_string(plBlockInfo[id][GModifiedBy], charsmax(plBlockInfo[][GModifiedBy])*2);
			
	if(TrieSetArray(g_trhBlockedUsers,  plBlockInfo[id][GAuthId], plBlockInfo[id], sizeof(plBlockInfo[])))
		FnChangeBlock(id, plBlockInfo[id][GBlockType]);
}

/*======== Запрос на сохранение блока =========*/
/*
id- client index
iFuncType - тип операции блокировки(1- создание, 2- удаление)
iBlockType - тип блока (чат / голос)
szBlockReason - причина
iTime - время

iTargetClient - target index

*/

BlockQueryFunc(id, iFuncType, iBlockType = BLOCK_STATUS_NONE, szBlockReason[] = "", iTime = 0, bool:printChat = true)
{
	new iTargetClient = plBlockInfo[id][GTargetID];
	if(!iTargetClient)
		iTargetClient = id;

	new iSystime = get_systime(), iTimeleft = get_timeleft() / 60;
	if(g_iAddLastId == iTargetClient && iSystime < g_iAddTime + 20)
	{
		client_print_color(id, 0, "%L %L", LANG_PLAYER, "CMSGAG_CHAT_PREFIX", LANG_PLAYER, "CMSGAG_CHAT_EDIT_ERROR");
		return PLUGIN_CONTINUE;
	}
		 
	get_user_authid(iTargetClient, plBlockInfo[id][GAuthId], charsmax(plBlockInfo[][GAuthId]));
	
	if(!TrieKeyExists(g_trhBlockedUsers,  plBlockInfo[id][GAuthId]))
	{
		if(iTargetClient == id)
			plBlockInfo[id][GAdminNick] = "Server";
		else
			get_user_name(id, plBlockInfo[id][GAdminNick], charsmax(plBlockInfo[][GAdminNick]));
	}
	else
	{
		get_user_name(id, plBlockInfo[id][GModifiedBy], charsmax(plBlockInfo[][GModifiedBy]));
	}

	new iBlockTime;
	
	if(iFuncType == BLOCK_FUNC_REMOVE)
	{
		plBlockInfo[id][GExpired] = TIME_MAP_END_EXPIRING;
	}
	else
	{
		if(!strlen(plBlockInfo[id][GName]))
			get_user_name(iTargetClient, plBlockInfo[id][GName], charsmax(plBlockInfo[][GName]));

		if(!strlen(plBlockInfo[id][GBlockReason]))
			copy(plBlockInfo[id][GBlockReason], charsmax(plBlockInfo[][GBlockReason]), szBlockReason);
		
		if(plBlockInfo[id][GBlockType] == BLOCK_STATUS_NONE)
			plBlockInfo[id][GBlockType] = iBlockType;
	
		if(plBlockInfo[id][GBlockTime] == TIME_NOT_EXPIRING)
			plBlockInfo[id][GBlockTime] = iTime;
			
		iBlockTime = plBlockInfo[id][GBlockTime];
		plBlockInfo[id][GCreated] = iSystime;
		
		if(iBlockTime <= TIME_NOT_EXPIRING)
			plBlockInfo[id][GExpired] = iBlockTime;
		else
			plBlockInfo[id][GExpired] = plBlockInfo[id][GCreated] + iBlockTime * 60;
	}

	if(g_bitApiAllow & (UseGameCmsAdmins | UseGameCms))	//Запись в бд, если она доступна
	{
		mysql_insert_string(plBlockInfo[id][GName], charsmax(plBlockInfo[][GName])*2);
		mysql_insert_string(plBlockInfo[id][GBlockReason], charsmax(plBlockInfo[][GBlockReason]));
		mysql_insert_string(plBlockInfo[id][GAdminNick], charsmax(plBlockInfo[][GAdminNick])*2);
		mysql_insert_string(plBlockInfo[id][GModifiedBy], charsmax(plBlockInfo[][GModifiedBy])*2);
		
		if(plBlockInfo[id][GBid])
		{
			if(iFuncType == BLOCK_FUNC_CHANGE)
				formatex(szQuery, charsmax(szQuery), "UPDATE `%s` SET expired='%d', length='%d', reason='%s', modified_by='%s',type='%d' WHERE `bid` = '%d';",
					sqlTable, plBlockInfo[id][GExpired], plBlockInfo[id][GBlockTime], plBlockInfo[id][GBlockReason], 
					plBlockInfo[id][GModifiedBy], plBlockInfo[id][GBlockType], plBlockInfo[id][GBid]);
			else if(iFuncType == BLOCK_FUNC_REMOVE)
				formatex(szQuery, charsmax(szQuery), "UPDATE `%s` SET expired='%d', modified_by='%s' WHERE `bid` = '%d';",
					sqlTable, plBlockInfo[id][GExpired], plBlockInfo[id][GModifiedBy], plBlockInfo[id][GBid]);
		}
		else
		{
			if(iFuncType != BLOCK_FUNC_ADD || iBlockTime == TIME_MAP_END_EXPIRING)
				return ASD_BlockFunc(id, iFuncType, iTargetClient, printChat);

			if(!iTimeleft || (iTimeleft && iTimeleft < iBlockTime) || iBlockTime == TIME_NOT_EXPIRING)	//Запись в бд, если время блока больше времени карты или Навсегда
			{
				formatex(szQuery, charsmax(szQuery),
				"INSERT INTO `%s` (bid, authId, name, created, expired, length, reason, admin_id, admin_nick, server_id, modified_by, type) \
				values ('%d', '%s', '%s', '%d', '%d', '%d', '%s', '%d', '%s', '%d', '%s', '%d');",
				sqlTable, plBlockInfo[id][GBid], plBlockInfo[id][GAuthId], plBlockInfo[id][GName], plBlockInfo[id][GCreated], 
				plBlockInfo[id][GExpired], plBlockInfo[id][GBlockTime], plBlockInfo[id][GBlockReason], 
				plBlockInfo[id][GAdminId], plBlockInfo[id][GAdminNick], g_iServerId, plBlockInfo[id][GModifiedBy], plBlockInfo[id][GBlockType]);
			}
			else
				return ASD_BlockFunc(id, iFuncType, iTargetClient, printChat);
		}

		static Data[4];
		Data[0] = id;
		Data[1] = iFuncType;
		Data[2] = iTargetClient;

		SQL_ThreadQuery(g_hSqlHandle, "BlockQueryFunc_post", szQuery, Data, sizeof(Data));
		return PLUGIN_HANDLED;
	}

	if(plBlockInfo[id][GExpired]- iSystime < iTimeleft)
		plBlockInfo[id][GExpired] = TIME_MAP_END_EXPIRING;

	return ASD_BlockFunc(id, iFuncType, iTargetClient, printChat);
}

public BlockQueryFunc_post(failstate, Handle:query, const error[], errornum, const postData[], DataSize)
{
	new id, iTargetClient, iFuncType;
	id = postData[0];
	iFuncType = postData[1];
	iTargetClient = postData[2];
	
	if(SQL_Error(error, errornum, failstate))
		plBlockInfo[id][GBlockTime] = TIME_MAP_END_EXPIRING;
	else if(iFuncType == BLOCK_FUNC_ADD)
		plBlockInfo[id][GBid] = SQL_GetInsertId(query);

	return ASD_BlockFunc(id, iFuncType, iTargetClient);
}

public CBasePlayerSpawn(id)
{
	#if defined _reapi_included
		g_iPlayerTeam[id] = get_member(id, m_iTeam);
	#else
		g_iPlayerTeam[id] = CsTeams:get_user_team(id);
	#endif
	
	if(is_user_alive(id))
		g_isAlivePlayer[id] = true;
}

public CBasePlayerKilled(id)
{
	if(cv_iAllTalk == 0 || cv_iAllTalk == 4)
	{
		if(g_bRoundEnd)
			return;

		if(cv_flTimeForInfo > 0.0)
			set_task(cv_flTimeForInfo, "FnBlockChatForInfo", id);
		else
			FnBlockChatForInfo(id);	
	}
	else
		g_isAlivePlayer[id] = false;
}

public FnBlockChatForInfo(id)
{
	g_isAlivePlayer[id] = false;
	
	set_hudmessage(255, 150, 0, -1.0, 0.63, 0, _, 5.0);
	show_hudmessage(id, "%L", LANG_PLAYER, "CMSGAG_HUD_INFO_TIME");
}

public CmdTeamVoiceOn(id)
{
	SetBit(g_bitUseTeamVoice, id);
	client_cmd(id, "+voicerecord");
	return PLUGIN_HANDLED;
}

public CmdTeamVoiceOff(id)
{
	ResetBit(g_bitUseTeamVoice, id);
	client_cmd(id, "-voicerecord");
	return PLUGIN_HANDLED;
}

public VTC_OnClientStartSpeak(iSender)
{
	if(VTC_IsClientMuted(iSender))
	{
		if(g_flPlayerSpamTimer[iSender] > cv_flBlockSpamTimer)
		{
			//log_to_file("gag.log", "VTC_OnClientStartSpeak muted %d | %n", iSender, iSender)
			client_print_color(iSender, 0, "%L %L", LANG_PLAYER, "CMSGAG_CHAT_PREFIX", LANG_PLAYER, "CMSGAG_CHAT_SOON_BE_UNBLOCKED", 
				FormatBlockTime(floatround(g_flPlayerSpamTimer[iSender] - get_gametime()), 1, timeunit_seconds));
			return PLUGIN_HANDLED;
		}
		
		return FnCheckBlockStatus(iSender, BLOCK_STATUS_VOICE);
	}

	if(g_iEntSpamChecker[iSender])
	{
		g_flPlayerSpamTimer[iSender] = cv_flBlockSpamTimer;
		#if defined _reapi_included
			#define MAX_ID_LEN	2
			set_entvar(g_iEntSpamChecker[iSender], var_nextthink, get_gametime() + 1.0);
			SetThink(g_iEntSpamChecker[iSender], "SpamEntThinkCallback", fmt("%d", iSender), MAX_ID_LEN);
		#else
			entity_set_float(g_iEntSpamChecker[iSender], EV_FL_nextthink, get_gametime() + 1.0);
		#endif
		//log_to_file("gag.log", "VTC_OnClientStartSpeak start timer %d | %n", iSender, iSender)
	}

	return PLUGIN_CONTINUE;
}

FnSpamTimerReset(iSender)
{
	g_flPlayerSpamTimer[iSender] = 0.0;
	#if defined _reapi_included
		set_entvar(g_iEntSpamChecker[iSender], var_nextthink, 0.0);
		SetThink(g_iEntSpamChecker[iSender], "");
	#else
		entity_set_float(g_iEntSpamChecker[iSender], EV_FL_nextthink, 0.0);
	#endif
}

#if defined _reapi_included
public SpamEntThinkCallback(iEntity, data[], len)
#else
public SpamEntThinkCallback(iEntity)
#endif
{
	static iClient, Float:flTime;
	#if defined _reapi_included
		iClient = str_to_num(data);
	#else
		iClient = entity_get_edict(iEntity, EV_ENT_owner);
	#endif

	if(iClient)
	{
		if(g_flPlayerSpamTimer[iClient] <= 0.0)
		{
			flTime = cv_flBlockSpamTimer * 30.0;
			g_flPlayerSpamTimer[iClient] = get_gametime() + flTime;

			VTC_MuteClient(iClient);
			client_print(iClient, print_center, "%L", LANG_PLAYER, "CMSGAG_CHAT_BLOCKED", FormatBlockTime(floatround(flTime), 1, timeunit_seconds));
			//log_to_file("gag.log", "SpamEntThinkCallback blocked %d (%.0f) | %n", iClient, g_flPlayerSpamTimer[iClient], iClient)
		}
		else if(g_flPlayerSpamTimer[iClient] <= cv_flBlockSpamTimer)
		{
			flTime = 1.0;
			if(g_flPlayerSpamTimer[iClient] < 6.0)
				client_print(iClient, print_center, "%L", LANG_PLAYER, "CMSGAG_CHAT_WILL_BE_BLOCKED", g_flPlayerSpamTimer[iClient]);
			g_flPlayerSpamTimer[iClient]--;

			//log_to_file("gag.log", "SpamEntThinkCallback will be blocked %d (%.0f) | %n", iClient, g_flPlayerSpamTimer[iClient], iClient)
		}
		else
		{
			flTime = 1.0;
			
			g_flPlayerSpamTimer[iClient] = cv_flBlockSpamTimer;
			VTC_UnmuteClient(iClient);
			client_print(iClient, print_center, "%L", LANG_PLAYER, "CMSGAG_CHAT_UNBLOCKED");
			//log_to_file("gag.log", "SpamEntThinkCallback unblocked %d (%.0f) | %n", iClient, g_flPlayerSpamTimer[iClient], iClient)
		}
	}
	//else
		//log_to_file("gag.log", "Not a client")

	#if defined _reapi_included
		set_entvar(iEntity, var_nextthink, get_gametime() + flTime);
	#else
		entity_set_float(iEntity, EV_FL_nextthink, get_gametime() + flTime);
	#endif
}

public VTC_OnClientStopSpeak(iSender)
{
	if(!g_iEntSpamChecker[iSender])
		return PLUGIN_CONTINUE;

	if(g_flPlayerSpamTimer[iSender] <= cv_flBlockSpamTimer)
		FnSpamTimerReset(iSender);

	return PLUGIN_CONTINUE;
}

public CanPlayerHearPlayer(Receiver, Sender, bool:Listen)
{
	if(Receiver == Sender)
		return FMRES_IGNORED;

	//Мертвого админа слышат все, если он говорит в командный чат или включен параметр cms_gag_deadadmtalk
	if(!g_isAlivePlayer[Sender] && g_iImmunLevel[Sender] > 1 && (GetBit(g_bitUseTeamVoice, Sender) || cv_iDeadAdmTalk))
		return FnCanHearSender(Receiver, Sender, true);

	if(IsPlayerMuted(Receiver, Sender) && g_szPlayerMuteType[Receiver] != BLOCK_STATUS_CHAT)
		return FnCanHearSender(Receiver, Sender, false);
		
	//если регейм, то учесть время на инфу
	#if defined _reapi_included
		if(GetBit(g_bitUseTeamVoice, Sender))
		{
			if(IsEnemy(Receiver, Sender))
				return FnCanHearSender(Receiver, Sender, false);
		}
		
		if(cv_flTimeForInfo > 0.0)
		{
			if(g_isAlivePlayer[Sender] && g_isAlivePlayer[Receiver])
				return FnCanHearSender(Receiver, Sender, true);
		}
		
		return FMRES_IGNORED;	
	#else
	
	if(cv_iAllTalk == 1)		//все со всеми
	{
		if(GetBit(g_bitUseTeamVoice, Sender))
		{
			if(/*g_iPlayerTeam[Receiver] != g_iPlayerTeam[Sender]*/IsEnemy(Receiver, Sender))
				return FnCanHearSender(Receiver, Sender, false);
		}

		//return FMRES_IGNORED;
		return FnCanHearSender(Receiver, Sender, true);
	}

	if(g_iPlayerTeam[Receiver] == CS_TEAM_SPECTATOR)
	{
		if(cv_iAllTalk == 0 || cv_iAllTalk == 2)
		{
			if(g_iPlayerTeam[Sender] != CS_TEAM_SPECTATOR)
				return FnCanHearSender(Receiver, Sender, false);
		}

		return FMRES_IGNORED;
	}

	if(g_isAlivePlayer[Sender])
	{
		if(cv_iAllTalk == 4)
		{
			if(GetBit(g_bitUseTeamVoice, Sender))
			{
				if(/*g_iPlayerTeam[Receiver] != g_iPlayerTeam[Sender]*/IsEnemy(Receiver, Sender))
					return FnCanHearSender(Receiver, Sender, false);
			}
			
			return FMRES_IGNORED;
		}
		
		if(/*g_iPlayerTeam[Receiver] == g_iPlayerTeam[Sender]*/!IsEnemy(Receiver, Sender))
			return FMRES_IGNORED;
	}
	else	//мертвый говорун
	{
		if(!g_isAlivePlayer[Receiver])
			return FMRES_IGNORED;
			
		if(/*g_iPlayerTeam[Receiver] != g_iPlayerTeam[Sender]*/IsEnemy(Receiver, Sender))
			return FnCanHearSender(Receiver, Sender, false);
		
		if(cv_iAllTalk != 0 && cv_iAllTalk != 4)
			return FMRES_IGNORED;
	}

	return FnCanHearSender(Receiver, Sender, false);
	#endif
}

FnCanHearSender(Receiver, Sender, bool:status)
{
	#if defined _reapi_included
		#pragma unused Receiver, Sender
		// SetHookChainReturn(ATYPE_BOOL, status);
		SetHookChainReturn(ATYPE_BOOL, status);
	#else
		engfunc(EngFunc_SetClientListening, Receiver, Sender, status);
		forward_return(FMV_CELL, status);
	#endif

	return FMRES_SUPERCEDE;
}

public HookSayChat(id)
{
	new szMsg[MAX_CHAT_MSG_LENGTH]; 
	read_args(szMsg, charsmax(szMsg));
	remove_quotes(szMsg);

	if(TrieKeyExists(g_trhWhiteListCmdArray, szMsg))
		return PLUGIN_CONTINUE;

	if(BLOCK_STATUS_NONE < cmsgag_is_user_blocked(id) < BLOCK_STATUS_VOICE)
		return FnCheckBlockStatus(id, BLOCK_STATUS_CHAT);

	return PLUGIN_CONTINUE;
}

stock FnCheckBlockStatus(id, iBlockType)
{
	new szAuthId[MAX_AUTHID_LENGTH];
	get_user_authid(id, szAuthId,  charsmax(szAuthId));
	if(TrieGetArray(g_trhBlockedUsers,  szAuthId, plBlockInfo[id], charsmax(plBlockInfo[])))
	{
		if(plBlockInfo[id][GExpired] > get_systime() || plBlockInfo[id][GExpired] <= TIME_NOT_EXPIRING)
		{
			client_print_color(id, 0, "%L %L %L", LANG_PLAYER, "CMSGAG_CHAT_PREFIX", 
				LANG_PLAYER, iBlockType == BLOCK_STATUS_CHAT ? "CMSGAG_CHAT_BLOCK_VOICE" : "CMSGAG_CHAT_BLOCK_CHAT", 
					FormatBlockTime(plBlockInfo[id][GBlockTime], 1, timeunit_minutes), LANG_PLAYER, "CMSGAG_CHAT_BLOCK_REASON", plBlockInfo[id][GBlockReason]);

			client_cmd(id, "play %s", g_szSoundFilePath);
			return PLUGIN_HANDLED;
		}
		else
			return FnRemoveBlockStatus(id);
	}

	return PLUGIN_CONTINUE;
}

public MessageSayText(MsgID, MsgDEST, MsgRecipient)
{
	if(MsgDEST != MSG_ONE)
		return PLUGIN_CONTINUE;

	if(IsPlayerMuted(MsgRecipient, get_msg_arg_int(1)) && g_szPlayerMuteType[MsgRecipient] != BLOCK_STATUS_VOICE)
	//if(cmsgag_is_user_muted_by(MsgRecipient, get_msg_arg_int(1)))
		return PLUGIN_HANDLED;

	return PLUGIN_CONTINUE;
}

/*======== Создание/Сохранение/Удаление блока =========*/
stock ASD_BlockFunc(id, iFuncType, iTargetClient, printChat = true)
{
	if(iFuncType == BLOCK_FUNC_REMOVE)
		return FnRemoveBlockStatus(id, printChat);

	if(TrieSetArray(g_trhBlockedUsers,  plBlockInfo[id][GAuthId], plBlockInfo[id], sizeof(plBlockInfo[])))
	{
		new iBlockType = plBlockInfo[id][GBlockType];
		new szTime[MAX_STRING_LEN];
		copy(szTime, charsmax(szTime), FormatBlockTime(plBlockInfo[id][GBlockTime], 1, timeunit_minutes));

		new szAdmName[MAX_NAME_LENGTH];
		new szMessage[MAX_CHAT_MSG_LENGTH];
		if(iFuncType == BLOCK_FUNC_ADD)
		{
			if(get_pcvar_num(cpAmxShowActivity) == 2)
			{
				get_user_name(id, szAdmName, charsmax(szAdmName));
				formatex(szMessage, charsmax(szMessage), "%L %L %L", LANG_PLAYER, "CMSGAG_CHAT_PREFIX", 
					LANG_PLAYER, "CMSGAG_CHAT_BLOCK_ADMIN_ACT", szAdmName, g_szBlockType[iBlockType], plBlockInfo[id][GName], szTime, 
						LANG_PLAYER, "CMSGAG_CHAT_BLOCK_REASON", plBlockInfo[id][GBlockReason]);
				if(printChat)
					client_print_color(0, 0, szMessage);
				FnClearText(szMessage);
				log_amx(szMessage);
			}
			else
			{
				formatex(szMessage, charsmax(szMessage), "%L %L %L", LANG_PLAYER, "CMSGAG_CHAT_PREFIX", 
					LANG_PLAYER, "CMSGAG_CHAT_BLOCK_ADMIN", plBlockInfo[id][GName], g_szBlockType[iBlockType], szTime, 
						LANG_PLAYER, "CMSGAG_CHAT_BLOCK_REASON", plBlockInfo[id][GBlockReason]);
				if(printChat)
					client_print_color(0, 0, szMessage);
				FnClearText(szMessage);
				log_amx(szMessage);
			}
		}
		else
		{
			if(get_pcvar_num(cpAmxShowActivity) == 2)
			{
				get_user_name(id, szAdmName, charsmax(szAdmName));
				formatex(szMessage, charsmax(szMessage), "%L %L", LANG_PLAYER, "CMSGAG_CHAT_PREFIX", LANG_PLAYER, "CMSGAG_CHAT_BLOCK_EDIT_ADMIN", 
					szAdmName, plBlockInfo[id][GName], g_szBlockType[iBlockType], szTime);
				if(printChat)
					client_print_color(0, 0, szMessage);
				FnClearText(szMessage);
				log_amx(szMessage);
			}
			else
			{
				formatex(szMessage, charsmax(szMessage), "%L %L", LANG_PLAYER, "CMSGAG_CHAT_PREFIX", LANG_PLAYER, "CMSGAG_CHAT_BLOCK_EDIT", 
					plBlockInfo[id][GName], g_szBlockType[iBlockType], szTime);
				if(printChat)
					client_print_color(0, 0, szMessage);
				FnClearText(szMessage);
				log_amx(szMessage);
			}
		}

		if(is_user_connected(iTargetClient))		
			FnChangeBlock(iTargetClient, iBlockType);
				
		g_iAddTime = plBlockInfo[id][GCreated];
		g_iAddLastId = iTargetClient;

	}
	else
		log_amx("ASD_BlockFunc: Error");

	arrayset(plBlockInfo[id], 0, BlockInfo);
	return PLUGIN_HANDLED;
}

/*======== Сброс блока =========*/
stock FnResetBlock(iTargetClient)
{
	ResetBit(g_bitBlockedFully, iTargetClient);
	ResetBit(g_bitBlockedChat, iTargetClient);
	ResetBit(g_bitBlockedVoice, iTargetClient);
	
	VTC_UnmuteClient(iTargetClient);
}	

/*======== Изменение блока =========*/
stock FnChangeBlock(iTargetClient, Type)
{
	FnResetBlock(iTargetClient);

	switch(Type)
	{
		case 0:
		{
			SetBit(g_bitBlockedFully, iTargetClient);
			VTC_MuteClient(iTargetClient);
		}
		case 1: SetBit(g_bitBlockedChat, iTargetClient);
		case 2:
		{
			SetBit(g_bitBlockedVoice, iTargetClient);
			VTC_MuteClient(iTargetClient);
		}
	}
}

/*======== Удаление блока =========*/		
stock FnRemoveBlockStatus(id, printChat = true)
{
	new iClient = find_player("c", plBlockInfo[id][GAuthId]);
	if(TrieDeleteKey(g_trhBlockedUsers, plBlockInfo[id][GAuthId]))
	{
		new type[MAX_NAME_LENGTH];
		copy(type, charsmax(type), g_szBlockType[plBlockInfo[id][GBlockType]]);
		
		new szMessage[MAX_CHAT_MSG_LENGTH];
		if(get_pcvar_num(cpAmxShowActivity) == 2 && (id != iClient))
		{
			new szAdmName[MAX_NAME_LENGTH];
			get_user_name(id, szAdmName, charsmax(szAdmName));
			formatex(szMessage, charsmax(szMessage), "%L %L", LANG_PLAYER, "CMSGAG_CHAT_PREFIX", 
				LANG_PLAYER, "CMSGAG_CHAT_BLOCK_REMOVE_ADMIN", szAdmName, type, plBlockInfo[id][GName]);
			
			if(printChat)
				client_print_color(0, 0, szMessage);
			
			FnClearText(szMessage);
			log_amx(szMessage);
		}
		else
		{
			formatex(szMessage, charsmax(szMessage), "%L %L", LANG_PLAYER, "CMSGAG_CHAT_PREFIX", 
				LANG_PLAYER, "CMSGAG_CHAT_BLOCK_REMOVE", plBlockInfo[id][GName], type);
			
			if(printChat)
				client_print_color(0, 0, szMessage);
			
			FnClearText(szMessage);
			log_amx(szMessage);
		}
	
		arrayset(plBlockInfo[id], 0, BlockInfo);
		FnResetBlock(iClient);
		VTC_UnmuteClient(iClient);
		
		return PLUGIN_HANDLED;
	}
	else
		log_amx("FnRemoveBlockStatus: Error");

	return PLUGIN_CONTINUE;
}

/*======== Стоки / нативы =========*/
stock bool:FnAboveImmunity(requester, target, bool:byFlags = false)
{
	if(byFlags)
		return (FnImmunityLevel(requester) > FnImmunityLevel(target));
		
	return (g_iImmunLevel[requester] == g_iImmunLevelsNum || g_iImmunLevel[requester] > g_iImmunLevel[target]);
}
	
stock FnImmunityLevel(flags)
{
	static i;
	i = g_iImmunLevelsNum;
	while(i > 0)
	{
		i--;
		if(flags & g_bitAccessLvl[i])
			break;
	}

	return i+1;
}

stock FormatBlockTime(iTime, type, unitType) 
{
	new szTime[MAX_STRING_LEN];
	switch(iTime)
	{
		case TIME_MAP_END_EXPIRING: formatex(szTime, charsmax(szTime), "%L", LANG_PLAYER, type == 1 ? "CMSGAG_TIME_LENGTH_1" : "CMSGAG_TIME_LENGTH_2");
		case TIME_NOT_EXPIRING: formatex(szTime, charsmax(szTime), "%L", LANG_PLAYER, type == 1 ? "CMSGAG_TIME_NON_EXPIRED_1" : "CMSGAG_TIME_NON_EXPIRED_2");
		default: get_time_length(0, iTime, unitType, szTime, charsmax(szTime));
	}

	return szTime;
}

FnClearText(langText[])
{
	replace_all(langText, MAX_CHAT_MSG_LENGTH, "^4", "");
	replace_all(langText, MAX_CHAT_MSG_LENGTH, "^1", "");
	replace_all(langText, MAX_CHAT_MSG_LENGTH, "^3", "");
}

public plugin_end()
{
	if(g_arhBlockTime)
		ArrayDestroy(g_arhBlockTime);
	if(g_arhBlockReasons)
		ArrayDestroy(g_arhBlockReasons);
}

public plugin_natives() 
{
	set_native_filter("native_filter");
	register_native("cmsgag_is_user_blocked", "native_cmsgag_is_user_blocked");
	register_native("cmsgag_is_user_muted_by", "native_cmsgag_is_user_muted_by");
	register_native("cmsgag_add_user_block", "native_cmsgag_add_user_block");
}

public native_filter(const GName[], index, trap)
	return !trap ? PLUGIN_HANDLED : PLUGIN_CONTINUE;

/*
	Проверка текущего блока от Админа
	@index - индекс игрока
	0-	voice + chat
	1-	chat
	2-	voice
	-1- not blocked
*/
public native_cmsgag_is_user_blocked()
{
	new id = get_param(1);
	return GetBit(g_bitBlockedFully, id) ?
		BLOCK_STATUS_ALL : GetBit(g_bitBlockedChat, id) ?
			BLOCK_STATUS_CHAT : GetBit(g_bitBlockedVoice, id) ?
				BLOCK_STATUS_VOICE : BLOCK_STATUS_NONE;
}

/*
	Проверка текущего блока от игрока
	@iSender - индекс отправителя 
	@receiver - индекс получателя 
*/
public native_cmsgag_is_user_muted_by()
{
	static iClient;
	iClient = get_param(1);
	
	if(IsPlayerMuted(iClient, get_param(2)))
		return g_szPlayerMuteType[iClient];
	return BLOCK_STATUS_NONE;

	//return IsPlayerMuted(g_bitPlayerMuted[get_param(1)], get_param(2));
}

/**======== Добавление блока =========
*
*	@id- индекс игрока
*	@iFuncType - тип операции блокировки(1- создание, 2- удаление)
*	@iBlockType - тип блока (чат / голос)
*	@szBlockReason[] - причина
*	@iTime - время
*	native_cmsgag_add_user_block(id, iFuncType, const iBlockType, const szBlockReason[], const iTime);
*/
public native_cmsgag_add_user_block()
{
	new szReason[MAX_REASON_LENGTH], iTargetClient;
	
	iTargetClient = get_param(1);
	get_string(4, szReason, charsmax(szReason));
	get_user_authid(iTargetClient, plBlockInfo[iTargetClient][GAuthId], charsmax(plBlockInfo[][GAuthId]));
	
	return BlockQueryFunc(iTargetClient, get_param(2), get_param(3), szReason, get_param(5), bool:get_param(6));
}