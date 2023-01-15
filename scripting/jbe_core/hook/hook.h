/********************************
*	Shared Functions Header File
*********************************/

new Float:g_vecHookOrigin[MAX_PLAYERS + 1][3];
new Float:g_fUserHookGravity[MAX_PLAYERS + 1];

new g_iBitUserHook;
new g_iBitHookUse;	// юзает щас хук, для блока урона при падении

/*=========-> Hook Menu Setting ->==================*/

new const g_iszHookSpeed[][] = {
	{ 90,  "Очень медленно" },
	{ 120, "Медленно" },
	{ 180, "Нормально" },
	{ 240, "Быстро" },
	{ 320, "Очень быстро" }
};
new const g_iHookWidth[] = { 60, 80, 120, 160, 200, 20 };	// ширина
new const g_iHookAmplitude[] = { 5, 10, 30, 50, 60, 90, 120, 0 };	// амплитуда
new const g_iHookTrailWidth[] = { 12, 16, 20, 24, 4, 8 };	// траил ширина
new const g_iHookTrailLength[] = { 7, 10, 13, 16, 3 };	// траил длина
new const g_iszHookColour[][] = {	// цвет (хука+траила)
	{ 0,0,0, 		"случайный" },
	{ 0,255,255, 	"голубой" },
	{ 0,0,255, 		"ярко-синий" },
	{ 255,0,255, 	"розовый" },
	{ 128,128,128, 	"серый" },
	{ 0,128,0, 		"зеленый" },
	{ 0,255,0, 		"ярко-зеленый" },
	{ 128,0,0, 		"бордовый" },
	{ 0,0,128, 		"синий" },
	{ 128,128,0, 	"оливковый" },
	{ 128,0,128, 	"фиолетовый" },
	{ 255,0,0, 		"красный" },
	{ 192,192,192, 	"серебряный" },
	{ 0,128,128, 	"серо-зеленый" },
	{ 255,255,0, 	"желтый" }
};

enum _: DATA_HOOK {
	HOOK_SPEED,
	HOOK_COLOUR,
	HOOK_WIDTH,
	HOOK_AMPLITUDE,
	HOOK_TRAIL_COLOUR,
	HOOK_TRAIL_WIDTH,
	HOOK_TRAIL_LENGTH
}

new g_eHookSetting[MAX_PLAYERS + 1][DATA_HOOK];
new g_eHookSettingSize[DATA_HOOK];	// размеры всего храним
new g_iBitNewHook;	// юзаем новый хук (да/нет)
new g_iBitHookMenu;	// открывать меню (да/нет)

#define hook_speed_name(%0) g_iszHookSpeed[g_eHookSetting[%0][HOOK_SPEED]][1]
#define hook_speed_num(%0) g_iszHookSpeed[g_eHookSetting[%0][HOOK_SPEED]][0]
#define hook_width_num(%0) g_iHookWidth[g_eHookSetting[%0][HOOK_WIDTH]]
#define hook_color_name(%0) g_iszHookColour[g_eHookSetting[%0][HOOK_COLOUR]][3]
#define hook_amplitude(%0) g_iHookAmplitude[g_eHookSetting[%0][HOOK_AMPLITUDE]]

/*==========<- Hook Menu Setting <-=================*/