/********************************
*	Shared Functions Header File
*********************************/

/*===== -> Битсуммы, переменные и массивы для работы с модом -> =====*///{

/* -> Переменные -> */
new bool:g_bRoundEnd = false;
new bool:g_bRestartGame = true;
new bool:g_bOneOpenDoors; // Были ли открыты клетки в этом раунде
new g_iFriendlyFire;
new g_iCountDown;
new Float:g_fRoundStartTime;	// Время старта раунда

new g_iSyncMainInformer;
new g_iSyncSoccerScore;
new g_iSyncDuelInformer;
new g_iSyncStatusText;
new g_iSyncDamageAttacker;	// Нанесенный урон
new g_iSyncDamageVictim;	// Полученный урон
new g_iSyncDeathMatchInformer;
new g_iSyncInfoCenter;		// разная инфа по центру

new g_iEvent_StatusValueShow;
new g_iEvent_StatusValueHide;

new g_iFakeMetaKeyValue;
new g_iFakeMetaSpawn;
new g_iFakeMetaUpdateClientData;
new g_iFakeMetaSetModel;		// Для FrostNade
new g_iFakeMetaSetModelDuel;	// Для Дуэлей

new Ham:Ham_Player_ResetMaxSpeed = Ham_Item_PreFrame;

/* -> Указатели для моделей -> */
new g_pModelGlass;

new cvar_sv_maxspeed;
new cvar_sv_gravity;
new cvar_mp_roundtime;
new cvar_mp_item_staytime;

/* -> Указатели для спрайтов -> */
new g_pSpriteWave,		// цилиндр когда упала грена
	g_pSpriteBeam,		// полоса за греной
	g_pSpriteBall,		// мяч над головой в футболе
	g_pSpriteLgtning,	// молния для хука
	g_pSpriteRicho2;	// еффект прикосновения хука с объектом

/* -> Массивы -> */
new g_iPlayersNum[4],
	g_iAlivePlayersNum[4],
	Trie:g_tRemoveEntities;

/* -> Массивы для кваров -> */

/* -> Индексы моделей -> */
enum _:PLAYER_MODELS_LIST
{
    PRISONER, PRISONER_GIRL,
    GUARD, GUARD_GIRL,
    CHIEF, CHIEF_GIRL,
};
new models_player_body[PLAYER_MODELS_LIST];

/* -> Переменные и массивы для дней и дней недели -> */
new g_iDay, g_iDayWeek;
new const g_szDaysWeek[][] = {
	"Начало игры ",
	"Понедельник",
	"Вторник",
	"Среда",
	"Четверг",
	"Пятница",
	"Суббота",
	"Воскресенье"
};

// Причины убийства
new const g_szKillReason[][] = {
	"Не выполнил приказ",
	"Проиграл в игру",
	"Попытка нападения",
	"Ношение оружия",
	"Был в розыске",
	"Зашёл в оружейную",
	"Залез в нычку",
	"Случайно"			// FD
};

new const g_szIpServer[]		= "37.230.210.119:27015";
new const g_szRulesMOTD[]		= "addons/amxmodx/configs/info/rules.txt";
new const g_szChatPrefix[]		= "^1[^4Тюрьма^1]"; // JailBreak
new const g_szRobShopPrefix[] 	= "^1[^4Бандитские дела^1]";
new const g_szDemoName[]		= "CS-One.Ru"; // Demo Name (auto play put in server)
new const g_szMotdCmds[]		= "addons/amxmodx/configs/info/cmds.txt";

/* -> Битсуммы, переменные и массивы для режимов игры -> */
enum _:DATA_DAY_MODE
{
	LANG_MODE[32],
	MODE_BLOCKED,
	VOTES_NUM,
	MODE_TIMER,
	MODE_BLOCK_DAYS,
	MODE_TRACK // id трека с g_aDataGameMusic
};

new Array:g_aDataDayMode;
new Array:g_aFileDayMode;
new g_iDayModeListSize;
new g_iDayModeVoteTime;
new g_iHookDayModeStart;
new g_iHookDayModeEnded;
new g_iReturnDayMode;
new g_iDayMode;
new g_szDayMode[64];
new g_iDayModeTimer;
new g_szDayModeTimer[6] = "";
new g_iVoteDayMode = -1;
new g_iBitUserVoteDayMode;
new g_iBitUserDayModeVoted;
new g_iBitOffDayModeSound;	// Выключил музыку в игре

