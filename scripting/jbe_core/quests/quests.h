/********************************
*	Shared Functions Header File
*********************************/

/*==================================================
 #			-> Квесты ->
====================================================*/

new g_szQuestsTable[32] = "jbe_quests"; // таблица квестов

// id квестов (хз как еще сделать чтоб были простые проверки)
enum _:DATA_QUESTS // 16 квестов
{
	QUEST_ID_NONE,			// 0 - Нет квеста
	
	// Легкие
	QUEST_ID_LIFER,			// Живучий
	QUEST_ID_LAST_PN,		// Последний герой
	QUEST_ID_CHIEF_GAME,	// Командир
	QUEST_ID_GR_KILL_PN,	// Строгий охранник
	QUEST_ID_DUELS,			// Дуэлянт
	QUEST_ID_KILL_WANTED,	// Устранитель бунта
	QUEST_ID_GOOD_GAMER,	// Хороший игрок - Выжить в иговые дни (Пятница, суббота)
	QUEST_ID_TIMER,			// Усидчивый

	// Сложные
	QUEST_ID_KILLER_CHIEF,	// Убийца начальника
	QUEST_ID_PN_KILL_PN,	// Убийца товарищей
	QUEST_ID_BOXING,		// Боксер
	QUEST_ID_PN_KILL_GR,	// Любитель охраны
	QUEST_ID_ROBS,			// Работяга
	QUEST_ID_EXP,			// Опытный
	QUEST_ID_DAMAGER,		// Садист

	// Прочие
	QUEST_ID_KING_DM		// Король ДМ - Убить 100+ зеков в дм режиме
};

#define ALL_QUESTS		3	// кол-во квестов для всех игроков
const MAX_EXECUTE_QUEST = ALL_QUESTS;// + VIP_QUESTS; // Макс кол-во выполняемых квестов (если увеличивать то редачить таблицу)

enum _:DATA_USER_QUESTS
{
	// указатель на MAX_EXECUTE_QUEST 0, 1, 2 (если 3)
	ID [DATA_QUESTS],
	
	// сколько уже выполнил раз задачу
	NUM [MAX_EXECUTE_QUEST],
	
	// до скольки квест действителен (unixtime) 
	END [MAX_EXECUTE_QUEST],
	
	// время для прибавления к квесту если мало игроков
	NO_TIME [MAX_EXECUTE_QUEST],
	
	// указатель 0, 1, 2 на g_iszQuests | для циклов меню и сохранения
	EXEC [MAX_EXECUTE_QUEST],
	
	// Кол-во выполняемых квестов (при взятии ++) если нет при спавн выводить меню
	ALL_NUM

	// выполнил квестов за карту
//	SUCCESFUL
};

new g_iUserQuest[MAX_PLAYERS + 1][DATA_USER_QUESTS];
new const g_szQuestPrefix[]		= "^1[^4Задания^1]";
new g_iSyncQuestInfo; // Худ выполнения квеста (куда-то в угол или вправо)

enum _:QUEST_DATA_STRUCT
{
	QUEST_NUM,		// Кол-во выполнения
	QUEST_TIME,		// Время на выполнение
	QUEST_EXP,		// Опыт за квест
//	QUEST_MONEY,	// Деньги за квест
	QUEST_PL,		// Мин игроков для того чтоб квест засчитывался
};

enum _:DATA_QUEST_STRUCT
{
	QUEST_NAME,	// имя квеста
	QUEST_DESC,	// описание квеста
	QUEST_DATA,	// данные для квеста
};

