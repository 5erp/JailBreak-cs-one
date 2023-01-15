new bool:g_bJihadGame = false;
new bool:g_bJihadStart = false;
new const g_szJihadPrefix[] = "^1[^4Битва за Джихад^1]";
new g_iJihadRole[MAX_PLAYERS + 1];

enum _:JIHAD_ROLES
{
	DJR_NONE,
	DJR_CAMPER, 		// кэмпер
	DJR_JABBA,			// джабба
	DJR_MARTIAN,		// марсианин
	DJR_WORN,			// глиста
	DJR_SPY,			// шпион
	DJR_SMUGGLER,		// контрабандист
	DJR_RIOT_POLICE,	// омоновец
	DJR_ALTAIR,			// альтаир
	DJR_DIGGER,			// копатель
	DJR_STRATEGIST,		// стратег
	DJR_MARTYR,			// шахид
	DJR_FAN,			// фанат
};

new const g_szJihadNameRoles[][] = {
	"Нет роли",
	"Кэмпер",
	"Джабба",
	"Марсианин",
	"Глиста",
	"Шпион",
	"Контрабандист",
	"Омоновец",
	"Альтаир",
	"Копатель",
	"Стратег",
	"Шахид",
	"Фанат",
};
