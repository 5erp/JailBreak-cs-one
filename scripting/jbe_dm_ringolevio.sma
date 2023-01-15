#include <amxmodx>
#include <fakemeta>
#include <hamsandwich>
#include <xs>
#include "jbe_core/macroses.h"   
#include "jbe_core/jbe_natives.inc"

new const dm_name[] = "Чай-чай выручай";
new const dm_block = 4;
new const dm_timer = 181;
new const dm_sound_file[] = "sound/csone/days_mode/ringolevio/ambience3.mp3"; //sound/csone/%s
new const dm_sound_name[] = "Тату – Нас не догонят";
new const dm_sound_time = 76;

#define TIME_IN_FORZE 5
#define TASK_TIME_HIDE 234543
#define TASK_CHECK_CAMPER 6543

#pragma semicolon 1
#define lunux_offset_player 5

#define BREAK_GLASS 0x01
#define TASK_DEATH_TIMER 785689
#define TASK_PROTECTION_TIME 125908
#define IUSER1_DEATH_TIMER 754645

new g_iDayModeRingolevio,
	g_iBitUserGame,
	g_iBitUserFrozen,
	g_iUserTeam[MAX_PLAYERS + 1],
	g_iUserEntityTimer[MAX_PLAYERS + 1],
	Float:g_fUserDeathTimer[MAX_PLAYERS + 1],
	g_iUserLife[MAX_PLAYERS + 1],
	g_pSpriteFrost,
	g_pModelFrost,
	g_iFakeMetaAddToFullPack,
	g_iFakeMetaCheckVisibility,
	HamHook:g_iHamHookForwards[15];

new g_iTimeHideCount, g_iSyncTimeHide;
new g_iSyncLife;

