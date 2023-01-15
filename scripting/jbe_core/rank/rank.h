// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

/*==================================================
 #			-> Rank System ->
====================================================*/

#define RANK_REG_LEVEL	13	// lvl с которого требует регистрация для доустпа к ранг бонусу
#define RANK_EXP_MINUTES 5 // за сколько минут давать 1 EXP игрокам
#define RANK_ITEM_REASON_STR 12 // в меню длина для слов ('round', 'exp', '&team')

new const g_szRankPrefix[]  = "^1[^4Ранг^1]";
new Float:g_fRankRewardExp[MAX_PLAYERS + 1]; // save client reward exp
new g_iUserExp[MAX_PLAYERS + 1];
new g_iUserLevel[MAX_PLAYERS + 1];
new g_iUserExpPercent[MAX_PLAYERS + 1]; // Кеш процентов уровня для информера
new g_iRankMaxLevel;
new g_iBitUsedRankBonus;	// испльзовал ли игрок ранг бонус в этом раунде

enum _:DATA_RANK_BONUS
{
	RB_MONEY, // pn+gr
	RB_HEALTH, // pn+gr
	RB_GRENADES, // pn
	RB_FREEDAY, // pn
	RB_GLOCK, // pn
	RB_EL_COSTUME, // gr
	RB_KOKAIN, // gr
	RB_GOD // gr (chief)
};

// кому доступен пункт с ранг бонусу
enum
{
	RB_TYPE_ALL,
	RB_TYPE_PN,
	RB_TYPE_GR,
	RB_TYPE_CHIEF
}

new g_iRankBonus[MAX_PLAYERS + 1][DATA_RANK_BONUS];
new g_eRankBonusRound[DATA_RANK_BONUS];
new g_iRankBonusExp[DATA_RANK_BONUS];

#define RANK_EXP	0
#define RANK_NAME	1

new const g_iszRankExpName[][] = {
/*0*/	{ 0,		"Новичок"	},
/*1*/	{ 30,		"Прохожий"	},
/*2*/	{ 50,		"Петушок"	}, 
/*3*/	{ 60,		"Пакетик"	},
/*4*/	{ 80,		"Трус"		},
/*5*/	{ 105,		"Лютый"		},
/*6*/	{ 130,		"Воришка"	},
/*7*/	{ 150,		"Педофил"	},
/*8*/	{ 170,		"Патрик"	},
/*9*/	{ 215,		"Косой"		},
/*10*/	{ 275,		"Чушка"		},
/*11*/	{ 355,		"Дьяк"		},
/*12*/	{ 450,		"Зануда"	},
/*13*/	{ 580,		"Карлсон"	},
/*14*/	{ 740,		"Лентяй"	},
/*15*/	{ 950,		"Рыцарь"	},
/*16*/	{ 1215,		"Студент"	},
/*17*/	{ 1560,		"Амфетамин"	},
/*18*/	{ 1995,		"Нытик"		},
/*19*/	{ 2550,		"Кирпич"	},
/*20*/	{ 3265,		"Маньяк"	},
/*21*/	{ 4180,		"Умник"		},
/*22*/	{ 5350,		"Клоун"		},
/*23*/	{ 6850,		"Капитошка"	},
/*24*/	{ 8770,		"Дедуля"	},
/*25*/	{ 11225,	"Чепушила"	},
/*26*/	{ 14370,	"Аутист"	},
/*27*/	{ 18390,	"Гангстер"	},
/*28*/	{ 23540,	"Пахан"		},
/*29*/	{ 30130,	"Батяня"	},
/*30*/	{ 38565,	"Лидер"		},
/*31*/	{ 50000,	"Святой"	}
};


/*==================================================
 #			<- Rank System <-
====================================================*/