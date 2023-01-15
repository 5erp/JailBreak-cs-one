/********************************
*	Shared Functions Header File
*********************************/

#include "jbe_core\sql\setting.h"
#include "jbe_core\sql\limits.h"

#if defined ENABLE_CACHE
	#include "jbe_core\sql\cache.h"
#endif

/*
* Таблицы сохранения данных jb, кт блок, бан должны быть в одной БД
* Юзается на всех 1 g_sqlTuple
* Будет 1 массив под все запросы - g_szSqlQuery (? возможно)
* Тут в основном под ранг от jbe и немного под остальные sql системы
*/

/*==================================================
 #			-> MySQL ->
====================================================*/

//#define TIME_SQL_CREATE_TABLE	4 // - del
//#define TIME_SQL_CLEAR_TABLE	10 // - del

#define TIME_SQL_LOAD_USER 3 // задержка в сек. перед загрузкой данных при входе на сервер

#define LIMIT_SAVE_MONEY		100		// Лимит сохранения денег у игроков
#define LIMIT_SAVE_MONEY_VIP	150		// Лимит сохранения денег у VIP

new g_iBitUnvalidAuthId;	// не валидный steam id
new g_iSqlLastUpdateTime;	// время крайнего обновления в конце раунда

new Handle:g_sqlTuple;

//#define DEVELOPER_SQL_OFF	// off работа sql

#define ENABLE_IP_PASS_SAVE	// вкл сохранение ip+pass (если офнуть чет править над)

#if defined DEVELOPER_SQL_OFF
	new bool:g_bSqlConnected = false;
#else
	new bool:g_bSqlConnected = true;
#endif

new const g_szPassStr[] = "_pw"; // Строка в setinfo с паролем (setinfo _pw)

new g_szSqlQuery[8192]; // MAX_STRING_LENGTH Общий массив под все sql запросы


new g_szSqlHost[32];
new g_szSqlUser[32];
new g_szSqlPass[32];
new g_szSqlDB[32];
new g_szRankTable[32];

// время выбора команды, для сохраннеия времени игры
new g_iUserGameTime[MAX_PLAYERS + 1]; 

new g_iSqlUserID[MAX_PLAYERS + 1];			// id записи в таблице
new g_iBitUserSqlLoad;		// загружен ли игрок

enum {
	SQL_CHECK,
	SQL_INSERT,
	SQL_RELOAD,
	SQL_INSERT_QUEST, // создание записи под квесты игрока
	SQL_IGNORE
};


/*==================================================
 #			<- MySQL <-
====================================================*/