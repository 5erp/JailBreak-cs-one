cvars_init()
{
	register_cvar("jbe_core", PLUGIN_VERSION, FCVAR_SERVER|FCVAR_UNLOGGED);

	// Shop
	bind_pcvar_num(create_cvar("jbe_price_balisong", "320", _, "Price - Нож-Бабочка"), g_eShopCvars[BALISONG]);
	bind_pcvar_num(create_cvar("jbe_price_basaballbat", "400", _, "Price - Бита"), g_eShopCvars[BASABALLBAT]);
	bind_pcvar_num(create_cvar("jbe_price_guard_baton", "380", _, "Price - Дубинка"), g_eShopCvars[GUARD_BATON]);
	bind_pcvar_num(create_cvar("jbe_price_usp", "400", _, "Price - USP"), g_eShopCvars[USP]);
	bind_pcvar_num(create_cvar("jbe_price_latchkey", "150", _, "Price - Отмычка"), g_eShopCvars[LATCHKEY]);
	bind_pcvar_num(create_cvar("jbe_price_kokain", "200", _, "Price - Доза кокаина"), g_eShopCvars[KOKAIN]);
	bind_pcvar_num(create_cvar("jbe_price_stimulator", "230", _, "Price - Стимулятор"), g_eShopCvars[STIMULATOR]);
	bind_pcvar_num(create_cvar("jbe_price_frostnade", "170", _, "Price - Граната-заморозка"), g_eShopCvars[FROSTNADE]);
	bind_pcvar_num(create_cvar("jbe_price_armor", "70", _, "Price - Бронижелет"), g_eShopCvars[ARMOR]);
	bind_pcvar_num(create_cvar("jbe_price_clothing_guard", "300", _, "Price - Одежда охраны"), g_eShopCvars[CLOTHING_GUARD]);
	bind_pcvar_num(create_cvar("jbe_price_hing_jump", "200", _, "Price - Высокий прыжок"), g_eShopCvars[HING_JUMP]);
	bind_pcvar_num(create_cvar("jbe_price_fast_run", "240", _, "Price - Быстрый бег"), g_eShopCvars[FAST_RUN]);
	bind_pcvar_num(create_cvar("jbe_price_double_jump", "280", _, "Price - Двойной прыжок"), g_eShopCvars[DOUBLE_JUMP]);
	bind_pcvar_num(create_cvar("jbe_price_fall_no_dmg", "100", _, "Price - Защита от падений"), g_eShopCvars[FALL_NO_DMG]);
	bind_pcvar_num(create_cvar("jbe_price_auto_bhop", "180", _, "Price - Автораспрыг"), g_eShopCvars[AUTO_BHOP]);
	bind_pcvar_num(create_cvar("jbe_price_double_damage", "250", _, "Price - Двойной урон"), g_eShopCvars[DOUBLE_DAMAGE]);
	bind_pcvar_num(create_cvar("jbe_price_low_gravity", "220", _, "Price - Низкая гравитация"), g_eShopCvars[LOW_GRAVITY]);
	bind_pcvar_num(create_cvar("jbe_price_long_jump", "130", _, "Price - Длинный прыжок"), g_eShopCvars[LONG_JUMP]);
	bind_pcvar_num(create_cvar("jbe_price_close_case", "250", _, "Price - Закрыть дело"), g_eShopCvars[CLOSE_CASE]);
	bind_pcvar_num(create_cvar("jbe_price_free_day", "300", _, "Price - Свободный день"), g_eShopCvars[FREE_DAY_SHOP]);
	bind_pcvar_num(create_cvar("jbe_price_resolution_voice", "400", _, "Price - Разрешение на голос"), g_eShopCvars[RESOLUTION_VOICE]);
	bind_pcvar_num(create_cvar("jbe_price_transfer_guard", "800", _, "Price - Перевод за охрану"), g_eShopCvars[TRANSFER_GUARD]);
	bind_pcvar_num(create_cvar("jbe_price_lottery_ticket", "150", _, "Price - Моментальная лотерея"), g_eShopCvars[LOTTERY_TICKET]);
	bind_pcvar_num(create_cvar("jbe_price_prank_prisoner", "350", _, "Price - Подставить заключенного"), g_eShopCvars[PRANK_PRISONER]);
	bind_pcvar_num(create_cvar("jbe_price_electro_costume", "300", _, "Price - "), g_eShopCvars[ELECTRO_COSTUME]);
	bind_pcvar_num(create_cvar("jbe_price_skin", "300", _, "Price - Смена одежды"), g_eShopCvars[SKIN]);
	bind_pcvar_num(create_cvar("jbe_price_grenades", "200", _, "Price - Гранаты"), g_eShopCvars[GRENADES]);
	bind_pcvar_num(create_cvar("jbe_price_weapons_gr", "100", _, "Price - Оружейная охраны"), g_eShopCvars[WEAPONS_GR]);
	bind_pcvar_num(create_cvar("jbe_shop_open_time", "40", _, "лимит времени на магазин"), g_eShopCvars[SHOP_OPEN_TIME]);
	bind_pcvar_num(create_cvar("jbe_price_buy_respawn", "60", _, "Покупка возрождения"), g_eShopCvars[BUY_RESPAWN]);
	
	// All cvars
	bind_pcvar_num(create_cvar("jbe_free_day_id_time", "120", _, "длительность свободного дня одного заключенного"), g_eAllCvars[FREE_DAY_ID]);
	bind_pcvar_num(create_cvar("jbe_free_day_all_time", "240", _, "длительность общего свободного дня"), g_eAllCvars[FREE_DAY_ALL]);
	bind_pcvar_num(create_cvar("jbe_team_balance", "4", _, "баланс заключеных на одного охранника"), g_eAllCvars[TEAM_BALANCE]);
	bind_pcvar_num(create_cvar("jbe_day_mode_vote_time", "15", _, "Длительность голосования за режим игрового дня"), g_eAllCvars[DAY_MODE_VOTE_TIME]);
	bind_pcvar_num(create_cvar("jbe_restart_game_time", "40", _, "Длительность рестарта игры после смены карты"), g_eAllCvars[RESTART_GAME_TIME]);
	bind_pcvar_num(create_cvar("jbe_riot_start_money", "30", _, "сколько давать денег за начало бунта"), g_eAllCvars[RIOT_START_MODEY]);
	bind_pcvar_num(create_cvar("jbe_killed_guard_money", "40", _, "сколько давать денег за убийство охраника"), g_eAllCvars[KILLED_GUARD_MODEY]);
	bind_pcvar_num(create_cvar("jbe_killed_chief_money", "65", _, "сколько давать денег за убийство начальника"), g_eAllCvars[KILLED_CHIEF_MODEY]);
	bind_pcvar_num(create_cvar("jbe_killed_wanted_money", "65", _, "сколько давать денег за убийство розыска"), g_eAllCvars[KILLED_WANTED_MODEY]);
	bind_pcvar_num(create_cvar("jbe_round_free_money", "10", _, "сколько давать денег в начале каждого раунда"), g_eAllCvars[ROUND_FREE_MODEY]);
	bind_pcvar_num(create_cvar("jbe_round_alive_money", "20", _, "сколько давать денег в начале раунда если остался жив"), g_eAllCvars[ROUND_ALIVE_MODEY]);
	bind_pcvar_num(create_cvar("jbe_last_prisoner_money", "300", _, "сумма в меню последнего заключенного"), g_eAllCvars[LAST_PRISONER_MODEY]);
	bind_pcvar_num(create_cvar("jbe_shop_setspeed", "400", _, "скорость выдаваемая в магазине"), g_eAllCvars[SHOP_SPEED]);
	bind_pcvar_num(create_cvar("jbe_changeteam_time", "10", _, "как часто можно менять команду через меню (в сек); 0 - без ограничения"), g_eAllCvars[CHANGETEAM_TIME]);
	bind_pcvar_string(create_cvar("jbe_game_name", "JailBreak", _, "Имя игры"), g_eAllCvars[GAME_NAME], charsmax(g_eAllCvars[GAME_NAME]));
	bind_pcvar_string(create_cvar("jbe_game_name_night", "FREE VIP", _, "Имя игры при бесплатном вип"), g_eAllCvars[GAME_NAME_NIGHT], charsmax(g_eAllCvars[GAME_NAME_NIGHT]));

	// VIP
	bind_pcvar_num(create_cvar("jbe_vip_respawn_num", "2", _, "сколько раз за раунд может возродится вип игрок"), g_eAllCvars[VIP_RESPAWN_NUM]);
	bind_pcvar_num(create_cvar("jbe_vip_health_num", "3", _, "сколько раз за раунд может подлечится вип игрок"), g_eAllCvars[VIP_HEALTH_NUM]);
	bind_pcvar_num(create_cvar("jbe_vip_money_num", "1000", _, "сколько можно взять денег через вип меню"), g_eAllCvars[VIP_MONEY_NUM]);
	bind_pcvar_num(create_cvar("jbe_vip_money_round", "10", _, "через сколько раундов можно взять деньги через вип меню"), g_eAllCvars[VIP_MONEY_ROUND]);
	bind_pcvar_num(create_cvar("jbe_vip_invisible_round", "4", _, "через сколько раундов можно взять невидимость через вип меню"), g_eAllCvars[VIP_INVISIBLE]);
	bind_pcvar_num(create_cvar("jbe_vip_hp_ap_round", "2", _, "через сколько раундов можно взять HP и AP через вип меню"), g_eAllCvars[VIP_HP_AP_ROUND]);
	bind_pcvar_num(create_cvar("jbe_vip_ghost_round", "3", _, "через сколько раундов можно взять призрака"), g_eAllCvars[VIP_GHOST_ROUND]);
	bind_pcvar_num(create_cvar("jbe_vip_god_round", "5", _, "через сколько рнд можно брать бесмертие"), g_eAllCvars[VIP_GOD_ROUND]);
	bind_pcvar_num(create_cvar("jbe_vip_footsteps_round", "2", _, "через сколько раундов можно взять бесшумные шаги через вип"), g_eAllCvars[VIP_FOOTSTEPS_ROUND]);
	bind_pcvar_num(create_cvar("jbe_respawn_player_num", "2", _, "при каком количестве игроков в своей команде, игрок с привилегиями может возродится"), g_eAllCvars[RESPAWN_PLAYER_NUM]);
	bind_pcvar_num(create_cvar("jbe_respawn_players", "4", _, "сколько игроков можно возродить за раунд випу"), g_eAllCvars[VIP_RESPAWN_PLAYERS]);
	bind_pcvar_num(create_cvar("jbe_vip_night_start", "22", _, "начало ночного вип (часы)"), g_eAllCvars[VIP_NIGHT_START]);
	bind_pcvar_num(create_cvar("jbe_vip_night_ended", "7", _, "конец ночного вип (часы)"), g_eAllCvars[VIP_NIGHT_ENDED]);
	bind_pcvar_string(create_cvar("jbe_vip_night_prefix", "Night VIP", _, "префикс ночного випа"), g_eAllCvars[VIP_NIGHT_PREFIX], charsmax(g_eAllCvars[VIP_NIGHT_PREFIX]));

	// Rank Bonus 
	bind_pcvar_num(create_cvar("jbe_rank_bonus_money", "5", _, "через сколько раундов можно взять деньги"), g_eRankBonusRound[RB_MONEY]);
	bind_pcvar_num(create_cvar("jbe_rank_bonus_health", "8", _, "через сколько раундов можно взять жизни"), g_eRankBonusRound[RB_HEALTH]);
	bind_pcvar_num(create_cvar("jbe_rank_bonus_grenades", "7", _, "через сколько раундов можно взять гранаты"), g_eRankBonusRound[RB_GRENADES]);
	bind_pcvar_num(create_cvar("jbe_rank_bonus_freeday", "9", _, "через сколько раундов можно взять фд"), g_eRankBonusRound[RB_FREEDAY]);
	bind_pcvar_num(create_cvar("jbe_rank_bonus_glock", "10", _, "через сколько раундов можно взять глок"), g_eRankBonusRound[RB_GLOCK]);
	bind_pcvar_num(create_cvar("jbe_rank_bonus_el_costume", "8", _, "через сколько раундов можно взять эллектро костюм"), g_eRankBonusRound[RB_EL_COSTUME]);
	bind_pcvar_num(create_cvar("jbe_rank_bonus_kokain", "5", _, "через сколько раундов можно взять кокаин"), g_eRankBonusRound[RB_KOKAIN]);
	bind_pcvar_num(create_cvar("jbe_rank_bonus_god", "10", _, "через сколько раундов можно взять бессмертие"), g_eRankBonusRound[RB_GOD]);

	// DeathMatch
	bind_pcvar_num(create_cvar("jbe_dm_remove_guns", "8", _, "через сколько удалять оружие в ДМ режиме (в секундах)"), g_eAllCvars[DEATHMATCH_REMOVE_GUNS]);
	bind_pcvar_num(create_cvar("jbe_dm_update_time", "360", _, "частота начисления денег и прочего в ДМ (в секундах)"), g_eAllCvars[DEATHMATCH_UPDATE_TIME]);
	bind_pcvar_num(create_cvar("jbe_dm_max_players", "0", _, "максимум игроков для активного дм, если больше то дм выключаеться; 0 - без лимита"), g_eAllCvars[DEATHMATCH_MAX_PLAYERS]);
	bind_pcvar_num(create_cvar("jbe_dm_radar", "0", _, "радар в ДМ режиме; -1 - всегда выкл; 0 - всегда вкл; другое значение число игроков с которого радар выключаеться"), g_eAllCvars[DEATHMATCH_RADAR]);

	// Data base
	bind_pcvar_string(create_cvar("jbe_sql_host", "localhost", FCVAR_PROTECTED), g_szSqlHost, charsmax(g_szSqlHost));
	bind_pcvar_string(create_cvar("jbe_sql_user", "user", FCVAR_PROTECTED), g_szSqlUser, charsmax(g_szSqlUser));
	bind_pcvar_string(create_cvar("jbe_sql_password", "", FCVAR_PROTECTED), g_szSqlPass, charsmax(g_szSqlPass));
	bind_pcvar_string(create_cvar("jbe_sql_database", "database", FCVAR_PROTECTED), g_szSqlDB, charsmax(g_szSqlDB));
	bind_pcvar_string(create_cvar("jbe_sql_table", "jbe_rank", FCVAR_PROTECTED), g_szRankTable, charsmax(g_szRankTable));
}

