/*===== -> Сообщения -> =====*///{***

message_init()
{
	register_message(MsgId_TextMsg, "Message_TextMsg");
	register_message(MsgId_ResetHUD, "Message_ResetHUD");
//	register_message(MsgId_ShowMenu, "Message_ShowMenu");
//	register_message(MsgId_VGUIMenu, "Message_VGUIMenu");
	register_message(MsgId_SendAudio, "Message_SendAudio");
	register_message(MsgId_StatusIcon, "Message_StatusIcon");
	register_message(MsgId_Money, "Message_Money");

	set_msg_block(MsgId_ClCorpse, BLOCK_SET);
	set_msg_block(MsgId_HudTextArgs, BLOCK_SET);
	set_msg_block(MsgId_StatusText, BLOCK_SET);
	
	g_tReplaceInfoMsg = TrieCreate();
	TrieSetString(g_tReplaceInfoMsg, "#Game_Commencing",	"Ну что, поехали :D");
	TrieSetString(g_tReplaceInfoMsg, "#CTs_Win",			"Охраники победили!");
	TrieSetString(g_tReplaceInfoMsg, "#Terrorists_Win",		"Заключенные победили!");
	TrieSetString(g_tReplaceInfoMsg, "#Round_Draw",			"Ничья, победила дружба!");	

	RegisterHookChain(RG_ShowVGUIMenu, "ShowVGUIMenu", .post = 0);
}

public ShowVGUIMenu(const id, VGUIMenu:menuType, bitsSlots, const szOldMenu[], bool:bForceOldMenu)
{
	if (menuType == VGUI_Menu_Team)
	{
		if (g_iUserTeam[id] == TEAM_UNASSIGNED)
			set_task(0.1, "auto_change_team", id);
		else
			Show_ChooseTeamMenu(id, 0);
		
		return HC_SUPERCEDE;
    }
	else if (menuType == VGUI_Menu_Class_T || menuType == VGUI_Menu_Class_CT)
		return HC_SUPERCEDE;

	return HC_CONTINUE;
}

public Message_Money(iMsgId, iMsgDest, iReceiver)
{
	if (IsNotSetBit(g_iBitUserConnected, iReceiver)) return;
	set_msg_arg_int(1, ARG_LONG, g_iUserMoney[iReceiver]); // hud игрока
	set_member(iReceiver, m_iAccount, g_iUserMoney[iReceiver]); // для отображения в табе
}

public Message_TextMsg()
{
	if (g_bRestartGame)
		return PLUGIN_HANDLED;
		
	static szArg[32], szMsg[64]; // szMsg[192]
	get_msg_arg_string(2, szArg, charsmax(szArg));

	if (TrieGetString(g_tReplaceInfoMsg, szArg, szMsg, charsmax(szMsg)))
	{
		if (get_msg_args() > 2)
		{
			get_msg_arg_string(3, szArg, charsmax(szArg));
			replace(szMsg, charsmax(szMsg), "%s", szArg);
		}
		set_msg_arg_string(2, szMsg);
		return PLUGIN_CONTINUE;
	}

	if (szArg[0] == '#' && (szArg[1] == 'G' && szArg[2] == 'a' && szArg[3] == 'm'
	&& (equal(szArg[6], "teammate_attack", 15)	// %s attacked a teammate
	|| equal(szArg[6], "teammate_kills", 14)	// Teammate kills: %s of 3
	|| equal(szArg[6], "join_terrorist", 14)	// %s is joining the Terrorist force
	|| equal(szArg[6], "join_ct", 7)			// %s is joining the Counter-Terrorist force
	|| equal(szArg[6], "scoring", 7)			// Scoring will not start until both teams have players
	|| equal(szArg[6], "will_restart_in", 15))	// The game will restart in %s1 %s2
//	|| equal(szArg[6], "Commencing", 10))		// Game Commencing!
	|| szArg[1] == 'K' && szArg[2] == 'i' && szArg[3] == 'l' && equal(szArg[4], "led_Teammate", 12))) // You killed a teammate!
		return PLUGIN_HANDLED;

	if (get_msg_args() != 5) return PLUGIN_CONTINUE;

	get_msg_arg_string(5, szArg, charsmax(szArg));
	if (szArg[1] == 'F' && szArg[2] == 'i' && szArg[3] == 'r' && equal(szArg[4], "e_in_the_hole", 13)) // Fire in the hole!
		return PLUGIN_HANDLED;

	return PLUGIN_CONTINUE;
}

public Message_ResetHUD(iMsgId, iMsgDest, iReceiver)
{
	if (g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY)
		return;

	// Проверка нужна, бывает отправка ResetHUD на игрока который отключился
	if (IsNotSetBit(g_iBitUserConnected, iReceiver))
		return;
	
	set_member(iReceiver, m_iClientHideHUD, 0);
	set_member(iReceiver, m_iHideHUD, HIDEHUD_TIMER);
}

public Message_ShowMenu(iMsgId, iMsgDest, iReceiver)
{
	switch (get_msg_arg_int(1))
	{
		case ShowMenu_TeamMenu, ShowMenu_TeamSpectMenu:
		{
			if (g_iUserTeam[iReceiver] == TEAM_UNASSIGNED)
				set_task(0.1, "auto_change_team", iReceiver);
			else Show_ChooseTeamMenu(iReceiver, 0);
			return PLUGIN_HANDLED;
		}
		case ShowMenu_ClassMenu, ShowMenu_IgTeamMenu, ShowMenu_IgTeamSpectMenu: return PLUGIN_HANDLED;
	}
	return PLUGIN_CONTINUE;
}

public Message_VGUIMenu(iMsgId, iMsgDest, iReceiver)
{
	switch (get_msg_arg_int(1))
	{
		case VGUIMenu_TeamMenu:
		{
			if (g_iUserTeam[iReceiver] == TEAM_UNASSIGNED)
				set_task(0.1, "auto_change_team", iReceiver);
			else Show_ChooseTeamMenu(iReceiver, 0);
			return PLUGIN_HANDLED;
		}
		case VGUIMenu_ClassMenuTe, VGUIMenu_ClassMenuCt: return PLUGIN_HANDLED;
	}
	return PLUGIN_CONTINUE;
}

public auto_change_team(id) 
{
	#if defined DEVELOPER_OPTIONS
//	if (is_user_bot(id)) return PLUGIN_HANDLED;
	#endif

	if (!jbe_set_user_team(id, TEAM_PRISONER))
		return Show_ChooseTeamMenu(id, 0);

	jbe_informer_offset_down(id);
	return PLUGIN_HANDLED;
}

public Message_SendAudio()
{
	if (g_bRestartGame)
		return PLUGIN_HANDLED;
		
	static szArg[32];
	get_msg_arg_string(2, szArg, charsmax(szArg));
	if (szArg[0] == '%' && (szArg[2] == 'M' && szArg[3] == 'R' 
	&& szArg[4] == 'A' && szArg[5] == 'D' && equal(szArg[7], "FIREINHOLE", 10))) // !MRAD_FIREINHOLE
		return PLUGIN_HANDLED;
	return PLUGIN_CONTINUE;
}

public Message_StatusIcon()
{
	if (get_msg_args() != 5) return;
	static szSpriteName[10];
	get_msg_arg_string(2, szSpriteName, charsmax(szSpriteName));
	if (equal(szSpriteName, "buyzone")) // скрываем иконку байзоны
	{
		set_msg_arg_int(3, ARG_BYTE, 0);
		set_msg_arg_int(4, ARG_BYTE, 0);
		set_msg_arg_int(5, ARG_BYTE, 0);
	}
}

/*===== <- Сообщения <- =====*///}