#include "jbe_core\mafia\mafia_menu.inl"

mafia_init()
{
	register_menucmd(register_menuid("Show_MafiaMenu"), MENU_ALL_KEYS, "Handle_MafiaMenu");
	register_menucmd(register_menuid("Show_MafiaScreenRoleMenu"), MENU_ALL_KEYS, "Handle_MafiaScreenRoleMenu");
	register_menucmd(register_menuid("Show_MafiaRolesMenu"), MENU_ALL_KEYS, "Handle_MafiaRolesMenu");

	disable_event(g_iMafiaStatusValueShow = register_event("StatusValue", "Mafia_StatusValueShow", "be", "1=2", "2!0"));
	disable_event(g_iMafiaStatusValueHide = register_event("StatusValue", "Mafia_StatusValueHide", "be", "1=1", "2=0"));

	g_iSyncStatusMafia = CreateHudSyncObj();
	register_clcmdex("mafia", "ClCmd_Mafia", 1);
	register_clcmd("say", "ClCmd_Mafia_Say");
	register_clcmd("say_team", "ClCmd_Mafia_SayTeam");
}

public ClCmd_Mafia_Say(id) return jbe_mafia_chat(id, false);
public ClCmd_Mafia_SayTeam(id) return jbe_mafia_chat(id, true);

jbe_mafia_chat(id, bool:bIsTeam)
{
	if (!g_bMafiaGame || IsNotSetBit(g_iBitUserAlive, id)) return PLUGIN_CONTINUE;

	// днем общий чат не трогаем
	if (!bIsTeam && g_bMafiaDay) return PLUGIN_CONTINUE;

	// ночью в общий чат может писать только та роль что щас ходит и ведущий
	if (!bIsTeam && !g_bMafiaDay && id != g_iChiefId && (!g_iMafiaCurrentRole || g_iUserMafiaRole[id] != g_iMafiaCurrentRole))
	{
		client_print_color(id, id, "%s ^1Общий чат в ночное время заблокирован!", g_szMafiaPrefix);
		return PLUGIN_HANDLED;
	}

	static sMessage[190];

	read_args(sMessage, charsmax(sMessage));
	remove_quotes(sMessage);
	trim(sMessage);

	if (sMessage[0] == EOS)
		return PLUGIN_HANDLED;

	if (IsSetBit(g_iBitUserRusChat, id))
		translite_string(sMessage, charsmax(sMessage), sMessage);

	switch (bIsTeam)
	{
		case false:
		{
			if (!g_iMafiaCurrentRole && id == g_iChiefId)
			{
				client_print_color(id, id, "(Чат с ведущим) : Нет ролей с включенным экраном!");
				return PLUGIN_HANDLED;
			}

			for (new i = 1; i <= MaxClients; i++)
			{
				if (IsNotSetBit(g_iBitUserAlive, i) || g_iUserMafiaRole[i] != g_iMafiaCurrentRole && i != g_iChiefId)
					continue;

				client_print_color(i, id, "(Чат с ведущим) ^3%n ^1:  %s", id, sMessage);
			}
		}
		case true:
		{
			if (g_iUserMafiaRole[id] == DMR_PEACE_CITIZEN)
			{
				client_print_color(id, id, "%s ^1Командный чат заблокирован во время мафии.", g_szMafiaPrefix);
				return PLUGIN_HANDLED;
			}

			new iTeamNum;
			for (new i = 1; i <= MaxClients; i++)
			{
				if (IsNotSetBit(g_iBitUserAlive, i) || g_iUserMafiaRole[id] != g_iUserMafiaRole[i] || i == id)
					continue;
		
				iTeamNum++;
				client_print_color(i, id, "^4(^1Чат роли: %s^4) ^3%n ^1:  %s", g_szMafiaRoles[g_iUserMafiaRole[id]], id, sMessage);
			}
		
			if (iTeamNum)
				client_print_color(id, id, "^4(^1Чат роли: %s^4) ^3%n ^1:  %s", g_szMafiaRoles[g_iUserMafiaRole[id]], id, sMessage);
			else
				client_print_color(id, id, "^4(^1Чат роли: %s^4) ^1У вас нет живых союзников для использования чата.", g_szMafiaRoles[g_iUserMafiaRole[id]]);
		}
	}
			
	return PLUGIN_HANDLED;
}

