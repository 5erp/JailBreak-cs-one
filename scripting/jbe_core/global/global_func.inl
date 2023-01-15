#include "jbe_core\global\global_menu.inl"
/*
#include "jbe_core\global\global_user_menu.inl"
#include "jbe_core\global\global_set_menu.inl"
#include "jbe_core\global\global_control_menu.inl" 
#include "jbe_core\global\global_block_menu.inl"
*/ 

global_menus_init()
{
	// global_menu_main
	register_menucmd(register_menuid("Show_GlobalMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_GlobalMenu");
	
	// global_team_menu
	register_menucmd(register_menuid("Show_GlobalTeamMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<7|1<<8|1<<9), "Handle_GlobalTeamMenu");
	
	// global_skill_menu
	register_menucmd(register_menuid("Show_GlobalSkillMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_GlobalSkillMenu");
	
	// global_gun_menu
	register_menucmd(register_menuid("Show_GlobalGunMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_GlobalGunMenu");

	// global_user_menu
	register_menucmd(register_menuid("Show_GlobalUserMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_GlobalUserMenu");
	
	// global_set_menu
	register_menucmd(register_menuid("Show_GlobalSetMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<8|1<<9), "Handle_GlobalSetMenu");

	// global_control_menu
	register_menucmd(register_menuid("Show_GlobalControlMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_GlobalControlMenu");

	// global_block_menu
	register_menucmd(register_menuid("Show_GlobalBlockMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_GlobalBlockMenu");

	register_menucmd(register_menuid("Show_GlobalAmmoMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<8|1<<9), "Handle_Global_AmmoMenu");

	register_menucmd(register_menuid("Show_SetLightsMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_SetLightsMenu");

	register_menucmd(register_menuid("Show_GlobalEntityBlockMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_GlobalEntityBlockMenu");

	register_clcmdex("globalmenu", "ClCmd_GlobalMenu", 1); // say /globalmenu
	register_clcmdex("gmenu", "ClCmd_GlobalMenu", 1); // say /gmenu
	register_clcmd("global_num", "ClCmd_GlobalNum"); // /global_num
	register_clcmd("global_name", "ClCmd_GlobalName"); // /global_name
}

public ClCmd_GlobalMenu(id) 
{
	if (IsNotSetBit(g_iBitUserGlobalAdmin, id))
		return ClCmd_InfoBuy(id);

	return Show_GlobalMenu(id);
}

return_to_global_menu(id) 
{
	client_print_color(id, print_team_red,
		"%s ^1Вы ввели ^3неверные параметры ^1операции, попробуйте ещё раз.",
		g_szGlobalPrefix);
	
	if (g_iGlobalMenuType[id] == GLOBAL_GIVE_GUN)
		return Show_GlobalGunMenu(id, g_iMenuPosition[id]);
	
	return Show_GlobalSetMenu(id);
}

public ClCmd_GlobalNum(id) 
{
	if (is_not_global_menu(id)) return PLUGIN_HANDLED;
	
	new szArg[5];
	read_argv(1, szArg, charsmax(szArg));

	switch (g_iGlobalMenuType[id])
	{
		case GLOBAL_GIVE_GUN:
		{
			if (szArg[0] == EOS)
			{
				g_iMenuTarget[id] = global_weapon_is_grenade(g_iGlobalItem[id]) ? 1 : -1;
			}
			else if (!is_str_num(szArg))
				return return_to_global_menu(id);
			else
				g_iMenuTarget[id] = str_to_num(szArg);
		}
		case GLOBAL_HEALTH:
		{
			if (szArg[0] == EOS)
				g_iMenuTarget[id] = 100;
			else if (!is_str_num(szArg))
				return return_to_global_menu(id);
			else
				g_iMenuTarget[id] = str_to_num(szArg);
			
			if (g_iMenuTarget[id] == 0)
				g_iMenuTarget[id] = 1;
		}
		case GLOBAL_ARMOR:
		{
			if (szArg[0] == EOS)
				g_iMenuTarget[id] = 100;
			else if (!is_str_num(szArg))
				return return_to_global_menu(id);
			else
				g_iMenuTarget[id] = str_to_num(szArg);
		}
		case GLOBAL_SPEED:
		{
			if (szArg[0] == EOS)
				g_iMenuTarget[id] = 320; // def speed ?
			else if (!is_str_num(szArg))
				return return_to_global_menu(id);
			else
				g_iMenuTarget[id] = str_to_num(szArg);
			
			if (g_iMenuTarget[id] > cvar_sv_maxspeed)
				g_iMenuTarget[id] = cvar_sv_maxspeed;
		}
		case GLOBAL_GRAVITY:
		{
			if (szArg[0] == EOS)
				g_iMenuTarget[id] = 100;
			// отрицательные значения
			else if (!is_str_numex(szArg))
				return return_to_global_menu(id);
			else
				g_iMenuTarget[id] = str_to_num(szArg);
		}
	}

	return Show_GlobalTeamMenu(id);
}

public ClCmd_GlobalName(id)
{
	if (is_not_global_menu(id) || g_iDayMode != DAY_NORMAL)
		return PLUGIN_HANDLED;

	new szArg[32];
	read_argv(1, szArg, charsmax(szArg));

	jbe_set_name_mode(szArg);

	client_print_color(0, 0, "%s ^4%n ^1%s^4%s",
		g_szGlobalPrefix, id, szArg[0] == EOS ? "обнулил имя режима." : "выставил имя режима: ", szArg);

	return PLUGIN_HANDLED;
}

/* Проверка доступности игроку global menu */
is_not_global_menu(id) // if (is_not_global_menu(id)) return PLUGIN_HANDLED;
{
	if (IsNotSetBit(g_iBitUserGlobalAdmin, id))
		ClCmd_InfoBuy(id);
	else if (g_iDayMode == DAY_DAYMODE || g_iDayMode == DAY_RESTART)
		client_print_color(id, id, "%s Сейчас ^4Global Меню ^1недоступно!", g_szChatPrefix);
	else return false;
	return true;
}

_global_format_menu_type(id)
{
	switch (g_iGlobalMenuType[id])
	{
		case GLOBAL_RESPAWN: menu_item("\dВозродить^n");
		case GLOBAL_GIVE_GUN:
		{
			if (global_weapon_id(g_iGlobalItem[id]) > 0)
			{
				menu_item("\dВыдать %s ", global_weapon_name(g_iGlobalItem[id]));
				if (global_weapon_id(g_iGlobalItem[id]))
				{
					switch (g_iMenuTarget[id])
					{
						case -1:	menu_item("(с 1 обоймой)^n");
						case 0:		menu_item("(без боеприпасов)^n");
						default:	menu_item("и %d боеприпасов^n", g_iMenuTarget[id]);
					}
				}
				else menu_item("^n");
			}
			else
				menu_item("\dЗабрать %s^n", global_weapon_name(g_iGlobalItem[id]));			
		}
		case GLOBAL_SKILL:
		{
			menu_item("\d%s ", g_iGlobalSkillType[id][g_iGlobalItem[id]] ? "Выдать" : "Забрать");
			switch (g_iGlobalItem[id])
			{
				case GLOBAL_SKILL_SPEED:		menu_item("Скорость^n");
				case GLOBAL_SKILL_GRAVITY:		menu_item("Гравитацию^n");
				case GLOBAL_SKILL_250HP_250AP:	menu_item("250HP + 250AP^n");
				case GLOBAL_SKILL_GODMODE:		menu_item("Бессмертие^n");
				case GLOBAL_SKILL_NOCLIP:		menu_item("Ноуклип^n");
				case GLOBAL_SKILL_DOUBLE_JUMP:	menu_item("Двойной прыжок^n");
				case GLOBAL_SKILL_AUTO_JUMP:	menu_item("Автораспрыг^n");
				case GLOBAL_SKILL_HIGHT_JUMP:	menu_item("Высокий прыжок^n");
				case GLOBAL_SKILL_KOKAIN:		menu_item("Уменьшеный урон^n");
				case GLOBAL_SKILL_DUBLE_DMG:	menu_item("Двойной урон^n");
				case GLOBAL_SKILL_SEMICLIP:		menu_item("Семиклип^n");
				case GLOBAL_SKILL_INVIZ:		menu_item("Невидимость^n");
				case GLOBAL_SKILL_FOOTSTEPS:	menu_item("Бесшумные шаги^n");
				case GLOBAL_SKILL_LONG_JUMP:	menu_item("Длиный прыжок^n");
				case GLOBAL_SKILL_GHOST:		menu_item("Призрак^n");
				case GLOBAL_SKILL_FALL_NO_DMG:	menu_item("Защита от падений^n");
				case GLOBAL_SKILL_3DVIEW:		menu_item("Вид от 3-го лица^n");
			}
		}
		case GLOBAL_HEALTH:		menu_item("\dВыставить %d HP^n", g_iMenuTarget[id]);
		case GLOBAL_ARMOR:		menu_item("\dВыставить %d AP^n", g_iMenuTarget[id]);
		case GLOBAL_SPEED:		menu_item("\dВыставить %d скорость^n", g_iMenuTarget[id]);
		case GLOBAL_GRAVITY:	menu_item("\dВыставить %d％ гравитации^n", g_iMenuTarget[id]); // ％ - unicode
		case GLOBAL_DEL_SKILLS:	menu_item("\dОбнулить все навыки^n");
		case GLOBAL_SKREEN_OFF:	menu_item("\dВыкл экран^n");
		case GLOBAL_FREEZE:		menu_item("\dЗаморозить^n");
		case GLOBAL_BURIED:		menu_item("\dЗакопать^n");
		case GLOBAL_SKREEN_ON:	menu_item("\dВкл экран^n");
		case GLOBAL_UNFREEZE:	menu_item("\dРазморозить^n");
		case GLOBAL_UNBURIED:	menu_item("\dОткопать^n");
	}
}

_global_menu_select(id, iTarget) 
{
	if (is_user_cmd_flood(id, FLOOD_GLOBAL, 0.2, 0.7) || is_not_global_menu(id))
		return;

	switch (g_iGlobalMenuType[id]) 
	{
		case GLOBAL_RESPAWN:	_global_menu_respawn(id, iTarget);

		case GLOBAL_GIVE_GUN:	_global_give_gun(id, iTarget);

		case GLOBAL_SKILL:		_global_menu_skill(id, iTarget, g_iGlobalSkillType[id][g_iGlobalItem[id]] ? true : false);

		case GLOBAL_HEALTH:		_global_menu_health(id, iTarget);

		case GLOBAL_ARMOR:		_global_menu_armor(id, iTarget);

		case GLOBAL_SPEED:		_global_menu_speed(id, iTarget);

		case GLOBAL_GRAVITY:	_global_menu_gravity(id, iTarget);

		case GLOBAL_DEL_SKILLS:	_global_menu_del_skills(id, iTarget);

		case GLOBAL_SKREEN_OFF:	_global_menu_screen(id, iTarget, .bOff = true);

		case GLOBAL_FREEZE:		_global_menu_freeze(id, iTarget, .bFreeze = true);

		case GLOBAL_BURIED:		_global_menu_buried(id, iTarget, .bBuried = true);

		case GLOBAL_SKREEN_ON:	_global_menu_screen(id, iTarget, .bOff = false);

		case GLOBAL_UNFREEZE:	_global_menu_freeze(id, iTarget, .bFreeze = false);

		case GLOBAL_UNBURIED:	_global_menu_buried(id, iTarget, .bBuried = false);
	}
}

global_clear_blocks(pPlayer = 0)
{
	if (pPlayer)
	{
		if (IsNotSetBit(g_iBitUserGlobalAdmin, pPlayer)
		|| IsNotSetBit(g_iBitUseGlobalBlock, pPlayer))
			return;

		for (new i; i < _:eGlobalBlock; i++)
		{
			if (g_iGlobalBlock[eGlobalBlock:i] && g_iGlobalBlock[eGlobalBlock:i] == pPlayer)
				g_iGlobalBlock[i] = 0;
		}

		ClearBit(g_iBitUseGlobalBlock, pPlayer);
	}
	else
	{
		arrayset(g_iGlobalBlock, 0, eGlobalBlock); // обнуляем глобал блок
		g_iBitUseGlobalBlock = 0;
	}
}

// Устновка блока сервером
eGlobalBlock:global_set_blocks_server(any:...)
{
	for (new i; i < numargs(); i++)
	{
		g_iGlobalBlock[getarg(i)] = -1;
	}
}

// Установка/снятие блока игроком
bool:global_set_block(id = -1, iType, bool:bSet = false)
{
	if (g_iGlobalBlock[iType])
	{
		if (bSet && id < 0)
		{
			g_iGlobalBlock[iType] = id;
			g_iBitUseGlobalBlock = -1;
			return true;
		}

		if (g_iGlobalBlock[iType] > 0)
		{
			g_iGlobalBlock[iType] = 0;
			return true;
		}
		else
		{
			if (id > 0)
			{
				client_print_color(id, id, "%s Нельзя разблокировать, заблокированное игрой!", g_szGlobalPrefix);
				return false;
			}
			else
			{
				g_iGlobalBlock[iType] = 0;
				return true;
			}
		}
	}
	else
	{
		g_iGlobalBlock[iType] = id;
		return true;
	}
}

global_get_name_block(iType)
{
	static szMessage[64];

	if (g_iGlobalBlock[iType] > 0)
		formatex(szMessage, charsmax(szMessage), "^1администратором ^4%n", g_iGlobalBlock[iType]);
	else
		szMessage = "^1для игры";

	return szMessage;
}

global_get_name_target(iTarget, bool:bF = false) 
{
	new szName[32];

	if (0 < iTarget <= MaxClients)
	{
		get_user_name(iTarget, szName, charsmax(szName));
	}
	else
	{
	//	iTarget *= -1;
		switch (iTarget * -1) 
		{
			case TEAM_PRISONER: szName = bF ? "заключенных" : "заключенным";
			case TEAM_GUARD: szName = bF ? "охранников" : "охранникам";
			default: szName = bF ? "всех игроков" : "всем игрокам";
		}
	}

	return szName;
}

global_chat_team(iTarget)
{
	iTarget = 0 < iTarget <= MaxClients ? g_iUserTeam[iTarget] : iTarget * -1;

	switch (iTarget)
	{
		case TEAM_PRISONER: return print_team_red;
		case TEAM_GUARD: return print_team_blue;
		default: return print_team_grey;
	}

	return print_team_default;
}

_global_entity_block(id, iItem)
{
	new const szEntityBlockInfo[][] = {
		// Use
		"Управляемая машина",			// 
		"Управляемый поезд",		// 
		"Управляемая пушка",			// 
		"При активации наносит игроку повреждения",		// 
		"Увеличение запаса бронижелета",		// 
		"Увеличение процентов здоровья",	// 
		"Выдаёт оружие",	// 
		"Забирает всё оружие",	// 
		"Кнопка",			// 
		// Touch
		"Наносит игроку повреждения",			// 
		"Устанавливает игроку силу гравитации",		// 
		"Объект лежащий на карте, оружия, броня или гранаты",		// 
		"Оружие выброшенное игроком",			// 
		"Щит"			// 
	};

	if (IsNotSetBit(g_iBitGlobalEntBlock, iItem))
		EnableHamForward(g_iHamHookForwards[iItem]);
	else
		DisableHamForward(g_iHamHookForwards[iItem]);

	InvertBit(g_iBitGlobalEntBlock, iItem);

	client_print_color(0, print_team_blue, "%s ^4%n ^1%s ^3%s", g_szGlobalPrefix, id, IsSetBit(g_iBitGlobalEntBlock, iItem) ? "заблокировал" : "разблокировал", szHamHookEntityBlock[iItem]);
	client_print_color(0, print_team_blue, "%s Описание: ^3%s", g_szGlobalPrefix, szEntityBlockInfo[iItem]);
}

_global_menu_skill(id, iTarget, bool:bGive = true)
{
	static szSkill[36], szGive[16];
	szGive = !bGive ? "забрал" : "выдал";
	
	switch (g_iGlobalItem[id]) 
	{
		case GLOBAL_SKILL_SPEED:
		{
			szSkill = "скорость";
			_global_give_speed(iTarget, bGive);
		}
		case GLOBAL_SKILL_GRAVITY: 
		{
			szSkill = "гравитацию";
			_global_give_gravity(iTarget, bGive);
		}
		case GLOBAL_SKILL_250HP_250AP:
		{
			szSkill = "250HP и 250AP";
			_global_give_250hp_250ap(iTarget, bGive);
		}
		case GLOBAL_SKILL_GODMODE: 
		{
			szSkill = "бессмертие";
			_global_give_godmode(iTarget, bGive);
		}
		case GLOBAL_SKILL_NOCLIP: 
		{
			szSkill = "ноуклип";
			_global_give_noclip(iTarget, bGive);
		}
		case GLOBAL_SKILL_DOUBLE_JUMP: 
		{
			szSkill = "двойной прыжок";
			_global_give_doublejump(iTarget, bGive);
		}
		case GLOBAL_SKILL_AUTO_JUMP: 
		{
			szSkill = "автораспрыг";
			_global_give_autojump(iTarget, bGive);
		}
		case GLOBAL_SKILL_HIGHT_JUMP: 
		{
			szSkill = "высокий прыжок";
			_global_give_hightjump(iTarget, bGive);
		}
		case GLOBAL_SKILL_KOKAIN: 
		{
			szSkill = "уменьшеный урон";
			_global_give_kokain(iTarget, bGive);
		}
		case GLOBAL_SKILL_DUBLE_DMG: 
		{
			szSkill = "двойной урон";
			_global_give_doubledmg(iTarget, bGive);
		}
		case GLOBAL_SKILL_SEMICLIP:
		{
			szSkill = "семиклип";
			_global_give_semiclip(iTarget, bGive);
		}
		case GLOBAL_SKILL_INVIZ: 
		{
			szSkill = "невидимость";
			_global_give_inviz(iTarget, bGive);
		}
		case GLOBAL_SKILL_FOOTSTEPS: 
		{
			szSkill = "бесшумные шаги";
			_global_give_footsteps(iTarget, bGive);
		}
		case GLOBAL_SKILL_LONG_JUMP: 
		{
			szSkill = "длиный прыжок";
			_global_give_longjump(iTarget, bGive);
		}
		case GLOBAL_SKILL_GHOST: 
		{
			szSkill = "призрака";
			_global_give_ghost(iTarget, bGive);
		}
		case GLOBAL_SKILL_FALL_NO_DMG:
		{
			szSkill = "защиту от падений";
			_global_give_fallnodmg(iTarget, bGive);
		}
		case GLOBAL_SKILL_3DVIEW:
		{
			szSkill = "вид от 3-го лица";
			_global_give_3dview(iTarget, bGive);
		}
	}

	client_print_color(0, global_chat_team(iTarget), 
		"%s ^4%n ^1%s^4 %s ^3%s^1.", g_szGlobalPrefix,
		id, szGive, szSkill, global_get_name_target(iTarget));
	
	return PLUGIN_HANDLED;
}

_global_give_speed(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients) 
	{
		jbe_set_user_maxspeed(iTarget, bGive ? 400.0 : 0.0, true);
	} 
	else 
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++) 
		{
			if (IsNotSetBit(g_iBitUserAlive, i)
			|| iTarget && g_iUserTeam[i] != iTarget) 
				continue;

			jbe_set_user_maxspeed(i, bGive ? 400.0 : 0.0, true);
		}
	}
}

_global_give_gravity(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients) 
	{
		set_pev(iTarget, pev_gravity, bGive ? SHOP_GRAVITY_NUM : 1.0);
	} 
	else 
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++) 
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget) continue;
			set_pev(i, pev_gravity, bGive ? SHOP_GRAVITY_NUM : 1.0);
		}
	}
}

_global_give_250hp_250ap(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients) 
	{
		set_pev(iTarget, pev_health, bGive ? 250.0 : 100.0);
		rg_set_user_armor(iTarget, bGive ? 250 : 100, ARMOR_VESTHELM);
	} 
	else 
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++) {
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget) continue;
			set_pev(i, pev_health, bGive ? 250.0 : 100.0);
			rg_set_user_armor(i, bGive ? 250 : 100, ARMOR_VESTHELM);
		}
	}
}

_global_give_godmode(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients) 
	{
		jbe_set_user_godmode(iTarget, bGive ? 1 : 0);
	} 
	else 
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++) {
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget) continue;
			jbe_set_user_godmode(i, bGive ? 1 : 0);
		}
	}
}

