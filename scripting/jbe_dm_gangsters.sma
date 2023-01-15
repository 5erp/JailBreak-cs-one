#include <amxmodx>
#include <fakemeta>
#include <hamsandwich>
#include <reapi>

#include "jbe_core/macroses.h"   
#include "jbe_core/jbe_natives.inc"


new const dm_name[] = "Гангстеры";
new const dm_block = 4;
new const dm_timer = 181;
new const dm_sound_file[] = "sound/csone/days_mode/gangsters/ambience5.mp3"; //sound/csone/%s
new const dm_sound_name[] = "LITTLE BIG feat. TOMMY CASH - GIVE ME YOUR MONEY";
new const dm_sound_time = 63;

new const model_prisoner[] = "csone_males6";
new const model_guard[] = "csone_males6";

new const body_prisoner = 12;
new const body_guard = 9;

#define TIME_IN_FORZE	20	// заморозка зеков в старте раунда
#define TASK_HEALTH_CHECK	235423
#define TASK_TIME_HIDE 2323423

new g_iDayModeTheGame, 
	g_iSyncTimeHide, 
	g_iTimeHideCount, 
	HamHook:g_iHamHookForwards[15];

new g_iPlayers[32];
new g_iNum;
new g_iGrNum;

public plugin_precache()
{
	engfunc(EngFunc_PrecacheGeneric, dm_sound_file);
	new szBuffer[64];
	formatex(szBuffer, charsmax(szBuffer), "models/player/%s/%s.mdl", model_prisoner, model_prisoner);
	precache_model(szBuffer);
//	formatex(szBuffer, charsmax(szBuffer), "models/player/%s/%s.mdl", model_guard, model_guard);
//	precache_model(szBuffer);

	precache_model(fmt("models/player/%s/%s.mdl", model_guard, model_guard));
}

public plugin_init()
{
	register_plugin("[JBE_DM] Gangsters", "1.3", "Serp");
	g_iDayModeTheGame = jbe_register_day_mode(
		dm_name, dm_block, dm_timer, 
		dm_sound_file, dm_sound_name, dm_sound_time
	);
	
	new g_szHamHookEntityBlock[][] = {
		"func_vehicle",			// Управляемая машина
		"func_tracktrain",		// Управляемый поезд
		"func_tank",			// Управляемая пушка
		"game_player_hurt",		// При активации наносит игроку повреждения
		"func_recharge",		// Увеличение запаса бронижелета
		"func_healthcharger",	// Увеличение процентов здоровья
		"game_player_equip",	// Выдаёт оружие
		"player_weaponstrip",	// Забирает всё оружие
		"func_button",			// Кнопка
		"trigger_hurt",			// Наносит игроку повреждения
		"trigger_gravity",		// Устанавливает игроку силу гравитации
		"armoury_entity",		// Объект лежащий на карте, оружия, броня или гранаты
		"weaponbox",			// Оружие выброшенное игроком
		"weapon_shield"			// Щит
	};
	new i;
	for(i = 0; i <= 7; i++) DisableHamForward(g_iHamHookForwards[i] = RegisterHam(Ham_Use, g_szHamHookEntityBlock[i], "HamHook_EntityBlock", 0));
	for(i = 8; i <= 13; i++) DisableHamForward(g_iHamHookForwards[i] = RegisterHam(Ham_Touch, g_szHamHookEntityBlock[i], "HamHook_EntityBlock", 0));
	DisableHamForward(g_iHamHookForwards[14] = RegisterHam(Ham_Killed, "player", "Ham_PlayerKilled_Post", 1));

	g_iSyncTimeHide = CreateHudSyncObj();
	register_clcmd("drop", "ClCmd_Drop");
	register_event("CurWeapon", "Event_DrawTracer", "be", "1=1", "3>0");
}

public HamHook_EntityBlock() return HAM_SUPERCEDE;

public Ham_PlayerKilled_Post(iVictim, iKiller)
{
	if (jbe_is_user_valid(iVictim) && jbe_get_user_team(iVictim) == TEAM_GUARD)
	{
		remove_task(iVictim + TASK_HEALTH_CHECK);
		g_iGrNum--;
	}
}

public ClCmd_Drop(id) {
//	if(iDayMode == g_iDayModeTheGame)
		return PLUGIN_HANDLED;
}

