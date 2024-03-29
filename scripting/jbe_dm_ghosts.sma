#include <amxmodx>
#include <engine>
#include <fakemeta>
#include <hamsandwich>
#include "jbe_core/macroses.h"   
#include "jbe_core/jbe_natives.inc"

new const dm_name[] = "Призраки";
new const dm_block = 4;
new const dm_timer = 181;
new const dm_sound_file[] = "sound/csone/days_mode/ghosts/ambience2.mp3"; //sound/csone/%s
new const dm_sound_name[] = "Охотники за привидениями – Каспер";
new const dm_sound_time = 60;

new const model_prisoner[] = "csone_males6";
new const body_prisoner = 3;

#pragma semicolon 1

/*
#define SetBit(%0,%1) ((%0) |= (1 << (%1)))
#define ClearBit(%0,%1) ((%0) &= ~(1 << (%1)))
#define IsSetBit(%0,%1) ((%0) & (1 << (%1)))
#define InvertBit(%0,%1) ((%0) ^= (1 << (%1)))
#define IsNotSetBit(%0,%1) (~(%0) & (1 << (%1)))
#define MsgId_CurWeapon 66
#define m_bloodColor 89
#define jbe_is_user_valid(%0) (%0 && %0 <= MaxClients)
native jbe_register_day_mode(szLang[32], iBlock, iTime, szSoundFile[128], szSoundName[64], iSoundTime);
native jbe_get_user_team(id);
native jbe_set_user_model(id, const szModel[]);
*/

new g_iDayModeGhosts, bool:g_bDayModeStatus,
g_iFakeMetaEmitSound, HamHook:g_iHamHookForwards[14];


public plugin_precache()
{
	engfunc(EngFunc_PrecacheModel, "models/jb_engine/days_mode/ghosts/v_ghost.mdl");
	engfunc(EngFunc_PrecacheModel, "models/jb_engine/days_mode/ghosts/p_ghost.mdl");
	engfunc(EngFunc_PrecacheModel, fmt("models/player/%s/%s.mdl", model_prisoner, model_prisoner));
	engfunc(EngFunc_PrecacheSound, "jb_engine/days_mode/ghosts/ghost_slash.wav");
	engfunc(EngFunc_PrecacheSound, "jb_engine/days_mode/ghosts/ghost_stab.wav");
	engfunc(EngFunc_PrecacheSound, "jb_engine/days_mode/ghosts/ghost_death.wav");
	engfunc(EngFunc_PrecacheSound, "jb_engine/days_mode/ghosts/ghost_pain.wav");
	engfunc(EngFunc_PrecacheSound, "jb_engine/days_mode/ghosts/ghost_hit.wav");
	engfunc(EngFunc_PrecacheGeneric, dm_sound_file);
}

public plugin_init()
{
	register_plugin("[JBE_DM] Ghosts", "1.1", "Freedo.m & Serp");
	g_iDayModeGhosts = jbe_register_day_mode(
		dm_name, dm_block, dm_timer, 
		dm_sound_file, dm_sound_name, dm_sound_time
	);
	
	new g_szHamHookEntityBlock[][] =
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
	new i;
	for(i = 0; i <= 7; i++) DisableHamForward(g_iHamHookForwards[i] = RegisterHam(Ham_Use, g_szHamHookEntityBlock[i], "HamHook_EntityBlock", 0));
	for(i = 8; i <= 12; i++) DisableHamForward(g_iHamHookForwards[i] = RegisterHam(Ham_Touch, g_szHamHookEntityBlock[i], "HamHook_EntityBlock", 0));
	DisableHamForward(g_iHamHookForwards[13] = RegisterHam(Ham_Killed, "player", "HamHook_Killed_Player_Post", 1));
	register_clcmd("drop", "ClCmd_Drop");
}

