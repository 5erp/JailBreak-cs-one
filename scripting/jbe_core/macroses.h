/*===== -> Макросы -> =====*///{

#define MAX_PLAYERS 32
#define MAX_DAYMODES_NUM	10	// макс. игр
#define MAX_NAME_LENGTH		32
#define MAX_PASS_LENGTH		32
#define MAX_USERIP_LENGTH	16 // 255.255.255.255
#define MAX_CHAT_LENGTH		190
#define MODEL_STRING_SIZE	64
#define SOUND_STRING_SIZE	64
#define jbe_is_user_valid(%0) (%0 && %0 <= MaxClients)
#define IsValidPrivateData(%0) (pev_valid(%0) == 2)
#define IUSER1_DOOR_KEY 376027
#define IUSER1_BUYZONE_KEY 140658
#define IUSER1_FROSTNADE_KEY 235876

/* -> Бит сумм -> */
#define SetBit(%0,%1) ((%0) |= (1 << (%1)))
#define ClearBit(%0,%1) ((%0) &= ~(1 << (%1)))
#define IsSetBit(%0,%1) ((%0) & (1 << (%1)))
#define InvertBit(%0,%1) ((%0) ^= (1 << (%1)))
#define IsNotSetBit(%0,%1) (~(%0) & (1 << (%1)))
#define SetBitBool(%0,%1,%2) ((%2) ? ((%0) |= (1 << (%1))) : ((%0) &= ~(1 << (%1))))
#define GetBitBool(%0,%1) (((%0) & (1 << (%1))) ? 1 : 0)

/* -> Команды -> */
#define TEAM_UNASSIGNED 	0
#define TEAM_PRISONER 		1
#define TEAM_GUARD 			2
#define TEAM_SPECTATOR 		3

/* -> Оффсеты -> */
//#define linux_diff_weapon 4
//#define linux_diff_player 5
#define PLAYER_SUPERJUMP 7

//#define m_flNextPrimaryAttack 46
//#define m_flNextSecondaryAttack 47

/* -> Индексы сообщений -> */
#define MsgId_CurWeapon		66
#define MsgId_Damage		71
#define MsgId_SayText		76
#define MsgId_TextMsg		77
#define MsgId_WeaponList	78
#define MsgId_ResetHUD		79
#define MsgId_ScoreAttrib	84
#define MsgId_ScoreInfo		85
#define MsgId_ShowMenu		96
#define MsgId_ScreenShake	97
#define MsgId_ScreenFade	98
#define MsgId_SendAudio		100
#define MsgId_Money			102
#define MsgId_ArmorType		103
#define MsgId_StatusText	106
#define MsgId_StatusIcon	107 // Мессага показа иконок (для блока иконки шопа)
#define MsgId_Radar			112
#define MsgId_VGUIMenu		114
#define MsgId_ClCorpse		122
#define MsgId_HostageAdd	123
#define MsgId_HostageDel	124
#define MsgId_BuyClose		136 // Мессага закрытия шопа (для закрытия VGUI меню)
#define MsgId_HudTextArgs	145

/* Seconds in each time unit */
#define SECONDS_IN_MINUTE 60
#define SECONDS_IN_HOUR   3600
#define SECONDS_IN_DAY    86400
#define SECONDS_IN_WEEK   604800

/*===== <- Макросы <- =====*///}