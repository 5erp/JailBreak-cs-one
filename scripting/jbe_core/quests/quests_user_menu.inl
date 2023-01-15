// Меню - список взятых квестов игрока

Show_QuestsUserMenu(id)
{
	if (!quest_user_all_num(id))
		return Cmd_QuestMenu(id);
		
	new iKeys = (1<<8|1<<9);

	menu_title("Меню заданий^n\
		\rУровень: %d | Опыт: %d/%d^n\
		\dТекущие задания:^n",
		g_iUserLevel[id], g_iUserExp[id],
		g_iszRankExpName[g_iUserLevel[id] + 1][RANK_EXP]);

	new iHead;
	for (new i; i < MAX_EXECUTE_QUEST; i++)
	{	
		if (g_iUserQuest[id][EXEC][i])
		{
			iHead = g_iUserQuest[id][EXEC][i];

			menu_item("\y%d. \w%s \r[\y%d\d|\y%d\r]^n", i+1,
				quest_name_global(iHead), quest_user_num(id, iHead), quest_num_global(iHead));

			iKeys |= (1<<i);
		}
		else
		{
			menu_item("\d[%d] Слот пуст (взять квест)^n", i+1);
			iKeys |= (1<<i);
		}
	}

	menu_item("^n^n^n\y9. \wВсе задания^n^n");
	menu_item("\y0. \wВыход");

	return menu_show(id, iKeys, "Show_QuestsUserMenu");
}

public Handle_QuestsUserMenu(id, iKey)
{
	switch (iKey)
	{
		case 8: return Cmd_QuestMenu(id);
		case 9: return PLUGIN_HANDLED;
	}

	if (g_iUserQuest[id][EXEC][iKey])
	{
		new iHead = g_iUserQuest[id][EXEC][iKey];

		return Show_QuestInfoMenu(id, iHead);
	}
	else
		return Cmd_QuestMenu(id);
}