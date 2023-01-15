/********************************
*	Shared Functions Header File
*********************************/

new const g_szGlobalPrefix[]	= "^1[^4Global Menu^1]";

// id команд для меню
#define GT_ALL			0
#define GT_PRISONER		-1
#define GT_GUARD		-2
/*
enum ( -= 1 )
{
	GT_ALL = 0,
	GT_PRISONER,
	GT_GUARD,
	GT_ORANGE,		// Оранжевый
	GT_GRAY,		// Серый
	GT_YELLOW,		// Желтый
	GT_BLUE,		// Синий
	GT_BROWN,		// Коричневый
	GT_BLACK,		// Черный
};
*/
/* -> Global Menu -> */
new g_iGlobalMenuType[MAX_PLAYERS + 1];
new g_iGlobalItem[MAX_PLAYERS + 1];
//new g_iGlobalTeamType[MAX_PLAYERS + 1]; // Тип меню выбора игрока - игрока или команды

enum
{
	GLOBAL_RESPAWN = 1,
	GLOBAL_GIVE_GUN,
	GLOBAL_SKILL,

	GLOBAL_HEALTH,
	GLOBAL_ARMOR,
	GLOBAL_SPEED,
	GLOBAL_GRAVITY,

	GLOBAL_DEL_SKILLS,
	GLOBAL_SKREEN_OFF,
	GLOBAL_SKREEN_ON,
	GLOBAL_FREEZE,
	GLOBAL_UNFREEZE,
	GLOBAL_BURIED,
	GLOBAL_UNBURIED
}

// global give|take skills
enum _:DATA_GLOBAL_SKILLS
{
	GLOBAL_SKILL_SPEED,			// Скорость
	GLOBAL_SKILL_GRAVITY,		// Гравитацию
	GLOBAL_SKILL_250HP_250AP,	// 250HP + 250AP
	GLOBAL_SKILL_GODMODE,		// Бессмертие
	GLOBAL_SKILL_NOCLIP,		// Ноуклип
	GLOBAL_SKILL_DOUBLE_JUMP,	// Двойной прыжок
	GLOBAL_SKILL_AUTO_JUMP,		// Автораспрыг
	GLOBAL_SKILL_HIGHT_JUMP,	// Высокий прыжок
	GLOBAL_SKILL_KOKAIN,		// Уменьшеный урон
	GLOBAL_SKILL_DUBLE_DMG,		// Двойной урон
	GLOBAL_SKILL_SEMICLIP, 		// Семиклип
	GLOBAL_SKILL_INVIZ,			// Невидимость
	GLOBAL_SKILL_FOOTSTEPS,		// Бесшумные шаги
	GLOBAL_SKILL_LONG_JUMP,		// Длиный прыжок
	GLOBAL_SKILL_GHOST,			// Призрак
	GLOBAL_SKILL_FALL_NO_DMG,	// Защита от падений
	GLOBAL_SKILL_3DVIEW			// Вид от 3-го лица
}

new g_iGlobalSkillType[MAX_PLAYERS + 1][DATA_GLOBAL_SKILLS];

/* Global Block */

enum _:eGlobalBlock
{
	GB_SHOP,
	GB_RANK_BONUS,
	GB_BUY_RESPAWN,
	GB_GUARD_TEAM,
	GB_VIP_MENU,
	GB_HOOK,
	GB_CHAT_DEAD,
	GB_CHAT_ALL,
	GB_PM,
};

new g_iGlobalBlock[eGlobalBlock];

new g_iBitUseGlobalBlock; // Заюзал ли игрок глобал блок

/* Give Weapons */

