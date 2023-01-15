#include "jbe_core\hook\hook_menu.inl"

hook_init()
{
	register_menucmd(register_menuid("Show_HookMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_HookMenu");

	register_clcmd("+hook", "ClCmd_HookOn");
	register_clcmd("-hook", "ClCmd_HookOff");
	register_clcmdex("hook", "ClCmd_HookMenu", 1); // say /hook
}

public ClCmd_HookMenu(id)
{
	if (IsNotSetBit(g_iBitUserHook, id))
		return ClCmd_InfoBuy(id);

	return Show_HookMenu(id);
}

is_not_use_hook(id)
{
	if (IsNotSetBit(g_iBitUserHook, id))
		ClCmd_InfoBuy(id);
	
	else if (g_iDayMode == DAY_DAYMODE || IsNotSetBit(g_iBitUserAlive, id) || task_exists(id+TASK_HOOK_THINK))
		{ /*заглушка типо*/ }

	else if (IsSetBit(g_iBitUserBoxing, id) || IsSetBit(g_iBitUserSoccer, id) || IsSetBit(g_iBitUserDuel, id))
		client_print_color(id, 0, "%s ^1Hook ^4недоступен ^1в игре!", g_szChatPrefix);
	
	else if (g_iGlobalBlock[GB_HOOK])
		client_print_color(id, 0, "%s ^1Hook ^4заблокирован %s^1.", g_szChatPrefix, global_get_name_block(GB_HOOK));

	else if (IsSetBit(g_iBitUserWanted, id))
		client_print_color(id, 0, "%s ^1Hook ^4недоступен в розыске!", g_szChatPrefix);
	
	else
		return false;

	return true;
}

public ClCmd_HookOn(id)
{
	if (is_not_use_hook(id))
		return PLUGIN_HANDLED;
/*
	if (IsSetBit(g_iBitParachuteUse, id))
		remove_task(id + TASK_PARACHUTE_THINK);
	else
		pev(id, pev_gravity, g_fUserHookGravity[id]);
*/
	SetBit(g_iBitHookUse, id);

	get_entvar(id, var_gravity, g_fUserHookGravity[id]);
	CREATE_KILLBEAM(id);
	
	if (g_eHookSetting[id][HOOK_TRAIL_COLOUR])
		CREATE_BEAMFOLLOW (id, g_pSpriteBeam,
			g_iHookTrailLength[g_eHookSetting[id][HOOK_TRAIL_LENGTH]],
			g_iHookTrailWidth[g_eHookSetting[id][HOOK_TRAIL_WIDTH]],
			g_iszHookColour[g_eHookSetting[id][HOOK_TRAIL_COLOUR]][0],
			g_iszHookColour[g_eHookSetting[id][HOOK_TRAIL_COLOUR]][1],
			g_iszHookColour[g_eHookSetting[id][HOOK_TRAIL_COLOUR]][2],
			200);
	else
		CREATE_BEAMFOLLOW (id, g_pSpriteBeam, 
			g_iHookTrailLength[g_eHookSetting[id][HOOK_TRAIL_LENGTH]],
			g_iHookTrailWidth[g_eHookSetting[id][HOOK_TRAIL_WIDTH]],
			random_num(50, 255), 
			random_num(50, 255), 
			random_num(50, 255), 
			200);

	if (jbe_is_wanted() || (g_iDayMode == DAY_DEATHMATCH && g_bDeathMatchEnabled && !jbe_get_user_godmode(id)))
	{
		set_entvar(id, var_gravity, 0.4);
		new Float:vecVelocity[3];
		get_entvar(id, var_velocity, vecVelocity);
		vecVelocity[2] = -100.0;
		set_entvar(id, var_velocity, vecVelocity);
		jbe_parachute_think(id + TASK_HOOK_THINK);
		set_task(0.1, "jbe_parachute_think", id+TASK_HOOK_THINK, _, _, "b");
		UTIL_PlayerAnimation(id, "so_am_I");
	}
	else
	{
		if (IsSetBit(g_iBitHookMenu, id))
			Show_HookMenu(id);

		set_entvar(id, var_gravity, 0.04);

		new iOrigin[3];
		get_user_origin(id, iOrigin, 3);

		g_vecHookOrigin[id][0] = float(iOrigin[0]);
		g_vecHookOrigin[id][1] = float(iOrigin[1]);
		g_vecHookOrigin[id][2] = float(iOrigin[2]);

		CREATE_SPRITE(g_vecHookOrigin[id], g_pSpriteRicho2, 10, 255);
		
		if (!is_user_cmd_flood(id, FLOOD_HOOK, 0.7, 1.7, .bMessage = false))
			emit_sound(id, CHAN_STATIC, sound_hook, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		
		if (IsSetBit(g_iBitNewHook, id))
		{
			jbe_hook_think_new(id+TASK_HOOK_THINK);
			set_task(0.1, "jbe_hook_think_new", id+TASK_HOOK_THINK, _, _, "b");
		}
		else
		{
			jbe_hook_think_old(id+TASK_HOOK_THINK);
			set_task(0.1, "jbe_hook_think_old", id+TASK_HOOK_THINK, _, _, "b");
		}
	}

	return PLUGIN_HANDLED;
}

public ClCmd_HookOff(id)
{
	if (IsSetBit(g_iBitUserHook, id) && task_exists(id + TASK_HOOK_THINK))
	{
		ClearBit(g_iBitHookUse, id);
		set_entvar(id, var_gravity, g_fUserHookGravity[id]);
		remove_task(id + TASK_HOOK_THINK);
	}
	return PLUGIN_HANDLED;
}

public jbe_hook_think_new(pPlayer)
{
	pPlayer -= TASK_HOOK_THINK;
	
	new Float:vecVelocity[3];
	velocity_by_aim(pPlayer, 5 * hook_speed_num(pPlayer), vecVelocity);
	set_entvar(pPlayer, var_velocity, vecVelocity);

	hook_beamentpoint(pPlayer);

	return PLUGIN_HANDLED;
}

public jbe_hook_think_old(pPlayer)
{
	pPlayer -= TASK_HOOK_THINK;
	
	new Float:vecOrigin[3];
	get_entvar(pPlayer, var_origin, vecOrigin);

	new Float:vecVelocity[3];
	vecVelocity[0] = (g_vecHookOrigin[pPlayer][0] - vecOrigin[0]) * 3.0;
	vecVelocity[1] = (g_vecHookOrigin[pPlayer][1] - vecOrigin[1]) * 3.0;
	vecVelocity[2] = (g_vecHookOrigin[pPlayer][2] - vecOrigin[2]) * 3.0;

	new Float:flY = vecVelocity[0] * vecVelocity[0] + vecVelocity[1] * vecVelocity[1] + vecVelocity[2] * vecVelocity[2];
	new Float:flX = (5 * hook_speed_num(pPlayer)) / floatsqroot(flY);
	vecVelocity[0] *= flX;
	vecVelocity[1] *= flX;
	vecVelocity[2] *= flX;
	set_entvar(pPlayer, var_velocity, vecVelocity);

	hook_beamentpoint(pPlayer);

	return PLUGIN_HANDLED;
}

public jbe_parachute_think(id)
{
	id -= TASK_HOOK_THINK;

	if (get_entvar(id, var_oldbuttons) & IN_JUMP || get_entvar(id, var_button) & IN_JUMP)
		return;

	if (get_entvar(id, var_flags) & FL_ONGROUND) // на замле
	{
		set_entvar(id, var_gravity, g_fUserHookGravity[id]);
		remove_task(id + TASK_HOOK_THINK);
		return;
	}

	new Float:vecVelocity[3];
	get_entvar(id, var_velocity, vecVelocity);
	if (vecVelocity[2] < 0.0)
	{
		vecVelocity[2] = (vecVelocity[2] + 40.0 < -100.0) ? vecVelocity[2] + 40.0 : -100.0;
		set_entvar(id, var_velocity, vecVelocity);
	}
}

// "рисуем паутинку
hook_beamentpoint(pPlayer)
{
	if (IsSetBit(g_iBitNewHook, pPlayer))
	{
		new iOrigin[3];
		get_user_origin(pPlayer, iOrigin, 3);
		g_vecHookOrigin[pPlayer][0] = float(iOrigin[0]);
		g_vecHookOrigin[pPlayer][1] = float(iOrigin[1]);
		g_vecHookOrigin[pPlayer][2] = float(iOrigin[2]);
	}

	if (g_eHookSetting[pPlayer][HOOK_COLOUR])
		CREATE_BEAMENTPOINT ( pPlayer, g_vecHookOrigin[pPlayer], g_pSpriteLgtning, 0, 1, 1, 
			g_iHookWidth[g_eHookSetting[pPlayer][HOOK_WIDTH]],
			g_iHookAmplitude[g_eHookSetting[pPlayer][HOOK_AMPLITUDE]],
			g_iszHookColour[g_eHookSetting[pPlayer][HOOK_COLOUR]][0],
			g_iszHookColour[g_eHookSetting[pPlayer][HOOK_COLOUR]][1],
			g_iszHookColour[g_eHookSetting[pPlayer][HOOK_COLOUR]][2],
			200, _);
	else
		CREATE_BEAMENTPOINT (pPlayer, g_vecHookOrigin[pPlayer], g_pSpriteLgtning, 0, 1, 1, 
			g_iHookWidth[g_eHookSetting[pPlayer][HOOK_WIDTH]],
			g_iHookAmplitude[g_eHookSetting[pPlayer][HOOK_AMPLITUDE]],
			random_num(0, 255),
			random_num(0, 255),
			random_num(0, 255),
			200, _);
}

set_hook_setting_default(id)
{
	SetBit(g_iBitNewHook, id);
	SetBit(g_iBitHookMenu, id);

	g_eHookSetting[id][HOOK_SPEED] = 3;

	g_eHookSetting[id][HOOK_COLOUR] = random(sizeof g_iszHookColour);
	g_eHookSetting[id][HOOK_WIDTH] = random(sizeof g_iHookWidth);
	g_eHookSetting[id][HOOK_AMPLITUDE] = random(sizeof g_iHookAmplitude);

	g_eHookSetting[id][HOOK_TRAIL_COLOUR] = random(sizeof g_iszHookColour);
	g_eHookSetting[id][HOOK_TRAIL_WIDTH] = random(sizeof g_iHookTrailWidth);
	g_eHookSetting[id][HOOK_TRAIL_LENGTH] = random(sizeof g_iHookTrailLength);
}