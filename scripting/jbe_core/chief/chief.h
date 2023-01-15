/********************************
*	Shared Functions Header File
*********************************/

#define CHIEF_GLOBAL_GAME_LIMIT 2 // лимит глоб игр начальника на неделю (штурм, мясо итд)

new const g_szInfoChief[][] =
{
	"Начальнику доступен маркер ^3( в консоль: bind v +paint )",
	"Быстрое открытие меню начальника на N ^3( в консоль: bind n nightvision )",
	"Начальник может снимать розыск ^4M -> 4 -> 5 ^3(или в чат: /wanted)",
	"Обнулить заключенным навыки ^4N -> 9 -> 1 ^3(или в чат: /ts)",
	"Смотри игры для начальника на форуме: ^4cs-one.ru/t?224"
};

new const g_szChiefPrefix[]		= "Начальник";	// Для общих функций ("Начальник" : "Админ")

/* -> Переменные и массивы для начальника -> */
new g_iChiefId;
new g_iChiefIdOld;
new g_iChiefChoiceTime;
new g_szChiefName[72]; // больше 72 не нужно
new g_iChiefStatus;
new g_iChiefGameType;
new g_iChiefGlobalGameLimit; // лимит глоб игр начальника на неделю (штурм, мясо итд)
new g_iUserGameTeam[MAX_PLAYERS + 1] = { -1, ... };

enum
{
	// Некоторые мини игры
	GAME_NONE,				// Нет игры - 0
	GAME_SOCCER,			// Футбол
	GAME_BOXING,			// Бокс
	GAME_FFIRE,				// Огонь по своим
	GAME_FF_TEAM,			// Командный огонь по своим
	// Игровые дни
	GAME_SHTURM,			// Штурм
	GAME_HUNGER,			// Голодные игры
	GAME_M9SO,				// Мясо
	GAME_GHOST,				// Невидимки
	GAME_SNIPER,			// Снайпера
	GAME_BOMBER,			// Бомбер
	// Глобальные игры
	GAME_MAFIA,				// Мафия
	GAME_JIHAD,				// Битва за Джихад
};

#define CHIEF_ALL_GLOBAL_GAMES (1<<GAME_SHTURM|1<<GAME_HUNGER|1<<GAME_M9SO|1<<GAME_GHOST|1<<GAME_SNIPER|1<<GAME_BOMBER|1<<GAME_MAFIA|1<<GAME_JIHAD)

new const g_szMiniGameNames[][] = {
	"", //GAME_NONE
	"Футбол",
	"Бокс",
	"Огонь по своим",
	"Штурм",
	"Голодные игры",
	"Мясо",
	"Невидимки",
	"Снайпера",
	"Бомбер"
};

new bool:g_bCGameBomberType;	// Чей ход в игре "Снайпера"

#define GAME_DURATION		181	// Длительность некоторых игр симона (сек)

#define CHIEF_WAIT			0
#define CHIEF_ALIVE			1
#define CHIEF_DEAD			2
#define CHIEF_DISCONNECT	3
//#define CHIEF_FREE		4

/* Frendly Fire */
enum
{
	FF_OFF,			// Frendly Fire off
	FF_PRISONER,	// Frendly Fire on of prisioners
	FF_GUARD,		// Frendly Fire on of guards
	FF_OVERALL,		// Frendly Fire on of all
	FF_TEAM,		// 
};

// chief take skills menu
enum
{
	CHIEF_TAKE_SKILL_JUMPS, // двойной, высокий, длинный, авто-распрыг, гравитация
	CHIEF_TAKE_SKILL_SPEED,
	CHIEF_TAKE_SKILL_HP_AP,
	CHIEF_TAKE_SKILL_DAMAGE, // двойной и уменьшенный урон
	CHIEF_TAKE_SKILL_GLOW
};

new const g_szMiniGameWord[][] = {
	"Машина",
	"Дом",
	"Лето",
	"Жара",
	"Песок",
	"Зефир",
	"Холодильник",
	"Булочка",
	"Единорог",
	"Окно",
	"Президент",
	"Рюкзак",
	"Водопад",
	"Газировка",
	"Студент",
	"Вода",
	"Креветка",
	"Кофе",
	"Вино",
	"Дерево",
	"Книга",
	"Пуфик",
	"Школа",
	"Шоколад",
	"Мармелад",
	"Король",
	"Север",
	"Дракон",
	"Тюрьма",
	"Игра",
	"Корова",
	"Потолок"
};