public jbe_get_cvars()
{
	g_sqlTuple = SQL_MakeDbTuple(g_szSqlHost, g_szSqlUser, g_szSqlPass, g_szSqlDB, 5);
	SQL_SetCharset(g_sqlTuple, "utf8");

	set_member_game(m_GameDesc, fmt(" | %s | ", g_eAllCvars[GAME_NAME]));

	cvar_sv_maxspeed		= get_cvar_num("sv_maxspeed");
	cvar_sv_gravity			= get_cvar_num("sv_gravity");
	cvar_mp_roundtime		= get_cvar_num("mp_roundtime");
	cvar_mp_item_staytime	= get_cvar_num("mp_item_staytime");

	bans_cfg_init(); // Банс
	spectbot_check();
	check_vip_night();
	fnHudAdvertsRead();
	
	g_iRankMaxLevel				= sizeof(g_iszRankExpName);
	g_iSizePrisonReason1		= charsmax(g_szPrisonReason1);
	g_iSizePrisonReason2		= charsmax(g_szPrisonReason2);
	
	g_eHookSettingSize[HOOK_SPEED]			= sizeof(g_iszHookSpeed);
	g_eHookSettingSize[HOOK_WIDTH]			= sizeof(g_iHookWidth);
	g_eHookSettingSize[HOOK_AMPLITUDE]		= sizeof(g_iHookAmplitude);
	g_eHookSettingSize[HOOK_COLOUR]			= sizeof(g_iszHookColour);
	g_eHookSettingSize[HOOK_TRAIL_COLOUR]	= sizeof(g_iszHookColour);
	g_eHookSettingSize[HOOK_TRAIL_WIDTH]	= sizeof(g_iHookTrailWidth);
	g_eHookSettingSize[HOOK_TRAIL_LENGTH]	= sizeof(g_iHookTrailLength);
}

