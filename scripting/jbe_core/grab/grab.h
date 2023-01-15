/********************************
*	Shared Functions Header File
*********************************/

/*==================================================
 #			-> Grab ->
====================================================*/



#define SF_FADEOUT			0
#define CDF_IN_PUSH			(1<<0)
#define CDF_IN_PULL			(1<<1)
#define CDF_NO_CHOKE		(1<<2)
#define GRAB_MIN_DIST		90		// Минимальная дистанция между id и target
#define GRAB_THROW_FORCE	1500	// drop - сила откидывания на G
#define GRAB_FORCE			12		// Скорость пермещения грабнутого
#define GRAB_SPEED			7		// Скорость граба (Push and Pull)
#define GRAB_CHOKE_DMG		5		// Дамаг на E USE (уже на ЛКМ)

new const g_szGrabPrefix[]		= "^1[^4Grab^1]";

enum _: DATA_GRAB
{
	GRABBED,	// Кого держим мы
	GRABBER,	// Кто держит нас
	GRAB_LEN,	// Расстояние между id и target
	GRAB_FLAGS	// Флаги: притягиваем | отдаляем | калечим
};

new g_iGrabData[MAX_PLAYERS + 1][DATA_GRAB];
new g_iBitGrabUse;			// Юзает ли кто-то граб
new g_iBitUserGrab;
new g_iBitOpenGrabMenu;		// Открыто ли меню граба

enum // grab menu types
{
	GRAB_MENU_NONE,
	GRAB_MENU_NORMAL,
	GRAB_MENU_ADMIN,
	GRAB_MENU_SKIN,
	GRAB_MENU_GLOW
};

new g_iGrabMenuType[MAX_PLAYERS + 1]; // храним выбранный тип граба

// меню свечения граба
new const g_iGrabMenuGlows[][] = {
	{ 0, 0, 0 },
	{ 0, 255, 0, "зеленное" },
	{ 255, 0, 0, "красное" },
	{ 0, 0, 255, "синее" },
	{ 255, 255, 0, "желтое" },
	{ 148, 0, 211, "фиолетовое" },
	{ 255, 140, 0, "оранжевое" },
	{ 71, 74, 81, "серое" }
};

/*==================================================
 #			<- Grab <-
====================================================*/