_global_give_noclip(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients)
	{
		rg_set_user_noclip(iTarget, bGive ? 1 : 0);
	} 
	else 
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++) 
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget) continue;
			rg_set_user_noclip(i, bGive ? 1 : 0);
		}
	}
}

_global_give_doublejump(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients)
	{
		SetBitBool(g_iBitDoubleJump, iTarget, bGive);
	}
	else
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;
			SetBitBool(g_iBitDoubleJump, i, bGive);
		}
	}
	ham_jump_check();
}

_global_give_autojump(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients)
	{
		bGive ? SetBit(g_iBitAutoBhop, iTarget) : ClearBit(g_iBitAutoBhop, iTarget);
	}
	else
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;
			bGive ? SetBit(g_iBitAutoBhop, i) : ClearBit(g_iBitAutoBhop, i);
		}
	}
	ham_jump_check();
}

_global_give_hightjump(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients)
	{
		bGive ? SetBit(g_iBitHingJump, iTarget) : ClearBit(g_iBitHingJump, iTarget);
	}
	else
	{
		iTarget *= -1;iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;
			bGive ? SetBit(g_iBitHingJump, i) : ClearBit(g_iBitHingJump, i);
		}
	}
	ham_jump_check();
}

_global_give_kokain(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients)
	{
		bGive ? SetBit(g_iBitKokain, iTarget) : ClearBit(g_iBitKokain, iTarget);
	}
	else
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;
			bGive ? SetBit(g_iBitKokain, i) : ClearBit(g_iBitKokain, i);
		}
	}
}