public HamHook_EntityBlock() return HAM_SUPERCEDE;
public HamHook_Killed_Player_Post(iVictim)
{
	if (jbe_get_user_team(iVictim) == 2)
		jbe_set_user_rendering(iVictim, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
}

public ClCmd_Drop()
{
	if(g_bDayModeStatus) return PLUGIN_HANDLED;
	return PLUGIN_CONTINUE;
}

public jbe_day_mode_start(iDayMode, iAdmin)
{
	if(iDayMode == g_iDayModeGhosts)
	{
		for(new i = 1; i <= MaxClients; i++)
		{
			if(!is_user_alive(i)) continue;
			switch(jbe_get_user_team(i))
			{
				case 1:
				{
					fm_strip_user_weapons(i);
					fm_give_item(i, "weapon_m249");
					fm_set_user_bpammo(i, CSW_M249, 9999);
					fm_give_item(i, "item_assaultsuit");
					set_pev(i, pev_health, 120.0);
				}
				case 2:
				{
					jbe_set_user_model(i, model_prisoner);
					set_pev(i, pev_body, body_prisoner);

					fm_strip_user_weapons(i);
					fm_give_item(i, "weapon_knife");
					static iszViewModel, iszWeaponModel;
					if(iszViewModel || (iszViewModel = engfunc(EngFunc_AllocString, "models/jb_engine/days_mode/ghosts/v_ghost.mdl"))) set_pev_string(i, pev_viewmodel2, iszViewModel);
					if(iszWeaponModel || (iszWeaponModel = engfunc(EngFunc_AllocString, "models/jb_engine/days_mode/ghosts/p_ghost.mdl"))) set_pev_string(i, pev_weaponmodel2, iszWeaponModel);
					jbe_set_user_rendering(i, kRenderFxGlowShell, 150, 150, 170, kRenderNormal, 0);
					jbe_save_user_rendering(i);
					set_pev(i, pev_movetype, MOVETYPE_NOCLIP);
					set_pev(i, pev_health, 506.0);
					set_pdata_int(i, m_bloodColor, 15);
					set_pev(i, pev_maxspeed, 320.0);
				}
			}
		}
		for(new i; i < sizeof(g_iHamHookForwards); i++) EnableHamForward(g_iHamHookForwards[i]);
		set_lights("c");
		g_iFakeMetaEmitSound = register_forward(FM_EmitSound, "FakeMeta_EmitSound", 0);
		g_bDayModeStatus = true;
	}
}

public FakeMeta_EmitSound(id, iChannel, szSample[], Float:flVolume, Float:flAttn, iFlag, iPitch)
{
	if(jbe_is_user_valid(id) && jbe_get_user_team(id) == 2)
	{
		if(szSample[8] == 'k' && szSample[9] == 'n' && szSample[10] == 'i' && szSample[11] == 'f' && szSample[12] == 'e')
		{
			switch(szSample[17])
			{
				case 'l': {} // knife_deploy1.wav
				case 'w': emit_sound(id, iChannel, "jb_engine/days_mode/ghosts/ghost_slash.wav", flVolume, flAttn, iFlag, iPitch); // knife_hitwall1.wav
				case 's': emit_sound(id, iChannel, "jb_engine/days_mode/ghosts/ghost_slash.wav", flVolume, flAttn, iFlag, iPitch); // knife_slash(1-2).wav
				case 'b': emit_sound(id, iChannel, "jb_engine/days_mode/ghosts/ghost_stab.wav", flVolume, flAttn, iFlag, iPitch); // knife_stab.wav
				default: emit_sound(id, iChannel, "jb_engine/days_mode/ghosts/ghost_hit.wav", flVolume, flAttn, iFlag, iPitch); // knife_hit(1-4).wav
			}
			return FMRES_SUPERCEDE;
		}
		if(szSample[7] == 'd' && ((szSample[8] == 'i' && szSample[9] == 'e') || (szSample[8] == 'e' && szSample[9] == 'a')))
		{
			emit_sound(id, iChannel, "jb_engine/days_mode/ghosts/ghost_death.wav", flVolume, flAttn, iFlag, iPitch);
			return FMRES_SUPERCEDE;
		}
		if(szSample[7] == 'b' && szSample[8] == 'h' && szSample[9] == 'i' && szSample[10] == 't')
		{
			emit_sound(id, iChannel, "jb_engine/days_mode/ghosts/ghost_pain.wav", flVolume, flAttn, iFlag, iPitch);
			return FMRES_SUPERCEDE;
		}
	}
	return FMRES_IGNORED;
}

public jbe_day_mode_ended(iDayMode, iWinTeam)
{
	if(iDayMode == g_iDayModeGhosts)
	{
		for(new i; i < sizeof(g_iHamHookForwards); i++) DisableHamForward(g_iHamHookForwards[i]);
		for(new i = 1; i <= MaxClients; i++)
		{
			if(is_user_alive(i))
			{
				switch(jbe_get_user_team(i))
				{
					case 1: fm_strip_user_weapons(i, 1);
					case 2:
					{
						if(iWinTeam) fm_strip_user_weapons(i, 1);
						else ExecuteHamB(Ham_Killed, i, i, 0);
						jbe_set_user_rendering(i, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
						jbe_save_user_rendering(i);
					}
				}
			}
		}
		set_lights("#OFF");
		client_cmd(0, "mp3 stop");
		unregister_forward(FM_EmitSound, g_iFakeMetaEmitSound, 0);
		g_bDayModeStatus = false;
		pause("a");
	}
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