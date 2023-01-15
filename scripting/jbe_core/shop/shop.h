/********************************
*	Shared Functions Header File
*********************************/

#define ROB_SHOP_MIN_LVL	15 // мин лвл для грабежа шопа

const Float:SHOP_GRAVITY_NUM = 0.3;

/* -> Ограбление магазина -> */
new bool:g_bRobShop;	// Буллевая ограблен ли магазин.
new g_iBitRobShop;		// Бит у ограбившего.
new g_iRobShopNum;		// Кол-во попыток ограбить.

/* -> Битсуммы для магазина -> */
new g_iBitBalisong;
new g_iBitBasaballbat;
new g_iBitGuardBaton;
new g_iBitWeaponStatus;
new g_iBitLatchkey;
new g_iBitKokain;
new g_iBitFrostNade;
new g_iBitUserFrozen;
new g_iBitClothingGuard;
new g_iBitClothingType;
new g_iBitHingJump;
new g_iBitFastRun;
new g_iBitDoubleJump;
new g_iBitFallNoDamage; // Блок урона от падений
new g_iBitAutoBhop;
new g_iBitLongJump;
new g_iBitDoubleDamage;
new g_iBitLotteryTicket;
new g_iBitElectroCostume;
new g_iBitBuyWeaponsGuard;	// покупает ли КТ оружие в меню оружия КТ
new g_iBitOpenShopInfo;		// 1й раз открывает шоп, для уведомления.
new g_iBitBuyFrostNade;
//new g_iBitCandyCane; // карамельная трость

enum _:SHOP_CVARS
{
	NONE,
	BALISONG,
	BASABALLBAT,
	GUARD_BATON,
	USP,
	LATCHKEY,
	KOKAIN,
	STIMULATOR,
	FROSTNADE,
	ARMOR,
	CLOTHING_GUARD,
	HING_JUMP,
	FAST_RUN,
	DOUBLE_JUMP,
	FALL_NO_DMG,
	AUTO_BHOP,
	DOUBLE_DAMAGE,
	LOW_GRAVITY,
	LONG_JUMP,
	CLOSE_CASE,
	FREE_DAY_SHOP,
	RESOLUTION_VOICE,
	TRANSFER_GUARD,
	LOTTERY_TICKET,
	PRANK_PRISONER,
	SKIN,
	GRENADES,
	WEAPONS_GR,
	ELECTRO_COSTUME,
	BUY_RESPAWN,
	SHOP_OPEN_TIME,
};

new g_eShopCvars[SHOP_CVARS];