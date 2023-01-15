#include <amxmodx>
#include <fakemeta>
#include <hamsandwich>
#include <xs>
#include <engine>
//#include <jbe_core>

#pragma semicolon 0

/*===== -> Макросы -> =====*///{

/* -> Бит сумм -> */
#define SetBit(%0,%1) ((%0) |= (1 << (%1)))
#define ClearBit(%0,%1) ((%0) &= ~(1 << (%1)))
#define IsSetBit(%0,%1) ((%0) & (1 << (%1)))
#define InvertBit(%0,%1) ((%0) ^= (1 << (%1)))
#define IsNotSetBit(%0,%1) (~(%0) & (1 << (%1)))

/* -> Оффсеты и другое -> */
#define jbe_is_user_valid(%0) (%0 && %0 <= MaxClients)
#define MAX_PLAYERS 32

/* -> Индексы сообщений -> */
#define MsgId_SayText 76
#define MsgId_CurWeapon 66
#define MsgId_ScreenFade 98

/* -> Оффсеты -> */
#define lunux_offset_player 5
#define m_flNextAttack 83

/* -> Таск -> */
#define TASK_DEATH_TIMER 1337
#define TASK_BEAMCYLINDER 228

#define TASK_AMBIENCE_SOUND 232131
#define SOUND_DURATION 76 // длительность песни игры

new const g_szDayMode_Sound[] = "sound/CsOneRu/jb/days_mode/ringolevio/ambience3.mp3";

native jbe_register_day_mode(szLang[32], iBlock, iTime);
native jbe_get_user_team(id);
native jbe_register_game_music(const szTrackName[], const szFileName[]);

/*===== -> Битсуммы, переменные и массивы для работы с игрой -> =====*///}

new g_iDayModePotato, g_iBitUserGame, g_iBitUserPotato, g_pSpriteWave, g_iDeathTimer, MaxClients, HamHook:g_iHamHookForwards[15], g_iSyncTimer, g_iKartowka;
new const g_szHamHookEntityBlock[][] =
{
	"func_vehicle", // Управляемая машина
	"func_tracktrain", // Управляемый поезд
	"func_tank", // Управляемая пушка
	"game_player_hurt", // При активации наносит игроку повреждения
	"func_recharge", // Увеличение запаса бронижелета
	"func_healthcharger", // Увеличение процентов здоровья
	"game_player_equip", // Выдаёт оружие
	"player_weaponstrip", // Забирает всё оружие
	"trigger_hurt", // Наносит игроку повреждения
	"trigger_gravity", // Устанавливает игроку силу гравитации
	"armoury_entity", // Объект лежащий на карте, оружия, броня или гранаты
	"weaponbox", // Оружие выброшенное игроком
	"weapon_shield" // Щит
};

/*===== <- Битсуммы, переменные и массивы для работы с игрой <- =====*///}

public plugin_precache()
{
	g_pSpriteWave = engfunc(EngFunc_PrecacheModel, "sprites/shockwave.spr");
	engfunc(EngFunc_PrecacheGeneric, g_szDayMode_Sound);
}

public plugin_init()
{
	register_plugin("***[DEAD]~[J-KARTOWKA]***", "1.1", "Finer & [VaMpIr] DEAD & Serp");

	g_iDayModePotato = jbe_register_day_mode("Горячая картошка", 0, 130);
	jbe_register_game_music("Тату – Нас не догонят", g_szDayMode_Sound);

	new i;
	for(i = 0; i <= 7; i++) DisableHamForward(g_iHamHookForwards[i] = RegisterHam(Ham_Use, g_szHamHookEntityBlock[i], "HamHook_EntityBlock", 0));
	for(i = 8; i <= 12; i++) DisableHamForward(g_iHamHookForwards[i] = RegisterHam(Ham_Touch, g_szHamHookEntityBlock[i], "HamHook_EntityBlock", 0));
	DisableHamForward(g_iHamHookForwards[13] = RegisterHam(Ham_TraceAttack, "player", "Ham_TraceAttack_Pre", 0));
	DisableHamForward(g_iHamHookForwards[14] = RegisterHam(Ham_Killed, "player", "Ham_PlayerKilled_Post", 1));
	g_iSyncTimer = CreateHudSyncObj();
}

