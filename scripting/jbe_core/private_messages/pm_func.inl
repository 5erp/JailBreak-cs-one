#include "jbe_core\private_messages\pm_main_menu.inl"
#include "jbe_core\private_messages\pm_select_menu.inl"
#include "jbe_core\private_messages\pm_block_menu.inl"

private_messages_init()
{
	// main menu
	register_menucmd(register_menuid("Show_PrivateMessageMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_PrivateMessageMenu");

	// select user pm menu
	register_menucmd(register_menuid("Show_PM_SelectMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_PM_SelectMenu");

	// block user pm menu
	register_menucmd(register_menuid("Show_PM_BlockMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_PM_BlockMenu");
	
	// clcmd
	register_clcmd("pm", "ClCmd_Messagemode_PM"); // messagemode pm
	register_clcmdex("pm", "ClCmd_PM"); // приватные сообщения say /pm
}

public ClCmd_Messagemode_PM(id)
{
	if (check_not_pm_access(id))
		return Show_PM_SelectMenu(id, g_iMenuPosition[id]);

	if (is_user_cmd_flood(id, FLOOD_CHAT, 0.75, 3.75))
		return PLUGIN_HANDLED;
		
	static sArg[180];
	read_argv(1, sArg, charsmax(sArg));

	if (sArg[0] == EOS)
	{
		client_print_color(id, id, "%s Введите сообщение.", g_szChatPrefix);
		return Show_PM_SelectMenu(id, g_iMenuPosition[id]);
	}

	if (IsAdvertsString(sArg))
	{
		client_print_color(id, id, "%s Реклама запрещена!", g_szChatPrefix);
		return PLUGIN_HANDLED;
	}
	
	new iTarget = g_PM_CurrentTarget[id];

	client_print_color(iTarget, id, "^1[PM] ^3%n ^1> : %s", id, sArg);
	client_print_color(id, iTarget, "^1[PM] > ^3%n ^1: %s", iTarget, sArg);
	client_print(iTarget, print_console, "[PM] %n > : %s", id, sArg);
					
	// звук входящего PM
	client_cmd(iTarget, "spk weapons/mine_activate.wav"); // spk buttons/button9.wav

	set_last_pm(id, iTarget);
				
	return PLUGIN_HANDLED;
}

// say /pm name message
public ClCmd_PM(id) return Show_PrivateMessageMenu(id);

// Сохраняем последних получателей
set_last_pm(id, iTarget)
{
	new i, aLastTargets[LAST_PM_TARGETS_NUM];

	// Проверяем есть ли получатель в списке
	for (i = 0; i < LAST_PM_TARGETS_NUM; i++)
	{
		if (g_PM_LastTargets[id][i] == iTarget)
			return PLUGIN_HANDLED;
	}

	aLastTargets = g_PM_LastTargets[id];
	g_PM_LastTargets[id][0] = iTarget;

	// Заносим нового получателя в начало списка, остальных смещаем на 1 пункт
	for (i = 0; i < LAST_PM_TARGETS_NUM - 1; i++)
	{
		g_PM_LastTargets[id][i + 1] = aLastTargets[i];
	}

	return PLUGIN_HANDLED;
}

// сортируем, отсекая отключившихся.
reset_last_pm(id)
{
	new aLastTargets[LAST_PM_TARGETS_NUM];
	for (new i, j; i < LAST_PM_TARGETS_NUM; i++)
	{
		if (!g_PM_LastTargets[id][i]
		|| IsNotSetBit(g_iBitUserConnected, g_PM_LastTargets[id][i]))
			continue;

		aLastTargets[j++] = g_PM_LastTargets[id][i];
	}
	g_PM_LastTargets[id] = aLastTargets;
}

bool:check_not_pm_access(id)
{
	if (IsNotSetBit(g_iBitUserConnected, g_PM_CurrentTarget[id]))
		client_print_color(id, 0, "%s Игрок вышел с сервера", g_szChatPrefix);

	else if (IsSetBit(g_iBitBlockedPM[g_PM_CurrentTarget[id]], id))
		client_print_color(id, 0, "%s Игрок заблокировал вас.", g_szChatPrefix);

	else if (IsSetBit(g_iBitUserOffPM, g_PM_CurrentTarget[id]))
		client_print_color(id, 0, "%s Игрок отключил входящие.", g_szChatPrefix);

	else if (g_iGlobalBlock[GB_PM])
		client_print_color(id, 0, "%s ^1Приватные сообщения ^3заблокированы %s^1.",
			g_szChatPrefix, global_get_name_block(GB_PM));

	else if (g_iGlobalBlock[GB_CHAT_DEAD] && IsNotSetBit(g_iBitUserAlive, id)
		&& IsSetBit(g_iBitUserAlive, g_PM_CurrentTarget[id]))
	{
		
		client_print_color(id, 0, "%s Чат мертвых заблокирован %s^1.",
			g_szChatPrefix, global_get_name_block(GB_CHAT_DEAD));
		
		client_print_color(id, 0, "%s Вы не можете написать живому игроку.", 
			g_szChatPrefix);
	}

	else return false;

	return true;
}

stock pm_target(id, const szArg[])
{
	new iPlayer = find_player("bl", szArg);
	if (iPlayer)
	{
		if (iPlayer != find_player("blj", szArg))
		{
			client_print_color(id, 0, "[PM] Много игроков подпадает под ваш аргумент.");
			return 0;
		}
	}
	else if ((iPlayer = find_player("c", szArg)) == 0 && szArg[0] == '*' && szArg[1])
	{
		iPlayer = find_player("k", str_to_num(szArg[1]));
	}
	if (!iPlayer)
	{
		client_print_color(id, 0, "[PM] Игрок с этим именем или идентификатором пользователя не найден.");
		return 0;
	}
	if (iPlayer == id)
	{
		client_print_color(id, 0, "[PM] Нельзя писать самому себе.");
		return 0;
	}
	return iPlayer;
}