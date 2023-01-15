#include "jbe_core\jihad\jihad_menu.inl"

jihad_init()
{
	register_menucmd(register_menuid("Show_JihadMenu"), MENU_ALL_KEYS, "Handle_JihadMenu");
	register_menucmd(register_menuid("Show_JihadRolesMenu"), MENU_ALL_KEYS, "Handle_JihadRolesMenu");
	register_menucmd(register_menuid("Show_JihadTeleportMenu"), MENU_ALL_KEYS, "Handle_JihadTeleportMenu");

	register_clcmd("jihad", "ClCmd_Jihad");
}

public ClCmd_Jihad(id)
{
	if (is_not_access_jihad_menu(id))
		return PLUGIN_HANDLED;

	return Show_JihadMenu(id);
}

jihad_enable()
{
	g_bJihadGame = true;
	remove_task(TASK_PLAYER_CHECK);
	jbe_set_name_mode("Битва за Джихад");
	g_iChiefGameType = GAME_JIHAD;

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i))
			continue;

		jbe_set_user_godmode(i, 1);
	}
}

jihad_disable()
{
	g_bJihadGame = false;
	enable_task_player_check();
	g_bJihadStart = false;
	g_iChiefGameType = GAME_NONE;
	g_szDayModeTimer[0] = EOS;
	jbe_set_name_mode("");

	arrayset(g_iJihadRole, 0, sizeof g_iJihadRole);
	remove_task(TASK_CHIEF_GAME_START);

	for (new i; i < sizeof szHamHookEntityBlock; i++)
		DisableHamForward(g_iHamHookForwards[i]);

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i))
			continue;

		jbe_set_user_godmode(i, 0);
	}

	global_clear_blocks();
	jbe_radar_on();
}

is_not_access_jihad_menu(id)
{
	if (IsNotSetBit(g_iBitUserAdmin, id) && IsNotSetBit(g_iBitUserGlobalAdmin, id))
		client_print_color(id, id, "%s Доступно только администраторам!", g_szChatPrefix);

	else if (id != g_iChiefId)
		client_print_color(id, id, "%s Доступно только начальнику!", g_szChatPrefix);

	else if (g_iDayMode != DAY_NORMAL)
		{}

	else if (g_iChiefGameType && g_iChiefGameType != GAME_JIHAD || g_iDuelStatus)
		client_print_color(id, id, "%s Запущена другая игра!", g_szMafiaPrefix);

	else return false;

	return true;
}

jihad_start()
{
	g_bJihadStart = true;
	jbe_set_name_mode("Разбегаемся: ");
	jbe_set_hamblock(func_vehicle, func_tracktrain, func_tank, game_player_hurt, func_recharge, func_healthcharger, game_player_equip, player_weaponstrip, trigger_gravity, armoury_entity, weaponbox, weapon_shield);
	global_set_blocks_server(GB_SHOP, GB_RANK_BONUS, GB_BUY_RESPAWN, GB_VIP_MENU, GB_HOOK);
	jbe_radar_off();

	set_task(1.0, "jihad_start_task", TASK_CHIEF_GAME_START, _, _, "a", g_iDayModeTimer = 41);
}

public jihad_start_task()
{
	if (--g_iDayModeTimer)
	{
		formatex(g_szDayModeTimer, charsmax(g_szDayModeTimer), "[%i]", g_iDayModeTimer);
		if (g_iDayModeTimer <= 10)
			rg_send_audio(0, sounds_countdown[g_iDayModeTimer]);
			
	}
	else
	{
		g_szDayModeTimer[0] = EOS;
		jbe_set_name_mode("Битва за Джихад");
		g_iFriendlyFire = FF_PRISONER;

		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserAlive, i) || i == g_iChiefId)
				continue;

			_delete_user_all_skills(i);
			rg_remove_all_items(i, .removeSuit = false);
			rg_give_item(i, "weapon_knife");

			jihad_set_user_role(i, g_iJihadRole[i]);
		}

		if (IsSetBit(g_iBitUserAlive, g_iChiefId))
		{
			rg_set_user_noclip(g_iChiefId, 1);
			jbe_set_user_maxspeed(g_iChiefId, 700.0, true);
			jbe_set_user_rendering(g_iChiefId, kRenderFxGlowShell, 0, 0, 0, kRenderTransAlpha, 0);
			jbe_save_user_rendering(g_iChiefId);
			jbe_hide_user_costumes(g_iChiefId);
			set_entvar(g_iChiefId, var_solid, SOLID_NOT);
		}
	}
}