public client_disconnect(id)
{
	if(IsSetBit(g_iBitUserPotato, id))
	{
		ClearBit(g_iBitUserPotato, id);
		fm_set_user_rendering(id, kRenderFxNone, 0.0, 0.0, 0.0, kRenderNormal, 0.0);
		UTIL_ScreenFade(id, 0, 0, 0, 0, 0, 0, 0, 1);
		remove_task(TASK_DEATH_TIMER);
		set_task(1.0, "jbe_potato_new");
	}
	ClearBit(g_iBitUserGame, id);
}

/*===== -> 'hamsandwich' события -> =====*///{

public HamHook_EntityBlock() return HAM_SUPERCEDE;
public Ham_TraceAttack_Pre(iVictim, iAttacker, Float:fDamage, Float:vecDeriction[3], iTrace, iBitDamage)
{
	if(IsSetBit(g_iBitUserGame, iAttacker) && jbe_is_user_valid(iAttacker)) jbe_dm_user_kartowka(iVictim, iAttacker);
	return HAM_SUPERCEDE;
}
public Ham_PlayerKilled_Post(iVictim) 
{
	ClearBit(g_iBitUserGame, iVictim);
	fm_set_user_rendering(iVictim, kRenderFxNone, 0.0, 0.0, 0.0, kRenderNormal, 0.0);
	UTIL_ScreenFade(iVictim, 0, 0, 0, 0, 0, 0, 0, 1);
}

jbe_dm_user_kartowka(iVictim, iAttacker)
{
	if(IsSetBit(g_iBitUserPotato, iAttacker))
	{
		ClearBit(g_iBitUserPotato, iAttacker);
		fm_set_user_rendering(iAttacker, kRenderFxGlowShell, 0.0, 0.0, 0.0, kRenderNormal, 0.0);
		UTIL_ScreenFade(iAttacker, 0, 0, 0, 0, 0, 0, 0, 1);
		set_pev(iAttacker, pev_maxspeed, 380.0);
		
		SetBit(g_iBitUserPotato, iVictim);
		fm_set_user_rendering(iVictim, kRenderFxGlowShell, 255.0, 0.0, 0.0, kRenderNormal, 0.0);
		UTIL_ScreenFade(iVictim, 0, 0, 4, 255, 0, 0, 100, 1);
		set_pev(iVictim, pev_maxspeed, 450.0);
	}
}

/*===== <- 'hamsandwich' события <- =====*///}

/*===== -> Игровой процесс -> =====*///{
/* -> Начало игры -> */
public jbe_day_mode_start(iDayMode, iAdmin)
{
	if(iDayMode == g_iDayModePotato)
	{
		new iPlayers[32], iNum, i, g_iKartowka;
		for(i = 1; i <= MaxClients; i++)
		{
			if(!is_user_alive(i)) continue;
			SetBit(g_iBitUserGame, i);
			fm_strip_user_weapons(i);
			fm_give_item(i, "weapon_knife");
			set_pev(i, pev_gravity, 0.3);
			set_pev(i, pev_maxspeed, 380.0);
			iPlayers[iNum++] = i;
		}
		for(i = 0; i < sizeof(g_iHamHookForwards); i++) EnableHamForward(g_iHamHookForwards[i]);
		
		/* Выбор игрока */
		g_iKartowka = iPlayers[random_num(0, iNum - 1)];
		UTIL_ScreenFade(g_iKartowka, 0, 0, 4, 255, 0, 0, 100, 1);
		fm_set_user_rendering(g_iKartowka, kRenderFxNone, 255.0, 0.0, 0.0, kRenderNormal, 0.0);
		UTIL_SayText(g_iKartowka, "Внимание! Вы превратились в картошку. Скорей кого-то ударьте!");
		SetBit(g_iBitUserPotato, g_iKartowka);
		g_iDeathTimer = 20;
		set_task(1.0, "jbe_kartowka_informer", TASK_DEATH_TIMER, _, _, "a", g_iDeathTimer);
		set_task(1.0, "jbe_bream_cylinder", TASK_BEAMCYLINDER, _, _, "b");
		
		jbe_dm_ambience_sound_task();
	}
}