public ClCmd_Mafia(id)
{
	if (jbe_is_acces_mafia_menu(id))
		return Show_MafiaMenu(id);

	return PLUGIN_HANDLED;
}

jbe_is_acces_mafia_menu(id)
{
	if (g_bMafiaGame && id == g_iChiefId)
		return true;

	else if (!g_bMafiaGame && IsNotSetBit(g_iBitUserAdmin, id) && IsNotSetBit(g_iBitUserGlobalAdmin, id))
		client_print_color(id, id, "%s Включить мафию может ^4Admin ^1и ^4Global^1.", g_szMafiaPrefix);

	else if (g_iDayMode != DAY_NORMAL)
		{}

	else if (g_bMafiaGame)
		client_print_color(id, id, "%s Запущена мафия, управлять игрой может только ведущий!", g_szMafiaPrefix);

	else if (g_iChiefGameType && g_iChiefGameType != GAME_MAFIA || g_iDuelStatus)
		client_print_color(id, id, "%s Запущена другая игра!", g_szMafiaPrefix);

	else return true;

	return false;
}

jbe_mafia_start()
{
	if (!g_iChiefId) return;
	
	g_bMafiaGame = true;
	g_bMafiaDay = true;
	g_iBitUserOffPM = 0; // включаем всем ПМ
	g_iChiefGameType = GAME_MAFIA;

	jbe_set_name_mode("Мафия");
	jbe_set_name_chief(g_iChiefId);
	jbe_set_user_godmode(g_iChiefId, 1);
	rg_set_user_footsteps(g_iChiefId, true);

	g_iGlobalBlock[GB_CHAT_ALL] = 0;
	g_iGlobalBlock[GB_PM] = 0;

	global_set_blocks_server(GB_SHOP, GB_RANK_BONUS, GB_BUY_RESPAWN, GB_GUARD_TEAM, GB_VIP_MENU, GB_HOOK, GB_CHAT_DEAD);

	enable_event(g_iMafiaStatusValueShow);
	enable_event(g_iMafiaStatusValueHide);

	disable_event(g_iEvent_StatusValueShow);
	disable_event(g_iEvent_StatusValueHide);

	set_lights(MAFIA_DAY_LIGHT);

	remove_task(TASK_PLAYER_CHECK);

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i))
			continue;

		jbe_reset_user_informer(i);
		if (g_iUserTeam[i] != TEAM_PRISONER)
			continue;

		_delete_user_all_skills(i);
		rg_remove_all_items(i);
		rg_give_item(i, "weapon_knife");
	}
}

jbe_mafia_ended()
{
	g_bMafiaGame = false;
	g_bMafieFreezed = false;
	g_bMafiaSetsRoles = false;
	g_bMafiaOneDay = false;
	g_iBitMafiaNight = 0;
	g_iChiefGameType = GAME_NONE;

	jbe_set_name_mode("");
	jbe_set_name_chief(g_iChiefId);

	disable_event(g_iMafiaStatusValueShow);
	disable_event(g_iMafiaStatusValueHide);

	enable_event(g_iEvent_StatusValueShow);
	enable_event(g_iEvent_StatusValueHide);

	set_lights("#OFF");

	arrayset(g_iUserMafiaRole, 0, sizeof g_iUserMafiaRole);
	
	if (g_bRoundEnd) return;

	global_clear_blocks();
	enable_task_player_check();

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i))
			continue;

		jbe_reset_user_informer(i);

		if (i == g_iChiefId)
			jbe_set_user_godmode(i, 0);

		if (g_iUserTeam[i] != TEAM_PRISONER)
			continue;

		jbe_set_user_maxspeed(i, _, true);

		UTIL_ScreenFade(i, 0, 0, 0, 0, 0, 0, 255, 1);

		reload_user_voice(i);
	}
}

