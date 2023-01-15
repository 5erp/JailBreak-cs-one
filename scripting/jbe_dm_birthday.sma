#include <amxmodx>
#include <fakemeta>
#include <hamsandwich>
#include <xs>
#include "jbe_core/macroses.h"   
#include "jbe_core/jbe_natives.inc"

new const dm_name[] = "День рождения";
new const dm_block = 4;
new const dm_timer = 181;
new const dm_sound_file[] = "sound/csone/days_mode/birthday/ambience2.mp3"; //sound/csone/%s
new const dm_sound_name[] = "КЛУБНЯЧЕК [vol.1] – #1";
new const dm_sound_time = 60;

#define TIME_IN_FORZE 10 // заморозка кт при старте
#define TASK_TIME_HIDE 234543
#define TASK_CHECK_CAMPER 6543

#pragma semicolon 1

new g_iTimeHideCount, g_iSyncTimeHide;
new g_iDayModeBirthday, g_pCakeIndex, g_pDecalIndex[4],
g_iFakeMetaSetModel, HamHook:g_iHamHookForwards[13];

public plugin_precache()
{
	engfunc(EngFunc_PrecacheModel, "models/jb_engine/days_mode/birthday/v_cake.mdl");
	engfunc(EngFunc_PrecacheModel, "models/jb_engine/days_mode/birthday/p_cake.mdl");
	engfunc(EngFunc_PrecacheModel, "models/jb_engine/days_mode/birthday/w_cake.mdl");
	g_pCakeIndex = engfunc(EngFunc_PrecacheModel, "sprites/jb_engine/cake_explosion.spr");
	engfunc(EngFunc_PrecacheSound, "jb_engine/days_mode/birthday/cake_explosion.wav");
	engfunc(EngFunc_PrecacheGeneric, dm_sound_file);
	engfunc(EngFunc_PrecacheGeneric, "sprites/jb_engine/wpn_cake.spr");
	engfunc(EngFunc_PrecacheGeneric, "sprites/jbe_dm_wpn_cake.txt");
	g_pDecalIndex[0] = engfunc(EngFunc_DecalIndex,"{blood1");
	g_pDecalIndex[1] = engfunc(EngFunc_DecalIndex,"{blood2");
	g_pDecalIndex[2] = engfunc(EngFunc_DecalIndex,"{blood3");
	g_pDecalIndex[3] = engfunc(EngFunc_DecalIndex,"{blood4");
}

public plugin_init()
{
	register_plugin("[JBE_DM] Birth Day", "1.3", "Freedo.m & Serp");

	g_iDayModeBirthday = jbe_register_day_mode(
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
		"weaponbox" // Оружие выброшенное игроком
	};
	new i;
	for(i = 0; i <= 7; i++) DisableHamForward(g_iHamHookForwards[i] = RegisterHam(Ham_Use, g_szHamHookEntityBlock[i], "HamHook_EntityBlock", 0));
	for(i = 8; i < 12; i++) DisableHamForward(g_iHamHookForwards[i] = RegisterHam(Ham_Touch, g_szHamHookEntityBlock[i], "HamHook_EntityBlock", 0));
	DisableHamForward(g_iHamHookForwards[12] = RegisterHam(Ham_Touch, "grenade", "HamHook_Touch_Grenade_Post", 1));
	register_clcmd("jbe_dm_wpn_cake", "ClCmd_WpnCake");
	g_iSyncTimeHide = CreateHudSyncObj();
}

public HamHook_EntityBlock() return HAM_SUPERCEDE;

