/*==================================================
 #			-> Квесты ->
====================================================*/

#include "jbe_core\quests\quests_menu.inl"
#include "jbe_core\quests\quest_info_menu.inl"
#include "jbe_core\quests\quests_user_menu.inl"

quests_init() {
	// Список квестов quests_menu.inl
	register_menucmd(register_menuid("Show_QuestsMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_QuestMenu");
	
	// Инфо о квесте quest_info_menu.inl
	register_menucmd(register_menuid("Show_QuestInfoMenu"),
		(1<<0|1<<8|1<<9), "Handle_QuestInfoMenu");
	
	// Квесты игрока quests_user_menu.inl
	register_menucmd(register_menuid("Show_QuestsUserMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<7|1<<8|1<<9), "Handle_QuestsUserMenu");

	// clcmd
	register_clcmdex("quest", "ClCmd_QuestMenu"); // say /quest
}


public ClCmd_QuestMenu(id)
{
	if (quest_user_all_num(id))
		return Show_QuestsUserMenu(id);
	
	return Cmd_QuestMenu(id);	
}

const QUEST_ADDTIME_INFO = 60 * 10;	// как часто писать о добавленом времени к квест

check_user_quest(id, iQuest, iNum = 1, bool:bMessage = true, bool:bGameDay = false)
{
	if (g_iUserQuest[id][ID][iQuest] == -1) // !jbe_is_user_valid(id) ||
		return;

	if (g_iDayMode == DAY_RESTART || g_iDayMode == DAY_DEATHMATCH && bGameDay == false)
		return;

	new iSysTime = get_systime();
	
	if (quest_user_end(id, iQuest) < iSysTime)
	{
		client_print_color(id, 0, "%s ^1Время на выполнение задания ^4%s ^1истекло.", g_szQuestPrefix, quest_name_global(iQuest));
		
		user_quest_reset(id, iQuest);
	}

	// Чек на кол-во игроков для квеста / (?) put in server	g_iUserQuestNoTime[id] = iSysTime;
	else if (g_iPlayersNum[TEAM_PRISONER] + g_iPlayersNum[TEAM_GUARD] < quest_players_global(iQuest))
	{
		// добавляем время к выполнению
		if (quest_user_no_time(id, iQuest))
		{
			quest_user_end(id, iQuest) += (iSysTime - quest_user_no_time(id, iQuest));

			if (bMessage)
			{
				if (iSysTime - quest_user_no_time(id, iQuest) > QUEST_ADDTIME_INFO)
				{ // Если добавили времени
					client_print_color(id, print_team_blue, "%s Добавленно ^3+^4%s ^1на выполнение задания ^4%s^1.", g_szQuestPrefix,
						seconds_to_str(iSysTime - quest_user_no_time(id, iQuest)), quest_name_global(iQuest));
				}
			}
		}
		quest_user_no_time(id, iQuest) = iSysTime;

		return;
	}

	else if ((quest_user_num(id, iQuest) += iNum) >= quest_num_global(iQuest))
	{

		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsNotSetBit(g_iBitUserConnected, i) || i == id)
				continue;

			client_print_color(i, id, "%s ^3%n ^1Выполнил задание ^4%s ^1и получил ^4%d ^1опыта.",
				g_szQuestPrefix, id, quest_name_global(iQuest), quest_exp_global(iQuest));
		}

		client_print_color(id, id, "%s ^1Выполнено задание ^4%s^1, Награда ^4+%d ^1опыта.",
			g_szQuestPrefix, quest_name_global(iQuest), quest_exp_global(iQuest));
		
		jbe_set_user_exp(id, g_iUserExp[id] + quest_exp_global(iQuest));
		
		user_quest_reset(id, iQuest);
		
		client_cmd(id, "spk common/launch_select2.wav");
		
		log_amx("%n выполнил квест %s", id, quest_name_global(iQuest));
	}

	else if (bMessage)
	{
		set_hudmessage(127, 127, 127, 0.7, 0.8, 0, 0.0, 2.8, 0.2, 0.2, -1);
		ShowSyncHudMsg(id, g_iSyncQuestInfo,
			"Задание: %s^n\
			Выполнено: %d|%d",
		quest_name_global(iQuest),
		quest_user_num(id, iQuest), quest_num_global(iQuest));
	}
}

// если отказался от квеста, отнимает exp / 2
quest_del_exp(iExp)
{
	new Float:fExp = iExp / 2.0;
	new iExp = floatround(fExp, floatround_floor); // floatround_floor - округляет в меньшую сторону
	if (iExp < 1) iExp = 1;
	return iExp;
}

// выдача квеста
user_quest_set(id, iQuest, iNum = 0, iEnd = 0)
{
	if (IsNotSetBit(g_iBitUserSqlLoad, id) && g_bSqlConnected)
	{
		client_print_color(id, id, "%s Дождитесь загрузки данных.", g_szQuestPrefix);
		return -1;
	}

	if (!iQuest || iEnd > 0 && iEnd < get_systime())
		return -1;

	if (g_iUserQuest[id][ALL_NUM] >= MAX_EXECUTE_QUEST)
		return -1;

	for (new i; i < MAX_EXECUTE_QUEST; i++)
	{
		if (!g_iUserQuest[id][EXEC][i])
		{
			quest_user_head(id, iQuest) = i; // указатель ячейки 0,1,2..
			quest_user_num(id, iQuest) = iNum; //0; // сколько выполнил
			quest_user_no_time(id, iQuest) = 0;
			g_iUserQuest[id][EXEC][i] = iQuest; // ид квеста в наших ячейках
			g_iUserQuest[id][ALL_NUM]++;

			if (iEnd)
				quest_user_end(id, iQuest) = iEnd;
			else
				quest_user_end(id, iQuest) = get_systime() + (quest_time_global(iQuest) * 60 * 60);
		
			return i;
		}
	}

	return -1;
}

// обнуление квестов (при выполении, отмене, входе)
user_quest_reset(id, iQuest = 0)
{
	if (iQuest)
	{
		new iColID = g_iUserQuest[id][ID][iQuest];

		g_iUserQuest[id][NUM][iColID] = 0;
		g_iUserQuest[id][END][iColID] = 0;
		g_iUserQuest[id][EXEC][iColID] = 0;
		g_iUserQuest[id][NO_TIME][iColID] = 0;
		g_iUserQuest[id][ID][iQuest] = -1;
		g_iUserQuest[id][ALL_NUM]--;
	}
	else
	{
		arrayset(g_iUserQuest[id][ID], -1, DATA_QUESTS); // всему массиву игрока значение '-1'

		for (new i; i < MAX_EXECUTE_QUEST; i++)
		{
			g_iUserQuest[id][NUM][i] = 0;
			g_iUserQuest[id][END][i] = 0;
			g_iUserQuest[id][EXEC][i] = 0;
			g_iUserQuest[id][NO_TIME][i] = 0;
			g_iUserQuest[id][ALL_NUM] = 0;
		}
	}
}


/*==================================================
 #			<- Квесты <-
====================================================*/

