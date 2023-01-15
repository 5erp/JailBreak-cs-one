// check players afk

#define PLAYER_CHECK_SEC		30	// как часто проверять игроков
#define AFK_SECONDS_TRANSFER	30	//20 скок можно стоять в афк сек до варна
#define AFK_WARN_LIMIT			5	// лимит варнов
#define AFK_MAX_LASTMOVEMENT	180	//300

enable_task_player_check()
{
	set_task_ex(PLAYER_CHECK_SEC.0, "task_player_check", TASK_PLAYER_CHECK, _, _, SetTask_Repeat);
}

public task_player_check()
{
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserConnected, i))
			continue;

		afk_user_check(i);
	}
}

afk_user_check(id)
{
	if (IsNotSetBit(g_iBitUserAlive, id))
		return;

	if (IsSetBit(g_iBitUserFrozen, id) || IsSetBit(g_iBitUserBuried, id))
		return;

	static Float:fAngles[MAX_PLAYERS + 1][3];
	new Float:tmpAngles[3];
	new Float:fLastMovement = get_gametime() - Float:get_member(id, m_fLastMovement);

	get_entvar(id, var_angles, tmpAngles); // направление взгляда

	if ((xs_vec_equal(tmpAngles, fAngles[id]) && fLastMovement >= AFK_SECONDS_TRANSFER.0)
	|| (fLastMovement > AFK_MAX_LASTMOVEMENT.0)) /* если вектор сменился, но LastMovement был давно */	
	{
		if (++g_iUserAfkWarns[id] >= AFK_WARN_LIMIT)
		{
			for (new i = 1; i <= MaxClients; i++)
			{
				if (IsNotSetBit(g_iBitUserConnected, i) || i == id) continue;
				client_print_color(i, id, "%s ^3%n ^1переведен в наблюдатели за ^4AFK^1.", g_szChatPrefix, id);
			}

			jbe_set_user_team(id, TEAM_SPECTATOR);
			Show_ChooseTeamMenu(id, 0);
			client_print_color(id, id, "%s ^1Вы переведены в наблюдатели за ^4AFK^1.", g_szChatPrefix);
		}
		else
		{
			client_print_color(id, id, "%s ^1Двигайтесь или будете переведены в наблюдатели, ^4warns ^1(^3%d^1/^3%d^1).", g_szChatPrefix, g_iUserAfkWarns[id], AFK_WARN_LIMIT);
		}
	}
	else
	{
		xs_vec_copy(tmpAngles, fAngles[id]);
		g_iUserAfkWarns[id] = 0;
	}

	IsSetBit(g_iBitRconAdmin, id) && client_print(id, print_console, "(afk) fLastMovement %f", fLastMovement);
}

afk_user_clear_warns(id) g_iUserAfkWarns[id] = 0;