enum _:DATA_DAY_MODE_MUSIC { DM_FILE_NAME[128], DM_TRACK_NAME[64], DM_TRACK_TIME };
new Array:g_aDataGameMusic;
new g_iGameMusicSize;

/* -> Переменные и массивы для работы с клетками -> */
new bool:g_bDoorStatus;
new Array:g_aDoorList;
new g_iDoorListSize;
new Trie:g_tButtonList;

/* -> Массивы для работы с событиями 'hamsandwich' -> */
new HamHook:g_iHamHookForwards[14];
new HamHook:g_iHamHookItemDeploy[28];
new HamHook:g_iHamHookPrimaryAttackDuel[6];
new HamHook:g_iHamHookPlayerJump;
new HamHook:g_iHamHookGrenadeTouch;

	// Duel and RoundEnd
new szHamHookEntityBlock[][] = {
	// Use
	"func_vehicle",			// Управляемая машина
	"func_tracktrain",		// Управляемый поезд
	"func_tank",			// Управляемая пушка
	"game_player_hurt",		// При активации наносит игроку повреждения
	"func_recharge",		// Увеличение запаса бронижелета
	"func_healthcharger",	// Увеличение процентов здоровья
	"game_player_equip",	// Выдаёт оружие
	"player_weaponstrip",	// Забирает всё оружие
	"func_button",			// Кнопка
	// Touch
	"trigger_hurt",			// Наносит игроку повреждения
	"trigger_gravity",		// Устанавливает игроку силу гравитации
	"armoury_entity",		// Объект лежащий на карте, оружия, броня или гранаты
	"weaponbox",			// Оружие выброшенное игроком
	"weapon_shield"			// Щит
};

enum eHamBlock
{
	// Use
	func_vehicle,			// Управляемая машина
	func_tracktrain,		// Управляемый поезд
	func_tank,				// Управляемая пушка
	game_player_hurt,		// При активации наносит игроку повреждения
	func_recharge,			// Увеличение запаса бронижелета
	func_healthcharger,		// Увеличение процентов здоровья
	game_player_equip,		// Выдаёт оружие
	player_weaponstrip,		// Забирает всё оружие
	func_button,			// Кнопка
	// Touch
	trigger_hurt,			// Наносит игроку повреждения
	trigger_gravity,		// Устанавливает игроку силу гравитации
	armoury_entity,			// Объект лежащий на карте, оружия, броня или гранаты
	weaponbox,				// Оружие выброшенное игроком
	weapon_shield,			// Щит	
};

new HookChain:HookChain_PreThink;

enum _:DATA_ROUND_SOUND { FILE_NAME[32], TRACK_NAME[64] }
new Array:g_aDataRoundSound, g_iRoundSoundSize;

/*===== <- Переменные и массивы для работы с модом <- =====*///}


/*===== -> Битсуммы, переменные и массивы для работы с игроками -> =====*///{

/* -> Битсуммы -> */
new g_iBitRconAdmin;
new g_iBitUserConnected;
new g_iBitUserAlive;
new g_iBitUserVoice;
new g_iBitUserVoiceNextRound;
new g_iBitEternalVoice;			// выдавать ли голос при спавне.
new g_iBitUserModel;
new g_iBitBlockMenu;
new g_iBitKilledUsers[MAX_PLAYERS + 1];
new g_iBitUserRoundSound;
new g_iBitUserGlobalAdmin;
new g_iBitUserChangeName;
new g_iBitShowDeathMenu;
new g_iBitUserSteam;	// Стим игрок
new g_iBitUserBuried;	// Закопан админом
new g_iBitUserGhost;	// Бит если игрок призрак
new g_iBitActiveGhost;	// Бит для отлова респавна призрака (после отлова удаляем)
new g_iBitGhostNoClip;
new g_iBitGhostGravity;
new g_iBitPriceGunPn;	// бит если продавал уже пушку
new g_iBitAutoWeapons;	// авто-взятие оружие для охраны 
new g_iBitUserGodMode;	// Игрок бессмертен
new g_iBitGirl;			// 
new g_iBitUser3DView;	// cam
new g_iBitOffCostumes;	// off visible costumes
new g_iBitButtonSound;	// звук нажатий кнопок в меню
new g_iBitBuyRespawn;	// покупал ли возрождение
new g_iBitOpenMainMenu;

/* -> Переменные -> */
new g_iLastPnId;
new g_iLastPnType;

#define LAST_PN_NONE	0
#define LAST_PN_GUN		2
#define LAST_PN_DUEL	1

