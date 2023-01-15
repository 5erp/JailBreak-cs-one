// duel_prize_menu.inl

Show_DuelPrizeMenu(id)
{
	if (id != g_iLastPnId || IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;

	if (!g_iMenuTarget[id] || !jbe_is_user_valid(g_iMenuTarget[id])
	|| IsNotSetBit(g_iBitUserAlive, g_iMenuTarget[id])
	|| g_iUserTeam[g_iMenuTarget[id]] != TEAM_GUARD)
	{
		g_iMenuTarget[id] = fnGetRandomAlive(random_num(1, g_iAlivePlayersNum[TEAM_GUARD]), TEAM_GUARD);
	}

	if (!g_iDuelPrizeTarget)
	{
		g_iDuelPrizeTarget = id;
	}
	else if (g_iDuelPrizeTarget != id
	&& (IsNotSetBit(g_iBitUserConnected, g_iDuelPrizeTarget) || g_iUserTeam[g_iDuelPrizeTarget] != TEAM_PRISONER))
	{
		g_iDuelPrizeTarget = id;
	}

	if (g_iDuelPrizeType > DUEL_PRIZE_VOICE)
		g_iDuelPrizeType = DUEL_PRIZE_MONEY;

	menu_title("Выберите награду^n^n");
	menu_item("\y1. \wСтарт дуэли^n^n");
	menu_item("\y2. \wДуэль на: \r%s^n", g_iDuelLang[g_iDuelType]);
	menu_item("\y3. \wПротивник: \y%n^n^n", g_iMenuTarget[id]);
	if (jbe_get_players_num(NUM_PRISONER_GUARD) >= PRIZE_MIN_PLAYERS)
	{
		menu_item("\y4. \wНаграда: \r%s^n", g_szDuelPrizes[g_iDuelPrizeType]);

		if (g_iDuelPrizeTarget == id)
			menu_item("\y5. \wПолучит: \yНаграда мне^n");
		else
			menu_item("\y5. \wПолучит: \y%n^n", g_iDuelPrizeTarget);
	}
	else
	{
		menu_item("\y4. \dНаграда: Нет^n");
		menu_item("\y5. \dПолучит: Никто^n");
	}
	menu_item("\y6. \wНастройки награды^n");

	menu_item("^n^n\y9. \wОтмена");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Show_DuelPrizeMenu");
}

public Handle_DuelPrizeMenu(id, iKey)
{
	if (id != g_iLastPnId || IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 0:
		{
			if (IsSetBit(g_iBitUserAlive, g_iMenuTarget[id]))
			{
				jbe_duel_start_ready(id, g_iMenuTarget[id]);
				g_iLastPnType = LAST_PN_DUEL;

				if (jbe_get_players_num(NUM_PRISONER_GUARD) < PRIZE_MIN_PLAYERS)
					return PLUGIN_HANDLED;

				if (g_iDuelPrizeTarget == id)
					client_print_color(0, 0, "%s ^4%n ^1играет на ^4%s^1.", g_szDuelPrefix, id, g_szDuelPrizes[g_iDuelPrizeType]);
				else
					client_print_color(0, 0, "%s ^4%n ^1играет на ^4%s ^1для ^4%n^1.", g_szDuelPrefix, id, g_szDuelPrizes[g_iDuelPrizeType], g_iDuelPrizeTarget);
			}
			else
			{
				client_print_color(id, 0, "%s Игрок мертв..", g_szDuelPrefix);
				return Show_DuelPrizeMenu(id);
			}
		}
		case 1: return Show_ChoiceDuelMenu(id);
		case 2: return Cmd_DuelUsersMenu(id);
		case 3:
		{
			if (jbe_get_players_num(NUM_PRISONER_GUARD) >= PRIZE_MIN_PLAYERS)
			{
				++g_iDuelPrizeType;
				g_iDuelPrizeType %= (sizeof(g_szDuelPrizes) - 2);

				if (g_iDuelPrizeType == DUEL_PRIZE_FD && g_iPlayersNum[TEAM_PRISONER] < MIN_ALL_PL_FD)
				{
					++g_iDuelPrizeType;
					g_iDuelPrizeType %= (sizeof(g_szDuelPrizes) - 2);
					client_print_color(id, id, "%s ^1Выбрать выходной возможно если на сервере от %d заключенных.", g_szChatPrefix, MIN_ALL_PL_FD);
				}
			}
			else
			{
				client_print_color(id, 0, "%s Награду можно выбрать если на сервере от ^4%d-ти ^1игроков.", g_szDuelPrefix, PRIZE_MIN_PLAYERS);
			}
			return Show_DuelPrizeMenu(id);
		}
		case 4:
		{
			if (jbe_get_players_num(NUM_PRISONER_GUARD) >= PRIZE_MIN_PLAYERS)
				return Cmd_DuelPrizeTargetMenu(id);
			else
			{
				client_print_color(id, 0, "%s Награду можно выбрать если на сервере от ^4%d-ти ^1игроков", g_szDuelPrefix, PRIZE_MIN_PLAYERS);
				return Show_DuelPrizeMenu(id);
			}
		}
		case 5: return Show_DuelCustomPrizeMenu(id);
		case 8: return Show_LastPrisonerMenu(id);
	}
	return PLUGIN_HANDLED;
}