enum _:DATA_GLOABL_WEAPONS_WEAPON { DGW_ID, DGW_ITEM };
enum _:DATA_GLOABL_WEAPONS { DGW_NAME, DGW_WEAPON[DATA_GLOABL_WEAPONS_WEAPON] };
#define global_weapon_name(%0) g_iszGlobalWeapons[%0][DGW_NAME]
#define global_weapon_item(%0) g_iszGlobalWeapons[%0][DGW_WEAPON][DGW_ITEM]
#define global_weapon_id(%0) g_iszGlobalWeapons[%0][DGW_WEAPON][DGW_ID]
#define global_weapon_is_grenade(%0) ((1<<g_iszGlobalWeapons[%0][DGW_WEAPON][DGW_ID]) & (1<<_:WEAPON_HEGRENADE|1<<_:WEAPON_SMOKEGRENADE|1<<_:WEAPON_FLASHBANG))

new g_iszGlobalWeapons[][][] = {
	// Забираем оружие
	{ "Забрать оружие", { -1, "weapon_none", } },
	{ "Забрать +нож", { -2, "weapon_none", } },
	// Штурмовые
	{ "M4A1", { _:WEAPON_M4A1, "weapon_m4a1" } },
	{ "AK-47", { _:WEAPON_AK47, "weapon_ak47" } },
	{ "Famas", { _:WEAPON_FAMAS, "weapon_famas" } },
	{ "Galil", { _:WEAPON_GALIL, "weapon_galil" } },
	{ "AUG", { _:WEAPON_AUG, "weapon_aug" } },
	{ "SG552", { _:WEAPON_SG552, "weapon_sg552" } },
	// Пулеметы  
	{ "M249", { _:WEAPON_M249, "weapon_m249" } },
	// Снайперские
	{ "AWP", { _:WEAPON_AWP, "weapon_awp" } },
	{ "SG550", { _:WEAPON_SG550, "weapon_sg550" } },
	{ "G3SG1", { _:WEAPON_G3SG1, "weapon_g3sg1" } },
	{ "Scout", { _:WEAPON_SCOUT, "weapon_scout" } },
	// Дробовики 
	{ "XM1014", { _:WEAPON_XM1014, "weapon_xm1014" } },
	{ "M3", { _:WEAPON_M3, "weapon_m3" } },
	// Пистолеты-пулеметы
	{ "Mac 10", { _:WEAPON_MAC10, "weapon_mac10" } },
	{ "TMP", { _:WEAPON_TMP, "weapon_tmp" } },
	{ "MP5", { _:WEAPON_MP5N, "weapon_mp5navy" } },
	{ "UMP45", { _:WEAPON_UMP45, "weapon_ump45" } },
	{ "P90", { _:WEAPON_P90, "weapon_p90" } },
	// Пистолеты
	{ "Deagle", { _:WEAPON_DEAGLE, "weapon_deagle" } },
	{ "USP", { _:WEAPON_USP, "weapon_usp" } },
	{ "P228", { _:WEAPON_P228, "weapon_p228" } },
	{ "Elite", { _:WEAPON_ELITE, "weapon_elite" } },
	{ "Glock 18", { _:WEAPON_GLOCK18, "weapon_glock18" } },
	{ "Fiveseven", { _:WEAPON_FIVESEVEN, "weapon_fiveseven" } },
	// Гранаты
	{ "SmokeGrenade", { _:WEAPON_SMOKEGRENADE, "weapon_smokegrenade" } },
	{ "Flashbang", { _:WEAPON_FLASHBANG, "weapon_flashbang" } },
	{ "HeGrenade", { _:WEAPON_HEGRENADE, "weapon_hegrenade" } },
	// Щит и нож
	{ "Shield", { 0, "weapon_shield" } },
	{ "Knife", { 0, "weapon_knife" } }
};

// Меню освещения
new const g_szLightsMenu[][][] = {
	{ "#OFF", "Стандартное" },
	{ "z", "Солнечный денек" },
	{ "v", "День" },
	{ "r", "Рассвет" },
	{ "n", "Тускло" },
	{ "i", "Вечер" },
	{ "e", "Поздний вечер" },
	{ "c", "Ночь" },
	{ "a", "Полная темнота" },
	{ "0", "Полный свет" }
};

new bool:g_bGlobalLight;	// Заюзано ли меню освещения (в конце рануда чтоб отключать)
new g_iBitGlobalEntBlock;