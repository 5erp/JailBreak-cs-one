#include <time>

#if !defined MAX_PLAYERS
	const MAX_PLAYERS = 32;
#endif
#if !defined MAX_NAME_LENGTH
	const MAX_NAME_LENGTH = 32;
#endif
#if !defined MAX_AUTHID_LENGTH
	const MAX_AUTHID_LENGTH = 25;
#endif
#if !defined MAX_REASON_LENGTH
	const MAX_REASON_LENGTH = 96;
#endif

enum _:ban_global_cvars
{ 
	ban_srv_name[64], 
	ban_srv_ip[24], 
	ban_cookie_link[128], 
	ban_hud_msg[512], 
	ban_hud_msg_color[3], 
	ban_superadmin,
	ban_global_bans, 
	ban_ip_bantime,
	ban_static_reasons, 
	ban_static_time 
};

enum _:BanData 
{ 
	ban_index, 
	bantime, 
	reason[MAX_REASON_LENGTH]
};

enum _:OffData
{
	name[MAX_NAME_LENGTH],
	ip[16],
	authid[MAX_AUTHID_LENGTH],
	immunity
};

enum _:BanKickData 
{ 
	auth[MAX_AUTHID_LENGTH], 
	u_name[MAX_NAME_LENGTH], 
	a_name[MAX_NAME_LENGTH], 
	ban_reason[MAX_REASON_LENGTH],
	ban_time,
	ban_length,
	bid
};

enum _:BanPlrData
{
	bool:cookie,
	pstate
};

enum
{ 
	none, 
	checked, 
	ban,
};

enum _:
{ 
	Ban, 
	Unban, 
	UnbanMenu,
	OffbanMenu,
	Check, 
	Search, 
	Expired,
	Update,	
	AddServer, 
	GetServer 
};

enum fwd 
{ 
	SqlInit,
	PreBan
};

enum BAN_CVARS
{ 
//	host, 
//	user, 
//	pass, 
//	db, 
	pref, 
	delay, 
	srvname, 
	srvip, 
	allbans, 
	ipban, 
	reasons, 
	rsnbtm, 
	crsn, 
	rmvexp, 
	sadmin, 
	unbanm, 
	lnkck, 
	hud, 
	hudpos, 
	hudclr 
};

//new Handle:g_hSqlTuple, - del
new g_Data[2];
new g_szTablePrefix[64];
// newg_szQueryBans[1024];

new g_iBanUserData[MAX_PLAYERS + 1][BanPlrData];
//new g_playerData[MAX_PLAYERS + 1][BanPlrData];
new g_arrBanData[MAX_PLAYERS + 1][BanData];
new g_arrKickData[MAX_PLAYERS + 1][BanKickData];

new g_iTimeMenu, g_iReasonMenu;

new g_fwdHandle[fwd];

new g_pBanCvars[BAN_CVARS];
new g_BanCvars[ban_global_cvars];

new Float:g_fHudPos[2];

new g_szConfigDir[64];

new Array:g_aOffPlayers, g_arrOffPlayers[OffData];

#define MAX_STRINGS 20

new g_iStrings;
new g_szConsole[MAX_STRINGS][256];

#if AMXX_VERSION_NUM < 183
	#define replace_string replace_all
#endif	