_global_give_doubledmg(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients)
	{
		SetBitBool(g_iBitDoubleDamage, iTarget, bGive);
	}
	else
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;
			SetBitBool(g_iBitDoubleDamage, i, bGive);
		}
	}
}

_global_give_semiclip(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients)
	{
		set_pev(iTarget, pev_solid, bGive ? SOLID_NOT : SOLID_SLIDEBOX); 
	} 
	else 
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++) 
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget) continue;
			set_pev(i, pev_solid, bGive ? SOLID_NOT : SOLID_SLIDEBOX); 
		}
	}
}

_global_give_inviz(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients) 
	{
		if (bGive) 
			jbe_set_user_rendering(iTarget, kRenderFxGlowShell, 0, 0, 0, kRenderTransAlpha, 0);
		else
			jbe_set_user_rendering(iTarget, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
		
		jbe_save_user_rendering(iTarget);
	} 
	else 
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++) 
		{
			if (IsNotSetBit(g_iBitUserAlive, i) 
			|| iTarget && g_iUserTeam[i] != iTarget) 
				continue;
			
			if (bGive) 
				jbe_set_user_rendering(i, kRenderFxGlowShell, 0, 0, 0, kRenderTransAlpha, 0);
			else 
				jbe_set_user_rendering(i, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
			
			jbe_save_user_rendering(i);
		}
	}
}