/* -> Конец игры -> */
public jbe_day_mode_ended(iDayMode)
{
	if(iDayMode == g_iDayModePotato)
	{
		if(task_exists(TASK_DEATH_TIMER)) remove_task(TASK_DEATH_TIMER);
		if(task_exists(TASK_BEAMCYLINDER)) remove_task(TASK_BEAMCYLINDER);
		new i;
		for(i = 0; i < sizeof(g_iHamHookForwards); i++) DisableHamForward(g_iHamHookForwards[i]);
		for(i = 1; i <= MaxClients; i++)
		{
			if(IsSetBit(g_iBitUserGame, i))
			{
				fm_strip_user_weapons(i, 1);
				//if(IsSetBit(g_iBitUserPotato, i)) ClearBit(g_iBitUserPotato, i);
				ExecuteHamB(Ham_Killed, i, i, 2);
			}
		}
		g_iBitUserGame = 0;
		g_iBitUserPotato = 0;
		
		client_cmd(0, "mp3 stop");
		remove_task(TASK_AMBIENCE_SOUND);
		pause("a");
	}
}

public jbe_dm_ambience_sound_task()
{
	client_cmd(0, "mp3 play %s", g_szDayMode_Sound);
	set_task(SOUND_DURATION.0, "jbe_dm_ambience_sound_task", TASK_AMBIENCE_SOUND);
}

/* -> Таймер -> */
public jbe_kartowka_informer()
{
	if(!--g_iDeathTimer) jbe_potato_new();
	for(new pPlayer = 1; pPlayer <= MaxClients; pPlayer++)
	{
		set_hudmessage(255, 69, 0, -1.0, -1.0, 0, 0.0, 0.9, 0.1, 0.1, -1);
		ShowSyncHudMsg(pPlayer, g_iSyncTimer, "Картошка умрет: %d", g_iDeathTimer);
	}
}

/* -> Выбор нового игрока -> */
public jbe_potato_new()
{
	new iPlayers[32], iNum, i;
	for(i = 1; i <= MaxClients; i++)
	{
		if(IsSetBit(g_iBitUserPotato, i)) 
		{
			ExecuteHamB(Ham_Killed, i, i, 0);
			ClearBit(g_iBitUserPotato, i);
		}
		if(!is_user_alive(i) || IsSetBit(g_iBitUserPotato, i)) continue;
		iPlayers[iNum++] = i;
	}
	g_iKartowka = iPlayers[random_num(0, iNum - 1)];
	UTIL_ScreenFade(g_iKartowka, 0, 0, 4, 255, 0, 0, 100, 1);
	fm_set_user_rendering(i, kRenderFxNone, 255.0, 0.0, 0.0, kRenderNormal, 0.0);
	UTIL_SayText(g_iKartowka, "Внимание! Вы превратились в картошку. Скорей кого-то ударьте!");
	SetBit(g_iBitUserPotato, g_iKartowka);
	g_iDeathTimer = 15 + 1;
	set_task(1.0, "jbe_kartowka_informer", TASK_DEATH_TIMER, _, _, "a", g_iDeathTimer);
	set_pev(g_iKartowka, pev_maxspeed, 1500.0);
}

/*===== <- Игровой процесс <- =====*///}

/*===== -> Стоки -> =====*///{
stock fm_give_item(id, const szItem[])
{
	new iEntity = engfunc(EngFunc_CreateNamedEntity, engfunc(EngFunc_AllocString, szItem));
	if(!pev_valid(iEntity)) return 0;
	new Float:fOrigin[3];
	pev(id, pev_origin, fOrigin);
	set_pev(iEntity, pev_origin, fOrigin);
	set_pev(iEntity, pev_spawnflags, pev(iEntity, pev_spawnflags) | SF_NORESPAWN);
	dllfunc(DLLFunc_Spawn, iEntity);
	new iSolid = pev(iEntity, pev_solid);
	dllfunc(DLLFunc_Touch, iEntity, id);
	if(pev(iEntity, pev_solid) == iSolid)
	{
		engfunc(EngFunc_RemoveEntity, iEntity);
		return -1;
	}
	return iEntity;
}

stock fm_strip_user_weapons(id, iType = 0)
{
	new iEntity;
	static iszWeaponStrip = 0;
	if(iszWeaponStrip || (iszWeaponStrip = engfunc(EngFunc_AllocString, "player_weaponstrip"))) iEntity = engfunc(EngFunc_CreateNamedEntity, iszWeaponStrip);
	if(!pev_valid(iEntity)) return 0;
	if(iType && get_user_weapon(id) != CSW_KNIFE)
	{
		engclient_cmd(id, "weapon_knife");
		message_begin(MSG_ONE_UNRELIABLE, MsgId_CurWeapon, _, id);
		write_byte(1);
		write_byte(CSW_KNIFE);
		write_byte(0);
		message_end();
	}
	dllfunc(DLLFunc_Spawn, iEntity);
	dllfunc(DLLFunc_Use, iEntity, id);
	engfunc(EngFunc_RemoveEntity, iEntity);
	return 1;
}

