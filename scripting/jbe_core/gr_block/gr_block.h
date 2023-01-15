/********************************
*	Shared Functions Header File
*********************************/


new g_iBitUserGrBlock;

// потом на квары
new g_szGrBlockTable[32] = "jbe_grblocks"; // g_szBlockTable

new const g_szGrBlockPrefix[] = "^1[^4Блок охраны^1]"; // chat prefix

new g_iTimeBlockGrEnd[MAX_PLAYERS + 1];

new g_iMenuPlayer[MAX_PLAYERS + 1];
new g_iMenuReason[MAX_PLAYERS + 1];
new g_iMenuTime[MAX_PLAYERS + 1];

new g_szReason[MAX_PLAYERS + 1][92];

new const g_szReasons[][] = {
	"Своя причина",
	"Нет микрофона",
	"FreeKill",
	"Привилегии игрокам",
	"Не умеет играть за КТ",
	"Нет 16+",
	"Отдохни"
};

new const g_iszTimeBlock[][] = {
	{ 30,		"30 минут" },
	{ 60,		"1 час" },
	{ 180,		"3 часа" },
	{ 360,		"6 часов" },
	{ 1440,		"24 часа" },
	{ 4320,		"3 дня" },
	{ 10080,	"1 неделя" },
	{ 20160,	"2 недели"},
	{ 43200,	"1 месяц"},
	{ 86400,	"2 месяца" },
	{ 129600,	"3 месяца" }
};

new g_GrAdmin_Blocker[MAX_PLAYERS + 1][MAX_NAME_LENGTH];