_global_give_footsteps(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients)
	{
		rg_set_user_footsteps(iTarget, bGive ? true : false);
	}
	else
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;
			rg_set_user_footsteps(i, bGive ? true : false);
		}
	}
}

_global_give_longjump(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients)
	{
		bGive ? SetBit(g_iBitLongJump, iTarget) : ClearBit(g_iBitLongJump, iTarget);
	}
	else
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;
			bGive ? SetBit(g_iBitLongJump, i) : ClearBit(g_iBitLongJump, i);
		}
	}
	ham_jump_check();
}

_global_give_ghost(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients) 
	{
		if (bGive) 
			jbe_set_user_rendering(iTarget, kRenderFxDistort, 0, 0, 0, kRenderTransAdd, 127); // голограмма
		else 
			jbe_set_user_rendering(iTarget, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
		
		jbe_save_user_rendering(iTarget);
	} 
	else 
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++) 
		{
			if (IsNotSetBit(g_iBitUserAlive, i) 
			|| iTarget && g_iUserTeam[i] != iTarget) 
				continue;

			if (bGive)
				jbe_set_user_rendering(i, kRenderFxDistort, 0, 0, 0, kRenderTransAdd, 127); // голограмма
			else 
				jbe_set_user_rendering(i, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
			
			jbe_save_user_rendering(i);
		}
	}
}

