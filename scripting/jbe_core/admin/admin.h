// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

/********************************
*	Shared Functions Header File
*********************************/

new g_iBitUserAdmin;

new const g_szAdminPrefix[]		= "^1[^4ADMIN^1]";

new const g_iAdminSlaps[] = { 0, 0, 5, 25, 50 };

// admin say @ - @[@|@|@][w|r|g|b|y|m|c]<text> - displays hud message
new g_iAdminHudChannel;

new const g_iAdminHudColors[][] = {
	{255, 255, 255}, //w white
	{255, 0, 0}, //r red
	{0, 255, 0}, //g green
	{0, 0, 255}, //b blue
	{255, 255, 0}, //y yellow
	{255, 0, 255}, //m
	{0, 255, 255}, //c
	{227, 96, 8}, 
	{45, 89, 116}, 
	{103, 44, 38}
};

enum _:TELEPORT_MENU_POSITION
{
	TMP_AIMING,		// По прицелу
	TMP_CURRENT,	// Моя позиция
	TMP_SAVED,		// Сохраненная позиция
	TMP_TO_PLAYER	// Телепорт себя к игроку
}

new g_iAdminCurPosTeleport[MAX_PLAYERS + 1];
new Float:g_fAdminTeleportOrigin[MAX_PLAYERS + 1][3];
new Float:g_fAdminTeleportAngle[MAX_PLAYERS + 1][3];
