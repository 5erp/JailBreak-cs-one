/* -> Bet Menu -> */
Show_BetMenu(id)
{
	if (!g_iDuelStatus || (g_iUserTeam[id] != TEAM_PRISONER && g_iUserTeam[id] != TEAM_GUARD)
	|| g_iDuelStatus == DS_RUN || g_iDuelUsersId[0] == id || g_iDuelUsersId[1] == id)
		return PLUGIN_HANDLED;
		
	menu_title("Время делать ставки!^n\yУ вас \r%d\y$^n", g_iUserMoney[id]);
	menu_item("\wСумма ставки \r%d\y$^n^n", g_iUserBetCost[id]);
	menu_item("\y1. \wПоставить на \r%n^n", g_iDuelUsersId[0]);
	menu_item("\y2. \wПоставить на \r%n^n^n", g_iDuelUsersId[1]);
	menu_item("\y3. \wПовысить ставку до \r%d\y$^n", fnGetUserBetMoney_up(id));
	menu_item("\y4. \wПонизить ставку до \r%d\y$^n", fnGetUserBetMoney_down(id));
	menu_item("\y5. \wВвести сумму ставки^n");
	menu_item("^n\d0. \w %L", id, "JBE_MENU_EXIT");
	
	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<9), "Show_BetMenu");
}

public Handle_BetMenu(id, iKey)
{
	if (!g_iDuelStatus || g_iUserTeam[id] == TEAM_SPECTATOR
	&& g_iUserTeam[id] == TEAM_UNASSIGNED || g_iDuelStatus == DS_RUN 
	|| g_iDuelUsersId[0] == id || g_iDuelUsersId[1] == id)
		return PLUGIN_HANDLED;

	switch(iKey)
	{
		case 0:
		{
			if (g_iUserBetCost[id] <= 0)
			{
				client_print_color(id, 0, "%s ^1Вы не ввели сумму ставки.", g_szBetPrefix);
				return Show_BetMenu(id);
			}
			else if (g_iUserMoney[id] < g_iUserBetCost[id])
			{
				client_print_color(id, 0, "%s ^1На вашем счету недостаточно средств для ставки.", g_szBetPrefix);
				return Show_BetMenu(id);
			}
			else
			{
				SetBit(g_iBitUserBetPlayer[0], id);
				jbe_set_user_money(id, g_iUserMoney[id] - g_iUserBetCost[id], 1);
				SetBit(g_iBitUserBet, id);
				client_print_color(0, id, "%s ^3%n ^1поставил ^4%d$ ^1на ^4%n^1.", 
					g_szBetPrefix, id, g_iUserBetCost[id], g_iDuelUsersId[0]);
				return PLUGIN_HANDLED;
			}
		}
		case 1:
		{
			if (g_iUserBetCost[id] <= 0)
			{
				client_print_color(id, 0, "%s ^1Вы не ввели сумму ставки.", g_szBetPrefix);
				return Show_BetMenu(id);
			}
			else if (g_iUserMoney[id] < g_iUserBetCost[id])
			{
				client_print_color(id, 0, "%s ^1На вашем счету недостаточно средств для ставки.", g_szBetPrefix);
				return Show_BetMenu(id);
			}
			else
			{
				SetBit(g_iBitUserBetPlayer[1], id);
				jbe_set_user_money(id, g_iUserMoney[id] - g_iUserBetCost[id], 1);
				SetBit(g_iBitUserBet, id);
				client_print_color(0, id, "%s ^3%n ^1поставил ^4%d$ ^1на ^4%n^1.", g_szBetPrefix, id, g_iUserBetCost[id], g_iDuelUsersId[1]);
				return PLUGIN_HANDLED;
			}
		}
		case 2:
		{
			new iMoney = fnGetUserBetMoney_up(id);
			if (g_iUserMoney[id] < iMoney)
			{
				client_print_color(id, 0, "%s ^1На вашем счету недостаточно средств для ставки.", g_szBetPrefix);
			}
			else 
				g_iUserBetCost[id] = iMoney;
			return Show_BetMenu(id);
		}
		case 3:
		{
			new iMoney = fnGetUserBetMoney_down(id);
			if (!iMoney)
			{
				client_print_color(id, 0, "%s ^1Некуда снижать ставку.", g_szBetPrefix);
			}
			else g_iUserBetCost[id] = iMoney;
			return Show_BetMenu(id);
		}
		case 4:
		{
			client_cmd(id, "messagemode money_bet");
			client_print_color(id, 0, "%s ^1Введите сумму ставки.", g_szBetPrefix);
		}
	}
	return PLUGIN_HANDLED;
}