_global_give_fallnodmg(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients)
	{
		SetBitBool(g_iBitFallNoDamage, iTarget, bGive);
	}
	else
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;
			SetBitBool(g_iBitFallNoDamage, i, bGive);
		}
	}
}

_global_give_3dview(iTarget, bool:bGive)
{
	if (0 < iTarget <= MaxClients)
	{
		jbe_set_user_view(iTarget, bGive ? 1 : 0);
	}
	else
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;
			jbe_set_user_view(i, bGive ? 1 : 0);
		}
	}
}

_global_menu_respawn(id, iTarget)
{
	client_print_color(0, global_chat_team(iTarget), 
		"%s ^4%n ^1возродил^4 ^3%s^1.", g_szGlobalPrefix, id, global_get_name_target(iTarget, true));

	if (0 < iTarget <= MaxClients) 
	{
		if (IsSetBit(g_iBitUserAlive, iTarget)
		|| (g_iUserTeam[iTarget] != TEAM_PRISONER && g_iUserTeam[iTarget] != TEAM_GUARD))
			return;

		rg_round_respawn(iTarget);
	}
	else
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++) 
		{
			if (IsSetBit(g_iBitUserAlive, i) || (iTarget && g_iUserTeam[i] != iTarget)
			|| (g_iUserTeam[i] != TEAM_PRISONER && g_iUserTeam[i] != TEAM_GUARD))
				continue;

			rg_round_respawn(i);
		}
	}
}