public jbe_day_mode_start(iDayMode, iAdmin)
{
	if (iDayMode == g_iDayModeTheGame)
	{
		new iGrNum, iPnNum;

		for (new i = 1; i <= MaxClients; i++)
		{
			if (!is_user_alive(i)) continue;
			fm_strip_user_weapons(i);
			switch (jbe_get_user_team(i))
			{
				case 1:
				{
					set_pev(i, pev_flags, pev(i, pev_flags) | FL_FROZEN);
					set_pev(i, pev_takedamage, DAMAGE_NO);
					UTIL_ScreenFade(i, 0, 0, 4, 0, 0, 0, 255, 1);
					jbe_set_user_model(i, model_prisoner);
					set_pev(i, pev_body, body_prisoner);
					iPnNum++;
				}
				case 2:
				{
					jbe_set_user_model(i, model_guard);
					set_pev(i, pev_body, body_guard);
					rg_give_item(i, "weapon_elite");
					rg_give_item(i, "item_kevlar");
					fm_set_user_bpammo(i, CSW_ELITE, 250);
				//	set_pev(i, pev_health, 250.0);
					g_iPlayers[g_iNum++] = i;
					iGrNum++;
					client_print_color(i, i, "^4[Гангстеры] ^1Запрещено поджидать заключенных возле клеток.");
				}
			}
		}
		g_iGrNum = iGrNum; // +1
		new Float:fGrHealth = ((iPnNum * 50.0) / iGrNum);
		fGrHealth += 1.0
		//new ;
		//fGrHealth += iGrHealth;
		for(new i = 0; i < g_iNum; i++) {
			if (!is_user_alive(g_iPlayers[i])) continue;
			set_pev(g_iPlayers[i], pev_health, fGrHealth);
		}
		g_iTimeHideCount = TIME_IN_FORZE + 1;
		jbe_time_hide();
		set_task(1.0, "jbe_time_hide", TASK_TIME_HIDE, _, _, "a", g_iTimeHideCount);
		for(new i; i < sizeof(g_iHamHookForwards); i++)
			EnableHamForward(g_iHamHookForwards[i]);
	}
}

public jbe_time_hide()
{
	if (--g_iTimeHideCount)
	{
		set_hudmessage(102, 69, 0, -1.0, 0.16, 0, 0.0, 0.9, 0.1, 0.1, -1);
		ShowSyncHudMsg(0, g_iSyncTimeHide, "У белых %d секунд^nчтобы занять свой район!", g_iTimeHideCount);
		check_watch_spawn_guards();
	}
	else
	{
		check_watch_spawn_guards_kill();

		for(new i = 1; i <= MaxClients; i++)
		{
			if (!is_user_alive(i)) continue;
			switch(jbe_get_user_team(i))
			{
				case 1:
				{
					UTIL_ScreenFade(i, 0, 0, 0, 0, 0, 0, 0, 1);
					set_pev(i, pev_flags, pev(i, pev_flags) & ~FL_FROZEN);
					set_pev(i, pev_takedamage, DAMAGE_YES);
					fm_give_item(i, "weapon_elite");
					fm_set_user_bpammo(i, CSW_ELITE, 250);
				}
				case 2: set_task(1.0, "jbe_dm_regeneration_health", i+TASK_HEALTH_CHECK, _, _, "b");
			}
		}
	}
}

check_watch_spawn_guards_kill()
{
	new Float:vecOrigin[3];
	new iTarget;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (!is_user_alive(i) || jbe_get_user_team(i) != 1) continue;

		pev(i, pev_origin, vecOrigin);
		while ((iTarget = engfunc(EngFunc_FindEntityInSphere, iTarget, vecOrigin, 500.0)))
		{
			if (!pev_valid(iTarget) || !jbe_is_user_valid(iTarget)
			|| !is_user_alive(iTarget) || jbe_get_user_team(iTarget) != 2)
				continue;

			user_kill(iTarget);
			client_print_color(iTarget, iTarget, "^4[Гангстеры] ^1Вы убиты за поджидание заключенных возле клеток.");
		}
	}
}

check_watch_spawn_guards()
{
	new Float:vecOrigin[3];
	new iTarget;
	new bool:bIsCheck;
	for (new i = 1; i <= MaxClients; i++)
	{
		if (!is_user_alive(i) || jbe_get_user_team(i) != TEAM_GUARD) continue;
		bIsCheck = false;

		pev(i, pev_origin, vecOrigin);
		while (!bIsCheck && (iTarget = engfunc(EngFunc_FindEntityInSphere, iTarget, vecOrigin, 550.0)))
		{
			if (!jbe_is_user_valid(iTarget) || !pev_valid(iTarget)
			|| !is_user_alive(iTarget) || jbe_get_user_team(iTarget) != TEAM_PRISONER)
				continue;

			bIsCheck = true;
		}
		if (bIsCheck)
			client_print_color(i, i, "^4[Гангстеры] ^1Уйдите дальше от заключенных или будете убиты!");
	}
}

