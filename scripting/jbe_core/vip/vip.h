/* -> Битсуммы, переменные и массивы для работы с випами -> */

const VIP_RES_ACCEPT_TIME = SECONDS_IN_MINUTE * 3; // скок ждать до след. запроса на рес.

new g_iBitUserVip;
new g_iBitUserNightVip; // ночная vip (для ограничений)

enum _: DATA_VIP
{
	DV_RESPAWN,
	DV_HEALTH,
	DV_MONEY,
	DV_INVISIBLE,
	DV_HP_AP,
	DV_GHOST,
	DV_GOD,
	DV_FOOTSTEPS,
	DV_RES_PLAYERS
};

new g_iVip[MAX_PLAYERS + 1][DATA_VIP];

// огр по раундам
//new g_eVipRound[DATA_VIP];

// respawn accept night vip
new g_iBitVipResAccept;
new g_iVipResAcceptTime[MAX_PLAYERS + 1];