_global_give_gun(id, iTarget) 
{
	if (global_weapon_id(g_iGlobalItem[id]) < 0)
	{
		_global_take_gun(id, iTarget, global_weapon_id(g_iGlobalItem[id]) == -1 ? true : false);
		return;
	}

	new szItemName[32];
	new iAmmo = g_iMenuTarget[id];

	formatex(szItemName, charsmax(szItemName), global_weapon_name(g_iGlobalItem[id]));

	if (global_weapon_id(g_iGlobalItem[id]) <= 0)
		client_print_color(0, global_chat_team(iTarget), "%s ^4%n ^1выдал ^4%s ^3%s^1.",
			g_szGlobalPrefix, id, szItemName, global_get_name_target(iTarget));

	else if (iAmmo == 0)
		client_print_color(0, global_chat_team(iTarget), "%s ^4%n ^1выдал ^4%s ^1(без боеприпасов) ^3%s^1.",
			g_szGlobalPrefix, id, szItemName, global_get_name_target(iTarget));
	
	else if (iAmmo > 0)
		client_print_color(0, global_chat_team(iTarget), "%s ^4%n ^1выдал ^4%s ^1(%d %s) ^3%s^1.",
			g_szGlobalPrefix, id, szItemName, iAmmo, global_weapon_is_grenade(g_iGlobalItem[id]) ? "штук" : "боеприпасов", global_get_name_target(iTarget));
	
	else
		client_print_color(0, global_chat_team(iTarget), "%s ^4%n ^1выдал ^4%s ^1(с 1 обоймой) ^3%s^1.",
			g_szGlobalPrefix, id, szItemName, global_get_name_target(iTarget));
	
	if (0 < iTarget <= MaxClients) 
	{
		if (IsNotSetBit(g_iBitUserAlive, iTarget))
			return;

		_global_give_user_weapon(id, iTarget);
	} 
	else 
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget) 
				continue;
			
			_global_give_user_weapon(id, i);
		}
	}
}

_global_give_user_weapon(id, iTarget)
{
	rg_remove_item(iTarget, global_weapon_item(g_iGlobalItem[id]), .removeAmmo = true);

	// iEntity для изменения пуль в обойме
	new iEntity = rg_give_item(iTarget, global_weapon_item(g_iGlobalItem[id]));

	if (iEntity < 0) return;

	new iAmmo = g_iMenuTarget[id];
	new iWpnID = global_weapon_id(g_iGlobalItem[id]);
	new iMaxAmmo = get_max_ammo_wpnid(global_weapon_id(g_iGlobalItem[id]));

	// -1 значит значение не было указано
	if (-1 < iAmmo < iMaxAmmo) 
	{
		set_member(iEntity, m_Weapon_iClip, iAmmo);
		rg_set_user_bpammo(iTarget, WeaponIdType:iWpnID, -1);
	}

	// если больше iMaxAmmo даем в запас пули отняв iMaxAmmo
	else if (iAmmo > iMaxAmmo)
	{
		rg_set_user_bpammo(iTarget, WeaponIdType:iWpnID, iAmmo - iMaxAmmo);
	}
	
	// -1 значит значение не было указано
	else if (iAmmo == -1)
	{
		rg_set_user_bpammo(iTarget, WeaponIdType:iWpnID, -1);
	}
}

_global_take_gun(id, iTarget, bool:bGiveKnife = true)
{	
	client_print_color(0, global_chat_team(iTarget),
		"%s ^4%n ^1забрал оружие%s у ^3%s^1.", g_szGlobalPrefix,
		id, bGiveKnife ? "" : " и нож", global_get_name_target(iTarget, true));

	if (0 < iTarget <= MaxClients) 
	{
		if (IsNotSetBit(g_iBitUserAlive, iTarget)) return;

		rg_remove_all_items(iTarget);
		if (bGiveKnife)
			rg_give_item(iTarget, "weapon_knife");
	} 
	else 
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++) 
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget) 
				continue;
			
			rg_remove_all_items(i);
			if (bGiveKnife)
				rg_give_item(i, "weapon_knife");
		}
	}
}