// 	fnReadChatPrefixes(); // в chat_init()
//	load_deathmatch_random_spawns();
	
//	g_eShopCvars[BALISONG]				= get_cvar_num("jbe_price_balisong");
//	g_eShopCvars[BASABALLBAT]			= get_cvar_num("jbe_price_basaballbat");
//	g_eShopCvars[GUARD_BATON]			= get_cvar_num("jbe_price_guard_baton");
//	g_eShopCvars[USP]					= get_cvar_num("jbe_price_usp");
//	g_eShopCvars[LATCHKEY]				= get_cvar_num("jbe_price_latchkey");
//	g_eShopCvars[KOKAIN]				= get_cvar_num("jbe_price_kokain");
//	g_eShopCvars[STIMULATOR]			= get_cvar_num("jbe_price_stimulator");
//	g_eShopCvars[FROSTNADE]				= get_cvar_num("jbe_price_frostnade");
//	g_eShopCvars[ARMOR]					= get_cvar_num("jbe_price_armor");
//	g_eShopCvars[CLOTHING_GUARD]		= get_cvar_num("jbe_price_clothing_guard");
//	g_eShopCvars[HEGRENADE]				= get_cvar_num("jbe_price_hegrenade");
//	g_eShopCvars[HING_JUMP]				= get_cvar_num("jbe_price_hing_jump");
//	g_eShopCvars[FAST_RUN]				= get_cvar_num("jbe_price_fast_run");
//	g_eShopCvars[DOUBLE_JUMP]			= get_cvar_num("jbe_price_double_jump");
//	g_eShopCvars[FALL_NO_DMG]			= get_cvar_num("jbe_price_fall_no_dmg");
//	g_eShopCvars[AUTO_BHOP]				= get_cvar_num("jbe_price_auto_bhop");
//	g_eShopCvars[DOUBLE_DAMAGE]			= get_cvar_num("jbe_price_double_damage");
//	g_eShopCvars[LOW_GRAVITY]			= get_cvar_num("jbe_price_low_gravity");
//	g_eShopCvars[LONG_JUMP]				= get_cvar_num("jbe_price_long_jump");
//	g_eShopCvars[CLOSE_CASE]			= get_cvar_num("jbe_price_close_case");
//	g_eShopCvars[FREE_DAY_SHOP]			= get_cvar_num("jbe_price_free_day");
//	g_eShopCvars[RESOLUTION_VOICE]		= get_cvar_num("jbe_price_resolution_voice");
//	g_eShopCvars[TRANSFER_GUARD]		= get_cvar_num("jbe_price_transfer_guard");
//	g_eShopCvars[LOTTERY_TICKET]		= get_cvar_num("jbe_price_lottery_ticket");
//	g_eShopCvars[PRANK_PRISONER]		= get_cvar_num("jbe_price_prank_prisoner");
//	g_eShopCvars[ELECTRO_COSTUME]		= get_cvar_num("jbe_price_electro_costume");
//	g_eShopCvars[WEAPONS_GR]			= get_cvar_num("jbe_price_weapons");
//	g_eShopCvars[BUY_RESPAWN]			= get_cvar_num("jbe_price_buy_respawn");
//	g_eShopCvars[GRENADES]				= get_cvar_num("jbe_price_grenades");
//	g_eShopCvars[SHOP_OPEN_TIME]		= get_cvar_num("jbe_shop_open_time");
//	g_eShopCvars[SKIN]					= get_cvar_num("jbe_price_skin");

