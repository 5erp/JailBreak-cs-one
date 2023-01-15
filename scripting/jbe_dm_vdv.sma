#include <amxmodx>
#include <fakemeta>
#include <hamsandwich>
#include "jbe_core/macroses.h"   
#include "jbe_core/jbe_natives.inc"

new const dm_name[] = "День ВДВ";
new const dm_block = 4;
new const dm_timer = 181;
new const dm_sound_file[] = "sound/csone/days_mode/vdv/ambience1.mp3"; //sound/csone/%s
new const dm_sound_name[] = "Денис Майданов – ВДВ";
new const dm_sound_time = 56;

new const model_prisioner[] = "csone_males6";
new const model_guard[] = "csone_males6";

new const body_prisoner = 4;
new const body_guard = 14;

new const sound_vdv_spitit[] = "csone/days_mode/vdv/vdv_spirit1.wav";

new const model_v_gr_knife[] = "models/CsOneRu/jb/weapons/v_vdv.mdl";
new const model_p_gr_knife[] = "models/CsOneRu/jb/weapons/p_vdv.mdl";

new g_iBitUseSpeed;
new g_iTimeHud[MAX_PLAYERS + 1 char];

#define TASK_REMOVE_SPEED	2000
#define TASK_HUD_SPEED		3000
#define TASK_DM_SPEED		4000

new g_iDayMode;

new g_pSpriteWave;
new HamHook:g_iHamHookForwards[15];
new g_iSyncInformer;

// Knockback Power values for weapons
// Note: negative values will disable knockback power for the weapon
new Float:g_iWeaponPower[] = 
{
	-1.0,	// ---
	2.4,	// P228
	-1.0,	// ---
	6.5,	// SCOUT
	-1.0,	// ---
	8.0,	// XM1014
	-1.0,	// ---
	2.3,	// MAC10
	5.0,	// AUG
	-1.0,	// ---
	2.4,	// ELITE
	2.0,	// FIVESEVEN
	2.4,	// UMP45
	5.3,	// SG550
	5.5,	// GALIL
	5.5,	// FAMAS
	2.2,	// USP
	2.0,	// GLOCK18
	10.0,	// AWP
	2.5,	// MP5NAVY
	5.2,	// M249
	8.0,	// M3
	5.0,	// M4A1
	2.4,	// TMP
	6.5,	// G3SG1
	-1.0,	// ---
	5.3,	// DEAGLE
	5.0,	// SG552
	6.0,	// AK47
	-1.0,	// ---
	2.0		// P90
}

