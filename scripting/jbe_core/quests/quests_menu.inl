// Меню - Список всех квестов

Cmd_QuestMenu(id) return Show_QuestsMenu(id, g_iMenuPosition[id] = 0);
Show_QuestsMenu(id, iPos)
{
	if (iPos < 0) return PLUGIN_HANDLED;
	
	new iQuestNum = (sizeof(g_iszQuests) - 1);
	new iStart = iPos * PLAYERS_PER_PAGE;
	
	if (iStart > iQuestNum) iStart = iQuestNum;
	iStart = iStart - (iStart % 8);
	
	g_iMenuPosition[id] = iStart / PLAYERS_PER_PAGE;
	
	new iEnd = iStart + PLAYERS_PER_PAGE;
	if (iEnd > iQuestNum) iEnd = iQuestNum;
		
	new iPagesNum = (iQuestNum / PLAYERS_PER_PAGE + ((iQuestNum % PLAYERS_PER_PAGE) ? 1 : 0));

	menu_title("Выбор заданий \r[\y%d\d|\y%d\r]^n\
				\rУровень: %d | Опыт: %d/%d^n",
		iPos + 1, iPagesNum, g_iUserLevel[id],
		g_iUserExp[id], g_iszRankExpName[g_iUserLevel[id] + 1][RANK_EXP]);
	
	new iCurrentQuests;	// id относительно масива
	new szColor[3];		// цвет в меню
	new iKeys = (1<<9), b;

	for (new a = iStart + 1; a < iEnd + 1; a++)
	{
		iKeys |= (1<<b);
		iCurrentQuests = a;

		if (is_user_quest(id, iCurrentQuests))
			szColor = "\r";
		
		// доступный - белый
		else
			szColor = "\w";
		
		menu_item("\y%d. %s%s^n", ++b, szColor, quest_name_global(iCurrentQuests));
	}

	for (new i = b; i < PLAYERS_PER_PAGE; i++)
		menu_item("^n");
	
	if (iEnd < iQuestNum) {
		iKeys |= (1<<8);
		menu_item("^n\y9. \w%L^n\y0. \w%L",
			id, "JBE_MENU_NEXT", id, iPos ? "JBE_MENU_BACK" : "JBE_MENU_EXIT");
	} else
		menu_item("^n^n\y0. \w%L",
			id, iPos ? "JBE_MENU_BACK" : "JBE_MENU_EXIT");
	
	return menu_show(id, iKeys, "Show_QuestsMenu");
}

public Handle_QuestMenu(id, iKey)
{
	switch(iKey)
	{
		case 8: return Show_QuestsMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_QuestsMenu(id, --g_iMenuPosition[id]);
		default:
		{
			g_iMenuTarget[id] = g_iMenuPosition[id] * PLAYERS_PER_PAGE + iKey + 1;
			new iQuestHeadPos = 0;
			new iCurrentQuests;
			
			if (iQuestHeadPos)
				iCurrentQuests = g_iMenuTarget[id] - (iQuestHeadPos * (sizeof(g_iszQuests) - 1));
			else
				iCurrentQuests = g_iMenuTarget[id];
			
			g_iMenuTarget[id] = iCurrentQuests;
			
			return Show_QuestInfoMenu(id, iCurrentQuests);
		}
	}
	return Show_QuestsMenu(id, g_iMenuPosition[id]);
}