/********************************
*	Shared Functions Header File
*********************************/

#define BR_WANTED_LIMIT 2	// лимит на бунт в неделю

new const g_szBrPrefix[] = "^1[^4Наручники^1]";

new const g_iszBraceletesTime[][] = {
// время в секундах, пункт в меню
	{ 10 * 60,		"10 минут" },
	{ 20 * 60,		"20 минут" },
	{ 30 * 60,		"30 минут" },
	{ 60 * 60,		"1 час" },
	{ 60 * 60 * 2,	"2 часа" },
	{ 60 * 60 * 3,	"3 часа" },
	{ 60 * 60 * 6,	"6 часов" }
};

#define BRACELETES_TIME 0
#define BRACELETES_NAME 1

new g_iUserBraceletsType[MAX_PLAYERS + 1];
new g_iUserBraceletsTime[MAX_PLAYERS + 1];
new g_iUserWantedNum[MAX_PLAYERS + 1];
new g_iBraceletsSelect[MAX_PLAYERS + 1];
new g_iBitUserRealWanted;	// реальный бунт. нападение на кт (подстава и пр. не в счет)

// время ласт чека на наручники
new g_iUserBrTimeCheck[MAX_PLAYERS + 1]; 

// Типы наручников
enum _:eBraceletsTypes
{
	BR_NONE,
	BR_ATTACK,
	BR_ATTACK_BUTTON
};

// Тип в меню
new const g_szBrTypes[][] = {
	"", // BR_NONE
	"Атака",
	"Атака+Кнопки"
};

// для is_user_braceletes
enum
{
	BR_CHECK_ATTACK,
	BR_CHECK_BUTTON
};

// сохраняем дейстиве выбранное
enum
{
	MB_SET,
	MB_EDIT
}