stock fm_set_user_rendering(id, iRenderFx, Float:flRed, Float:flGreen, Float:flBlue, iRenderMode,  Float:flRenderAmt)
{
	new Float:fRenderColor[3];
	fRenderColor[0] = flRed;
	fRenderColor[1] = flGreen;
	fRenderColor[2] = flBlue;
	set_pev(id, pev_renderfx, iRenderFx);
	set_pev(id, pev_rendercolor, fRenderColor);
	set_pev(id, pev_rendermode, iRenderMode);
	set_pev(id, pev_renderamt, flRenderAmt);
}

stock UTIL_ScreenFade(id, iDuration, iHoldTime, iFlags, iRed, iGreen, iBlue, iAlpha, iReliable = 0)
{
	message_begin(iReliable ? MSG_ONE : MSG_ONE_UNRELIABLE, MsgId_ScreenFade, _, id);
	write_short(iDuration);
	write_short(iHoldTime);
	write_short(iFlags);
	write_byte(iRed);
	write_byte(iGreen);
	write_byte(iBlue);
	write_byte(iAlpha);
	message_end();
}

stock UTIL_SayText(id, const szMessage[], any:...)
{
	new szBuffer[190];
	if(numargs() > 2) vformat(szBuffer, charsmax(szBuffer), szMessage, 3);
	else copy(szBuffer, charsmax(szBuffer), szMessage);
	while(replace(szBuffer, charsmax(szBuffer), "!y", "^1")) {}
	while(replace(szBuffer, charsmax(szBuffer), "!t", "^3")) {}
	while(replace(szBuffer, charsmax(szBuffer), "!g", "^4")) {}
	switch(id)
	{
		case 0:
		{
			for(new i = 1; i <= MaxClients; i++)
			{
				if(!is_user_connected(i)) continue;
				message_begin(MSG_ONE_UNRELIABLE, MsgId_SayText, _, i);
				write_byte(i);
				write_string(szBuffer);
				message_end();
			}
		}
		default:
		{
			message_begin(MSG_ONE_UNRELIABLE, MsgId_SayText, _, id);
			write_byte(id);
			write_string(szBuffer);
			message_end();
		}
	}
}

public jbe_bream_cylinder()
{
	new Float:vecOrigin[3];
	pev(g_iKartowka, pev_origin, vecOrigin);
	if(pev(g_iKartowka, pev_flags) & FL_DUCKING) vecOrigin[2] -= 15.0;
	else vecOrigin[2] -= 33.0;
	CREATE_BEAMCYLINDER(vecOrigin, 150, g_pSpriteWave, _, _, 5, 3, _, 255, 0, 0, 255, _);
}

stock CREATE_BEAMCYLINDER(Float:vecOrigin[3], iRadius, pSprite, iStartFrame = 0, iFrameRate = 0, iLife, iWidth, iAmplitude = 0, iRed, iGreen, iBlue, iBrightness, iScrollSpeed = 0)
{
	engfunc(EngFunc_MessageBegin, MSG_PVS, SVC_TEMPENTITY, vecOrigin, 0);
	write_byte(TE_BEAMCYLINDER);
	engfunc(EngFunc_WriteCoord, vecOrigin[0]);
	engfunc(EngFunc_WriteCoord, vecOrigin[1]);
	engfunc(EngFunc_WriteCoord, vecOrigin[2]);
	engfunc(EngFunc_WriteCoord, vecOrigin[0]);
	engfunc(EngFunc_WriteCoord, vecOrigin[1]);
	engfunc(EngFunc_WriteCoord, vecOrigin[2] + 32.0 + iRadius * 2);
	write_short(pSprite);
	write_byte(iStartFrame);
	write_byte(iFrameRate); // 0.1's
	write_byte(iLife); // 0.1's
	write_byte(iWidth);
	write_byte(iAmplitude); // 0.01's
	write_byte(iRed);
	write_byte(iGreen);
	write_byte(iBlue);
	write_byte(iBrightness);
	write_byte(iScrollSpeed); // 0.1's
	message_end();
}
/*===== <- Стоки <- =====*///}