//	g_eAllCvars[FREE_DAY_ID]			= get_cvar_num("jbe_free_day_id_time");
//	g_eAllCvars[FREE_DAY_ALL]			= get_cvar_num("jbe_free_day_all_time");
//	g_eAllCvars[TEAM_BALANCE]			= get_cvar_num("jbe_team_balance");
//	g_eAllCvars[DAY_MODE_VOTE_TIME]		= get_cvar_num("jbe_day_mode_vote_time");
//	g_eAllCvars[RESTART_GAME_TIME]		= get_cvar_num("jbe_restart_game_time");
//	g_eAllCvars[RIOT_START_MODEY]		= get_cvar_num("jbe_riot_start_money");
//	g_eAllCvars[KILLED_GUARD_MODEY]		= get_cvar_num("jbe_killed_guard_money");
//	g_eAllCvars[KILLED_CHIEF_MODEY]		= get_cvar_num("jbe_killed_chief_money");
//	g_eAllCvars[KILLED_WANTED_MODEY]	= get_cvar_num("jbe_killed_wanted_money");
//	g_eAllCvars[ROUND_FREE_MODEY]		= get_cvar_num("jbe_round_free_money");
//	g_eAllCvars[ROUND_ALIVE_MODEY]		= get_cvar_num("jbe_round_alive_money");
//	g_eAllCvars[LAST_PRISONER_MODEY]	= get_cvar_num("jbe_last_prisoner_money");
//	g_eAllCvars[VIP_RESPAWN_NUM]		= get_cvar_num("jbe_vip_respawn_num");
//	g_eAllCvars[VIP_HEALTH_NUM]			= get_cvar_num("jbe_vip_health_num");
//	g_eAllCvars[VIP_MONEY_NUM]			= get_cvar_num("jbe_vip_money_num");
//	g_eAllCvars[VIP_MONEY_ROUND]		= get_cvar_num("jbe_vip_money_round");
//	g_eAllCvars[VIP_INVISIBLE]			= get_cvar_num("jbe_vip_invisible_round");
//	g_eAllCvars[VIP_HP_AP_ROUND]		= get_cvar_num("jbe_vip_hp_ap_round");
//	g_eAllCvars[VIP_GHOST_ROUND]		= get_cvar_num("jbe_vip_ghost_round");
//	g_eAllCvars[VIP_GOD_ROUND]			= get_cvar_num("jbe_vip_god_round");
//	g_eAllCvars[VIP_FOOTSTEPS_ROUND]	= get_cvar_num("jbe_vip_footsteps_round");
//	g_eAllCvars[RESPAWN_PLAYER_NUM]		= get_cvar_num("jbe_respawn_player_num");
//	g_eAllCvars[VIP_RESPAWN_PLAYERS]	= get_cvar_num("jbe_respawn_players");
//	g_eAllCvars[VIP_NIGHT_START]		= get_cvar_num("jbe_vip_night_start");
//	g_eAllCvars[VIP_NIGHT_ENDED]		= get_cvar_num("jbe_vip_night_ended");
//	g_eAllCvars[SHOP_SPEED]				= get_cvar_num("jbe_shop_setspeed");

