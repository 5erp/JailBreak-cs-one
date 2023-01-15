#include "jbe_core\admin\admin_menu.inl"
#include "jbe_core\admin\teleport_menu.inl"
#include "jbe_core\admin\give_privilegies.inl"

admin_init()
{
	register_menucmd(register_menuid("Show_AdminMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_AdminMenu");
	
	register_menucmd(register_menuid("Show_AdminTeamMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_AdminTeamMenu");
		
	register_menucmd(register_menuid("Show_AdminSlapMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_AdminSlapMenu");
		
	register_menucmd(register_menuid("Show_AdminKickMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_AdminKickMenu");
		
	register_menucmd(register_menuid("Show_GivePrivilegiesMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_GivePrivilegiesMenu");
	
	register_menucmd(register_menuid("Show_AdminTeleportMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_AdminTeleportMenu");

	register_menucmd(register_menuid("Show_AdminModMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_AdminModMenu");

	register_menucmd(register_menuid("Show_PositionTeleportMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<8|1<<9), "Handle_PositionTeleportMenu");

	register_clcmdex("adminmenu", "ClCmd_AdminMenu", 1); // say /adminmenu
	register_clcmdex("amenu", "ClCmd_AdminMenu", 1); // say /amenu
	register_clcmdex("fam", "ClCmd_Fam", 1); // say /fam
	register_clcmdex("modmenu", "ClCmd_ModMenu", 1); // 
	register_clcmd("amxmodmenu", "ClCmd_AdminMenu"); 
	register_clcmd("givepriv", "ClCmd_GivePrivilegies");

	register_clcmdex("tp", "ClCmd_AdminTeleportMenu");

	register_clcmd("say_hud", "ClCmd_AdminSayHud"); // messagemode
	register_clcmd("say_dhud", "ClCmd_AdminSayDHud"); // messagemode
}

public ClCmd_AdminTeleportMenu(id)
{
	if (IsNotSetBit(g_iBitUserAdmin, id))
		return ClCmd_InfoBuy(id);

	return Show_AdminTeleportMenu(id);
}

public ClCmd_AdminMenu(id)
{
	if (IsNotSetBit(g_iBitUserAdmin, id))
		return ClCmd_InfoBuy(id);

	return Show_AdminMenu(id);
}

public ClCmd_Fam(id)
{
	if (IsNotSetBit(g_iBitUserAdmin, id))
		return ClCmd_InfoBuy(id);

	return Show_AdminPlayerMenu(id);
}

public ClCmd_ModMenu(id)
{
	if (IsNotSetBit(g_iBitUserAdmin, id))
		return ClCmd_InfoBuy(id);
		
	return Show_AdminModMenu(id);
}

admin_teleport_player(id, iTarget, Float:fOrigin[3], Float:fAngle[3] = {0.0, 0.0, 0.0})
{
	set_entvar(iTarget, var_origin, fOrigin);

	if (!xs_vec_equal(fAngle, Float:{0.0, 0.0, 0.0}))
	{
		set_entvar(iTarget, var_angles, fAngle);
		set_entvar(iTarget, var_fixangle, 1);
	}

	unstuck_user(iTarget);
	ClearBit(g_iBitUserBuried, iTarget);

	jbe_info_center_msg(0, "%n телепортировал %n", id, iTarget);
	client_print(0, print_console, " ** %n телепортировал %n", id, iTarget);
}

public ClCmd_AdminSayHud(id) return _cmd_admin_say_hud(id, false);
public ClCmd_AdminSayDHud(id) return _cmd_admin_say_hud(id, true);

_cmd_admin_say_hud(id, bool:bDhud)
{
	if (IsNotSetBit(g_iBitUserAdmin, id))
		return PLUGIN_HANDLED;

	if (is_user_cmd_flood(id, FLOOD_ADMIN_HUD, 0.75, 3.75))
		return PLUGIN_HANDLED;
		
	new sMessage[MAX_CHAT_LENGTH];
	read_argv(1, sMessage, charsmax(sMessage));

	if (sMessage[0] == EOS)
		return PLUGIN_HANDLED;

	switch (bDhud) {
		case false: admin_say_hud(id, sMessage, charsmax(sMessage), 1);
		case true: admin_say_hud(id, sMessage, charsmax(sMessage), 2);
	}

	return PLUGIN_HANDLED;
}

admin_say_hud(id, sMessage[], iLen, iType = 0)
{
	new Float:iAdminHudPos[] = { -1.0, 0.2 };

	new nLen;
	new iColor;

	if (!iType)
	{
		while (sMessage[iType] == '@') iType++;
		if (iType > 2) return PLUGIN_CONTINUE;

		nLen = iType;
	}

	switch (sMessage[nLen])
	{
		case 'r': iColor = 1;
		case 'g': iColor = 2;
		case 'b': iColor = 3;
		case 'y': iColor = 4;
		case 'm': iColor = 5;
		case 'c': iColor = 6;
		case 'o': iColor = 7;
	}
	if (iColor) nLen++;

	if (++g_iAdminHudChannel > 6 || g_iAdminHudChannel < 3)
		g_iAdminHudChannel = 3;

	new Float:fPosition = iAdminHudPos[1] + float(g_iAdminHudChannel) / 35.0;

	if (IsAdvertsString(sMessage))
		return client_print_color(id, id, "%s Реклама запрещена!", g_szChatPrefix);

	if (IsSetBit(g_iBitUserRusChat, id))
	{
		new szTranslited[MAX_CHAT_LENGTH];
		translite_string(szTranslited, charsmax(szTranslited), sMessage);
		copy(sMessage, iLen, szTranslited);
	}

	switch (iType)
	{
		case 1:
		{
			set_hudmessage(
				g_iAdminHudColors[iColor][0], 
				g_iAdminHudColors[iColor][1], 
				g_iAdminHudColors[iColor][2],
				iAdminHudPos[0],
				fPosition,
				0, 6.0, 6.0, 0.5, 0.15, -1
			);
	
			show_hudmessage(0, "%n :  %s", id, sMessage[nLen]);
		}
		case 2:
		{
			set_dhudmessage(
				.red = g_iAdminHudColors[iColor][0], 
				.green = g_iAdminHudColors[iColor][1], 
				.blue = g_iAdminHudColors[iColor][2],
				.x = iAdminHudPos[0],
				.y = fPosition,
				.effects = 0, 
				.fxtime = 6.0, 
				.holdtime = 6.0, 
				.fadeintime = 0.5, 
				.fadeouttime = 0.15
			);

			show_dhudmessage(0, "%n :  %s", id, sMessage[nLen]);
		}
	}
	
	client_print(0, print_notify, "%n :  %s", id, sMessage[nLen]);

	return true;
}