public plugin_init() {
	register_plugin("[JBE_DM] Day VDV", "0.1b", "Serp");
	g_iDayMode = jbe_register_day_mode(
		dm_name, dm_block, dm_timer, 
		dm_sound_file, dm_sound_name, dm_sound_time
	);
	
	new g_szHamHookEntityBlock[][] = {
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
	for (i = 0; i <= 7; i++)
		DisableHamForward(g_iHamHookForwards[i] = RegisterHam(Ham_Use, g_szHamHookEntityBlock[i], "HamHook_EntityBlock", 0));
	for (i = 8; i <= 12; i++)
		DisableHamForward(g_iHamHookForwards[i] = RegisterHam(Ham_Touch, g_szHamHookEntityBlock[i], "HamHook_EntityBlock", 0));
	
//	DisableHamForward(g_iHamHookForwards[13] = RegisterHam(Ham_TraceAttack, "player", "fw_TraceAttack_Post", 1));
	DisableHamForward(g_iHamHookForwards[13] = RegisterHam(Ham_TraceAttack, "player", "fw_TraceAttack_Post", false));
	DisableHamForward(g_iHamHookForwards[14] = RegisterHam(Ham_Killed, "player", "Ham_PlayerKilled_Pre", 0));
	
	RegisterHam(Ham_Weapon_PrimaryAttack, "weapon_knife", "Ham_KnifePrimaryAttack_Post", true);
	RegisterHam(Ham_Weapon_SecondaryAttack, "weapon_knife", "Ham_KnifeSecondaryAttack_Post", true);
		
	g_iSyncInformer = CreateHudSyncObj();
	register_clcmd("drop", "ClCmd_Drop");
}

public plugin_precache()
{
	precache_generic(dm_sound_file);
	precache_sound(sound_vdv_spitit);
	precache_model(model_v_gr_knife);
	precache_model(model_p_gr_knife);
	new szBuffer[64];
	
	formatex(szBuffer, charsmax(szBuffer), "models/player/%s/%s.mdl", model_prisioner, model_prisioner);
	precache_model(szBuffer);
	
	formatex(szBuffer, charsmax(szBuffer), "models/player/%s/%s.mdl", model_guard, model_guard);
	precache_model(szBuffer);
	
	g_pSpriteWave = engfunc(EngFunc_PrecacheModel, "sprites/shockwave.spr");
}

public Ham_PlayerKilled_Pre (iVictim, iKiller)
	if (jbe_get_user_team(iVictim) == 2) {
		UTIL_ScreenFade(iVictim, 0, 0, 0, 0, 0, 0, 0, 1);
		remove_task (iVictim + TASK_HUD_SPEED);
		remove_task (iVictim + TASK_REMOVE_SPEED);
		remove_task (iVictim + TASK_DM_SPEED);
	}

public client_disconnect(id) {
//	if (jbe_get_user_team(id) == 2) {
		remove_task(id + TASK_HUD_SPEED);
		remove_task(id + TASK_REMOVE_SPEED);
		remove_task(id + TASK_DM_SPEED);
//	}
}
public ClCmd_Drop(id)
{
	if (jbe_get_user_team(id) == 2 && g_iTimeHud[id] <= 0) {
		emit_sound(id, CHAN_STATIC, sound_vdv_spitit, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		SetBit(g_iBitUseSpeed, id);
		set_pev(id, pev_maxspeed, 450.0);
	
		set_task(2.0, "jbe_dm_remove_speed", id + TASK_REMOVE_SPEED);
		jbe_dm_set_speed(id + TASK_DM_SPEED);
		set_task(0.1, "jbe_dm_set_speed", id + TASK_DM_SPEED, _, _, "b");
		
		jbe_set_user_rendering(id, kRenderFxGlowShell, 0, 0, 255, kRenderNormal, 40); //
		jbe_save_user_rendering(id);
		
		new Float:vecOrigin[3];
		pev(id, pev_origin, vecOrigin); 
		CREATE_BEAMCYLINDER(vecOrigin, 150, g_pSpriteWave, _, _, 5, 3, _, 10, 10, 250, 255, _); //
		UTIL_ScreenFade(id, 0, 0, 4, 10, 10, 200, 100, 0);
		g_iTimeHud[id] = 13;
	}
	return PLUGIN_HANDLED;
}

public jbe_dm_set_speed(id) {
#define VDV_SPEED 800
	id -= TASK_DM_SPEED;
	new Float:vecAngle[3], Float:vecBlah[3];
	pev(id, pev_v_angle, vecAngle); // угол
	engfunc(EngFunc_AngleVectors, vecAngle, vecAngle, vecBlah, vecBlah);
	
	vecAngle[0] *= VDV_SPEED;
	vecAngle[1] *= VDV_SPEED;
//	vecAngle[2] *= VDV_SPEED;
	
	set_pev(id, pev_velocity, vecAngle);
}

public jbe_dm_remove_speed(id)
{
	id -= TASK_REMOVE_SPEED;
//	g_iTimeHud[id] = 11;
	ClearBit(g_iBitUseSpeed, id);
	remove_task(id + TASK_DM_SPEED);
	jbe_set_user_rendering(id, kRenderFxGlowShell, 0, 0, 0, kRenderNormal, 0);
	jbe_save_user_rendering(id);
	UTIL_ScreenFade(id, 0, 0, 0, 0, 0, 0, 0, 1);
	ExecuteHamB(Ham_Item_PreFrame, id);
}

public hud_speed_vdv(id) {
	id -= TASK_HUD_SPEED;
	set_hudmessage(255, 0, 0, 0.01, 0.54, 0, 0.0, 0.8, 0.2, 0.2, -1);
	
	if (g_iTimeHud[id] <= 0 && IsNotSetBit(g_iBitUseSpeed, id))
		ShowSyncHudMsg(id, g_iSyncInformer,"Нажмите << G >> ^nчтобы ЗА ВДВ!^n^n^nHP: %d", get_user_health(id));
	else if (IsSetBit(g_iBitUseSpeed, id))
		ShowSyncHudMsg(id, g_iSyncInformer,"ЗА ВДВ!^n^n^n^nHP: %d", get_user_health(id));
	else if (--g_iTimeHud[id])
		ShowSyncHudMsg(id, g_iSyncInformer,"Дух ВДВ накапливается,^nосталось %d сек.^n^n^nHP: %d", g_iTimeHud[id], get_user_health(id));
}

public HamHook_EntityBlock() return HAM_SUPERCEDE;

public jbe_day_mode_start(iDayMode, iAdmin)
{
	if (iDayMode == g_iDayMode) {
		new iPnNum, iGrNum;

		for (new i = 1; i <= MaxClients; i++) {
			if (!is_user_alive(i)) continue;
			switch (jbe_get_user_team(i)) {
				case 1: {
					jbe_set_user_model(i, model_prisioner);
					set_pev(i, pev_body, body_prisoner);
					fm_give_item(i, "weapon_ak47");
					fm_set_user_bpammo(i, CSW_AK47, 500);
					iPnNum++;
				}
				case 2: {
					jbe_set_user_model(i, model_guard);
					set_pev(i, pev_body, body_guard);
					jbe_set_knife_model(i);
					fm_give_item(i, "item_assaultsuit");
					g_iTimeHud[i] = 11;
					set_task(1.0, "hud_speed_vdv", i+TASK_HUD_SPEED, _, _, "b");
					iGrNum++;
				}
			}
		}
		new Float:fHealth = iPnNum * 1000.0 / iGrNum;
		for(new i = 1; i <= MaxClients; i++) {
			if (!is_user_alive(i)) continue;
			if (get_user_team(i) == 2)
				set_pev(i, pev_health, fHealth);
		}
		for(new i; i < sizeof(g_iHamHookForwards); i++)
			EnableHamForward(g_iHamHookForwards[i]);
	}
}

public Ham_KnifePrimaryAttack_Post(iEntity)
{
	new id = get_pdata_cbase(iEntity, m_pPlayer, linux_diff_weapon);
	if (jbe_get_user_team(id) == 2)
		set_pdata_float(id, m_flNextAttack, 0.5);
}

public Ham_KnifeSecondaryAttack_Post(iEntity)
{
	new id = get_pdata_cbase(iEntity, m_pPlayer, linux_diff_weapon);
	if (jbe_get_user_team(id) == 2)
		set_pdata_float(id, m_flNextAttack, 0.9);
}

jbe_set_knife_model(pPlayer)
{
	static iszViewModel, iszWeaponModel;
	if (iszViewModel || (iszViewModel = engfunc(EngFunc_AllocString, model_v_gr_knife)))
		set_pev_string(pPlayer, pev_viewmodel2, iszViewModel);

	if (iszWeaponModel || (iszWeaponModel = engfunc(EngFunc_AllocString, model_p_gr_knife)))
		set_pev_string(pPlayer, pev_weaponmodel2, iszWeaponModel);
}

public jbe_day_mode_ended(iDayMode, iWinTeam)
{
	if (iDayMode == g_iDayMode) {
		client_cmd(0, "mp3 stop");
		new i;
		for (i = 0; i < sizeof(g_iHamHookForwards); i++)
			DisableHamForward(g_iHamHookForwards[i]);
			
		for (i = 1; i <= MaxClients; i++) {
			if (!is_user_alive(i)) continue;
			switch (jbe_get_user_team(i)) {
				case 1: {
					if (iWinTeam) fm_strip_user_weapons(i, 1);
					else ExecuteHamB(Ham_Killed, i, i, 0);
				}
				case 2: {
					fm_strip_user_weapons(i, 1);
			
					remove_task(i + TASK_HUD_SPEED);
					remove_task(i + TASK_REMOVE_SPEED);
					remove_task(i + TASK_DM_SPEED);
					
					jbe_set_user_rendering(i, kRenderFxGlowShell, 0, 0, 0, kRenderNormal, 0);
					jbe_save_user_rendering(i);
					UTIL_ScreenFade(i, 0, 0, 0, 0, 0, 0, 0, 1);
					
					ClearBit(g_iBitUseSpeed, i);
				}
			}
		}
		pause("a");
	}
}

public fw_TraceAttack_Post(victim, attacker, Float:damage, Float:direction[3], tracehandle, damage_type)
{
//	if (IsSetBit(g_iBitUseSpeed, victim) || jbe_get_user_team(victim) != 2 || !jbe_is_user_valid(victim) || victim == attacker
	if (IsSetBit(g_iBitUseSpeed, victim) || !jbe_is_user_valid(victim) || !jbe_is_user_valid(attacker) || victim == attacker || jbe_get_user_team(victim) == jbe_get_user_team(attacker)
	|| !is_user_alive(attacker) || !(damage_type & DMG_BULLET) || damage <= 0.0 || get_tr2(tracehandle, TR_pHit) != victim)
		return;
	switch (jbe_get_user_team(attacker)) {
		case 1: {
			// Get whether the victim is in a crouch state
			new ducking = pev(victim, pev_flags) & (FL_DUCKING | FL_ONGROUND) == (FL_DUCKING | FL_ONGROUND)
			// Get distance between players
			static origin1[3], origin2[3]
			get_user_origin(victim, origin1)
			get_user_origin(attacker, origin2)
			// Max distance exceeded
			if (get_distance(origin1, origin2) > 400)
				return ;
			// Get victim's velocity
			static Float:velocity[3]
			pev(victim, pev_velocity, velocity)
			// Use damage on knockback calculation
			xs_vec_mul_scalar(direction, damage, direction)
			// Get attacker's weapon id
			new attacker_weapon = get_user_weapon(attacker)
			// Use weapon power on knockback calculation
			if (g_iWeaponPower[attacker_weapon] > 0.0)
				xs_vec_mul_scalar(direction, g_iWeaponPower[attacker_weapon], direction)
				
			// Apply ducking knockback multiplier
			if (ducking)
				xs_vec_mul_scalar(direction, 2.3, direction) // если сидя то отброс // 2.0			
			// Apply zombie class knockback multiplier
			xs_vec_mul_scalar(direction, 1.7, direction) // 3.25 knockback // 1.4
			
			xs_vec_add(velocity, direction, direction) // Add up the new vector
			direction[2] = velocity[2] // Should knockback also affect vertical velocity?
			set_pev(victim, pev_velocity, direction) // Set the knockback'd victim's velocity
		}
		case 2: {
			damage *= 3.0;
			SetHamParamFloat(3, damage);
		}
	}
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

stock xs_vec_mul_scalar(const Float:vec[], Float:scalar, Float:out[])
{
	out[0] = vec[0] * scalar;
	out[1] = vec[1] * scalar;
	out[2] = vec[2] * scalar;
}

stock xs_vec_add(const Float:vec1[], const Float:vec2[], Float:out[])
{
	out[0] = vec1[0] + vec2[0];
	out[1] = vec1[1] + vec2[1];
	out[2] = vec1[2] + vec2[2];
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

stock UTIL_ScreenFade(pPlayer, iDuration, iHoldTime, iFlags, iRed, iGreen, iBlue, iAlpha, iReliable = 0)
{
	switch(pPlayer)
	{
		case 0:
		{
			message_begin(iReliable ? MSG_ALL : MSG_BROADCAST, MsgId_ScreenFade);
			write_short(iDuration);
			write_short(iHoldTime);
			write_short(iFlags);
			write_byte(iRed);
			write_byte(iGreen);
			write_byte(iBlue);
			write_byte(iAlpha);
			message_end();
		}
		default:
		{
			engfunc(EngFunc_MessageBegin, iReliable ? MSG_ONE : MSG_ONE_UNRELIABLE, MsgId_ScreenFade, {0.0, 0.0, 0.0}, pPlayer);
			write_short(iDuration);
			write_short(iHoldTime);
			write_short(iFlags);
			write_byte(iRed);
			write_byte(iGreen);
			write_byte(iBlue);
			write_byte(iAlpha);
			message_end();
		}
	}
}