new const g_iszQuests[][][] = {
	{	// заглушка QUEST_NONE
		{ "" /* Имя квеста */ }, { "" /* Описание квеста */ },
		{ 0,0,0,0 /* num, time, exp, min pl */ },
	},

	{	// QUEST_ID_LIFER
		{ "Живучий" }, { "Выжить 9 раз до конца раунда" },
		{ 9,	108,	10,	5 },
	},

	{	// QUEST_ID_LAST_PN
		{ "Последний герой" }, { "Остаться последним зеком 8 раз" },
		{ 8,		96,		10,	6 },
	},

	{	// QUEST_ID_CHIEF_GAME
		{ "Командир" }, { "Сыграть за начальника 12 раундов" },
		{ 12,		48,		10,	4 },
	},

	{	// QUEST_ID_GR_KILL_PN
		{ "Строгий охранник" }, { "Убить 26 зеков играя за охрану" },
		{ 26,		96,		10,	6 },
	},

	{	// QUEST_ID_DUELS
		{ "Дуэлянт" }, { "Победить 6 противников в дуэли" },
		{ 6,		144,	8,	5 },
	},

	{	// QUEST_ID_KILL_WANTED
		{ "Устранитель бунта" }, { "Убить 16 бунтующих заключенных" },
		{ 16,		96,		12,	6 },
	},

	{	// QUEST_ID_GOOD_GAMER
		{ "Хороший игрок" }, { "Вижить 14 раз в игровые дни" },
		{ 14,	168,	12,	5 },
	},

	{	// QUEST_ID_TIMER
		{ "Усидчивый" }, { "Отыграть 480 минут на сервере" },
		{ 480,	192,	12,	2 },
	},

	{	// QUEST_ID_KILLER_CHIEF
		{ "Убийца начальника" }, { "Убить 8 раз начальника" },
		{ 8,		39,		8,	7 },
	},
	
	{	// QUEST_ID_PN_KILL_PN
		{ "Убийца товарищей" }, { "Убить 20 зеков будучи зеком" },
		{ 20,		144,	14,	6 },
	},

	{	// QUEST_ID_BOXING
		{ "Боксер" }, { "Победить в 9 боксерсих поединках" },
		{ 9,		216,	9,	5 },
	},
	
	{	// QUEST_ID_PN_KILL_GR
		{ "Любитель охраны" }, { "Убить 14 охранников играя за зеков" },
		{ 14, 	96,		8,	7 },
	},

	{	// QUEST_ID_ROBS	// Засчитывать при выдаче денег за: раунд, бунд
		{ "Работяга" }, { "Заработать 4000 $ за раунды и бунт" },
		{ 4000,	96,		15,	5 },
	},
	
	{	// QUEST_ID_EXP	// При выдаче опыта чекать
		{ "Опытный" }, { "Получить 600 опыта" },
		{ 600,	288,	30,	5 },
	},

	{	// QUEST_ID_DAMAGER // при нанесении урона
		{ "Садист" }, { "Нанести 8000 урона по игрокам" },
		{ 8000,	96,		20,	6 },
	},

	{	// QUEST_ID_KING_DM // при убийстве в дм
		{ "Король DeathMatch" }, { "Убить 100 зеков в DM режиме" },
		{ 100,	288,	7,	2 },
	},
};


// g_iUserQuest [id] [ID] [quest] - храним ид ячейки для остальных массивов
// g_iUserQuest [id] [EXEC] [col] - хранит ид квеста для ячейки

#define quest_offset 1

/* macroses */

#define quest_user_head(%0,%1) g_iUserQuest[%0][ID][%1]//g_iUserQuestID[%0][%1]

// текущий квест (id, iQuest)
#define quest_user_exec(%0,%1) g_iUserQuest[%0][EXEC][g_iUserQuest[%0][ID][%1]]

// кол-во выполнено
#define quest_user_num(%0,%1) g_iUserQuest[%0][NUM][g_iUserQuest[%0][ID][%1]]

// время окончания квеста
#define quest_user_end(%0,%1) g_iUserQuest[%0][END][g_iUserQuest[%0][ID][%1]]

// доп время на квест
#define quest_user_no_time(%0,%1) g_iUserQuest[%0][NO_TIME][g_iUserQuest[%0][ID][%1]]

// взят этот квест
#define is_user_quest(%0,%1) (g_iUserQuest[%0][ID][%1] > -1)

// взятое кол-во квестов
#define quest_user_all_num(%0) (g_iUserQuest[%0][ALL_NUM])

#define quest_name_global(%0)		g_iszQuests[%0][QUEST_NAME][0]
#define quest_description(%0)		g_iszQuests[%0][QUEST_DESC][0]
#define quest_num_global(%0)		g_iszQuests[%0][QUEST_DATA][QUEST_NUM]
#define quest_exp_global(%0)		g_iszQuests[%0][QUEST_DATA][QUEST_EXP]
#define quest_time_global(%0)		g_iszQuests[%0][QUEST_DATA][QUEST_TIME]
#define quest_players_global(%0)	g_iszQuests[%0][QUEST_DATA][QUEST_PL]