/* -> Массивы -> */
new g_iUserTeam[MAX_PLAYERS + 1];
new g_iUserSkin[MAX_PLAYERS + 1];
new g_iUserMoney[MAX_PLAYERS + 1];
new g_szUserModel[MAX_PLAYERS + 1][32];
new g_iUserAutoWeapon[MAX_PLAYERS + 1]; // какое оружие выбрано для авто закупа
new g_iUserAfkWarns[MAX_PLAYERS + 1];
new g_iUserDemoID[MAX_PLAYERS + 1];

new Float:g_fUserMaxSpeed[MAX_PLAYERS + 1];

new Float:g_fMainInformerPosX[MAX_PLAYERS + 1],
	Float:g_fMainInformerPosY[MAX_PLAYERS + 1];
	

/* -> Битсуммы, переменные и массивы для освобождённых заключённых -> */
new g_iBitUserFree,
	g_iBitUserFreeNextRound,
	g_szFreeNames[192],
	g_iFreeLang;

new const g_szFreeLang[][] =
{
	"",
	"^n^nСвободные:"
};

/* -> Битсуммы, переменные и массивы для разыскиваемых заключённых -> */
new g_iBitUserWanted,
	g_szWantedNames[192],
	g_iWantedLang;

new const g_szWantedLang[][] =
{
	"",
	"^n^nРозыск:"
};

/* -> Переменные и массивы для костюмов -> */
enum _:DATA_COSTUMES { COSTUMES, ENTITY, bool:HIDE }
new g_eUserCostumes[MAX_PLAYERS + 1][DATA_COSTUMES];
enum _:DATA_COSTUMES_FILE { COSTUME_NAME[32], COSTUME_FILE[64], COSTUME_BODY[3], COSTUME_ACCESS[24] };
new Array:g_aDataCostumes;
new g_iCostumesListSize;

/* -> Переменные и массивы для рендеринга -> */
enum _:DATA_RENDERING { RENDER_STATUS, RENDER_FX, RENDER_RED, RENDER_GREEN, RENDER_BLUE, RENDER_MODE, RENDER_AMT }
new g_eUserRendering[MAX_PLAYERS + 1][DATA_RENDERING];

enum _:ALL_CVARS
{
	FREE_DAY_ID,
	FREE_DAY_ALL,
	TEAM_BALANCE,
	DAY_MODE_VOTE_TIME,
	RESTART_GAME_TIME,
	RIOT_START_MODEY,
	KILLED_GUARD_MODEY,
	KILLED_CHIEF_MODEY,
	KILLED_WANTED_MODEY,
	ROUND_FREE_MODEY,
	ROUND_ALIVE_MODEY,
	LAST_PRISONER_MODEY,
	CHANGETEAM_TIME,

	VIP_RESPAWN_NUM,
	VIP_HEALTH_NUM,
	VIP_MONEY_NUM,
	VIP_MONEY_ROUND,
	VIP_INVISIBLE,
	VIP_HP_AP_ROUND,
	VIP_GHOST_ROUND,
	VIP_GOD_ROUND,
	VIP_FOOTSTEPS_ROUND,
	VIP_NIGHT_START,
	VIP_NIGHT_ENDED,
	RESPAWN_PLAYER_NUM,
	VIP_RESPAWN_PLAYERS,
	SHOP_SPEED,
	
	GAME_NAME[32],
	GAME_NAME_NIGHT[32],
	VIP_NIGHT_PREFIX[32],

	// DeatchMatch
	DEATHMATCH_REMOVE_GUNS,
	DEATHMATCH_UPDATE_TIME,
	DEATHMATCH_MAX_PLAYERS,
	DEATHMATCH_RADAR,
};

new g_eAllCvars[ALL_CVARS];

