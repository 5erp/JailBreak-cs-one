// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

/********************************
*	Shared Functions Header File
*********************************/

/* -> Битсуммы, переменные и массивы для бокса -> */
new g_iBoxingGame;
new g_iBitUserBoxing;
new g_iBoxingTypeKick[MAX_PLAYERS + 1];
new g_iBoxingUserTeam[MAX_PLAYERS + 1];
	
enum
{
	BOXING_NOT,			// бокс выключен
	BOXING_RUN,			// стартовал обычный бой
	BOXING_RUN_TEAM,	// стартовал командный бой
	BOXING_WAIT,		// раздаем перчатки, но бой еще не стартовал
}

