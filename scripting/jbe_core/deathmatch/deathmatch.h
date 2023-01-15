#define MIN_DIST_SPAWN	150.0 // минимальная дистанция между спавнами
#define MIN_SPAWNS_NUM	3 // минимум спавнов для их задействования

// Рандомные спавны в ДМ, просто меняем координаты игрока ;)

#define DEATHMATCH_SPAWN_NUM 128	//32 // лимит спавнов (размер масива под спавны)

// enum struct тут нельзя ибо будет выбивать предупреждение tag mistmatch
#define SPAWN_ORIGIN_X		0
#define SPAWN_ORIGIN_Y		1
#define SPAWN_ORIGIN_Z		2
#define SPAWN_ANGLES_X		3
#define SPAWN_ANGLES_Y		4
#define SPAWN_ANGLES_Z		5
#define SPAWN_V_ANGLE_X		6
#define SPAWN_V_ANGLE_Y		7
#define SPAWN_V_ANGLE_Z		8

new Array:g_aDeathMatchSpawns;
new g_iDeathMatchSpawnsNum; // хранит кол-во спавнов
new Float:g_fTempSpawnOrigin[MAX_PLAYERS + 1][3];

enum _:DEATHMATCH_SPAWNS
{
	Float:DMS_ORIGIN[3],
	Float:DMS_ANGLES[3],
	Float:DMS_V_ANGLE[3]
};

stock Float:g_fDeathMatchSpawns[DEATHMATCH_SPAWN_NUM][9]; // Храним координаты 'спавнов'

new bool:g_bDeathMatchEnabled = false;
new bool:g_bGameRadarStatus = true; // Состояние радара, true - вкл; false - выкл

new const g_szDeathMatchName[][] = {
	"Ждем охрану",
	"В поисках Начальника",
	"Loading..CT..99%..",
	"А CT будут?",
//	"Death Match",
//	"шоу 'Жди КТ..'",
//	"Стреляемся крч..",
//	"CSDM (без пушек и лазеров)",
//	"Сeрвeр слoмa#@*",
//	"Режим: Режим всех!",
//	"Убей админа за админку c:",
//	"Что здесь происходит?",
//	"Рекламное место свободно",
//	"Это вообще jail?"
};

new g_msgScoreAttrib;

#define DM_WPN_PISTOL_SHIELD 19 // номер с которого начинаються пистолеты к которым можно дать щит

new const g_szDeathMatchWeapons[][] = {
	"weapon_scout",
	"weapon_xm1014",
	"weapon_mac10",
	"weapon_aug",
	"weapon_ump45",
	"weapon_sg550",
	"weapon_galil",
	"weapon_famas",
	"weapon_awp",
	"weapon_mp5navy",
	"weapon_m249",
	"weapon_m3",
	"weapon_m4a1",
	"weapon_tmp",
	"weapon_g3sg1",
	"weapon_sg552",
	"weapon_ak47",
	"weapon_p90",
	"weapon_elite",
	"weapon_p228",
	"weapon_deagle",
	"weapon_usp",
	"weapon_glock18",
	"weapon_fiveseven"
};