_global_menu_health(id, iTarget) 
{
	client_print_color(0, global_chat_team(iTarget), 
		"%s ^4%n ^1выставил ^4%d HP ^3%s^1.", g_szGlobalPrefix,
		id, g_iMenuTarget[id], global_get_name_target(iTarget));
	
	new Float:fValue = float(g_iMenuTarget[id]);

	if (0 < iTarget <= MaxClients) 
	{
		if (IsNotSetBit(g_iBitUserAlive, iTarget)) return;
		set_pev(iTarget, pev_health, fValue);
	} 
	else 
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++) 
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget) 
				continue;
			set_pev(i, pev_health, fValue);
		}
	}
}

_global_menu_armor(id, iTarget) 
{
	client_print_color(0, global_chat_team(iTarget), 
		"%s ^4%n ^1выставил ^4%d AP ^3%s^1.", g_szGlobalPrefix,
		id, g_iMenuTarget[id], global_get_name_target(iTarget));

	new Float:fValue = float(g_iMenuTarget[id]);

	if (0 < iTarget <= MaxClients) 
	{
		if (IsNotSetBit(g_iBitUserAlive, iTarget)) return;
		set_pev(iTarget, pev_armorvalue, fValue);
	}
	else 
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget) 
				continue;
			set_pev(i, pev_armorvalue, fValue);
		}
	}
}

_global_menu_speed(id, iTarget)
{
	client_print_color(0, global_chat_team(iTarget), 
		"%s ^4%n ^1выставил ^4%d скорости ^3%s^1.", g_szGlobalPrefix,
		id, g_iMenuTarget[id], global_get_name_target(iTarget));

	new Float:fValue = float(g_iMenuTarget[id]);
	
	new szArg[5];
	num_to_str(g_iMenuTarget[id], szArg, charsmax(szArg));

	if (0 < iTarget <= MaxClients) 
	{
		if (IsNotSetBit(g_iBitUserAlive, iTarget))
			return;

		jbe_set_user_maxspeed(iTarget, fValue, true);

		check_user_cvars_speed(iTarget, szArg, charsmax(szArg));
	}
	else
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;

			jbe_set_user_maxspeed(i, fValue, true);

			check_user_cvars_speed(i, szArg, charsmax(szArg));
		}
	}
}

check_user_cvars_speed(id, speed[], len)
{
	if (is_user_bot(id)) return;
	query_client_cvar(id, "cl_forwardspeed", "ClCvar_Speed", len, speed);
	query_client_cvar(id, "cl_sidespeed", "ClCvar_Speed", len, speed);
	query_client_cvar(id, "cl_backspeed", "ClCvar_Speed", len, speed);
}
public ClCvar_Speed(id, szCvar[], szValue[], szParam[])
{
	new iValue = str_to_num(szValue);
	new iParam = str_to_num(szParam);

	if (iValue < iParam)
	{
		client_cmd(id, "%s ^"%d^"", szCvar, iParam);
		client_print_color(id, id, "^4 * ^1Чтоб работала скорость, введите в консоль: ^4%s %d", szCvar, iParam);
	}
}

_global_menu_gravity(id, iTarget)
{
	// ％ - unicode
	client_print_color(0, global_chat_team(iTarget),
		"%s ^4%n ^1выставил ^4%d％ гравитации ^3%s^1.", g_szGlobalPrefix,
		id, g_iMenuTarget[id], global_get_name_target(iTarget));

	new Float:fValue = float(g_iMenuTarget[id]) / 100.0;

	if (0 < iTarget <= MaxClients)
	{
		if (IsNotSetBit(g_iBitUserAlive, iTarget)) return;
		set_pev(iTarget, pev_gravity, fValue);
	}
	else
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;
			set_pev(i, pev_gravity, fValue);
		}
	}
}

_global_menu_del_skills(id, iTarget)
{
	client_print_color(0, global_chat_team(iTarget),
		"%s ^4%n ^1обнулил все навыки ^3%s^1.", g_szGlobalPrefix,
		id, global_get_name_target(iTarget));

	if (0 < iTarget <= MaxClients)
	{
		if (IsNotSetBit(g_iBitUserAlive, iTarget)) return;
		_delete_user_all_skills(iTarget);
	}
	else
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;
			_delete_user_all_skills(i);
		}
	}
}

