/********************************
*	Shared Functions Header File
*********************************/

new const g_szMafiaPrefix[] = "^1[^4Мафия^1]";

new bool:g_bMafiaGame;			// on|off mafia
new bool:g_bMafiaDay;			// status: day || night
new bool:g_bMafieFreezed;		// all players freezed || unfrezzed
new bool:g_bMafiaSetsRoles;		// выданы ли роли
new bool:g_bMafiaOneDay;		// был первый день уже

new g_iMafiaStatusValueShow;
new g_iMafiaStatusValueHide;
new g_iSyncStatusMafia;
new g_iUserMafiaRole[MAX_PLAYERS + 1];
new g_iBitMafiaNight; 					// player status night
new g_iMafiaCurrentRole;				// Какая роль щас ходит

#define MAFIA_DAY_LIGHT		"r"	// Освещение в день
#define MAFIA_NIGHT_LIGHT	"i" // Освещение в ночь

new const g_szMafiaRoles[][] = {
	"Мирный житель",
	"Мафия",
	"Доктор",
	"Шериф",
	"Шлюха",
	"Маньяк"
};

enum _:DATA_MAFIA_ROLES
{
	DMR_PEACE_CITIZEN,
	DMR_MAFIA,
	DMR_DOCTOR,
	DMR_SHERIF,
	DMR_WHORE,
	DMR_MANIAC
};