jihad_show_no_roles(id)
{
	new szMessage[512];
	new iLen;
	
	iLen = formatex(szMessage, charsmax(szMessage), "Игроки без роли:^n");

	for (new i = 1; i < MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || i == g_iChiefId || g_iJihadRole[i])
			continue;

		iLen += formatex(szMessage[iLen], charsmax(szMessage) - iLen, "^n%n", i);
	}

	set_hudmessage(200, 200, 300, -1.0, -3.0, 0, 0.0, 10.0, 0.0, 0.0, -1);
	ShowSyncHudMsg(id, g_iSyncInfoCenter, szMessage);
}

jihad_kill_no_roles()
{
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || i == g_iChiefId)
			continue;

		if (!g_iJihadRole[i])
			user_kill(i);
	}

	client_print_color(0, 0, "%s ^1Начальник убил всех игроков без роли.", g_szJihadPrefix);
}

jihad_set_user_role(id, iRole)
{
	if (iRole == DJR_RIOT_POLICE && g_iUserTeam[id] != TEAM_GUARD)
	{
		client_print_color(g_iChiefId, id, "%s Роль омоновец можно выдать только охране!", g_szJihadPrefix);
		return PLUGIN_HANDLED;
	}
	else if (iRole != DJR_RIOT_POLICE && g_iUserTeam[id] == TEAM_GUARD)
	{
		client_print_color(g_iChiefId, id, "%s Охране можно выдать только роль омоновец!", g_szJihadPrefix);
		return PLUGIN_HANDLED;
	}

	if (!g_bJihadStart)
	{
		g_iJihadRole[id] = iRole;
		client_print_color(0, id, "%s Игроку ^3%n ^1выдана роль: ^4%s^1.", g_szJihadPrefix, id, g_szJihadNameRoles[iRole]);
	}
	else
		client_print_color(id, id, "%s ^1Ваша роль: ^4%s^1.", g_szJihadPrefix, g_szJihadNameRoles[iRole]);

	switch (iRole)
	{
		case DJR_CAMPER:
		{
			if (g_bJihadStart)
			{
				set_entvar(id, var_health, 1000.0);
				rg_give_item_ex(id, "weapon_awp", GT_APPEND, 1000);
				rg_give_item_ex(id, "weapon_usp", GT_APPEND, 1000);
			}
			client_print_color(g_bJihadStart ? id : 0, 0, "^4 *^1 1000 HP, AWP, USP, Можно кэмперить.");
		}
		case DJR_JABBA:
		{
			if (g_bJihadStart)
			{
				set_entvar(id, var_health, 2500.0);
				rg_give_item_ex(id, "weapon_m249", GT_APPEND, 1000);
				rg_give_item_ex(id, "weapon_deagle", GT_APPEND, 1000);
				jbe_set_user_maxspeed(id, 50.0, .bResetSpeed = true);
			}
			client_print_color(g_bJihadStart ? id : 0, 0, "^4 *^1 2500 HP, M249, Deagle, Скорость 50.");
		}
		case DJR_MARTIAN:
		{
			if (g_bJihadStart)
			{
				set_entvar(id, var_health, 300.0);
				rg_give_item_ex(id, "weapon_usp", GT_APPEND, 999999);
				jbe_set_user_rendering(id, kRenderFxGlowShell, 0, 0, 0, kRenderTransAlpha, 30);
				jbe_save_user_rendering(id);
				jbe_set_user_maxspeed(id, 550.0, .bResetSpeed = true);
				// +50 HP за килл
			}
			client_print_color(g_bJihadStart ? id : 0, 0, "^4 *^1 300 HP, USP, Видимость 20%, Скорость 550, +50 HP за убийство.");
		}
		case DJR_WORN:
		{
			if (g_bJihadStart)
			{
				set_entvar(id, var_health, 1500.0);
				rg_give_item_ex(id, "weapon_glock18", GT_APPEND, 999999);
				rg_give_item_ex(id, "weapon_m3", GT_APPEND, 999999);
				jbe_set_user_maxspeed(id, 750.0, .bResetSpeed = true);
				UTIL_ScreenShake(id, (255<<14), SF_SHAKE_EVERYONE, (255<<14), .iReliable = 1);
			}
			client_print_color(g_bJihadStart ? id : 0, 0, "^4 *^1 1500 HP, Glock, M3, Скорость 750, Бесконечные патроны, Накачан наркотиками.");
		}
		case DJR_SPY:
		{
			if (g_bJihadStart)
			{
				set_entvar(id, var_health, 1500.0);
				rg_give_item_ex(id, "weapon_usp", GT_APPEND, 1000);

				SetBit(g_iBitBasaballbat, id);
				new iActiveItem = get_member(id, m_pActiveItem);
				if (iActiveItem > 0)
				{
					InvertBit(g_iBitWeaponStatus, id);
					ExecuteHamB(Ham_Item_Deploy, iActiveItem);
					UTIL_WeaponAnimation(id, 3);
				}
				
				SetBit(g_iBitFrostNade, id);
				check_frostnade_fwd();
				rg_give_item(id, "weapon_smokegrenade");
	
				rg_set_user_footsteps(id, true);
			}
			client_print_color(g_bJihadStart ? id : 0, 0, "^4 *^1 1500 HP, USP, Бита, Граната заморозка, Тихий шаг.");
		}
		case DJR_SMUGGLER:
		{
			if (g_bJihadStart)
			{
				set_entvar(id, var_health, 1000.0);
				rg_give_item_ex(id, "weapon_elite", GT_APPEND, 999999);
				rg_give_item_ex(id, "weapon_scout", GT_APPEND, 999999);
				rg_give_item_ex(id, "weapon_mac10", GT_APPEND, 999999);
				// +100 HP за килл
			}
			client_print_color(g_bJihadStart ? id : 0, 0, "^4 *^1 1000 HP, ELite, Scout, MAC10, Бесконечные патроны, +100 HP за убийство.");
		}
		case DJR_RIOT_POLICE:
		{
			if (g_bJihadStart)
			{
				set_entvar(id, var_health, 1500.0);
				rg_set_user_armor(id, 250, ARMOR_VESTHELM);
				rg_give_item(id, "weapon_shield");
				rg_give_item_ex(id, "weapon_deagle", GT_APPEND, 1000);
				rg_give_item_ex(id, "weapon_usp", GT_APPEND, 1000);
				rg_give_item_ex(id, "weapon_p228", GT_APPEND, 1000);
			}
			client_print_color(g_bJihadStart ? id : 0, 0, "^4 *^1 1500 HP, Бронежилет+шлем, Щит, Deagle, USP, P228.");
		}
		case DJR_ALTAIR:
		{
			if (g_bJihadStart)
			{
				set_entvar(id, var_health, 1000.0);
				rg_give_item_ex(id, "weapon_ak47", GT_APPEND, 1000);
				rg_give_item_ex(id, "weapon_galil", GT_APPEND, 1000);
				rg_give_item_ex(id, "weapon_elite", GT_APPEND, 1000);
				set_entvar(id, var_gravity, 0.2);
			}
			client_print_color(g_bJihadStart ? id : 0, 0, "^4 *^1 1000 HP, AK47, Galil, Elite, Гравитация 20%.");
		}
		case DJR_DIGGER:
		{
			if (g_bJihadStart)
			{
				set_entvar(id, var_health, 9000.0);
				rg_give_item_ex(id, "weapon_awp", GT_APPEND, 1000);
				rg_give_item_ex(id, "weapon_elite", GT_APPEND, 1000);
				jbe_set_user_maxspeed(id, 0.01, .bResetSpeed = true);
				jbe_user_buried(id, .bSet = true);
			}
			client_print_color(g_bJihadStart ? id : 0, 0, "^4 *^1 9000 HP, AWP, Elite, Закопан на всю игру.");
		}
		case DJR_STRATEGIST:
		{
			if (g_bJihadStart)
			{
				set_entvar(id, var_health, 1500.0);
				rg_give_item_ex(id, "weapon_scout", GT_APPEND, 1000);
				rg_give_item_ex(id, "weapon_glock18", GT_APPEND, 1000);
				// показ радара и лишение других оружия на потом
			}
			client_print_color(g_bJihadStart ? id : 0, 0, "^4 *^1 1500 HP, Scout, Glock");
		}
		case DJR_MARTYR:
		{
			if (g_bJihadStart)
			{
				set_entvar(id, var_health, 5000.0);
				rg_give_item_ex(id, "weapon_glock18", GT_APPEND, 999999);
				rg_give_item_ex(id, "weapon_hegrenade", GT_APPEND, 999999);
				// всю игру отравлен - на потом
			}
			client_print_color(g_bJihadStart ? id : 0, 0, "^4 *^1 5000 HP, Glock, HeGrenade, Бесконечные патроны.");
		}
		case DJR_FAN:
		{
			if (g_bJihadStart)
			{
				set_entvar(id, var_health, 5000.0);

				SetBit(g_iBitBasaballbat, id);
				new iActiveItem = get_member(id, m_pActiveItem);
				if (iActiveItem > 0)
				{
					InvertBit(g_iBitWeaponStatus, id);
					ExecuteHamB(Ham_Item_Deploy, iActiveItem);
					UTIL_WeaponAnimation(id, 3);
				}

				jbe_set_user_maxspeed(id, 750.0, .bResetSpeed = true);
				UTIL_ScreenShake(id, (255<<14), SF_SHAKE_EVERYONE, (255<<14), .iReliable = 1);
			}
			client_print_color(g_bJihadStart ? id : 0, 0, "^4 *^1 5000 HP, Бита, Скорость 750, Накачан наркотиками.");
		}
	}
	return PLUGIN_HANDLED;
}

