#include <amxmodx>
#include <fakemeta>
#include <hamsandwich>
#include <engine>
#include "jbe_core/macroses.h"   
#include "jbe_core/jbe_natives.inc"

new const dm_name[] = "Галактика";
new const dm_block = 4;
new const dm_timer = 181;
new const dm_sound_file[] = "sound/csone/days_mode/galaxy/ambience2.mp3"; //sound/csone/%s
new const dm_sound_name[] = "Metalocalypse: Dethklok – The Galaxy";
new const dm_sound_time = 60;

/*
native jbe_register_day_mode(szLang[32], iBlock, iTime, szSoundFile[128], szSoundName[64], iSoundTime);
native jbe_get_user_team(id);
#define MsgId_CurWeapon 66
#define m_fHasPrimary 116
#define linux_diff_player 5
*/

new HamHook:g_iHamHookForwards[14];
new g_iDayModeTheGame;

public plugin_init()
{
	register_plugin("[JBE_DM] Galaxy", "1.0", "Serp");

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
	RegisterHam(Ham_Killed, "player", "Ham_PlayerKilled", false);	
	register_clcmd("drop", "ClCmd_Drop");
}

public plugin_precache() {
	engfunc(EngFunc_PrecacheGeneric, dm_sound_file);
}
public HamHook_EntityBlock()
	return HAM_SUPERCEDE;

public ClCmd_Drop(id)
	return PLUGIN_HANDLED;

public Ham_PlayerKilled(iVictim, iKiller)
	jbe_set_user_rendering(iVictim, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);

public jbe_day_mode_start(iDayMode, iAdmin)
{
	if (iDayMode == g_iDayModeTheGame) {
		new i;
		new iPnNum, iGrNum;
		for(i = 1; i <= MaxClients; i++) {
			if(!is_user_alive(i)) continue;
			
			fm_strip_user_weapons(i);
			fm_give_item(i, "weapon_scout");
			fm_set_user_bpammo(i, CSW_SCOUT, 256);
			set_pev(i, pev_gravity, 0.2);
			set_pev(i, pev_maxspeed, 350.0);
			
			switch (jbe_get_user_team(i)) {
				case 1:
				{
					iPnNum++;
					jbe_set_user_rendering(i, kRenderFxGlowShell, 255, random(100), random(100), kRenderTransColor, 50);
					jbe_save_user_rendering(i);
				}
				case 2:
				{
					iGrNum++; // R G B
					jbe_set_user_rendering(i, kRenderFxGlowShell, random(100), random(100), 255, kRenderTransColor, 50);
					jbe_save_user_rendering(i);
				}
			}
		}
		new Float:fHealth = iPnNum * 150.0 / iGrNum;
		for (i = 1; i <= MaxClients; i++) {
			if (!is_user_alive(i) || jbe_get_user_team(i) != 2) continue;
			set_pev(i, pev_health, fHealth);
			fm_give_item(i, "item_assaultsuit");
		}
		for(i = 0; i < sizeof(g_iHamHookForwards); i++)
			EnableHamForward(g_iHamHookForwards[i]);
			
		set_lights("b");
	}
}

public jbe_day_mode_ended(iDayMode, iWinTeam) {
	if(iDayMode == g_iDayModeTheGame) {
		client_cmd(0, "mp3 stop");
		new i;
		for(i = 0; i < sizeof(g_iHamHookForwards); i++) DisableHamForward(g_iHamHookForwards[i]);
		for(i = 1; i <= MaxClients; i++) {
			if(is_user_alive(i) ) {
				jbe_set_user_rendering(i, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
				jbe_save_user_rendering(i);
				switch (jbe_get_user_team(i)) {
					case 1: {
						if(!iWinTeam) ExecuteHamB(Ham_Killed, i, i, 0);
						else fm_strip_user_weapons(i, 1);
					}
					case 2: fm_strip_user_weapons(i, 1);
				}
			}
		}
		set_lights("#OFF");
		pause("a");
	}
}

stock fm_give_item(pPlayer, const szItem[])
{
	new iEntity = engfunc(EngFunc_CreateNamedEntity, engfunc(EngFunc_AllocString, szItem));
	if(!pev_valid(iEntity)) return 0;
	new Float:vecOrigin[3];
	pev(pPlayer, pev_origin, vecOrigin);
	set_pev(iEntity, pev_origin, vecOrigin);
	set_pev(iEntity, pev_spawnflags, pev(iEntity, pev_spawnflags) | SF_NORESPAWN);
	dllfunc(DLLFunc_Spawn, iEntity);
	dllfunc(DLLFunc_Touch, iEntity, pPlayer);
	if(pev(iEntity, pev_solid) != SOLID_NOT)
	{
		engfunc(EngFunc_RemoveEntity, iEntity);
		return -1;
	}
	return iEntity;
}

stock fm_strip_user_weapons(pPlayer, iType = 0)
{
	static iEntity, iszWeaponStrip = 0;
	if(iszWeaponStrip || (iszWeaponStrip = engfunc(EngFunc_AllocString, "player_weaponstrip"))) iEntity = engfunc(EngFunc_CreateNamedEntity, iszWeaponStrip);
	if(!pev_valid(iEntity)) return 0;
	if(iType && get_user_weapon(pPlayer) != CSW_KNIFE)
	{
		engclient_cmd(pPlayer, "weapon_knife");
		engfunc(EngFunc_MessageBegin, MSG_ONE_UNRELIABLE, MsgId_CurWeapon, {0.0, 0.0, 0.0}, pPlayer);
		write_byte(1);
		write_byte(CSW_KNIFE);
		write_byte(0);
		message_end();
	}
	dllfunc(DLLFunc_Spawn, iEntity);
	dllfunc(DLLFunc_Use, iEntity, pPlayer);
	engfunc(EngFunc_RemoveEntity, iEntity);
	set_pdata_int(pPlayer, m_fHasPrimary, 0, linux_diff_player);
	return 1;
}

stock fm_set_user_bpammo(pPlayer, iWeaponId, iAmount)
{
	new iOffset;
	switch(iWeaponId)
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
	set_pdata_int(pPlayer, iOffset, iAmount, linux_diff_player);
}

