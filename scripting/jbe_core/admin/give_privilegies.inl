// give privilegies menu

public ClCmd_GivePrivilegies(id) {
	if (!(get_user_flags(id) & ADMIN_RCON))
		return PLUGIN_HANDLED;

	return Cmd_GivePrivilegiesMenu(id);
}

new const g_szGivePrivilegies[][] = {
	"VIP",
	"Hook",
	"Grab",
	"Global",
	"Voice"
};

enum
{
	GIVE_PRIV_VIP,
	GIVE_PRIV_HOOK,
	GIVE_PRIV_GRAB,
	GIVE_PRIV_GLOBAL,
	GIVE_PRIV_VOICE
};

// Admin Team Menu
Cmd_GivePrivilegiesMenu(id)
{ 
	g_iMenuType[id] = 0; 
	return Show_GivePrivilegiesMenu(id, g_iMenuPosition[id] = 0); 
}
Show_GivePrivilegiesMenu(id, iPos)
{
	if (iPos < 0 || IsNotSetBit(g_iBitUserAdmin, id))
		return PLUGIN_HANDLED;

	new iPlayersNum;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserConnected, i)) continue;
		g_iMenuPlayers[id][iPlayersNum++] = i;
	}

	menu_list_title("\wGive Menu", iPos, iPlayersNum, .iPerPage = 7);

	new szItem[24], iBit;
	formatex(szItem, charsmax(szItem), "%s",
		g_szGivePrivilegies[g_iMenuType[id] % sizeof(g_szGivePrivilegies)]);

	menu_list_item("\y1. \wПривилегия: \y%s^n^n\
		\dВыберите игрока:^n", szItem);

	format(szItem, charsmax(szItem), "\r(\y%s\r)", szItem);
	
	switch (g_iMenuType[id])
	{
		case GIVE_PRIV_VIP:		iBit = g_iBitUserVip;
		case GIVE_PRIV_HOOK:	iBit = g_iBitUserHook;
		case GIVE_PRIV_GRAB:	iBit = g_iBitUserGrab;
		case GIVE_PRIV_GLOBAL:	iBit = g_iBitUserGlobalAdmin;
		case GIVE_PRIV_VOICE:	iBit = g_iBitEternalVoice;
	}

	new iPlayer;
	while (menu_list_count())
	{
		iPlayer = g_iMenuPlayers[id][menu_list_countid()];
		menu_list_item("\y%d. \w%n %s^n", menu_list_num(), iPlayer, IsSetBit(iBit, iPlayer) ? szItem : "");
	}

	return menu_list_show(id, "Show_GivePrivilegiesMenu");
}

public Handle_GivePrivilegiesMenu(id, iKey)
{
	if (IsNotSetBit(g_iBitUserAdmin, id)) return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 0:
		{
			g_iMenuType[id] = ((g_iMenuType[id] + 1) % sizeof(g_szGivePrivilegies));
			return Show_GivePrivilegiesMenu(id, g_iMenuPosition[id]);
		}
		case 8: return Show_GivePrivilegiesMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_GivePrivilegiesMenu(id, --g_iMenuPosition[id]);
		
		default:
		{
			new iTarget = g_iMenuPlayers[id][menu_list_itemid(id, iKey) - 1];
			
			if (IsNotSetBit(g_iBitUserConnected, iTarget))
			{
				client_print_color(id, id, "%s Игрок вышел с сервера", g_szChatPrefix);
				return PLUGIN_HANDLED;
			}

			jbe_give_admin_privilegies(id, iTarget, g_iMenuType[id]);

			return Show_GivePrivilegiesMenu(id, g_iMenuPosition[id]);
		}
	}
	return PLUGIN_HANDLED;
}

jbe_give_admin_privilegies(id, iTarget, iType = -1)
{
	new bool:bGive = true;
	switch (iType)
	{
		case GIVE_PRIV_VIP:
		{
			if (IsSetBit(g_iBitUserVip, iTarget))
				bGive = false;
			InvertBit(g_iBitUserVip, iTarget);
			jbe_get_user_prefix_flag(iTarget);
		}
		case GIVE_PRIV_HOOK:
		{
			if (IsSetBit(g_iBitUserHook, iTarget))
			{
				bGive = false;
				ClCmd_HookOff(iTarget);
			}
			InvertBit(g_iBitUserHook, iTarget);
		}
		case GIVE_PRIV_GRAB:
		{
			if (IsSetBit(g_iBitUserGrab, iTarget))
			{
				bGive = false;
				ClCmd_GrabOff(iTarget);
			}
			InvertBit(g_iBitUserGrab, iTarget);

		}
		case GIVE_PRIV_GLOBAL:
		{
			if (IsSetBit(g_iBitUserGlobalAdmin, iTarget))
				bGive = false;
			InvertBit(g_iBitUserGlobalAdmin, iTarget);
		}
		case GIVE_PRIV_VOICE:
		{
			if (IsSetBit(g_iBitEternalVoice, iTarget))
			{
				ClearBit(g_iBitUserVoice, iTarget);
				bGive = false;
			}
			else SetBit(g_iBitUserVoice, iTarget);

			InvertBit(g_iBitEternalVoice, iTarget);
			reload_user_voice(iTarget);
		}
	}

	client_print_color(0, iTarget, "%s %s ^4%n ^1%s ^4%s ^3%n^1.", 
		g_szChatPrefix, g_szAdminPrefix, id, bGive ? "выдал" : "забрал", 
		g_szGivePrivilegies[iType], iTarget);
}