// Удаление всех скилов игроку выдаваемых в Global Menu
_delete_user_all_skills(iTarget)
{
	// GLOBAL_SKILL_SPEED
//	ClearBit(g_iBitFastRun, iTarget);
//	g_fUserMaxSpeed[iTarget] = 0.0;
//	ExecuteHamB(Ham_Player_ResetMaxSpeed, iTarget);
	jbe_set_user_maxspeed(iTarget, _, true);
	
	// GLOBAL_SKILL_GRAVITY
	set_entvar(iTarget, var_gravity, 1.0);
	
	// GLOBAL_SKILL_250HP_250AP
	set_entvar(iTarget, var_health, 100.0);
	rg_set_user_armor(iTarget, 0, ARMOR_NONE);
	
	// GLOBAL_SKILL_GODMODE
	jbe_set_user_godmode(iTarget, 0);
	
	// GLOBAL_SKILL_NOCLIP
	rg_set_user_noclip(iTarget, 0);
	
	// GLOBAL_SKILL_DOUBLE_JUMP
	ClearBit(g_iBitDoubleJump, iTarget);
	
	// GLOBAL_SKILL_AUTO_JUMP
	ClearBit(g_iBitAutoBhop, iTarget);
	
	// GLOBAL_SKILL_HIGHT_JUMP
	ClearBit(g_iBitHingJump, iTarget);
	
	// GLOBAL_SKILL_KOKAIN
	ClearBit(g_iBitKokain, iTarget);

	// GLOBAL_SKILL_DUBLE_DMG
	ClearBit(g_iBitDoubleDamage, iTarget);

	// GLOBAL_SKILL_SEMICLIP
	set_entvar(iTarget, var_solid, SOLID_SLIDEBOX);

	// GLOBAL_SKILL_INVIZ | GLOBAL_SKILL_GHOST
	jbe_set_user_rendering(iTarget, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
	jbe_save_user_rendering(iTarget);

	// GLOBAL_SKILL_FOOTSTEPS
	rg_set_user_footsteps(iTarget, true);

	// GLOBAL_SKILL_LONG_JUMP
	ClearBit(g_iBitLongJump, iTarget);

	// GLOBAL_SKREEN_OFF
	UTIL_ScreenFade(iTarget, 0, 0, 0, 0, 0, 0, 255, 1);

	// GLOBAL_FREEZE
	if (IsSetBit(g_iBitUserFrozen, iTarget))
	{
		ClearBit(g_iBitUserFrozen, iTarget);
		set_entvar(iTarget, var_flags, get_entvar(iTarget, var_flags) & ~FL_FROZEN);
	}

	// GLOBAL_BURIED
	jbe_user_buried(iTarget, false);

	// оффаем прыжок если ни у кого нет
	ham_jump_check();
}

_global_menu_screen(id, iTarget, bool:bOff = true)
{
	new szTeam[26];

	if (0 < iTarget <= MaxClients)
		szTeam = "одному игроку";
	else if (iTarget == GT_PRISONER)
		szTeam = "заключенным";
	else if (iTarget == GT_GUARD)
		szTeam = "охранникам";
	else
		szTeam = "всем игрокам";

	new iChat = 0 < iTarget <= MaxClients ? print_team_grey : global_chat_team(iTarget);

	client_print_color(0, iChat,
		"%s ^4%n ^1%s экран ^3%s^1.", g_szGlobalPrefix,
		id, bOff ? "выключил" : "включил", szTeam);

	if (0 < iTarget <= MaxClients)
	{
		if (IsNotSetBit(g_iBitUserAlive, iTarget)) return;

		if (bOff)
			UTIL_ScreenFade(iTarget, 0, 0, 4, 0, 0, 0, 255, 1);
		else
			UTIL_ScreenFade(iTarget, 0, 0, 0, 0, 0, 0, 255, 1);
	}
	else
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;
			
			if (bOff)
				UTIL_ScreenFade(i, 0, 0, 4, 0, 0, 0, 255, 1);
			else
				UTIL_ScreenFade(i, 0, 0, 0, 0, 0, 0, 255, 1);
		}
	}

	if (iTarget)
		client_print_color(id, iTarget, "%s ^1Вы %s экран ^3%n^1.", g_szGlobalPrefix,
			bOff ? "выключили" : "включили", iTarget);
}

_global_menu_freeze(id, iTarget, bool:bFreeze = true) 
{
	client_print_color(0, global_chat_team(iTarget),
		"%s ^4%n ^1%s ^3%s^1.", g_szGlobalPrefix,
		id, bFreeze ? "заморозил" : "разморозил", global_get_name_target(iTarget, true));

	if (0 < iTarget <= MaxClients) 
	{
		if (IsNotSetBit(g_iBitUserAlive, iTarget)) 
			return;

		SetBitBool(g_iBitUserFrozen, iTarget, bFreeze);

		if (bFreeze)
			set_pev(iTarget, pev_flags, pev(iTarget, pev_flags) | FL_FROZEN);
		else
			set_pev(iTarget, pev_flags, pev(iTarget, pev_flags) & ~FL_FROZEN);
	} 
	else 
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++) 
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;

			SetBitBool(g_iBitUserFrozen, iTarget, bFreeze);
			
			if (bFreeze) 
				set_pev(i, pev_flags, pev(i, pev_flags) | FL_FROZEN);
			else 
				set_pev(i, pev_flags, pev(i, pev_flags) & ~FL_FROZEN);
		}
	}
}

_global_menu_buried(id, iTarget, bool:bBuried = true) 
{
	client_print_color(0, global_chat_team(iTarget),
		"%s ^4%n ^1%s ^3%s^1.", g_szGlobalPrefix,
		id, bBuried ? "закопал" : "откопал", global_get_name_target(iTarget, true));
	
	if (0 < iTarget <= MaxClients)
	{
		if (IsNotSetBit(g_iBitUserAlive, iTarget))
			return;
		jbe_user_buried(iTarget, bBuried);
	}
	else
	{
		iTarget *= -1;
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || iTarget && g_iUserTeam[i] != iTarget)
				continue;

			jbe_user_buried(i, bBuried);
		}
	}
}