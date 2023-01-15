new PLUGIN_VERSION[32] = "2.36.7";
// крупные изменения / новый функционал / мелкие правки&доработки

//#define DEVELOPER_OPTIONS		// комминтим перед тем как залить на серв :D
//#define SQL_DEBUG				// отладка работы sql
#define ENABLE_QUESTS			// включение квестов (если офнуть, то редачить работу с БД)
#define ENABLE_BRACELETS		// включение наручников (если офнуть, то редачить работу с БД)
#define ENABLE_IP_PASS_SAVE		// сохранение ip+pass (если офнуть, править работу с БД) (делалось для теста, убрать)
#define ENABLE_CACHE			// сохранение данных в кеш при выходе игрока
#pragma semicolon 1				// ;
#pragma tabsize 1				// Табуляция
#define MEMBER_UNSAFE		 	// If you want to use s/get_member unsafe version

native cmsapi_is_user_member(id);
native cmsapi_get_user_services(id, z[], fl[], i);
native cmsapi_is_admin_active(id, szReason[], iLen, const srvName[]="");
forward OnAPISendChatPrefix(id, prefix[], type);
forward OnAPIMemberConnected(id, memberId, memberName[]);
forward OnAPIAdminConnected(id, const szName[], adminID, Flags);
forward OnAPIAdminDisconnected(id);
forward OnAPIAdminsLoaded();

#include <amxmodx>
#include <amxmisc>
#include <fakemeta>
#include <engine>
#include <hamsandwich>
#include <sqlx>
#include <reapi>
#include <xs>
#include <cvars_manager>

// Header files that contain function declarations and variables
#include "jbe_core\macroses.h"
#include "jbe_core\main.h"
#include "jbe_core\message.h"
#include "jbe_core\chat\chat.h"
#include "jbe_core\grab\grab.h"
#include "jbe_core\rank\rank.h"
#include "jbe_core\quests\quests.h"
#include "jbe_core\bans\bans.h"
#include "jbe_core\sql\sql.h"
#include "jbe_core\global\global.h"
#include "jbe_core\other\prison_reason.h"
#include "jbe_core\p_models\p_models.h"
#include "jbe_core\hook\hook.h"
#include "jbe_core\menus.h"
#include "jbe_core\deathmatch\deathmatch.h"
#include "jbe_core\chief\chief.h"
#include "jbe_core\box\box.h"
#include "jbe_core\soccer\soccer.h"
#include "jbe_core\admin\admin.h"
#include "jbe_core\betmenu\betmenu.h"
#include "jbe_core\duel\duel.h"
#include "jbe_core\shop\shop.h"
#include "jbe_core\vip\vip.h"
#include "jbe_core\gr_block\gr_block.h"
#include "jbe_core\private_messages\private_messages.h"
#include "jbe_core\bracelets\bracelets.h"
#include "jbe_core\mafia\mafia.h"
#include "jbe_core\jihad\jihad.h"

// Source Code
#include "jbe_core\message_init.inl"
#include "jbe_core\resource.inl"
#include "jbe_core\stocks.inl"
#include "jbe_core\game_mode_init.inl"
#include "jbe_core\grab\grab_func.inl"
#include "jbe_core\sql\sql_func.inl"
#include "jbe_core\p_models\p_models_func.inl"
#include "jbe_core\hook\hook_func.inl"
#include "jbe_core\vip\vip_func.inl"
#include "jbe_core\chief\chief_func.inl"
#include "jbe_core\global\global_func.inl"
#include "jbe_core\box\box_func.inl"
#include "jbe_core\soccer\soccer_func.inl"
#include "jbe_core\admin\admin_func.inl"
#include "jbe_core\chat\chat_func.inl"
#include "jbe_core\shop\shop_func.inl"
#include "jbe_core\betmenu\betmenu_func.inl"
#include "jbe_core\duel\duel_func.inl"
#include "jbe_core\rank\rank_func.inl"
#include "jbe_core\fakemeta_init.inl"
#include "jbe_core\hamsandwich_init.inl"
#include "jbe_core\event_init.inl"
#include "jbe_core\reapi_init.inl"
#include "jbe_core\menu_init.inl"
#include "jbe_core\files.inl"
#include "jbe_core\clcmd_init.inl"
#include "jbe_core\cvars_init.inl"
#include "jbe_core\door_init.inl"
#include "jbe_core\natives.inl"
#include "jbe_core\other.inl"
#include "jbe_core\bans\bans_func.inl"
#include "jbe_core\gr_block\gr_block_func.inl"
#include "jbe_core\private_messages\pm_func.inl"
#include "jbe_core\quests\quests_func.inl"
#include "jbe_core\bracelets\bracelets_func.inl"
#include "jbe_core\mafia\mafia_func.inl"
#include "jbe_core\jihad\jihad_func.inl"
#include "jbe_core\other\informers.inl"
#include "jbe_core\other\player_check.inl"
#include "jbe_core\other\spectator_bots.inl"
#include "jbe_core\deathmatch\deathmatch_func.inl" // херит менюшки в подключаемом ниже .inl