//	g_eAllCvars[DEATHMATCH_REMOVE_GUNS]	= get_cvar_num("jbe_dm_remove_guns");
//	g_eAllCvars[DEATHMATCH_UPDATE_TIME]	= get_cvar_num("jbe_dm_update_time");
//	g_eAllCvars[DEATHMATCH_MAX_PLAYERS]	= get_cvar_num("jbe_dm_max_players");
//	g_eAllCvars[DEATHMATCH_RADAR]		= get_cvar_num("jbe_dm_radar");
//	g_eAllCvars[DEATHMATCH_RADAR]		= get_cvar_num("jbe_changeteam_time");

//	get_cvar_string("jbe_game_name", g_eAllCvars[GAME_NAME], charsmax(g_eAllCvars[GAME_NAME]));
//	get_cvar_string("jbe_game_name_night", g_eAllCvars[GAME_NAME_NIGHT], charsmax(g_eAllCvars[GAME_NAME_NIGHT]));
//	get_cvar_string("jbe_vip_night_prefix", g_eAllCvars[VIP_NIGHT_PREFIX], charsmax(g_eAllCvars[VIP_NIGHT_PREFIX]));

//	g_eRankBonusRound[RB_MONEY]			= get_cvar_num("jbe_rank_bonus_money");
//	g_eRankBonusRound[RB_HEALTH]		= get_cvar_num("jbe_rank_bonus_health");
//	g_eRankBonusRound[RB_GRENADES]		= get_cvar_num("jbe_rank_bonus_grenades");
//	g_eRankBonusRound[RB_FREEDAY]		= get_cvar_num("jbe_rank_bonus_freeday");
//	g_eRankBonusRound[RB_GLOCK]			= get_cvar_num("jbe_rank_bonus_glock");
//	g_eRankBonusRound[RB_EL_COSTUME]	= get_cvar_num("jbe_rank_bonus_el_costume");
//	g_eRankBonusRound[RB_KOKAIN]		= get_cvar_num("jbe_rank_bonus_kokain");
//	g_eRankBonusRound[RB_GOD]			= get_cvar_num("jbe_rank_bonus_god");

//	get_cvar_string("jbe_sql_host", g_szSqlHost, charsmax(g_szSqlHost));
//	get_cvar_string("jbe_sql_user", g_szSqlUser, charsmax(g_szSqlUser));
//	get_cvar_string("jbe_sql_password", g_szSqlPass, charsmax(g_szSqlPass));
//	get_cvar_string("jbe_sql_database", g_szSqlDB, charsmax(g_szSqlDB));
//	get_cvar_string("jbe_sql_table", g_szRankTable, charsmax(g_szRankTable));