public HamHook_Touch_Grenade_Post(iTouched, iToucher)	// Грена, игрок
{
	if (!(pev_valid(iTouched) == 2)) return;
	new Float:vecOrigin[3];
	pev(iTouched, pev_origin, vecOrigin);
	if (pev_valid(iToucher) == 2 && is_user_alive(iToucher))
	{
		new iOwner = pev(iTouched, pev_owner);
		if(jbe_is_user_valid(iToucher))
		{
			if (jbe_get_user_team(iToucher) == 1)
				ExecuteHamB(Ham_TakeDamage, iToucher, iOwner, iOwner, 50.0, DMG_SONIC);
			UTIL_ScreenFade(iToucher, (1<<12), (1<<12), 0, 24, 10, 10, 250, 0);
		}
		else ExecuteHamB(Ham_TakeDamage, iToucher, iOwner, iOwner, 50.0, DMG_SONIC);
	}
	else
		CREATE_WORLDDECAL(vecOrigin, g_pDecalIndex[random_num(0, 3)]);
	
	CREATE_SPRITE(vecOrigin, g_pCakeIndex, 15, 255);
	emit_sound(iTouched, CHAN_AUTO, "jb_engine/days_mode/birthday/cake_explosion.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	set_pev(iTouched, pev_flags, pev(iTouched, pev_flags) | FL_KILLME);
}

public ClCmd_WpnCake(id)
{
	engclient_cmd(id, "weapon_smokegrenade");
	return PLUGIN_HANDLED;
}

public jbe_day_mode_start(iDayMode, iAdmin)
{
	if(iDayMode == g_iDayModeBirthday)
	{
		for(new i = 1; i <= MaxClients; i++)
		{
			if(!is_user_alive(i)) continue;
			switch(jbe_get_user_team(i))
			{
				case 1:
				{
					fm_strip_user_weapons(i, 1);
					set_pev(i, pev_gravity, 0.5);
				}
				case 2:
				{
					UTIL_ScreenFade(i, 0, 0, 4, 0, 0, 0, 255, 1);
					set_pev(i, pev_flags, pev(i, pev_flags) | FL_FROZEN);
					fm_strip_user_weapons(i);
					fm_give_item(i, "weapon_smokegrenade");
					fm_set_user_bpammo(i, CSW_SMOKEGRENADE, 200);
				
					message_begin(MSG_ONE, MsgId_WeaponList, _, i);
					write_string("jbe_dm_wpn_cake");
					write_byte(13);
					write_byte(1);
					write_byte(-1);
					write_byte(-1);
					write_byte(3);
					write_byte(3);
					write_byte(9);
					write_byte(24);
					message_end();
				
					static iszViewModel, iszWeaponModel;
					if(iszViewModel || (iszViewModel = engfunc(EngFunc_AllocString, "models/jb_engine/days_mode/birthday/v_cake.mdl"))) 
						set_pev_string(i, pev_viewmodel2, iszViewModel);
					if(iszWeaponModel || (iszWeaponModel = engfunc(EngFunc_AllocString, "models/jb_engine/days_mode/birthday/p_cake.mdl"))) 
						set_pev_string(i, pev_weaponmodel2, iszWeaponModel);
				}
			}
		}

		g_iTimeHideCount = TIME_IN_FORZE+1;
		jbe_time_hide();
		set_task(1.0, "jbe_time_hide", TASK_TIME_HIDE, _, _, "a", g_iTimeHideCount);
		set_task(5.0, "jbe_check_user_camper_task", TASK_CHECK_CAMPER, _, _, "b");
		for(new i; i < sizeof(g_iHamHookForwards); i++) EnableHamForward(g_iHamHookForwards[i]);
		g_iFakeMetaSetModel = register_forward(FM_SetModel, "FakeMeta_SetModel_Post", 1);
	}
}

public jbe_time_hide() {
	if(--g_iTimeHideCount) {
		set_hudmessage(102, 69, 0, -1.0, 0.16, 0, 0.0, 0.8, 0.2, 0.2, -1);
		ShowSyncHudMsg(0, g_iSyncTimeHide, "У заключённых %d секунд^nчтобы разбежаться!", g_iTimeHideCount);
	} else {
		for(new i = 1; i <= MaxClients; i++) {
			if(!is_user_alive(i) || jbe_get_user_team(i) != 2) continue;
			UTIL_ScreenFade(i, 0, 0, 0, 0, 0, 0, 0, 1);
			set_pev(i, pev_flags, pev(i, pev_flags) & ~FL_FROZEN);
		}
	}
}

public jbe_check_user_camper_task()
{
	for (new i = 1; i <= MaxClients; i++)
	{
		if (!is_user_alive(i) || jbe_get_user_team(i) == TEAM_GUARD)
			continue;

		jbe_check_user_camper(i);
		check_user_inwater(i);
	}
}

#define SLAP_HEALTH	5
#define CAMPER_WARN 2
jbe_check_user_camper(id)
{
	new vecOrigin[3];
	static oldOrigin[MAX_PLAYERS + 1][3];
	static iWarn[MAX_PLAYERS + 1];

	get_user_origin(id, vecOrigin);

	if (get_distance(vecOrigin, oldOrigin[id]) < 100)
	{
		if (iWarn[id] > CAMPER_WARN || ++iWarn[id] > CAMPER_WARN)
		{
			new Float:fHealth;
			pev(id, pev_health, fHealth);

			if (fHealth > SLAP_HEALTH.0)
			{
				UTIL_ScreenShake(id, 999999, 9999, 999, 1);
				UTIL_ScreenFade(id, 9999, 100, SF_FADE_MODULATE, 50, 0, 0, 200, 1);
				UTIL_DamagePlayer(id, 0, SLAP_HEALTH, DMG_CRUSH, vecOrigin, 1);
				CREATE_BLOODSTREAM(vecOrigin, 70, random_num(50, 250));
				set_pev(id, pev_health, fHealth - SLAP_HEALTH.0);
				client_print_color(id, id, "^4[День рождения] ^1Меняйте позицию или будете убиты.");
			}
			else
			{
				user_kill(id);
				client_print_color(id, id, "^4[День рождения] ^1Вы убиты за то что не меняли позицию.");
			}
		}
	}
	else iWarn[id] = 0;

	oldOrigin[id] = vecOrigin;
}

check_user_inwater(id)
{
	if (pev(id, pev_flags) & FL_INWATER)
	{
		new Float:fHealth;
		pev(id, pev_health, fHealth);
		if (fHealth > SLAP_HEALTH.0)
		{
			new vecOrigin[3];
			get_user_origin(id, vecOrigin);
			UTIL_ScreenShake(id, 999999, 9999, 999, 1);
			UTIL_ScreenFade(id, 9999, 100, SF_FADE_MODULATE, 50, 0, 0, 200, 1);
			UTIL_DamagePlayer(id, 0, SLAP_HEALTH, DMG_CRUSH, vecOrigin, 1);
			CREATE_BLOODSTREAM(vecOrigin, 70, random_num(50, 250));
			set_pev(id, pev_health, fHealth - SLAP_HEALTH.0);
			client_print_color(id, id, "^4[День рождения] ^1Покиньте воду или будете убиты");
		}
		else
		{
			user_kill(id);
			client_print_color(id, id, "^4[День рождения] ^1Вы убиты за нахождение в воде");
		}
	}
}

stock UTIL_ScreenShake(pPlayer, iAmplitude, iDuration, iFrequency, iReliable = 0)
{
	engfunc(EngFunc_MessageBegin, iReliable ? MSG_ONE : MSG_ONE_UNRELIABLE, MsgId_ScreenShake, {0.0, 0.0, 0.0}, pPlayer);
	write_short(iAmplitude);
	write_short(iDuration);
	write_short(iFrequency);
	message_end();
}

stock UTIL_DamagePlayer(pPlayer, iArmor, iHealth, iType, vecVector[3], iReliable = 0)
{
	message_begin(iReliable ? MSG_ONE : MSG_ONE_UNRELIABLE, MsgId_Damage, _, pPlayer);
	write_byte(iArmor); //damage armor
	write_byte(iHealth); //damage health
	write_long(iType); //damage type
	write_coord(vecVector[0]); //origin[x]
	write_coord(vecVector[1]); //origin[y]
	write_coord(vecVector[2]); //origin[z]
	message_end();
}

stock CREATE_BLOODSTREAM(vecVector[3], iColIndex, iSpeed)
{
	message_begin(MSG_BROADCAST, SVC_TEMPENTITY);
	write_byte(TE_BLOODSTREAM);
	write_coord(vecVector[0]); //pos.x
	write_coord(vecVector[1]); //pos.y
	write_coord(vecVector[2] + 15); //pos.z
	write_coord(random_num(0, 255)); //vec.x
	write_coord(random_num(0, 255)); //vec.y
	write_coord(random_num(0, 255)); //vec.z
	write_byte(iColIndex); //col index
	write_byte(iSpeed); //speed
	message_end();
}

public FakeMeta_SetModel_Post(iEntity, const szModel[])
{
	if (szModel[7] == 'w' && szModel[8] == '_' && szModel[9] == 's' && szModel[10] == 'm')
	{
		engfunc(EngFunc_SetModel, iEntity, "models/jb_engine/days_mode/birthday/w_cake.mdl");
		new Float:vecVelocity[3];
		pev(iEntity, pev_velocity, vecVelocity);
		xs_vec_mul_scalar(vecVelocity, 1.5, vecVelocity);
		set_pev(iEntity, pev_velocity, vecVelocity);
		engfunc(EngFunc_SetSize, iEntity, Float:{-5.0, -5.0, -5.0}, Float:{5.0, 5.0, 5.0});
	}
}

public jbe_day_mode_ended(iDayMode, iWinTeam)
{
	if(iDayMode == g_iDayModeBirthday)
	{
		client_cmd(0, "mp3 stop");
		remove_task(TASK_CHECK_CAMPER);
		remove_task(TASK_TIME_HIDE);
		new i, iEntity, iOwner;
		for(i = 0; i < sizeof(g_iHamHookForwards); i++)
			DisableHamForward(g_iHamHookForwards[i]);
		unregister_forward(FM_SetModel, g_iFakeMetaSetModel, 1);
		for(i = 1; i <= MaxClients; i++)
		{
			if(is_user_alive(i) && jbe_get_user_team(i) == 2)
			{
				if(iWinTeam) fm_strip_user_weapons(i, 1);
				else ExecuteHamB(Ham_Killed, i, i, 0);
			}
		}
		while((iEntity = engfunc(EngFunc_FindEntityByString, iEntity, "classname", "grenade")))
		{
			if(!pev_valid(iEntity)) continue;
			iOwner = pev(iEntity, pev_owner);
			if(jbe_is_user_valid(iOwner)) set_pev(iEntity, pev_flags, pev(iEntity, pev_flags) | FL_KILLME);
		}
		pause("a");
	}
}

stock CREATE_SPRITE(Float:vecOrigin[3], pSptite, iWidth, iBrightness)
{
	engfunc(EngFunc_MessageBegin, MSG_PVS, SVC_TEMPENTITY, vecOrigin, 0);
	write_byte(TE_SPRITE);
	engfunc(EngFunc_WriteCoord, vecOrigin[0]);
	engfunc(EngFunc_WriteCoord, vecOrigin[1]);
	engfunc(EngFunc_WriteCoord, vecOrigin[2]);
	write_short(pSptite);
	write_byte(iWidth);
	write_byte(iBrightness);
	message_end();
}

stock CREATE_WORLDDECAL(Float:vecOrigin[3], pDecal)
{
	message_begin(MSG_BROADCAST, SVC_TEMPENTITY);
	write_byte(TE_WORLDDECAL);
	engfunc(EngFunc_WriteCoord, vecOrigin[0]);
	engfunc(EngFunc_WriteCoord, vecOrigin[1]);
	engfunc(EngFunc_WriteCoord, vecOrigin[2]);
	write_byte(pDecal);
	message_end();
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

stock fm_give_item(id, const szItem[])
{
	new iEntity = engfunc(EngFunc_CreateNamedEntity, engfunc(EngFunc_AllocString, szItem));
	if(!pev_valid(iEntity)) return 0;
	new Float:vecOrigin[3];
	pev(id, pev_origin, vecOrigin);
	set_pev(iEntity, pev_origin, vecOrigin);
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
