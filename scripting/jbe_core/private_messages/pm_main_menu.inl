/*
	Main Private Messages Menu
*/

Show_PrivateMessageMenu(id)
{
	menu_title("Личные сообщения^n^n");
	menu_item("\y1. \wНаписать сообщение^n");
	menu_item("\y2. \wЗаблокировать игрока^n");
	menu_item("\y3. \wВходящие \y[%s]^n",
		IsSetBit(g_iBitUserOffPM, id) ? "Выкл" : "Вкл");
	menu_item("\dПоследние получатели:^n");

	new iKeys = (1<<0|1<<1|1<<2|1<<9);
	new iTarget, b = 3;
	reset_last_pm(id);
	for (new i; i < LAST_PM_TARGETS_NUM; i++)
	{
		iTarget = g_PM_LastTargets[id][i];
		if (IsNotSetBit(g_iBitUserConnected, iTarget))
			continue;
		iKeys |= (1<<b);
		menu_item("\y%d. \w%n^n", ++b, iTarget);
	}

	for (new i = b; i < PLAYERS_PER_PAGE; i++)
		menu_item("^n");

	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");

	return menu_show(id, iKeys, "Show_PrivateMessageMenu");
}

public Handle_PrivateMessageMenu(id, iKey)
{
	switch (iKey)
	{
		case 0: return Cmd_PM_SelectMenu(id);
		case 1: return Cmd_PM_BlockMenu(id);
		case 2:
		{
			InvertBit(g_iBitUserOffPM, id);
			client_print_color(id, 0, "^1[PM] Вы ^4%s ^1приватные сообщения.",
				IsSetBit(g_iBitUserOffPM, id) ? "выключили" : "включили");
		}
		case 9: return PLUGIN_HANDLED;
		default:
		{
			g_PM_CurrentTarget[id] = g_PM_LastTargets[id][iKey - 3];

			if (IsNotSetBit(g_iBitUserConnected, g_PM_CurrentTarget[id]))
				client_print_color(id, 0, "[PM] Игрок вышел с сервера..");
			else
				client_cmd(id, "messagemode pm");
		}
	}
	return Show_PrivateMessageMenu(id);
}