enum ( += 1000 ) { // Task's
	TASK_ROUND_END = 1000,
	TASK_CHANGE_MODEL,
	TASK_SHOW_INFORMER,
	TASK_FREE_DAY_ENDED,
	TASK_CHIEF_CHOICE_TIME,
	TASK_COUNT_DOWN_TIMER,
	TASK_VOTE_DAY_MODE_TIMER,
	TASK_RESTART_GAME_TIMER,
	TASK_RESTART_RESPAWN,	// херня в ресстарт
	TASK_DAY_MODE_TIMER,
	TASK_SHOW_SOCCER_SCORE,
	TASK_REMOVE_SYRINGE,
	TASK_FROSTNADE_DEFROST,
	TASK_DUEL_COUNT_DOWN,
	TASK_DUEL_BEAMCYLINDER,
	TASK_DUEL_TIMER_ATTACK,
	TASK_HOOK_THINK,
	TASK_FREE_DAY_START,
	TASK_RADAR_LAST_PN,
	TASK_OPEN_DOORS,
	TASK_DEATHMATCH_PLAYER, // Respawn, give ammo, take god.
	TASK_LOAD_PLAYER_SQL,
	TASK_RELOAD_MYSQL_CONNECT,
	TASK_DEMORECORD_MESSAGE,
	TASK_USER_RESPAWN,
	TASK_RELOAD_FLAGS,
	TASK_RELOAD_ADMINS,
	TASK_CHIEF_GAME_START,	// Таск до старта некоторых мини игр начальника
	TASK_CHIEF_GAME_END,	// до окончания некотрых игр
	TASK_SPAWN_FIX,
	TASK_GRAB_NO_CHOKE,
	TASK_PLAYER_CHECK,
	TASK_READY_LIMITS,
	TASK_SPAWN_EFFECTS,
	TASK_DAYMODE_SOUND,
	TASK_MENU_ANIM,
	TASK_STATUSVALUE_HIDE
};

new g_iBitUsePaint;		// Юзает паинт
new g_iBitPaintHolding;	// Рисует паинтом

/*===== <- Битсуммы, переменные и массивы для работы с игроками <- =====*///}


new Array:g_aHudAdverts,
	g_iHudAdvertsSize,
	g_iHudAdvertUserCount[MAX_PLAYERS + 1 char];
#define DHUD_TIME 9


/* ->  -> */
new g_iszFuncVehicle = 0;
//new g_iszTrackTrain = 0;
new bool:g_bVehicleKilled = false;

#define USE_SET    2

/* Типы дня */

#define 	DAY_RESTART		0
#define 	DAY_NORMAL		1
#define 	DAY_FREEDAY		2
#define 	DAY_DAYMODE		3
#define 	DAY_DEATHMATCH	4

/* Skins prisiners*/

// Сколько скинов до скина футболиста
#define model_football_offset 8

enum
{
	SKIN_ORANGE,	// Оранжевый
	SKIN_GRAY,		// Серый
	SKIN_YELLOW,	// Желтый
	SKIN_BLUE,		// Синий
	SKIN_BROWN,		// Коричневый
	SKIN_BLACK,		// Черный
	SKIN_FREE,		// Свободный (зеленый)
	SKIN_WANTED,	// Розыск (красный)
	SKIN_FB_RED,	// Футбол красный
	SKIN_FB_BLUE 	// Футбол синий
};

// is user cmd flood
enum _:DATA_CMD_FLOOD
{
	FLOOD_ALL,			// Общая (не указывая тип)
	FLOOD_DOORS,		// Открытие/Закрытие клеток
	FLOOD_FREEDAY,		// Выдать/Забрать общий ФД
	FLOOD_BALL,			// Создать/Удалить мяч
	FLOOD_HOOK,			// Активация хука +hook (блок на звук)
	FLOOD_SLAY,			// Ударить/Убить в админ меню
	FLOOD_CHAT,			// Использование чата (say | say_team)
	FLOOD_LOGO,			// Обнуление логотипа "say /logo"
	FLOOD_HELLO,		// say /hi
	FLOOD_GLOBAL,		// Global функции
	FLOOD_MONEY,		// Передача денег
	FLOOD_COUNT_DOWN,	// Обратный отсчет / Звуки в меню симона
	FLOOD_PAINT,		// +paint
	FLOOD_ADMIN_HUD		// say_hud
};

// jbe_get_players_num(iType)
enum
{
	NUM_ALL_PLAYERS,
	NUM_ALIVE_ALL,
	NUM_PRISONER_GUARD,
	NUM_PRISONER,
	NUM_GUARD,
	NUM_ALIVE_PRISONER,
	NUM_ALIVE_GUARD,
	NUM_DEAD_PRISONER,
	NUM_DEAD_GUARD
}

new g_iUserChangeTeamTime[MAX_PLAYERS + 1];

// Имя профиля на сайте
new g_szMemberName[MAX_PLAYERS + 1][MAX_NAME_LENGTH];

enum GroupInfo ( <<=1 )
{
	GI_NONE,
	GI_PLAYER = 1,
	GI_COSTUMES,
	GI_WEAPONS
};