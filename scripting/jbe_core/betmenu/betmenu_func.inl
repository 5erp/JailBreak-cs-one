#include "jbe_core\betmenu\betmenu_menu.inl"

bets_init()
{
	register_menucmd(register_menuid("Show_BetMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<9), "Handle_BetMenu");

	register_clcmdex("betmenu", "ClCmd_BetMenu", 1); // say /betmenu
	register_clcmd("money_bet", "ClCmd_MoneyBet"); // /money_bet
}

/*===== -> Ставки в дуэлях -> =====*///{

public ClCmd_BetMenu(id)
{
	if (IsSetBit(g_iBitUserBet, id) || g_iDuelStatus == DS_RUN
	|| g_iDuelUsersId[0] == id || g_iDuelUsersId[1] == id)
	{
		if (IsSetBit(g_iBitUserBet, id))
			client_print_color(id, 0, "%s ^1Вы уже сделали ставку!", g_szChatPrefix);

		else if (g_iDuelUsersId[0] == id || g_iDuelUsersId[1] == id)
			client_print_color(id, 0, "%s ^1Дуэлянты не могут делать ставки!", g_szChatPrefix);

		else if (g_iDuelStatus == DS_RUN)
			client_print_color(id, 0, "%s ^1Дуэль началась, поздно делать ставки!", g_szChatPrefix);

		return PLUGIN_HANDLED;
	}
	
	return Show_BetMenu(id);
}

public ClCmd_MoneyBet(id, iMoney)
{
	if (!g_iDuelStatus || g_iDuelStatus == DS_RUN)
	{
		client_print_color(id, 0, "%s ^1Дуэль началась..", g_szBetPrefix);
		return PLUGIN_HANDLED;
	}
	
	new szArg[7];
	read_argv(1, szArg, charsmax(szArg));
	
	if (!is_str_num(szArg))
	{
		client_print_color(id, print_team_red, "%s ^1Вы ввели ^3неверные параметры ^1операции, попробуйте ещё раз.", g_szBetPrefix);
		return Show_BetMenu(id);		
	}
	
	iMoney = str_to_num(szArg);
	iMoney = floatround(iMoney * 0.5, floatround_floor) * 2;
	
	if (g_iUserMoney[id] < iMoney)
	{
		client_print_color(id, print_team_red, "%s ^1На вашем счету ^3недостаточно средств ^1для ставки.", g_szBetPrefix);
		return Show_BetMenu(id);
	}
	else if (iMoney < 2)
	{
		client_print_color(id, print_team_red, "%s ^3Минимальная ^1сумма для ставки^4 2$!y.", g_szBetPrefix);
		return Show_BetMenu(id);
	}
	else
	{
		g_iUserBetCost[id] = iMoney;
		client_print_color(id, 0, "%s ^1Теперь выберите на кого поставить ^4%d$^1.", g_szBetPrefix, iMoney);
		return Show_BetMenu(id);
	}
}

jbe_duel_bets_start()
{
	g_iBitUserBet = 0;
	g_iBitUserBetPlayer[0] = 0;
	g_iBitUserBetPlayer[1] = 0;
	
	for (new i = 1; i <= MaxClients; i++)
	{
		if (g_iDuelUsersId[0] == i || g_iDuelUsersId[1] == i
		|| g_iUserTeam[i] == TEAM_SPECTATOR || g_iUserTeam[i] == TEAM_UNASSIGNED)
			continue;

		g_iUserBetCost[i] = 0;
		Show_BetMenu(i);
	}
}

jbe_duel_bets_check(iWin)
{
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserBet, i) || IsNotSetBit(g_iBitUserConnected, i)) 
			continue;

		if (IsSetBit(g_iBitUserBetPlayer[iWin], i))
		{
			jbe_set_user_money(i, g_iUserMoney[i] + (g_iUserBetCost[i] + g_iUserBetCost[i] / 2), 0);
			client_print_color(i, i, "%s Вы получили ^4%d^3$ ^1за ставку в дуэли.", g_szBetPrefix, g_iUserBetCost[i] / 2);
		}
		else client_print_color(i, i, "%s Вы потеряли ^4%d^3$ ^1за ставку в дуэли.", g_szBetPrefix, g_iUserBetCost[i]);
	}
}

// возвращаем ставки (дуэлянт вышел или начальник отменил дуэль)
jbe_duel_bets_return()
{
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserBet, i) || IsNotSetBit(g_iBitUserConnected, i)) 
			continue;

		jbe_set_user_money(i, g_iUserMoney[i] + g_iUserBetCost[i], 0);
		client_print_color(i, i, "%s Вам возвращена ваша ставка ^4%d^3$^1.", g_szBetPrefix, g_iUserBetCost[i]);
	}
}

fnGetUserBetMoney_up(id)
{
	new iMoney;
	
	if (g_iUserMoney[id] - g_iUserBetCost[id] <= 6)
		iMoney = g_iUserBetCost[id] + 2;
	
	else if (g_iUserMoney[id] - g_iUserBetCost[id] <= 10)
		iMoney = g_iUserBetCost[id] + 4;
	
	else if (g_iUserMoney[id] - g_iUserBetCost[id] > 10)
		iMoney = g_iUserBetCost[id] + 6;
	
	return iMoney;
}

fnGetUserBetMoney_down(id)
{	
	new iMoney;
	
	if (g_iUserBetCost[id] <= 6)
		iMoney = g_iUserBetCost[id] - 2;
	
	else if (g_iUserBetCost[id] <= 10)
		iMoney = g_iUserBetCost[id] - 4;
	
	else if (g_iUserBetCost[id] > 10)
		iMoney = g_iUserBetCost[id] - 6;
	
	return iMoney > 0 ? iMoney : 0;
}

/*===== -> Ставки в дуэлях -> =====*///}