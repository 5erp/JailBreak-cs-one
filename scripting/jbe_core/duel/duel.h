/********************************
*	Shared Functions Header File
*********************************/

new const g_szDuelPrefix[] = "^1[^4Дуэли^1]";

/* -> Битсуммы, переменные и массивы для работы с дуэлями -> */
new g_iDuelStatus,
	g_iDuelType,
	g_iBitUserDuel,
	g_iDuelUsersId[2],
	g_iDuelCountDown,
	g_iDuelTimerAttack,
	g_iDuelCurrentAttacer;

enum // DUEL_STATUS
{
	DS_NOT,		// Нет дуэли
	DS_READY,	// Ожидание (отсчет до старта)
	DS_WAIT,	// Ждем первого выстрела
	DS_RUN		// Идет дуэль
};

// Для активации HamTraceAttack конкретному оружию
#define DUEL_WEAPON_DEAGLE		0
#define DUEL_WEAPON_M3			1
#define DUEL_WEAPON_M249		2
#define DUEL_WEAPON_AWP			3
#define DUEL_WEAPON_AK47		4
#define DUEL_WEAPON_SCOUT		5

enum _:DUEL_TYPES
{
	DT_NONE,
	DT_DEAGLE,
	DT_M3,
	DT_HEGRENADE,
	DT_M249,
	DT_AWP,
	DT_KNIFE,
	DT_AK47,
	DT_SCOUT
}

// на всякий проверяем оружие у атакующего в дуэли (обломаем если выдать ган через глобл меню)
new const g_iDuelAllWeapons[] = {
	0,
	CSW_DEAGLE, //1
	CSW_M3, //2
	CSW_HEGRENADE, //3
	CSW_M249, //4
	CSW_AWP, //5
	CSW_KNIFE, //6
	CSW_AK47, //7
	CSW_SCOUT //8
};

new const g_iDuelLang[][] = {
	"",
	"Пистолетах",
	"Дробовиках",
	"Гранатах",
	"Пулемётах",
	"Снайперках",
	"Ножах",
	"AK-47",
	"Scaut"
};


/* Приз за победу в дуэли (для зека) */
#define PRIZE_MIN_PLAYERS	5

new g_iDuelPrizeType;	// тип награды
new g_iDuelPrizeTarget;	// получатеь награды
new g_iDualPrizeNum;	// для игры на деньги/опыт

enum
{
	DUEL_PRIZE_MONEY,
	DUEL_PRIZE_FD,
	DUEL_PRIZE_EXP,
	DUEL_PRIZE_VOICE,

	DUEL_PRIZE_MONEY_CUSTOM,
	DUEL_PRIZE_EXP_CUSTOM
};

#define PRIZE_MONEY_LIMIT	100	// лимит денег (для кастом приза)
#define PRIZE_EXP_LIMIT		20	// лимит опыта (для кастом приза)

#define PRIZE_MONEY_NUM	20		// денег по умолчанию
#define PRIZE_EXP_NUM	2		// опыт по умолчанию

new const g_szDuelPrizes[][] = {
	"20 $",
	"Выходной",
	"2 опыта",
	"Голос",

	"Деньги",
	"Опыт"
};

/* -> Настройка отталкивания дуэлянтов -> */
#define DUEL_DMG_READY 				2000	// Урон при ожидании старта
#define DUEL_DMG_CUR_ATTACER		1500	// Урон от текущего стрелка
#define DUEL_DMG_READY_ATTACER		600		// Урон от ожидающего игрока

#define DUEL_DIST_READY 			600		// Дистанция при ожидании старта
#define DUEL_DIST_CUR_ATTACER		400		// Дистанция от текущего стрелка
#define DUEL_DIST_READY_ATTACER		300		// Дистанция от ожидающего игрока

#define DUEL_MIN_DMG				40		// Минимальный урон для толчка