public jbe_dm_regeneration_health(id)
{
	id -= TASK_HEALTH_CHECK;
	new Float:fOrigin[3], Float:fOrigin2[3], Float:fDistance;
	pev(id, pev_origin, fOrigin);
	for(new i; i < g_iNum; i++)
	{
		if (id == g_iPlayers[i] || !is_user_alive(g_iPlayers[i])) continue;
		pev(g_iPlayers[i], pev_origin, fOrigin2);
		fDistance = get_distance_f(fOrigin, fOrigin2);

		if (fDistance < 500.0)
			return PLUGIN_HANDLED;
	}

	if (!fDistance)	return PLUGIN_HANDLED;

	if (g_iGrNum > 1)
	{
		new iHealth = get_user_health(id);
		set_hudmessage(255, 0, 0, 0.01, 0.54, 0, 0.0, 0.8, 0.2, 0.2, -1);
		show_hudmessage(id, "Вы отбились от банды ^nи начинаете терять жизни!");
		if (iHealth > 1) set_pev(id, pev_health, iHealth - 1.0);
		else ExecuteHamB(Ham_Killed, id, id, 0);
	}
	else remove_task(id+TASK_HEALTH_CHECK);
	
	return PLUGIN_HANDLED;

//===
/*
	id -= TASK_HEALTH_CHECK;
	if (g_iGrNum <= 1) {
		remove_task(id + TASK_HEALTH_CHECK);
		return;
	}
	
	new Float:fOrigin[3], Float:fOrigin2[3], Float:fDistance;
	pev(id, pev_origin, fOrigin);
	
	for(new i; i < g_iNum; i++) {
		if (id == g_iPlayers[i] || !is_user_alive(g_iPlayers[i])) continue;
		pev(g_iPlayers[i], pev_origin, fOrigin2);
		fDistance = get_distance_f(fOrigin, fOrigin2);
		
		if (fDistance < 500.0) {
			break;
			return;
		}
	}
	new iHealth = get_user_health(id);
	set_hudmessage(255, 0, 0, 0.01, 0.54, 0, 0.0, 0.8, 0.2, 0.2, -1);
	show_hudmessage(id, "Вы отбились от банды ^nи начинаете терять жизни!");
	if (iHealth > 1)
		set_pev(id, pev_health, iHealth - 1.0);
	else
		ExecuteHamB(Ham_Killed, id, id, 0);
		*/
}

public jbe_day_mode_ended(iDayMode, iWinTeam)
{
	if (iDayMode == g_iDayModeTheGame)
	{
		for (new i = 1; i <= MaxClients; i++)
		{
			if (!is_user_alive(i)) continue;
			switch (jbe_get_user_team(i))
			{
				case 1:
				{
					if(iWinTeam) fm_strip_user_weapons(i, 1);
					else ExecuteHamB(Ham_Killed, i, i, 0);
				}
				case 2: fm_strip_user_weapons(i, 1);
			}
		}
		client_cmd(0, "mp3 stop");
		new i;
		for(i = 0; i < sizeof(g_iHamHookForwards); i++) DisableHamForward(g_iHamHookForwards[i]);
		for(i = 0; i < g_iNum; i++) remove_task(g_iPlayers[i]+TASK_HEALTH_CHECK);
		remove_task(TASK_TIME_HIDE);
		pause("a");
	}
}

public Event_DrawTracer(id) {
	new iVec1[3], iVec2[3];
	get_user_origin(id,iVec1,1);
	get_user_origin(id,iVec2,4);
	UTIL_Tracer(iVec1,iVec2);
}

stock UTIL_Tracer(iVec1[3],iVec2[3])
{
#define TE_UTIL_Tracer 6
	message_begin(MSG_PAS, SVC_TEMPENTITY, iVec1);	
	write_byte(TE_UTIL_Tracer) /* TE_UTIL_Tracer - see commo/const.h in HLSDK */
	write_coord(iVec1[0]);
	write_coord(iVec1[1]);
	write_coord(iVec1[2]);
	write_coord(iVec2[0]);
	write_coord(iVec2[1]);
	write_coord(iVec2[2]);
	message_end();
}

stock UTIL_ScreenFade(id, iDuration, iHoldTime, iFlags, iRed, iGreen, iBlue, iAlpha, iReliable = 0)
{
#define MsgId_ScreenFade 98
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

stock fm_strip_user_weapons(id, iType = 0)
{
	new iEntity; static iszWeaponStrip;
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

stock fm_set_user_bpammo(id, iWeapon, iAmount)
{
	new iOffset;
	switch(iWeapon)
	{
		case CSW_AWP: iOffset = 377; // ammo_338magnum
		case CSW_SCOUT, CSW_AK47, CSW_G3SG1: iOffset = 378; // ammo_762nato
		case CSW_M249: iOffset = 379; // ammo_556natobox
		case CSW_FAMAS, CSW_M4A1, CSW_AUG, CSW_SG550, CSW_GALI, CSW_SG552: iOffset = 380; // ammo_556nato
		case CSW_M3, CSW_XM1014: iOffset = 381; // ammo_buckshot
		case CSW_USP, CSW_UMP45, CSW_MAC10: iOffset = 382; // ammo_45acp
		case CSW_FIVESEVEN, CSW_P90: iOffset = 383; // ammo_57mm
		case CSW_DEAGLE: iOffset = 384; // ammo_50ae
		case CSW_P228: iOffset = 385; // ammo_357sig
		case CSW_GLOCK18, CSW_MP5NAVY, CSW_TMP, CSW_ELITE: iOffset = 386; // ammo_9mm
		case CSW_FLASHBANG: iOffset = 387;
		case CSW_HEGRENADE: iOffset = 388;
		case CSW_SMOKEGRENADE: iOffset = 389;
		case CSW_C4: iOffset = 390;
		default: return;
	}
	set_pdata_int(id, iOffset, iAmount);
}

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
