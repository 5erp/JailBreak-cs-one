/* -> Битсуммы; переменные и массивы для футбола -> */

#define SOCCER_TEAM_RED		0
#define SOCCER_TEAM_BLUE	1

new g_iSoccerBall;
new g_iBitUserSoccer;
new g_iSoccerBallOwner;
new g_iSoccerKickOwner;

new g_iSoccerScore[2];
new g_iSoccerUserTeam[MAX_PLAYERS + 1];

new bool:g_bSoccerBallTouch;
new bool:g_bSoccerBallTrail;
new bool:g_bSoccerStatus;
new bool:g_bSoccerGame;
new bool:g_bAllUseBall;

new g_iBallThinkID;
new g_iBallTouchID;
new g_iSoccerImpulseID;
new HookChain:g_iBallUseEmpty;

new Float:g_flSoccerBallOrigin[3];

// -> Custom ball ->

new const g_szBallTypes[][] = { "футбольный", "волейбольный", "баскетбольный"};

enum // BALL_TYPE
{
	BT_FOOTBALL,
	BT_VOLLEYBALL,
	BT_BASEBALL
}

enum // BALL_SKINS
{
	BS_FOOTBALL,
	BS_VOLLEYBALL,
	BS_BASEBALL
}

new g_iBallType;		// футбольный, волейбольный, баскетбольный
new g_iBallSpeed = 10;	// скорость мяча, 10 - стандарт
new g_iBallTouchNum;	// какой раз соприкасаеться с миром