#if defined DEVELOPER_OPTIONS
	#include "jbe_core/developer_init.inl"
#endif

public plugin_precache()
{
	new szConfigFile[128];
	get_localinfo("amxx_configsdir", szConfigFile, sizeof(szConfigFile));
	format(szConfigFile, charsmax(szConfigFile), "%s/jb_engine/resources.ini", szConfigFile);
	plugins_load_cvars(szConfigFile);

	files_precache();
	models_precache();
	sounds_precache();
	sprites_precache();
	jbe_personal_models_read_file();
	jbe_create_buyzone();
	
	g_tButtonList = TrieCreate();
	g_iFakeMetaKeyValue = register_forward(FM_KeyValue, "FakeMeta_KeyValue_Post", 1);

	g_tRemoveEntities = TrieCreate();
	new szRemoveEntities[][] = {
		"func_hostage_rescue", "info_hostage_rescue", "func_bomb_target",
		"info_bomb_target", "func_vip_safetyzone", "info_vip_start",
		"func_escapezone", "hostage_entity", "monster_scientist", "func_buyzone"};
	for (new i; i < sizeof(szRemoveEntities); i++)
		TrieSetCell(g_tRemoveEntities, szRemoveEntities[i], i);

	g_iFakeMetaSpawn = register_forward(FM_Spawn, "FakeMeta_Spawn", 0);

// для item pickup при подбирании мяча
//	precache_generic("sprites/csone/ball.txt");
//	precache_model("sprites/csone/ball.spr");
}

public plugin_init()
{
	main_init();
	cvars_init();
	event_init();
	clcmd_init();
	menu_init();
	message_init();
	door_init();
	fakemeta_init();
	hamsandwich_init();
	reapi_init();
	game_mode_init();
	fix_venicle_spawn_init();
	quests_init();
	bracelets_init();
	duel_init();
	bans_init();
	private_messages_init();
	personal_models_init();
	guard_block_init();
	admin_init();
	bets_init();
	hook_init();
	grab_init();
	shop_init();
	soccer_init();
	mafia_init();
	jihad_init();
	chat_init(); // вниз чтоб не перебивал команды с '!' и '.'

#if defined DEVELOPER_OPTIONS
	developer_init();
#endif
	
	format(PLUGIN_VERSION, charsmax(PLUGIN_VERSION), "%s (%d)", PLUGIN_VERSION, valve_build_number(__DATE__) - valve_build_number("07/09/2017")); // 01/04/2016
}

/*===== -> Основное -> =====*///{

main_init()
{
	register_plugin("[JBE] Core", PLUGIN_VERSION, "Sanlerus & Serp");
	register_dictionary("jbe_core.txt");

	g_iSyncMainInformer = CreateHudSyncObj();
	g_iSyncSoccerScore = CreateHudSyncObj();
	g_iSyncStatusText = CreateHudSyncObj();
	g_iSyncDuelInformer = CreateHudSyncObj();
	g_iSyncDamageAttacker = CreateHudSyncObj();
	g_iSyncDamageVictim = CreateHudSyncObj();
	g_iSyncDeathMatchInformer = CreateHudSyncObj();
	g_iSyncInfoCenter = CreateHudSyncObj();
	g_iSyncQuestInfo = CreateHudSyncObj();
	
	g_iSqlLastUpdateTime = get_systime();

#if defined ENABLE_CACHE
	g_aUsersCache = ArrayCreate(DATA_USER_CACHE);
	g_aBansCache = ArrayCreate(DATA_BANS_CACHE);
#endif

	enable_task_player_check();
}

public client_connect(id)
{
	if (is_user_bot(id))
		return;

	new aDataRoundSound[DATA_ROUND_SOUND];
	new iTrack = random_num(0, g_iRoundSoundSize - 1);
	ArrayGetArray(g_aDataRoundSound, iTrack, aDataRoundSound);
	client_cmd(id, "mp3 play sound/CsOneRu/jb/round_sound/%s.mp3", aDataRoundSound[FILE_NAME]);
}