jbe_mafia_day()
{
	g_iBitMafiaNight = 0;
	g_iMafiaCurrentRole = 0;
	g_bMafiaDay = true;
	g_bMafiaOneDay = true;
	set_lights(MAFIA_DAY_LIGHT);

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || g_iUserTeam[i] != TEAM_PRISONER)
			continue;

		VTC_UnmuteClient(i);
		UTIL_ScreenFade(i, 0, 0, 0, 0, 0, 0, 255, 1);
	}

	jbe_info_center_msg(0, "Мафия: Наступает день");
}

jbe_mafia_night()
{
	g_bMafiaDay = false;
	set_lights(MAFIA_NIGHT_LIGHT);

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || g_iUserTeam[i] != TEAM_PRISONER)
			continue;

		VTC_MuteClient(i);
		UTIL_ScreenFade(i, 0, 0, 4, 0, 0, 0, 255, 1);
		SetBit(g_iBitMafiaNight, i);
		jbe_default_player_model(i);
	}

	jbe_info_center_msg(0, "Мафия: Наступает ночь");
}

jbe_mafia_list_roles(id)
{
	new szRoles[512];
	new iLen;
	
	iLen = formatex(szRoles, charsmax(szRoles), "Активные роли:");

	new i, j;
	for (i = DMR_MAFIA; i < DATA_MAFIA_ROLES; i++)
	{
		iLen += formatex(szRoles[iLen], charsmax(szRoles) - iLen, "^n");

		for (j = 1; j <= MaxClients; j++)
		{
			if (IsNotSetBit(g_iBitUserAlive, j) || g_iUserMafiaRole[j] != i)
				continue;

			iLen += formatex(szRoles[iLen], charsmax(szRoles) - iLen, "^n%s: %n",
				g_szMafiaRoles[ g_iUserMafiaRole[j] ], j);
		}
	}

	set_hudmessage(200, 200, 300, -1.0, -3.0, 0, 0.0, 10.0, 0.0, 0.0, -1);
	ShowSyncHudMsg(id, g_iSyncStatusMafia, szRoles);
}

jbe_mafia_skin_aiming(id)
{
	new iTarget, iBody;
	get_user_aiming(id, iTarget, iBody);

	if (!jbe_is_user_valid(iTarget) || g_iUserTeam[iTarget] != TEAM_PRISONER)
		return client_print_center(id, "Наведите прицел");

	new iUserSkin = get_entvar(iTarget, var_skin);

	if (iUserSkin == SKIN_WANTED)
	{
		if (g_iPersonalModel[iTarget] > 0)
			jbe_personal_model_set(iTarget);
		else set_entvar(iTarget, var_skin, g_iUserSkin[iTarget]);

		client_print_center(id, "Убрана форма: %n", iTarget);
	}
	else
	{
		if (g_iPersonalModel[iTarget] > 0)
			jbe_set_player_prisoner_model(iTarget);

		set_entvar(iTarget, var_skin, SKIN_WANTED);

		client_print_center(id, "Выдана форма: %n", iTarget);
	}

	return PLUGIN_HANDLED;
}

jbe_mafia_freeze(bool:bFreeze)
{
	g_bMafieFreezed = bFreeze;

	new Float:fSpeed = bFreeze ? 0.001 : 0.0;

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i) || g_iUserTeam[i] != TEAM_PRISONER)
			continue;

		jbe_set_user_maxspeed(i, fSpeed, true);
	}
}

