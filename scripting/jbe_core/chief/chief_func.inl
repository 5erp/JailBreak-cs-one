#include "jbe_core\chief\chief_menus.inl"
#include "jbe_core\chief\minigame_func.inl"

//#include "jbe_core\chief\freeday_next_menu.inl"
//#include "jbe_core\chief\freeday_menu.inl"
//#include "jbe_core\chief\take_skill_users_menu.inl"

chief_menu_init()
{	
	// chief_menu.inl
	register_menucmd(register_menuid("Show_ChiefMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_ChiefMenu");
	// transfer_chief_menu
	register_menucmd(register_menuid("Show_TransferChiefMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_TransferChiefMenu");
	// countdown_menu
	register_menucmd(register_menuid("Show_CountDownMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<8|1<<9), "Handle_CountDownMenu");
	// freeday_control_menu
	register_menucmd(register_menuid("Show_FreeDayControlMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_FreeDayControlMenu");
	// punish_guard_menu
	register_menucmd(register_menuid("Show_PunishGuardMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_PunishGuardMenu");
	// treat_prisoner_menu
	register_menucmd(register_menuid("Show_TreatPrisonerMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_TreatPrisonerMenu");
	// wanted_menu
	register_menucmd(register_menuid("Show_WantedMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_WantedMenu");
	// voice_control_menu
	register_menucmd(register_menuid("Show_VoiceControlMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_VoiceControlMenu");
	// prisoners_divide_color_menu
	register_menucmd(register_menuid("Show_PrisonersDivideColorMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<7|1<<8|1<<9), "Handle_PrisonersDivideColorMenu");
	
	// chief_take_skills_menu
	register_menucmd(register_menuid("Show_ChiefTakeSkillMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_ChiefTakeSkillsMenu");
	
	// freeday_menu.inl
	register_menucmd(register_menuid("Show_FreeDayMenu"),
		(1<<0|1<<1|1<<2|1<<4|1<<6|1<<8|1<<9), "Handle_FreeDayMenu");
	// freeday_next_menu.inl
	register_menucmd(register_menuid("Show_FreeDayNextMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_FreeDayNextMenu");

	// mini game menus
	register_menucmd(register_menuid("Show_MiniGameMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_MiniGameMenu");

	register_menucmd(register_menuid("Show_DayModeChiefMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_DayModeChiefMenu");

	register_menucmd(register_menuid("Show_GlobalGamesMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_GlobalGamesMenu");

	// duel game
	register_menucmd(register_menuid("Show_GameDuelMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<8|1<<9), "Handle_GameDuelMenu");
		
	register_menucmd(register_menuid("Show_GameDuelUsersMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_GameDuelUsersMenu");

	register_menucmd(register_menuid("Show_RandomNumMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<8|1<<9), "Handle_RandomNumMenu");

	register_clcmdex("simon", "ClCmd_Simon"); // say /simon
	register_clcmdex("wanted", "ClCmd_WantedControl", 1);  // say /wanted
	register_clcmdex("smenu", "ClCmd_ChiefMenu"); // say /smenu
	register_clcmdex("ff", "ClCmd_FrendlyFire"); // say /ff
	register_clcmdex("open", "ClCmd_OpenDoors"); // say /open
	register_clcmdex("voice", "ClCmd_VoiceControl"); // say /voice
	register_clcmdex("fd", "ClCmd_FreeDayControl"); // say /fd
	register_clcmdex("hp", "ClCmd_TreatPrisonerMenu"); // say /hp
	register_clcmdex("sound", "ClCmd_Sound"); // say /sound
	register_clcmdex("music", "ClCmd_Music"); // say /music
	register_clcmdex("ball", "ClCmd_Ball"); // say /ball
	register_clcmdex("logo", "ClCmd_Logo"); // say /logo
	register_clcmdex("ts", "ClCmd_ChiefTakeSkill"); // say /ts
	register_clcmdex("score", "ClCmd_ScoreMenu"); // say /score
	register_clcmd("countdown", "ClCmd_CountDown"); // 
	register_clcmd("rnd_num", "ClCmd_RandomNum"); // 
}

public ClCmd_ChiefMenu(id)
{
	if (g_iChiefId == id)
	{		
		return Show_ChiefMenu(id);
	}
	else
		client_print_color(id, id, "%s ^1Меню доступно только Начальнику!", g_szChatPrefix);
	
	return PLUGIN_HANDLED;
}

public ClCmd_Simon(id)
{
	if (id == g_iChiefId)
	{
		return Show_ChiefMenu(id);
	}
	else if (g_iDayMode == DAY_FREEDAY && g_iDayWeek == 7 && g_iAlivePlayersNum[TEAM_PRISONER] > 2)
	{
		client_print_color(id, 0, "%s ^1В выходной брать начальника запрещено!", g_szChatPrefix);
		return PLUGIN_HANDLED;				
	}
	else if ((g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY) && IsSetBit(g_iBitUserAlive, id))
	{
		if (g_iChiefStatus != CHIEF_ALIVE && (g_iChiefIdOld != id || g_iChiefStatus != CHIEF_WAIT
		|| g_iAlivePlayersNum[TEAM_GUARD] == 1) && jbe_set_user_chief(id)) {
			g_iChiefIdOld = id;
			return Show_ChiefMenu(id);
		}
		else if (g_iChiefIdOld == id)
		{
			client_print_color(id, 0, "%s ^1Ждите ^4%d ^3сек. ^1чтоб взять начальника.", 
				g_szChatPrefix, g_iChiefChoiceTime - 30);
			return PLUGIN_HANDLED;
		}
	}
	return PLUGIN_HANDLED;
}

public ClCmd_CountDown(id)
{
#define COUNTDOWN_MAX	300 //60*5
	if (is_not_chief_menu(id))
		return PLUGIN_HANDLED;
	
	new szArg[5];
	read_argv(1, szArg, charsmax(szArg));

	if (!is_str_num(szArg))
	{
		client_print_color(id, id, "%s Введите число!", g_szChatPrefix);
		return Show_CountDownMenu(id);
	}

	g_iCountDown = str_to_num(szArg);

	if (g_iCountDown > COUNTDOWN_MAX)
	{
		client_print_color(id, id, "%s Максимум %d сек!", g_szChatPrefix, COUNTDOWN_MAX);
		return Show_CountDownMenu(id);
	}

	g_iCountDown++;

	set_task(1.0, "jbe_count_down_timer", TASK_COUNT_DOWN_TIMER, _, _, "a", g_iCountDown);

	return Show_CountDownMenu(id);
}

public ClCmd_WantedControl(id)
{
	if (g_iChiefId == id || IsSetBit(g_iBitUserAdmin, id))
	{
		return Show_WantedMenu(id, _, 1);
	} else ClCmd_InfoBuy(id);
	return PLUGIN_HANDLED;
}

public ClCmd_FrendlyFire(id)
{
	if (is_user_cmd_flood(id))
		return PLUGIN_HANDLED;
	
	if ((g_iChiefGameType == GAME_FFIRE || g_iChiefGameType == GAME_NONE)
	&& (g_iChiefId == id || IsSetBit(g_iBitUserAdmin, id)) && g_iDayMode == DAY_NORMAL) {
		jbe_minigame_friendlyfire(id);
	} else if (g_iChiefId != id && IsNotSetBit(g_iBitUserAdmin, id))
		ClCmd_InfoBuy(id);
	else if (g_iChiefGameType != GAME_FFIRE && g_iChiefGameType != GAME_NONE)
		client_print_color(id, 0, "%s Недоступно, запущенна другая игра!", g_szChatPrefix);
	else
	{
		if (g_iDayMode == DAY_DEATHMATCH && IsSetBit(g_iBitRconAdmin, id))
			jbe_minigame_friendlyfire(id);
		else
			client_print_color(id, 0, "%s Доступно только в обычный день!", g_szChatPrefix);
	}
	return PLUGIN_HANDLED;
}

public ClCmd_OpenDoors(id)
{
	if (g_iChiefId == id || IsSetBit(g_iBitUserAdmin, id))
	{

		if (is_user_cmd_flood(id, FLOOD_DOORS, 2.0, 5.0))
			return PLUGIN_HANDLED;

		if (g_bDoorStatus && id == g_iChiefId)
			jbe_close_doors();
		else
			jbe_open_doors();

		client_print_color(0, g_bDoorStatus ? print_team_blue : print_team_red,
			"%s %s ^4%n ^3%s ^1клетки.", g_szChatPrefix, 
			g_iChiefId == id ? g_szChiefPrefix : g_szAdminPrefix,
			id, g_bDoorStatus ? "открыл" : "закрыл");
	}
	else ClCmd_InfoBuy(id);
	return PLUGIN_HANDLED;
}

public ClCmd_VoiceControl(id)
{
	if (g_iChiefId == id || IsSetBit(g_iBitUserAdmin, id))
	{
		Show_VoiceControlMenu(id);
	}
	else ClCmd_InfoBuy(id);
	
	return PLUGIN_HANDLED;
}

public ClCmd_FreeDayControl(id)
{
	if (g_iChiefId == id || IsSetBit(g_iBitUserAdmin, id))
	{
		return Show_FreeDayControlMenu(id, _, 1);
	}
	else ClCmd_InfoBuy(id);
	
	return PLUGIN_HANDLED;
}

public ClCmd_Sound(id)
{
	if (id == g_iChiefId || IsSetBit(g_iBitUserAdmin, id))
	{
		if (g_iDayMode != DAY_DAYMODE && !g_bRoundEnd)
		{
			Show_SoundMenu(id);
		}
		else
		{
			client_print_color(id, id, "%s Сейчас меню музыки недоступно.", g_szChatPrefix);
		}
	}
	else ClCmd_InfoBuy(id);
	
	return PLUGIN_HANDLED;
}

public ClCmd_Music(id)
{
	if (id == g_iChiefId || IsSetBit(g_iBitUserAdmin, id))
	{
		if (g_iDayMode != DAY_DAYMODE && !g_bRoundEnd)
		{
			return Show_MusicMenu(id);
		}
		else
		{
			client_print_color(id, id, "%s Сейчас меню музыки недоступно.", g_szChatPrefix);
		}
	}
	else ClCmd_InfoBuy(id);
	return PLUGIN_HANDLED;
}

public ClCmd_Ball(id)
{
	if (is_user_cmd_flood(id, FLOOD_BALL, .fWhiteTime = 2.0, .fBlockTime = 4.0))
		return PLUGIN_HANDLED;
	
	if (id == g_iChiefId || IsSetBit(g_iBitUserAdmin, id))
	{
		if (g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY || g_iDayMode == DAY_DEATHMATCH)
			return Show_BallMenu(id);
		else
			client_print_color(id, 0, "%s ^1Мяч можно создать в обычный день, выходной или dm.", g_szChatPrefix);
	}
	else ClCmd_InfoBuy(id);
	
	return PLUGIN_HANDLED;
}

public ClCmd_Logo(id) {
	if (is_user_cmd_flood(id, FLOOD_LOGO, 3.0, 6.0))
		return PLUGIN_HANDLED;
	
	if (id == g_iChiefId || IsSetBit(g_iBitUserAdmin, id))
		jbe_minigame_logo(id);
	else ClCmd_InfoBuy(id);
	return PLUGIN_HANDLED;
}

public ClCmd_ChiefTakeSkill(id)
{
	if (g_iChiefId == id) return Show_ChiefTakeSkillMenu(id);
	else client_print_color(id, 0, "%s Меню доступно только Начальнику!", g_szChatPrefix);
	return PLUGIN_HANDLED;
}

public ClCmd_RandomNum(id)
{
	if (id != g_iChiefId)
		return PLUGIN_HANDLED;

	new szParse[16], szArg[2][7]; 

	read_args(szParse, charsmax(szParse));
	remove_quotes(szParse);
	trim(szParse);
	
	parse(szParse,
		szArg[0], charsmax(szArg[]),
		szArg[1], charsmax(szArg[])
	);

	new iResult, iNum[2], iArgNum;

	if (szArg[0][0]) iArgNum++;
	if (szArg[1][0]) iArgNum++;

	if (!iArgNum)
	{
		client_print_color(id, id, "%s ^1Укажите значение.", g_szChatPrefix);
		return Show_RandomNumMenu(id);
	}

	if (!is_str_num(szArg[0]))
	{
		client_print_color(id, id, "%s ^1Вы допустили ошибку при вводе.", g_szChatPrefix);
		return Show_RandomNumMenu(id);
	}

	if (iArgNum > 1)
	{	
		iNum[0] = str_to_num(szArg[0]);
		iNum[1] = str_to_num(szArg[1]);
	}
	else
	{
		iNum[0] = 1;
		iNum[1] = str_to_num(szArg[0]);
	}

	if (iNum[0] > iNum[1])
	{
		client_print_color(id, id, "%s ^1Ошибка, первое значение не может быть больше второго.", g_szChatPrefix);
		return Show_RandomNumMenu(id);
	}

	iResult = random_num(iNum[0], iNum[1]);

	client_print_color(0, print_team_blue,
			"%s ^1Случайное число ^3(от %d до %d) ^1Результат: ^4%d", g_szChatPrefix, iNum[0], iNum[1], iResult);

	return Show_RandomNumMenu(id);
}

is_not_chief_menu(id)
{
	if (IsNotSetBit(g_iBitUserAlive, id))
		client_print_color(id, id, "%s Меню недоступно мертвым!", g_szChatPrefix);
	
	else if (g_iChiefId != id)
		client_print_color(id, id, "%s Меню доступно только Начальнику!", g_szChatPrefix);
	
	else
		return false;
	
	return true;
}

is_not_chief_game_menu(id)
{
	if (is_not_chief_menu(id))
		return true;

	else if (g_iChiefId != id)
		client_print_color(id, id, "%s Меню доступно только Начальнику!", g_szChatPrefix);
	
	else if (g_iLastPnId)
		client_print_color(id, id, "%s Меню недоступно при последнем заключенном.", g_szChatPrefix);
	
	else if (g_iDayMode != DAY_NORMAL)
		client_print_color(id, id, "%s Меню доступно только в обычный день!", g_szChatPrefix);
	
	else if (is_shief_global_game())
		client_print_color(id, id, "%s Меню недоступно во время игры.", g_szChatPrefix);
	
	else
		return false;
	
	return true;
}

jbe_set_name_chief(pPlayer = 0)
{
	if (g_bMafiaGame && pPlayer && IsSetBit(g_iBitUserAlive, pPlayer))
		formatex(g_szChiefName, charsmax(g_szChiefName), "^nВедущий: %n", pPlayer);

	else if (pPlayer && g_iDayMode == DAY_NORMAL)
		formatex(g_szChiefName, charsmax(g_szChiefName), "^nНачальник: %n", pPlayer);
	
	else if (g_iDayMode == DAY_NORMAL && !g_bRoundEnd)
	{
		new const szChiefStatus[][] = {
			"Ждем",
			"Скоро..", //
			"Мёртв",
			"Отключился",
			"Освобождён" // ?
		};

		if (g_iChiefId)
			formatex(g_szChiefName, charsmax(g_szChiefName), "^nНачальник: %n", g_iChiefId);
		else
			formatex(g_szChiefName, charsmax(g_szChiefName), "^nНачальник: %s", szChiefStatus[g_iChiefStatus]);
	}

	else g_szChiefName[0] = EOS;
}

public jbe_count_down_timer()
{
	if (--g_iCountDown)
		client_print_center(0, "Обратный отсчёт: %d", g_iCountDown);
	else
		client_print_center(0, "Обратный отсчёт закончен");
	
	if (g_iCountDown <= 10)
	//	rg_send_audio(0, fmt("jb_engine/countdown/%d.wav", g_iCountDown)); 
		rg_send_audio(0, sounds_countdown[g_iCountDown]);
}


// Отменить всем ФД
jbe_freeday_disable_all()
{
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserFree, i)) continue;
		jbe_sub_user_free(i);
	}
}

// Перенести всем ФД на завтра
jbe_freeday_transfer_all()
{
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserFree, i)) continue;
		jbe_sub_user_free(i);
		SetBit(g_iBitUserFreeNextRound, i);
	}
}

stock is_not_wanted_menu(id)
{
	if (g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY){}

	else if (id != g_iChiefId && IsNotSetBit(g_iBitUserAdmin, id)){}

	else return false;

	return true;
}

/********************************************
*	Обнуление некоторых навыков самоном
*********************************************/

_chief_takeskill_select(id)
{
	if (is_user_cmd_flood(id, FLOOD_GLOBAL, 0.3, 0.7))
		return;

	switch (g_iMenuType[id])
	{
		case CHIEF_TAKE_SKILL_JUMPS:	_chief_takeskill_jumps();
		case CHIEF_TAKE_SKILL_SPEED:	_chief_takeskill_speed();
		case CHIEF_TAKE_SKILL_HP_AP:	_chief_takeskill_hp_ap();
		case CHIEF_TAKE_SKILL_DAMAGE:	_chief_takeskill_damage();
		case CHIEF_TAKE_SKILL_GLOW:		_chief_takeskill_glow();
	}
}

_chief_takeskill_speed()
{
	new iWantedFree;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || g_iUserTeam[i] != TEAM_PRISONER || IsNotSetBit(g_iBitFastRun, i)) continue;
		if (IsSetBit(g_iBitUserWanted, i) || IsSetBit(g_iBitUserFree, i))
		{
			iWantedFree = 1;
			continue;
		}

		jbe_set_user_maxspeed(i, _, true);
	}
	client_print_color(0, print_team_red, "%s Начальник забрал скорость заключенным%s.", g_szChatPrefix, iWantedFree ? " (кроме свободных и разыскиваемых)" : "");
}

_chief_takeskill_hp_ap()
{
	new iWantedFree;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || g_iUserTeam[i] != TEAM_PRISONER) continue;
		if (IsSetBit(g_iBitUserWanted, i) || IsSetBit(g_iBitUserFree, i))
		{
			iWantedFree = 1;
			continue;
		}
		set_pev(i, pev_health, 100.0);
		set_pev(i, pev_armorvalue, 0.0);
	}
	client_print_color(0, print_team_red, "%s Начальник выставил 100 HP и 0 AP заключенным%s.", g_szChatPrefix, iWantedFree ? " (кроме свободных и разыскиваемых)" : "");
}

_chief_takeskill_jumps()
{
	new iWantedFree;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || g_iUserTeam[i] != TEAM_PRISONER) continue;
		if (IsSetBit(g_iBitUserWanted, i) || IsSetBit(g_iBitUserFree, i))
		{
			iWantedFree = 1;
			continue;
		}
		ClearBit(g_iBitDoubleJump, i);
		ClearBit(g_iBitAutoBhop, i);
		ClearBit(g_iBitHingJump, i);
		ClearBit(g_iBitLongJump, i);
		set_pev(i, pev_gravity, 1.0);
	}
	ham_jump_check();

	client_print_color(0, print_team_red, "%s Начальник убрал все усиления прыжка и гравитацию заключенным%s.", g_szChatPrefix, iWantedFree ? " (кроме свободных и разыскиваемых)" : "");
}

_chief_takeskill_damage()
{
	new iWantedFree;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || g_iUserTeam[i] != TEAM_PRISONER) continue;
		if (IsSetBit(g_iBitUserWanted, i) || IsSetBit(g_iBitUserFree, i))
		{
			iWantedFree = 1;
			continue;
		}
		ClearBit(g_iBitKokain, i);
		ClearBit(g_iBitDoubleDamage, i);
	}

	client_print_color(0, print_team_red, "%s Начальник убрал двойной и уменьшенный урон заключенным%s.", g_szChatPrefix, iWantedFree ? " (кроме свободных и разыскиваемых)" : "");
}

_chief_takeskill_glow()
{
	new iWantedFree;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || g_iUserTeam[i] != TEAM_PRISONER) continue;
		if (IsSetBit(g_iBitUserFree, i) || IsSetBit(g_iBitUserWanted, i))
		{
			iWantedFree = 1;
			continue;
		}

		jbe_set_user_rendering(i, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
		jbe_save_user_rendering(i);
	}

	client_print_color(0, print_team_red, "%s Начальник убрал свечение заключенным%s.", g_szChatPrefix, iWantedFree ? " (кроме свободных и разыскиваемых)" : "");
}