public client_putinserver(id)
{

#if !defined DEVELOPER_OPTIONS
	if (is_user_bot(id))
#else
	if (is_user_bot(id) && !dev_is_accept_bot(id))
#endif
		return;

#if defined ENABLE_CACHE
	if (load_user_bans_cache(id))
		return;
#endif

	SetBit(g_iBitUserConnected, id);
	g_iPlayersNum[g_iUserTeam[id]]++;
	SetBit(g_iBitUserRoundSound, id);
	SetBit(g_iBitShowDeathMenu, id);
	jbe_reset_user_informer(id);
	jbe_get_user_privilegies(id);
	set_max_user_limits(id);
	user_quest_reset(id); // Обнуление квестов
	jbe_set_random_skin(id);

	if (is_user_steam(id))
	{
		SetBit(g_iBitUserSteam, id);
		ClearBit(g_iBitUserRusChat, id);
	}
	else
	{
		ClearBit(g_iBitUserSteam, id);
		SetBit(g_iBitUserRusChat, id);
	}

#if !defined DEVELOPER_OPTIONS
	if (is_user_bot(id)) return;
#endif

#if defined ENABLE_CACHE
	if (!load_user_data_cache(id))
#endif
		load_user_mysql(id);
	
	user_demo_record(id);
	bans_user_putinserver(id);
	check_adverts_name(id);
}

public OnAPIMemberConnected(id, memberId, memberName[])
{
	copy(g_szMemberName[id], charsmax(g_szMemberName[]), memberName);
}

public client_disconnected(id)
{
	if (IsNotSetBit(g_iBitUserConnected, id)) return;

	ClearBit(g_iBitUserConnected, id);
	g_iPlayersNum[g_iUserTeam[id]]--;

	if (IsSetBit(g_iBitUserAlive, id))
	{
		g_iAlivePlayersNum[g_iUserTeam[id]]--;
		ClearBit(g_iBitUserAlive, id);
	}

	if (g_iUserTeam[id] == TEAM_PRISONER)
	{
		if (g_iDayMode == DAY_FREEDAY && g_iAlivePlayersNum[TEAM_PRISONER] == 2)
			jbe_free_day_ended();

		if (g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY)
			jbe_check_last_prisoner();

		if (IsSetBit(g_iBitUserFree, id))
			jbe_sub_user_free(id);

		else if (IsSetBit(g_iBitUserWanted, id))
			jbe_sub_user_wanted(id);

		else if (IsSetBit(g_iBitUserBoxing, id))
		{
			ClearBit(g_iBitUserBoxing, id);
			if (!g_iBitUserBoxing)
				jbe_boxing_disable_all();
		}
	}

	else if (g_iUserTeam[id] == TEAM_GUARD)
	{
		if (id == g_iChiefId)
		{
			g_iChiefId = 0;
			g_iChiefStatus = CHIEF_DISCONNECT;
			jbe_set_name_chief();
		}

		if (g_iPlayersNum[TEAM_GUARD] == 0)
		{
			static msgid = 0;
			if (!msgid) 
				msgid = get_user_msgid("DeathMsg");
			new msgblock = get_msg_block(msgid);
			set_msg_block(msgid, BLOCK_ONCE);

			for (new i = 1; i <= MaxClients; i++)
			{
				if (IsNotSetBit(g_iBitUserAlive, i)) continue;
				user_kill(i, 1);
			}
			set_msg_block(msgid, msgblock);
		}
	}

	if (g_iDuelStatus && IsSetBit(g_iBitUserDuel, id))
		jbe_duel_ended(id, .bWinDuel = false);

	if (g_iDayMode == DAY_DEATHMATCH)
		jbe_deathmatch_check();

#if defined ENABLE_CACHE
	save_user_data_cache(id);
#endif

	save_user_mysql(id);
	bans_user_disconnect(id);
	grab_off_to_user(id);
	
	jbe_sub_user_soccer(id);
	jbe_clear_user_shop(id);
	jbe_clear_user_disconnect(id);
	jbe_clear_user_privilegies(id);
}

/*===== <- Основное <- =====*///}

/*===== -> Квары -> =====*///{
public plugin_cfg()
{
	new szCfgDir[64];
	get_localinfo("amxx_configsdir", szCfgDir, charsmax(szCfgDir));
	server_cmd("exec %s/jb_engine/shop_cvars.cfg", szCfgDir);
	server_cmd("exec %s/jb_engine/all_cvars.cfg", szCfgDir);
	set_task(1.0, "jbe_get_cvars");
}
/*===== <- Квары <- =====*///}

public plugin_end()
{
	if (g_sqlTuple)
		SQL_FreeHandle(g_sqlTuple);

	log_amx("plugin_end()");
}