public plugin_init()
{
	register_plugin("[JBE_DM] Ringolevio", "1.4a", "Freedo.m & Serp");
	g_iDayModeRingolevio = jbe_register_day_mode(
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
	DisableHamForward(g_iHamHookForwards[13] = RegisterHam(Ham_TraceAttack, "player", "Ham_TraceAttack_Pre", 0));
	DisableHamForward(g_iHamHookForwards[14] = RegisterHam(Ham_Killed, "player", "Ham_PlayerKilled_Post", 1));

	g_iSyncTimeHide = CreateHudSyncObj();
	g_iSyncLife = CreateHudSyncObj();
}

public plugin_precache()
{
	engfunc(EngFunc_PrecacheModel, "models/jb_engine/days_mode/ringolevio/p_candy_cane.mdl");
	engfunc(EngFunc_PrecacheModel, "models/jb_engine/days_mode/ringolevio/v_candy_cane.mdl");
	g_pSpriteFrost = engfunc(EngFunc_PrecacheModel, "sprites/jb_engine/frostgib.spr");
	g_pModelFrost = engfunc(EngFunc_PrecacheModel, "models/jb_engine/days_mode/ringolevio/frostgibs.mdl");
	engfunc(EngFunc_PrecacheSound, "jb_engine/days_mode/ringolevio/defrost_player.wav");
	engfunc(EngFunc_PrecacheSound, "jb_engine/days_mode/ringolevio/freeze_player.wav");
	engfunc(EngFunc_PrecacheGeneric, dm_sound_file);
	engfunc(EngFunc_PrecacheModel, "sprites/jb_engine/death_timer.spr");
}

public client_disconnect(id)
{
	if(IsSetBit(g_iBitUserFrozen, id))
	{
		ClearBit(g_iBitUserFrozen, id);
		if (pev_valid(g_iUserEntityTimer[id]))
			set_pev(g_iUserEntityTimer[id], pev_flags, pev(g_iUserEntityTimer[id], pev_flags) | FL_KILLME);
	}
	ClearBit(g_iBitUserGame, id);
}

public HamHook_EntityBlock() return HAM_SUPERCEDE;
public Ham_TraceAttack_Pre(iVictim, iAttacker, Float:fDamage, Float:vecDeriction[3], iTrace, iBitDamage)
{
	if (IsSetBit(g_iBitUserGame, iAttacker) && jbe_is_user_valid(iAttacker))
	{
		switch(jbe_get_user_team(iAttacker))
		{
			case 1: if (IsSetBit(g_iBitUserFrozen, iVictim) && jbe_get_user_team(iVictim) == 1)
				jbe_dm_user_defrost(iVictim, iAttacker);
			case 2: if (IsNotSetBit(g_iBitUserFrozen, iVictim) && jbe_get_user_team(iVictim) == 1 && !task_exists(iVictim+TASK_PROTECTION_TIME)) 
				jbe_dm_user_freeze(iVictim, iAttacker);
		}
	}
	return HAM_SUPERCEDE;
}
public Ham_PlayerKilled_Post(iVictim) {
	if(IsSetBit(g_iBitUserFrozen, iVictim)) {
		if(pev_valid(g_iUserEntityTimer[iVictim]))
			set_pev(g_iUserEntityTimer[iVictim], pev_flags, pev(g_iUserEntityTimer[iVictim], pev_flags) | FL_KILLME);
		jbe_dm_user_defrost(iVictim, 0);
	}
	jbe_set_user_rendering(iVictim, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
	ClearBit(g_iBitUserGame, iVictim);
}
jbe_dm_user_defrost(iVictim, iAttacker)
{
	remove_task(iVictim+TASK_DEATH_TIMER);
	ClearBit(g_iBitUserFrozen, iVictim);
	set_pev(iVictim, pev_flags, pev(iVictim, pev_flags) & ~FL_FROZEN);
	set_pdata_float(iVictim, m_flNextAttack, 0.0, lunux_offset_player);
	jbe_set_user_rendering(iVictim, kRenderFxGlowShell, 255, 0, 0, kRenderNormal, 0);
	jbe_save_user_rendering(iVictim);
	set_task(3.0, "jbe_dm_protection_time", iVictim+TASK_PROTECTION_TIME);
	UTIL_ScreenFade(iVictim, (1<<10), (1<<10), 0, 32, 164, 241, 200, 1);
	new Float:fOrigin[3];
	pev(iVictim, pev_origin, fOrigin);
	CREATE_BREAKMODEL(fOrigin, _, _, 10, g_pModelFrost, 10, 25, BREAK_GLASS);
	emit_sound(iVictim, CHAN_AUTO, "jb_engine/days_mode/ringolevio/defrost_player.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	if(pev_valid(g_iUserEntityTimer[iVictim])) set_pev(g_iUserEntityTimer[iVictim], pev_flags, pev(g_iUserEntityTimer[iVictim], pev_flags) | FL_KILLME);
	
	if (iAttacker)
	{
		g_iUserLife[iAttacker]++;
		show_user_game_health(iAttacker);
	}
}

public jbe_dm_protection_time(id)
{
	id -= TASK_PROTECTION_TIME;
	if (IsSetBit(g_iBitUserGame, id))
	{
		jbe_set_user_rendering(id, kRenderFxNone, 255, 0, 0, kRenderNormal, 0);
		jbe_save_user_rendering(id);
	}
}

jbe_dm_user_freeze(iVictim, iAttacker)
{
	if (--g_iUserLife[iVictim])
	{
		SetBit(g_iBitUserFrozen, iVictim);
		set_pdata_float(iVictim, m_flNextAttack, 20.0, lunux_offset_player);
		jbe_set_user_rendering(iVictim, kRenderFxGlowShell, 32, 164, 241, kRenderNormal, 0);
		jbe_save_user_rendering(iVictim);
		UTIL_ScreenFade(iVictim, 0, 0, 4, 32, 164, 241, 200);
		new Float:vecOrigin[3];
		pev(iVictim, pev_origin, vecOrigin);
		set_pev(iVictim, pev_flags, pev(iVictim, pev_flags) | FL_FROZEN);
		set_pev(iVictim, pev_origin, vecOrigin);
		vecOrigin[2] += 15.0;
		CREATE_SPRITETRAIL(vecOrigin, g_pSpriteFrost, 30, 20, 2, 20, 10);
		g_fUserDeathTimer[iVictim] = 20.0;
		jbe_dm_create_death_timer(iVictim, vecOrigin);
		emit_sound(iVictim, CHAN_AUTO, "jb_engine/days_mode/ringolevio/freeze_player.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		new iArg[1]; iArg[0] = iAttacker;
		set_task(1.0, "jbe_dm_user_death_timer", iVictim+TASK_DEATH_TIMER, iArg, sizeof(iArg), "a", 20);
		show_user_game_health(iVictim);
	}
	else ExecuteHamB(Ham_Killed, iVictim, iAttacker, 2);
}

public jbe_dm_user_death_timer(const iAttacker[], iVictim)
{
	iVictim -= TASK_DEATH_TIMER;
	
	if (IsNotSetBit(g_iBitUserFrozen, iVictim) && task_exists(iVictim+TASK_DEATH_TIMER))
	{
		remove_task(iVictim+TASK_DEATH_TIMER);
		return;
	}

	if (g_fUserDeathTimer[iVictim] -= 1.0)
	{
		check_watch_freeze(iVictim);
		return;
	}

	ClearBit(g_iBitUserFrozen, iVictim);
	set_pev(iVictim, pev_flags, pev(iVictim, pev_flags) & ~FL_FROZEN);
	jbe_set_user_rendering(iVictim, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
	jbe_save_user_rendering(iVictim);
	UTIL_ScreenFade(iVictim, (1<<10), (1<<10), 0, 32, 164, 241, 200, 1);
	ExecuteHamB(Ham_Killed, iVictim, iAttacker[0], 2);

	if (pev_valid(g_iUserEntityTimer[iVictim]))
		set_pev(g_iUserEntityTimer[iVictim], pev_flags, pev(g_iUserEntityTimer[iVictim], pev_flags) | FL_KILLME);
}

// Отталкиваем и дамажим КТ рядом с замороженным
check_watch_freeze(id)
{
	new iTarget, Float:vecPlayer[3];

	pev(id, pev_origin, vecPlayer);

	while ((iTarget = engfunc(EngFunc_FindEntityInSphere, iTarget, vecPlayer, 100.0)))
	{
		if (!jbe_is_user_valid(iTarget) || !pev_valid(iTarget)
		|| !is_user_alive(iTarget) || jbe_get_user_team(iTarget) != 2)
			continue;

		game_push_player(id, iTarget);
	}
}

game_push_player(iPlayer, iTarget)
{
	new const Float:g_fPushDistance = 200.0;
	new const Float:g_fPushDamage = 800.0;
	new const Float:g_fMinDamage = 40.0;

	if (!pev_valid(iPlayer) || !is_user_alive(iPlayer))
		return;

	new Float:vecOrigin[3];
	pev(iPlayer, pev_origin, vecOrigin);

	if (!pev_valid(iTarget) || !is_user_alive(iTarget))
		return;
	
	new Float:fOrigin[3], Float:fDistance, Float:fDamage;

	pev(iTarget, pev_origin, fOrigin);
	
	fDistance = get_distance_f(fOrigin, vecOrigin);
	fDamage = g_fPushDamage - floatmul(g_fPushDamage, floatdiv(fDistance, g_fPushDistance));
	fDamage *= EstimateDamage(vecOrigin, iTarget, 0);
	
	if (fDamage < g_fMinDamage)
		return;
	
	CreateKnockBack(iTarget, vecOrigin, fDamage, fDamage);
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

public jbe_day_mode_start(iDayMode, iAdmin)
{
	if (iDayMode == g_iDayModeRingolevio) {
		new i;
		for(i = 1; i <= MaxClients; i++) {
			if (!is_user_alive(i)) continue;
			SetBit(g_iBitUserGame, i);
			fm_strip_user_weapons(i);
			fm_give_item(i, "weapon_knife");
			set_pev(i, pev_gravity, 0.3);
			switch(jbe_get_user_team(i)) {
				case 1: {
					g_iUserTeam[i] = 1;
					set_pev(i, pev_maxspeed, 380.0);
					g_iUserLife[i] = 3;
				}
				case 2: {
					UTIL_ScreenFade(i, 0, 0, 4, 0, 0, 0, 255, 1);
					set_pev(i, pev_flags, pev(i, pev_flags) | FL_FROZEN);
					
					g_iUserTeam[i] = 2;
					
					static iszViewModel, iszWeaponModel;
					if(iszViewModel || (iszViewModel = engfunc(EngFunc_AllocString, "models/jb_engine/days_mode/ringolevio/v_candy_cane.mdl"))) set_pev_string(i, pev_viewmodel2, iszViewModel);
					if(iszWeaponModel || (iszWeaponModel = engfunc(EngFunc_AllocString, "models/jb_engine/days_mode/ringolevio/p_candy_cane.mdl"))) set_pev_string(i, pev_weaponmodel2, iszWeaponModel);
					set_pev(i, pev_maxspeed, 400.0);
				}
			}
		}
		for(i = 0; i < sizeof(g_iHamHookForwards); i++)
			EnableHamForward(g_iHamHookForwards[i]);
		
		g_iFakeMetaAddToFullPack = register_forward(FM_AddToFullPack, "FakeMeta_AddToFullPack_Post", 1);
		g_iFakeMetaCheckVisibility = register_forward(FM_CheckVisibility, "FakeMeta_CheckVisibility", 0);
		
		g_iTimeHideCount = TIME_IN_FORZE + 1;
		jbe_time_hide();
		set_task(1.0, "jbe_time_hide", TASK_TIME_HIDE, _, _, "a", g_iTimeHideCount);
		set_task(5.0, "jbe_check_user_camper_task", TASK_TIME_HIDE, _, _, "b");
	}
}

public jbe_time_hide() {
	if(--g_iTimeHideCount) {
		set_hudmessage(102, 69, 0, -1.0, 0.16, 0, 0.0, 0.8, 0.2, 0.2, -1);
		ShowSyncHudMsg(0, g_iSyncTimeHide, "У заключённых %d секунд^nчтобы разбежаться!", g_iTimeHideCount);
	} else {
		for (new i = 1; i <= MaxClients; i++) {
			if (!is_user_alive(i) || jbe_get_user_team(i) != 2) continue;
			UTIL_ScreenFade(i, 0, 0, 0, 0, 0, 0, 0, 1);
			set_pev(i, pev_flags, pev(i, pev_flags) & ~FL_FROZEN);
		}
	}
}

public jbe_check_user_camper_task()
{
	for (new i = 1; i <= MaxClients; i++)
	{
		if (!is_user_alive(i) || IsSetBit(g_iBitUserFrozen, i) || jbe_get_user_team(i) == TEAM_GUARD)
			continue;

		jbe_check_user_camper(i);

		show_user_game_health(i);
	}
}

show_user_game_health(id)
{
	set_hudmessage(250, 30, 30, 0.8, 0.8, 0, 6.0, 4.9, 0.1, 0.1, -1);
	ShowSyncHudMsg(id, g_iSyncLife, "Жизни « %d »", g_iUserLife[id]);
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
				client_print_color(id, id, "^4[Чай-чай выручай] ^1Меняйте позицию или будете убиты.");
			}
			else
			{
				user_kill(id);
				client_print_color(id, id, "^4[Чай-чай выручай] ^1Вы убиты за то что не меняли позицию.");
			}
		}
	}
	else iWarn[id] = 0;

	oldOrigin[id] = vecOrigin;
}

public FakeMeta_AddToFullPack_Post(ES_Handle, iE, iEntity, iHost, iHostFlags, iPlayer, pSet)
{
	if(!pev_valid(iEntity) || pev(iEntity, pev_iuser1) != IUSER1_DEATH_TIMER) return FMRES_IGNORED;
	if(IsNotSetBit(g_iBitUserGame, iHost) || g_iUserTeam[iHost] == 2)
	{
		static iEffects;
		if(!iEffects) iEffects = get_es(ES_Handle, ES_Effects);
		set_es(ES_Handle, ES_Effects, iEffects | EF_NODRAW);
		return FMRES_IGNORED;
	}
	new Float:vecHostOrigin[3], Float:vecEntityOrigin[3], Float:vecEndPos[3], Float:vecNormal[3];
	pev(iHost, pev_origin, vecHostOrigin);
	pev(iEntity, pev_origin, vecEntityOrigin);
	new pTr = create_tr2();
	engfunc(EngFunc_TraceLine, vecHostOrigin, vecEntityOrigin, IGNORE_MONSTERS, iEntity, pTr);
	get_tr2(pTr, TR_vecEndPos, vecEndPos);
	get_tr2(pTr, TR_vecPlaneNormal, vecNormal);
	xs_vec_mul_scalar(vecNormal, 10.0, vecNormal);
	xs_vec_add(vecEndPos, vecNormal, vecNormal);
	set_es(ES_Handle, ES_Origin, vecNormal);
	new Float:fDist, Float:fScale;
	fDist = get_distance_f(vecNormal, vecHostOrigin);
	fScale = fDist / 300.0;
	if(fScale < 0.4) fScale = 0.4;
	else if(fScale > 1.0) fScale = 1.0;
	set_es(ES_Handle, ES_Scale, fScale);
	set_es(ES_Handle, ES_Frame, g_fUserDeathTimer[pev(iEntity, pev_iuser2)]);
	free_tr2(pTr);
	return FMRES_IGNORED;
}

public FakeMeta_CheckVisibility(iEntity, pSet)
{
	if(!pev_valid(iEntity) || pev(iEntity, pev_iuser1) != IUSER1_DEATH_TIMER) return FMRES_IGNORED;
	// iEntity - виден 
	forward_return(FMV_CELL, 1);
	return FMRES_SUPERCEDE;
}

public jbe_dm_create_death_timer(id, Float:vecOrigin[3])
{
	static iszInfoTarget = 0;
	if(iszInfoTarget || (iszInfoTarget = engfunc(EngFunc_AllocString, "info_target"))) g_iUserEntityTimer[id] = engfunc(EngFunc_CreateNamedEntity, iszInfoTarget);
	if(!pev_valid(g_iUserEntityTimer[id])) return;
	vecOrigin[2] += 35.0;
	set_pev(g_iUserEntityTimer[id], pev_classname, "death_timer");
	set_pev(g_iUserEntityTimer[id], pev_origin, vecOrigin);
	set_pev(g_iUserEntityTimer[id], pev_iuser1, IUSER1_DEATH_TIMER);
	set_pev(g_iUserEntityTimer[id], pev_iuser2, id);
	engfunc(EngFunc_SetModel, g_iUserEntityTimer[id], "sprites/jb_engine/death_timer.spr");
	fm_set_user_rendering(g_iUserEntityTimer[id], kRenderFxNone, 0.0, 0.0, 0.0, kRenderTransAdd, 255.0);
	set_pev(g_iUserEntityTimer[id], pev_solid, SOLID_NOT);
	set_pev(g_iUserEntityTimer[id], pev_movetype, MOVETYPE_NONE);
}

public jbe_day_mode_ended(iDayMode, iWinTeam)
{
	if(iDayMode == g_iDayModeRingolevio)
	{
		new i;
		for(i = 0; i < sizeof(g_iHamHookForwards); i++) DisableHamForward(g_iHamHookForwards[i]);
		unregister_forward(FM_AddToFullPack, g_iFakeMetaAddToFullPack, 1);
		unregister_forward(FM_CheckVisibility, g_iFakeMetaCheckVisibility, 0);
		for(i = 1; i <= MaxClients; i++)
		{
			if(IsSetBit(g_iBitUserGame, i))
			{
				switch(jbe_get_user_team(i))
				{
					case 1:
					{
						fm_strip_user_weapons(i, 1);
						if(IsSetBit(g_iBitUserFrozen, i)) {
							jbe_dm_user_defrost(i, 0);
						//	jbe_set_user_rendering(i, kRenderFxGlowShell, 0, 0, 0, kRenderNormal, 0);
						}
						jbe_set_user_rendering(i, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
						jbe_save_user_rendering(i);
					}
					case 2:
					{
						if(iWinTeam) fm_strip_user_weapons(i, 1);
						else ExecuteHamB(Ham_Killed, i, i, 0);
					}
				}
			}
		}
		g_iBitUserGame = 0;
		g_iBitUserFrozen = 0;
		client_cmd(0, "mp3 stop");
		remove_task(TASK_CHECK_CAMPER);
		remove_task(TASK_TIME_HIDE);
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

stock CREATE_SPRITETRAIL(const Float:fOrigin[3], pSprite, iCount, iLife, iScale, iVelocityAlongVector, iRandomVelocity)
{
	message_begin(MSG_BROADCAST, SVC_TEMPENTITY);
	write_byte(TE_SPRITETRAIL);
	engfunc(EngFunc_WriteCoord, fOrigin[0]);
	engfunc(EngFunc_WriteCoord, fOrigin[1]);
	engfunc(EngFunc_WriteCoord, fOrigin[2]);
	engfunc(EngFunc_WriteCoord, fOrigin[0]);
	engfunc(EngFunc_WriteCoord, fOrigin[1]);
	engfunc(EngFunc_WriteCoord, fOrigin[2]);
	write_short(pSprite);
	write_byte(iCount);
	write_byte(iLife); // 0.1's
	write_byte(iScale);
	write_byte(iVelocityAlongVector);
	write_byte(iRandomVelocity);
	message_end(); 
}

stock CREATE_BREAKMODEL(const Float:fOrigin[3], Float:fSize[3] = {16.0, 16.0, 16.0}, Float:fVelocity[3] = {25.0, 25.0, 25.0}, iRandomVelocity, pModel, iCount, iLife, iFlags)
{
	message_begin(MSG_BROADCAST, SVC_TEMPENTITY);
	write_byte(TE_BREAKMODEL);
	engfunc(EngFunc_WriteCoord, fOrigin[0]);
	engfunc(EngFunc_WriteCoord, fOrigin[1]);
	engfunc(EngFunc_WriteCoord, fOrigin[2] + 24);
	engfunc(EngFunc_WriteCoord, fSize[0]);
	engfunc(EngFunc_WriteCoord, fSize[1]);
	engfunc(EngFunc_WriteCoord, fSize[2]);
	engfunc(EngFunc_WriteCoord, fVelocity[0]);
	engfunc(EngFunc_WriteCoord, fVelocity[1]);
	engfunc(EngFunc_WriteCoord, fVelocity[2]);
	write_byte(iRandomVelocity);
	write_short(pModel);
	write_byte(iCount); // 0.1's
	write_byte(iLife); // 0.1's
	write_byte(iFlags);
	message_end();
}

stock Float:EstimateDamage(Float:fPoint[3], ent, ignored) 
{
	new Float:fOrigin[3];
	new tr;
	new Float:fFraction;
	
	pev(ent, pev_origin, fOrigin);
	engfunc(EngFunc_TraceLine, fPoint, fOrigin, DONT_IGNORE_MONSTERS, ignored, tr);
	get_tr2(tr, TR_flFraction, fFraction);
	
	if (fFraction == 1.0 || get_tr2( tr, TR_pHit ) == ent)
		return 1.0;
	
	return 0.6;
}

stock CreateKnockBack(iVictim, Float:vAttacker[3], Float:fMulti, Float:fRadius)
{
	new Float:vVictim[3];
	pev(iVictim, pev_origin, vVictim);

	xs_vec_sub(vVictim, vAttacker, vVictim);

	xs_vec_mul_scalar(vVictim, fMulti * 0.7, vVictim);
	xs_vec_mul_scalar(vVictim, fRadius / xs_vec_len(vVictim), vVictim);

	vVictim[2] = 0.0; // чтоб не кидало вверх
	set_pev(iVictim, pev_velocity, vVictim);
}