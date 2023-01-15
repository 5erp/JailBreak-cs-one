// Меню - описание квеста

Show_QuestInfoMenu(id, iQuestHead)
{
	//TODO: потом чет сделать, это для перехода с меню квестов игрока
	g_iMenuTarget[id] = iQuestHead;

	new iKeys = (1<<8|1<<9);
	
	menu_title(" \y# \wОписание:^n^n");
	
	menu_item("\wНазвание: \y%s^n", quest_name_global(iQuestHead));
	
	menu_item("\wЗадача: \y%s^n", quest_description(iQuestHead));
	
	if (is_user_quest(id, iQuestHead))
		menu_item("\wВыполнено: \r%d/%d^n", quest_user_num(id, iQuestHead), quest_num_global(iQuestHead));

	menu_item("\wНаграда% \r%d \wопыта^n", quest_exp_global(iQuestHead));
	
	if (is_user_quest(id, iQuestHead))
		menu_item("\wОсталось на выполнение: \r%s^n",
			minutes_to_str((quest_user_end(id, iQuestHead) - get_systime()) / 60));
	else
		menu_item("\wВремя на выполнение: \r%s^n",
			minutes_to_str(quest_time_global(iQuestHead) * 60));

	menu_item("\wМин. игроков для квеста \r%d^n", quest_players_global(iQuestHead));
	
	menu_item("^n^n");
	
	if (quest_user_head(id, iQuestHead) == -1)
	{
		menu_item("\y1. \wВзять задание^n");
		iKeys |= (1<<0);
	}

	else if (is_user_quest(id, iQuestHead)) {
		menu_item("\y1. \yОтказаться от квеста \r( -%d опыта )^n",
			quest_del_exp(quest_exp_global(iQuestHead)));
		iKeys |= (1<<0);
	}
	else
		menu_item("\y1. \dВзять задание^n");
	
	menu_item("^n^n\y9. \w%L", id, "JBE_MENU_BACK");
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	return menu_show(id, iKeys, "Show_QuestInfoMenu");
}

public Handle_QuestInfoMenu(id, iKey)
{
	jbe_informer_offset_down(id);
	switch (iKey)
	{
		case 0:
		{
			new iHead = g_iMenuTarget[id];

			// Отказаться от квеста
			if (quest_user_head(id, iHead) > -1)
			{
				if (IsSetBit(g_iBitUserDuel, id))
				{
					client_print_color(id, id, "%s Дождитесь окончания дуэли для отказа от задания.", g_szChatPrefix);
					return PLUGIN_HANDLED;
				}

				new iExp = quest_del_exp(quest_exp_global(iHead));

				if (g_iUserExp[id] - iExp > 0)
				{
					jbe_set_user_exp(id, g_iUserExp[id] - iExp, .bMessage = false);
					client_print_color(id, print_team_red,
						"%s ^1Вы отказались от квеста ^4%s^1, наказние ^3-%d ^1опыта.",
							g_szQuestPrefix, quest_name_global(iHead), iExp);
				}
				else
				{
					client_print_color(id, id, "%s ^1Вы отказались от квеста ^4%s^1.",
						g_szQuestPrefix, quest_name_global(iHead));
				}

				user_quest_reset(id, iHead);

				return Show_QuestsMenu(id, g_iMenuPosition[id] > 0 ? g_iMenuPosition[id] : 0);
			}

			// лимит квестов
			else if (quest_user_all_num(id) >= ALL_QUESTS)
			{
				client_print_color(id, print_team_blue, "%s ^1Лимит взятых заданий ^3%d.", g_szQuestPrefix, ALL_QUESTS);

				return Show_QuestsMenu(id, g_iMenuPosition[id] > 0 ? g_iMenuPosition[id] : 0);
			}

			// взять квест
			if (user_quest_set(id, iHead) > -1)
			{
				client_print_color(id, print_team_blue, "%s ^1Вы взяли задание: ^4%s^1, время на выполнение: ^3%s^1.",
					g_szQuestPrefix, quest_name_global(iHead), minutes_to_str(quest_time_global(iHead) * 60));
				
				log_amx("%n взял квест %s", id, quest_name_global(iHead));

				if (quest_user_all_num(id) < ALL_QUESTS)
					return Show_QuestsMenu(id, g_iMenuPosition[id] > 0 ? g_iMenuPosition[id] : 0);

			}
		}
		case 8: return Show_QuestsMenu(id, g_iMenuPosition[id] > 0 ? g_iMenuPosition[id] : 0);
	}
	return PLUGIN_HANDLED;
}