jbe_mafia_freeze_aiming(id)
{
	new iTarget, iBody;
	get_user_aiming(id, iTarget, iBody);

	if (!jbe_is_user_valid(iTarget) || g_iUserTeam[iTarget] != TEAM_PRISONER)
		return client_print_center(id, "Наведите прицел");

	if (!g_bMafiaDay)
		return client_print_center(id, "Доступно только днем");

	new Float:fSpeed; pev(iTarget, pev_maxspeed, fSpeed);

	if (fSpeed == 0.001)
	{
		jbe_set_user_maxspeed(iTarget, _, true);

		client_print_center(id, "Разморожен: %n", iTarget);
		client_print_center(iTarget, "Вы разморожены");
	}
	else
	{
		jbe_set_user_maxspeed(iTarget, 0.001, true);

		client_print_center(id, "Заморожен: %n", iTarget);
		client_print_center(iTarget, "Вы заморожены");
	}

	return PLUGIN_HANDLED;
}

public Mafia_StatusValueShow(id)
{
	if (IsSetBit(g_iBitMafiaNight, id))
		return;

	remove_task(id + TASK_STATUSVALUE_HIDE);

	new iTarget = read_data(2);

	set_hudmessage(102, 69, 0, -1.0, 0.8, 0, 0.0, 10.0, 0.0, 0.0, -1);

	if (g_iUserTeam[iTarget] == TEAM_GUARD)
	{
		ShowSyncHudMsg(id, g_iSyncStatusMafia,
			"%s: %n", iTarget == g_iChiefId ? "Ведущий" : "Охранник", iTarget);
	}

	else if (id == g_iChiefId)
	{
		ShowSyncHudMsg(id, g_iSyncStatusMafia,
			"Житель: %n^n\
			Роль: %s^n\
			Статус: %s",
		iTarget,
		g_szMafiaRoles[ g_iUserMafiaRole[iTarget] ],
		IsSetBit(g_iBitMafiaNight, iTarget) ? "Спит" : "Не спит");
	}

	else if ((g_iUserMafiaRole[id] == DMR_MAFIA || g_iUserMafiaRole[id] == DMR_SHERIF)
			&& g_iUserMafiaRole[id] == g_iUserMafiaRole[iTarget])
	{
		ShowSyncHudMsg(id, g_iSyncStatusMafia,
			"Житель: %n^n\
			Роль: %s (Союзник)^n\
			Статус: %s",
		iTarget,
		g_szMafiaRoles[ g_iUserMafiaRole[iTarget] ],
		IsSetBit(g_iBitMafiaNight, iTarget) ? "Спит" : "Не спит");
	}

	else
	{
		ShowSyncHudMsg(id, g_iSyncStatusMafia, "Житель: %n", iTarget);
	}
}

public Mafia_StatusValueHide(id)
{
	set_task(1.0, "Mafia_StatusValueHide_task", id + TASK_STATUSVALUE_HIDE);
}

public Mafia_StatusValueHide_task(id) 
{
	id -= TASK_STATUSVALUE_HIDE;

	if (IsNotSetBit(g_iBitUserConnected, id))
		return;
	
	ClearSyncHud(id, g_iSyncStatusMafia);
}

public jbe_main_mafia_informer(id)
{
	id -= TASK_SHOW_INFORMER;

	set_hudmessage(0, 255, 0, g_fMainInformerPosX[id], g_fMainInformerPosY[id], 0, 0.0, 0.8, 0.2, 0.2, -1);

	if (g_iUserTeam[id] == TEAM_PRISONER)
	{
		ShowSyncHudMsg(id, g_iSyncMainInformer,
			"%s%s\
			%s\
			^nЖителей: %d\
			^n^nВаша роль: %s",
		g_szDayMode, g_szDayModeTimer,
		g_szChiefName,
		g_iAlivePlayersNum[TEAM_PRISONER],
		g_szMafiaRoles[ g_iUserMafiaRole[id] ]);
	}
	else
	{
		ShowSyncHudMsg(id, g_iSyncMainInformer,
			"%s%s\
			%s\
			^nЖителей: %d",
		g_szDayMode, g_szDayModeTimer,
		g_szChiefName,
		g_iAlivePlayersNum[TEAM_PRISONER]);
	}
}