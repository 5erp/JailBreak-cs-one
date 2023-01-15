// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

/*==================================================
 #			-> GRAB ->
====================================================*/

// Подключение файлов менюшек граба
#include "jbe_core\grab\grab_menu.inl"
#include "jbe_core\grab\grab_admin_menu.inl"
#include "jbe_core\grab\grab_skin_menu.inl"
#include "jbe_core\grab\grab_glow_menu.inl"

grab_init()
{ 
	register_menucmd(register_menuid("Show_GrabMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5), "Handle_GrabMenu");
		
	register_menucmd(register_menuid("Show_GrabAdminMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_GrabAdminMenu");

	register_menucmd(register_menuid("Show_GrabSkinMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_GrabSkinMenu");
		
	register_menucmd(register_menuid("Show_GrabGlowMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_GrabGlowMenu");

	register_clcmdex("grab", "ClCmd_GrabMenu", 1); // say /grab
	register_clcmd("amx_grab", "force_grab");	// на потом
	register_clcmd("grab_toggle", "grab_toggle"); // вкл/откл граб одним нажатием

	register_clcmd("+grab", "ClCmd_GrabOn");
	register_clcmd("-grab", "ClCmd_GrabOff");

	register_clcmd("+push", "ClCmd_GrabPush"); // Отдаляем
	register_clcmd("-push", "ClCmd_GrabPush");

	register_clcmd("+pull", "ClCmd_GrabPull"); // Притягиваем
	register_clcmd("-pull", "ClCmd_GrabPull");

	register_clcmd("push", "ClCmd_GrabPush_2"); // 
	register_clcmd("pull", "ClCmd_GrabPull_2"); // 
}

open_grab_menu(id)
{
	// меню граба выключенно
	if (!g_iGrabMenuType[id])
		return PLUGIN_HANDLED;

	// Открыто другое меню, не перекрываем грабом
	if (jbe_get_informer_offset(id))
	{
		client_print_color(id, id, "%s ^1Чтоб открыть меню нажмите `E` или закройте текущее меню.", g_szGrabPrefix);
		return PLUGIN_HANDLED;
	}

	return open_grab_menu_post(id);
}

open_grab_menu_post(id)
{
	SetBit(g_iBitOpenGrabMenu, id);
	
	// взяли оружие
	if (!jbe_is_user_valid(g_iGrabData[id][GRABBED]))
		return Show_GrabMenu(id);
	
	if (IsSetBit(g_iBitUserAdmin, id))
	{
		switch (g_iGrabMenuType[id])
		{
			case GRAB_MENU_NORMAL: return Show_GrabMenu(id);
			case GRAB_MENU_ADMIN: return Show_GrabAdminMenu(id, .bGrab = true);
			case GRAB_MENU_SKIN: return Show_GrabSkinMenu(id);
			case GRAB_MENU_GLOW: return Show_GrabGlowMenu(id);
			default: return Show_GrabMenu(id); // вдруг че
		}
	}
	else return Show_GrabMenu(id);

	return PLUGIN_HANDLED;
}

grab_menu_key_use(id) // IN_USE
{
	if (IsNotSetBit(g_iBitOpenGrabMenu,id) && g_iGrabMenuType[id])
		return open_grab_menu_post(id);

	if (!jbe_is_user_valid(g_iGrabData[id][GRABBED]))
	{
		if (g_iGrabMenuType[id] != GRAB_MENU_NONE)
		{
			g_iGrabMenuType[id] = GRAB_MENU_NONE;
			show_menu(id, 0, "^n");
			jbe_informer_offset_down(id);
			return PLUGIN_HANDLED;
		}
		else
		{
			g_iGrabMenuType[id] = GRAB_MENU_NORMAL;
			SetBit(g_iBitOpenGrabMenu, id);
			return Show_GrabMenu(id);
		}
	}
	
	if (IsSetBit(g_iBitUserAdmin, id))
	{
		switch (g_iGrabMenuType[id])
		{
			case GRAB_MENU_NORMAL:
			{
				g_iGrabMenuType[id] = GRAB_MENU_ADMIN;
				SetBit(g_iBitOpenGrabMenu, id);
				return Show_GrabAdminMenu(id, .bGrab = true);
			}
			case GRAB_MENU_ADMIN:
			{
				g_iGrabMenuType[id] = GRAB_MENU_NONE;
				show_menu(id, 0, "^n");
				jbe_informer_offset_down(id);
			}
			case GRAB_MENU_SKIN, GRAB_MENU_GLOW:
			{
				g_iGrabMenuType[id] = GRAB_MENU_NORMAL;
				SetBit(g_iBitOpenGrabMenu, id);
				return Show_GrabMenu(id);
			}
			default:
			{
				g_iGrabMenuType[id] = GRAB_MENU_NORMAL;
				SetBit(g_iBitOpenGrabMenu, id);
				return Show_GrabMenu(id);
			}
		}
	}
	else
	{
		if (!g_iGrabMenuType[id])
		{
			g_iGrabMenuType[id] = GRAB_MENU_NORMAL;
			SetBit(g_iBitOpenGrabMenu, id);
			return Show_GrabMenu(id);
		}
		else
		{
			g_iGrabMenuType[id] = GRAB_MENU_NONE;
			show_menu(id, 0, "^n");
			jbe_informer_offset_down(id);
		}
	}
	return PLUGIN_HANDLED;
}

grab_menu_key_reload(id) // IN_RELOAD
{
	if (!jbe_is_user_valid(g_iGrabData[id][GRABBED]))
		return PLUGIN_HANDLED;
	
	if (g_iDayMode == DAY_DAYMODE)
	{
		client_print_color(id, 0, "%s Сейчас это меню недоступно!", g_szGrabPrefix);
		return PLUGIN_HANDLED;
	}
	
	if (IsNotSetBit(g_iBitUserAdmin, id))
		return PLUGIN_HANDLED;

	switch (g_iGrabMenuType[id]) {
		case GRAB_MENU_SKIN: {
			g_iGrabMenuType[id] = GRAB_MENU_GLOW;
			return Show_GrabGlowMenu(id);
		}
		case GRAB_MENU_GLOW: {
			g_iGrabMenuType[id] = GRAB_MENU_NONE;
			show_menu(id, 0, "^n");
			jbe_informer_offset_down(id);
			return PLUGIN_HANDLED;
		}
		default: {
			g_iGrabMenuType[id] = GRAB_MENU_SKIN;
			return Show_GrabSkinMenu(id);
		}
	}
	return PLUGIN_HANDLED;
}

/* доступно ли grab glow menu */
is_not_use_grab_glow_menu(id) {
	if (IsNotSetBit(g_iBitUserAdmin, id) || IsNotSetBit(g_iBitUserGrab, id)) {
		ClCmd_InfoBuy(id);
	}
	else if (g_iDayMode == DAY_DAYMODE) {
		client_print_color(id, 0, "%s Сейчас меню свечения недоступно!", g_szGrabPrefix);
	}
	else if (IsSetBit(g_iBitGrabUse, id) && !jbe_is_user_valid(g_iGrabData[id][GRABBED])
		|| IsNotSetBit(g_iBitUserGrab, id) && !jbe_is_user_valid(g_iMenuTarget[id])) {
		client_print_color(id, 0, "%s Скины можно выставлять только игрокам!", g_szGrabPrefix);
	}	
	else
		return false;

	return true;	
}

is_not_use_grab_skin_menu(id) {
//	new iTarget = IsSetBit(g_iBitUserGrab, id) ? g_iGrabData[id][GRABBED] : g_iMenuTarget[id];

	if (IsNotSetBit(g_iBitUserAdmin, id) || IsNotSetBit(g_iBitUserGrab, id))
	{
		ClCmd_InfoBuy(id);
	}
	else if (g_iDayMode == DAY_DAYMODE)
	{
		client_print_color(id, 0, "%s Сейчас меню скинов недоступно!", g_szGrabPrefix);
	}
	else if (IsSetBit(g_iBitGrabUse, id) && !jbe_is_user_valid(g_iGrabData[id][GRABBED])
		|| IsNotSetBit(g_iBitUserGrab, id) && !jbe_is_user_valid(g_iMenuTarget[id]))
	{
		client_print_color(id, 0, "%s Скины можно выставлять только игрокам!", g_szGrabPrefix);
	}	
	else
		return false;

	return true;
}

stock is_not_use_grab_admin_menu(id) {
	if (IsNotSetBit(g_iBitUserAdmin, id) || IsNotSetBit(g_iBitUserGrab, id)) {
		ClCmd_InfoBuy(id);
		return true;
	}
	return false;
}

stock is_not_use_grab_menu(id)
{
	if (IsNotSetBit(g_iBitUserGrab, id))
	{
		ClCmd_InfoBuy(id);
		return true;
	}
	return false;
}

public ClCmd_GrabMenu(id)
{
	if (IsNotSetBit(g_iBitUserGrab, id))
		return ClCmd_InfoBuy(id);
		
	if (g_iGrabMenuType[id])
		g_iGrabMenuType[id] = GRAB_MENU_NONE;
	else
		g_iGrabMenuType[id] = GRAB_MENU_NORMAL;
	
	client_print_color(id, id, "%s ^1Вы ^3%s ^1меню граба.", g_szGrabPrefix, g_iGrabMenuType[id] ? "включили" : "выключили");
	
	return PLUGIN_HANDLED;
}

public ClCmd_GrabOn(id)
{
	if (IsNotSetBit(g_iBitUserGrab, id))
		return ClCmd_InfoBuy(id);
	
	SetBit(g_iBitGrabUse, id);

	check_player_pre_think();
	
	if (!g_iGrabData[id][GRABBED])
		g_iGrabData[id][GRABBED] = -1;
	
	return PLUGIN_HANDLED;
}

public ClCmd_GrabOff(id) 
{
	if (IsNotSetBit(g_iBitUserGrab, id))
		return PLUGIN_HANDLED;
	
	ClearBit(g_iBitGrabUse, id);
	check_player_pre_think();

	new iTarget = g_iGrabData[id][GRABBED];
	
	if (iTarget > 0 && pev_valid(iTarget))
	{
		if (jbe_is_user_valid(iTarget))
		{
			if (!jbe_get_user_godmode(iTarget))
				set_pev(iTarget, pev_takedamage, DAMAGE_YES);

			g_iGrabData[iTarget][GRABBER] = 0;

			// возвращаем скорость игроку
			ExecuteHamB(Ham_Player_ResetMaxSpeed, iTarget);
			jbe_reset_user_rendering(iTarget);
		}
		else
			jbe_set_user_rendering(iTarget, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
		
		if (g_iGrabMenuType[id] && IsSetBit(g_iBitOpenGrabMenu, id))
		{
			show_menu(id, 0, "^n");
			jbe_informer_offset_down(id);
		}
	}

	if (IsSetBit(g_iBitUserAlive, id))
	{
		set_member(id, m_flNextAttack, 0.1);
		new iActiveItem = get_member(id, m_pActiveItem);
		if (iActiveItem > 0)
			set_member(iActiveItem, m_Weapon_flNextSecondaryAttack, 0.1); 
	}

	ClearBit(g_iBitOpenGrabMenu, id);

	g_iGrabData[id][GRABBED] = 0;
	return PLUGIN_HANDLED;
}

public ClCmd_GrabPush(id)
{
	// Отдаляем
	g_iGrabData[id][GRAB_FLAGS] ^= CDF_IN_PUSH;
	
	return PLUGIN_HANDLED;
}

public ClCmd_GrabPull(id)
{
	// Притягиваем
	g_iGrabData[id][GRAB_FLAGS] ^= CDF_IN_PULL;
	
	return PLUGIN_HANDLED;
}

public ClCmd_GrabPush_2(id) {
	if (g_iGrabData[id][GRABBED] > 0) {
		if (g_iGrabData[id][GRAB_LEN] < 9999)
			g_iGrabData[id][GRAB_LEN] += (GRAB_SPEED * 20);
		
		return PLUGIN_HANDLED;
	}
	
	return PLUGIN_CONTINUE;
}

public ClCmd_GrabPull_2(id) {
	if (g_iGrabData[id][GRABBED] > 0) {
		new iLen = g_iGrabData[id][GRAB_LEN];
		
		if (iLen > GRAB_MIN_DIST) {
			iLen -= (GRAB_SPEED * 20);
			if (iLen < GRAB_MIN_DIST)
				iLen = GRAB_MIN_DIST;
			g_iGrabData[id][GRAB_LEN] = iLen;
		}
		
		return PLUGIN_HANDLED;
	}
	
	return PLUGIN_CONTINUE;
}

grab_think(id) //id of the grabber
{
	static iTarget;
	
	iTarget = g_iGrabData[id][GRABBED];
	
	//Keep grabbed clients from sticking to ladders
	if (pev(iTarget, pev_movetype) == MOVETYPE_FLY && !(pev(iTarget, pev_button) & IN_JUMP))
		client_cmd(iTarget, "+jump;wait;-jump");

	if (pev(iTarget, pev_flags) & FL_INWATER)
	{
		if (pev(iTarget, pev_maxspeed) < 320.0)
			set_pev(iTarget, pev_maxspeed, 320.0);
	}
	else if (pev(iTarget, pev_maxspeed) > 0.1)
		set_pev(iTarget, pev_maxspeed, 0.1);


	//Move targeted client
	static Float:tmpvec[3], Float:tmpvec2[3], Float:torig[3], Float:tvel[3];
	
	get_view_pos(id, tmpvec);
	velocity_by_aim(id, g_iGrabData[id][GRAB_LEN], tmpvec2);
	torig = get_target_origin_f(iTarget);
	
	tvel[0] = ((tmpvec[0] + tmpvec2[0]) - torig[0]) * GRAB_FORCE;
	tvel[1] = ((tmpvec[1] + tmpvec2[1]) - torig[1]) * GRAB_FORCE;
	tvel[2] = ((tmpvec[2] + tmpvec2[2]) - torig[2]) * GRAB_FORCE;

	set_pev(iTarget, pev_velocity, tvel);
}

stock Float:get_target_origin_f(id)
{
	new Float:orig[3];
	
	pev(id, pev_origin, orig);
	
	// If grabbed is not a player, move origin to center
	if (id > MaxClients)
	{
		new Float:mins[3], Float:maxs[3];
		pev(id, pev_mins, mins);
		pev(id, pev_maxs, maxs);
		
		if (!mins[2]) orig[2] += maxs[2] / 2;
	}
	
	return orig;
}

public grab_toggle(id) {
	if (IsNotSetBit(g_iBitUserGrab, id))
		return ClCmd_InfoBuy(id);
	
	if (!g_iGrabData[id][GRABBED])
		ClCmd_GrabOn(id);
	
	else
		ClCmd_GrabOff(id);
	
	return PLUGIN_HANDLED;
}

//Grabs onto someone
grab_set_grabbed(id, iTarget)
{
	jbe_set_user_rendering(iTarget, kRenderFxGlowShell, 0, 0, 50, kRenderTransColor, 200);
	
	if (jbe_is_user_valid(iTarget))
	{
		if (!jbe_get_user_godmode(iTarget))
			set_entvar(iTarget, var_takedamage, DAMAGE_NO);

		g_iGrabData[iTarget][GRABBER] = id;

		client_print_color(id, iTarget, "%s ^1Вы взяли грабом ^3%n^1.%s", g_szGrabPrefix, 
			iTarget, g_iGrabMenuType[id] ? "" : " ^1[^4E^1] - Открыть меню.");
		
		client_print_color(iTarget, id, "%s ^3%n ^1взял вас грабом.", g_szGrabPrefix, id);
		
		client_print(0, print_console, " ** [Grab] %n взял грабом %n.", id, iTarget);
		
		if (IsNotSetBit(g_iBitUserBuried, iTarget) && !rg_get_user_noclip(iTarget) && !(pev(iTarget, pev_solid) & SOLID_NOT))
			unstuck_user(iTarget);

		// снимаем с лестницы		
	//	new iFlags = get_entvar(iTarget, var_iuser3);
	//	iFlags &= ~PLAYER_PREVENT_CLIMB;
	//	set_entvar(iTarget, var_iuser3, iFlags);
	}
	else
	{
		client_print_color(id, id, "%s ^1Вы взяли грабом ^4оружие^1.%s", g_szGrabPrefix,
			g_iGrabMenuType[id] ? "" : " ^1[^4E^1] - Открыть меню.");
		
		client_print(0, print_console, " ** [Grab] %n взял грабом оружие.", id);
	}

	g_iGrabData[id][GRAB_FLAGS] = 0;
	g_iGrabData[id][GRABBED] = iTarget;
	
	new Float:vecOriginTarget[3], Float:vecOriginId[3];
	
	pev(iTarget, pev_origin, vecOriginTarget);
	pev(id, pev_origin, vecOriginId);
	
	g_iGrabData[id][GRAB_LEN] = floatround(get_distance_f(vecOriginTarget, vecOriginId));
	
	if (g_iGrabData[id][GRAB_LEN] < GRAB_MIN_DIST)
		g_iGrabData[id][GRAB_LEN] = GRAB_MIN_DIST;
	
	open_grab_menu(id);
}

grab_do_push(id) {
	if (g_iGrabData[id][GRAB_LEN] < 9999)
		g_iGrabData[id][GRAB_LEN] += GRAB_SPEED;
}

grab_do_pull(id) {
	new iLen = g_iGrabData[id][GRAB_LEN];
	
	if (iLen > GRAB_MIN_DIST) {
		iLen -= GRAB_SPEED;
		if (iLen < GRAB_MIN_DIST)
			iLen = GRAB_MIN_DIST;
		g_iGrabData[id][GRAB_LEN] = iLen;
	} // else do_choke(id);
}

grab_menu_key_attack(id) // grab_primary_attack
{
	set_member(id, m_flNextAttack, 9999.0);
	ClCmd_GrabPull_2(id);
}

grab_menu_key_attack2(id) // grab_secondary_attack
{
	new iActiveItem = get_member(id, m_pActiveItem);
	if (iActiveItem > 0)
		set_member(iActiveItem, m_Weapon_flNextSecondaryAttack, get_gametime() + 9999.0); 
	
	ClCmd_GrabPush_2(id);
}

grab_menu_key_damage(id)
{
	if (g_iDayMode == DAY_DAYMODE)
		return;

	new iTarget = g_iGrabData[id][GRABBED];
	
	// Держим оружие, - удалить
	if (!jbe_is_user_valid(iTarget) && pev_valid(iTarget) == 2)
	{
		ClCmd_GrabOff(id);
		
		engfunc(EngFunc_RemoveEntity, iTarget);
		client_print_color(0, print_team_red, "%s ^4%n ^1удалил ^3Оружие^1.", g_szGrabPrefix, id);
		return;
	}

	if (g_iGrabData[id][GRAB_FLAGS] & CDF_NO_CHOKE || id == iTarget || iTarget > MaxClients)
		return;

	if (!IsValidPrivateData(iTarget))
	{
		ClCmd_GrabOff(id);
		return;
	}
	
	new vecVector[3];
	
	FVecIVec(get_target_origin_f(iTarget), vecVector);
	UTIL_ScreenShake(iTarget, 999999, 9999, 999, 1);
	UTIL_ScreenFade(iTarget, 9999, 100, SF_FADE_MODULATE, 50, 0, 0, 200, 1);
	UTIL_DamagePlayer(iTarget, 0, GRAB_CHOKE_DMG, DMG_CRUSH, vecVector, 1);
	CREATE_BLOODSTREAM(vecVector, 70, random_num(50, 250));

	//Thanks to ConnorMcLeod for making this block of code more proper
	new Float:fHealth;
	
	pev(iTarget, pev_health , fHealth);
	fHealth -= GRAB_CHOKE_DMG;
	
	if (fHealth < 1)
	{
		ClCmd_GrabOff(id);
		client_print_color(0, iTarget, "%s ^4%n ^1убил ^3%n^1.", g_szGrabPrefix, id, iTarget);
		dllfunc(DLLFunc_ClientKill, iTarget);
		return;
	}
	else
	{
		set_pev(iTarget, pev_health, fHealth);
		set_member(iTarget, m_bitsDamageType, DMG_CRUSH); // m_iDamageType = 76 // found by VEN
		set_pev(iTarget, pev_dmg_take, GRAB_CHOKE_DMG);
		set_pev(iTarget, pev_dmg_inflictor, id);
		client_print_color(0, iTarget, "%s ^4%n ^1ударил на ^4%d HP ^3%n^1.", g_szGrabPrefix, id, GRAB_CHOKE_DMG, iTarget);
	}

	g_iGrabData[id][GRAB_FLAGS] ^= CDF_NO_CHOKE;
	set_task(1.5, "clear_no_choke", TASK_GRAB_NO_CHOKE + id);
}

public clear_no_choke(id) {
	id -= TASK_GRAB_NO_CHOKE;
	g_iGrabData[id][GRAB_FLAGS] ^= CDF_NO_CHOKE;
}

grab_menu_key_health(id)
{
	if (g_iDayMode == DAY_DAYMODE)
		return PLUGIN_HANDLED;

	new iTarget = g_iGrabData[id][GRABBED];
	
	if (!jbe_is_user_valid(iTarget))
		return PLUGIN_HANDLED;

	if (pev(iTarget, pev_health) < 100.0) 
	{
		set_pev(iTarget, pev_health, 100.0);
		client_print_color(0, iTarget, "%s ^4%n ^1вылечил ^3%n^1.",
			g_szGrabPrefix, id, iTarget);
	} 
	else 
	{
		client_print_color(id, iTarget, "%s ^1У игрока ^3%n ^1полные жизни.",
			g_szGrabPrefix, iTarget);
	}
	
	return PLUGIN_HANDLED;
}


grab_is_grabbed(iTarget, iGrabber)
{
	for (new i = 1; i <= MaxClients; i++)
	{
		if (g_iGrabData[i][GRABBED] == iTarget)
		{
			if (jbe_is_user_valid(iTarget))
			{
				client_print_color(iGrabber, 0, "%s Игрока ^4%n ^1держит грабом ^4%n^1.", g_szGrabPrefix, iTarget, i);
			}
			else
			{
				client_print_color(iGrabber, 0, "%s ^1Это оружие держит грабом ^4%d^1.", g_szGrabPrefix, i);
			}
			ClCmd_GrabOff(iGrabber);
			return true;
		}
	}
	return false;
}

grab_off_to_user(pPlayer)
{
	//If given client has grabbed, or has a grabber, unset it
	if (g_iGrabData[pPlayer][GRABBED]) // Если pPlayer держит
		ClCmd_GrabOff(pPlayer);

	if (g_iGrabData[pPlayer][GRABBER]) // Если pPlayer'a держат
		ClCmd_GrabOff(g_iGrabData[pPlayer][GRABBER]);

	return;
}

stock grab_traceline(const Float:vStart[3], const Float:vEnd[3], const pIgnore, Float:vHitPos[3])
{
	engfunc(EngFunc_TraceLine, vStart, vEnd, 0, pIgnore, 0);
	get_tr2(0, TR_vecEndPos, vHitPos);
	return get_tr2(0, TR_pHit);
}

stock get_view_pos(const id, Float:vViewPos[3])
{
	new Float:vOfs[3];
	
	pev(id, pev_origin, vViewPos);
	pev(id, pev_view_ofs, vOfs);
	
	vViewPos[0] += vOfs[0];
	vViewPos[1] += vOfs[1];
	vViewPos[2] += vOfs[2];
}

grab_player_think(id)
{	
	// Никто не юзает граб
	if (!g_iBitGrabUse)
		return PLUGIN_CONTINUE;

	// If they're grabbed // Кто-то нас держит
	if (g_iGrabData[id][GRABBER])
		grab_think(g_iGrabData[id][GRABBER]);

	// Если мы не используем граб
	if (IsNotSetBit(g_iBitGrabUse, id))
		return PLUGIN_CONTINUE;

	static iTarget;

	//Search for a target
	if (g_iGrabData[id][GRABBED] == -1) // если никого не держим грабом (готовы взять grab_on)
	{
		new Float:vecOrigin[3], Float:vecEnd[3];
	
		get_view_pos(id, vecOrigin);
		velocity_by_aim(id, 9999, vecEnd);
		
		vecEnd[0] += vecOrigin[0];
		vecEnd[1] += vecOrigin[1];
		vecEnd[2] += vecOrigin[2];
		
		iTarget = grab_traceline(vecOrigin, vecEnd, id, vecEnd); // ret

		if (0 < iTarget <= MaxClients)
		{
			if (g_iGrabData[iTarget][GRABBER])
			{
				client_print_color(id, 0, "%s Игрока ^4%n ^1держит грабом ^4%n^1.", g_szGrabPrefix, iTarget, g_iGrabData[iTarget][GRABBER]);
				ClCmd_GrabOff(id);

				return PLUGIN_CONTINUE;
			}
			grab_set_grabbed(id, iTarget);
		}
		else
		{
			new iMoveType;
		
			if (iTarget && pev_valid(iTarget))
			{
				iMoveType = pev(iTarget, pev_movetype);
				if (!(iMoveType == MOVETYPE_WALK || iMoveType == MOVETYPE_STEP || iMoveType == MOVETYPE_TOSS))
					return PLUGIN_CONTINUE;
			}
			else
			{
				iTarget = 0;
				
				new iEntity = engfunc(EngFunc_FindEntityInSphere, -1, vecEnd, 12.0);
				
				while (!iTarget && iEntity > 0)
				{
					iMoveType = pev(iEntity, pev_movetype);
					if ((iMoveType == MOVETYPE_WALK || iMoveType == MOVETYPE_STEP || iMoveType == MOVETYPE_TOSS)
					&& iEntity != id && IsNotSetBit(g_iBitUserGhost, iEntity)) // чтоб не хватануть призрака
						iTarget = iEntity;
					iEntity = engfunc(EngFunc_FindEntityInSphere, iEntity, vecEnd, 12.0);
				}
			}
			
			if (iTarget)
			{
				if (grab_is_grabbed(iTarget, id))
					return PLUGIN_CONTINUE;
				
				// g_iBitUserAlive - Not Ghost User
				if (jbe_is_user_valid(iTarget) && IsSetBit(g_iBitUserAlive, iTarget))
				{
					grab_set_grabbed(id, iTarget);
				}
				else
				{
				//	new szClassName[15];
				//	pev(iTarget, pev_classname, szClassName, charsmax(szClassName));
				//	if (equal(szClassName, "armoury_entity"))
					if (FClassnameIs(iTarget, "armoury_entity"))
					{
						client_print_color(id, print_team_red, "%s ^3Запрещено ^1брать оружие с места его ^4спавна^1!", g_szGrabPrefix);
						ClCmd_GrabOff(id);
						return PLUGIN_CONTINUE; // c:
					}
					grab_set_grabbed(id, iTarget);
				}
			}
		}
	}
	
	iTarget = g_iGrabData[id][GRABBED]; // Кого держим
	
	//If they've grabbed something
	if (iTarget > 0)
	{
		if (!pev_valid(iTarget) || (pev(iTarget, pev_health) < 1 && pev(iTarget, pev_max_health)))
		{
			ClCmd_GrabOff(id);
			return PLUGIN_CONTINUE;
		}

		new iButton = get_entvar(id, var_button);
		new iOldButton = get_entvar(id, var_oldbuttons);

		if (~iOldButton & IN_USE && iButton & IN_USE)
			grab_menu_key_use(id);
		
		else if (~iOldButton & IN_ATTACK && iButton & IN_ATTACK)
			grab_menu_key_attack(id);
		
		else if (~iOldButton & IN_ATTACK2 && iButton & IN_ATTACK2)
			grab_menu_key_attack2(id);

		else if(~iOldButton & IN_RELOAD && iButton & IN_RELOAD && jbe_is_user_valid(iTarget))
			grab_menu_key_reload(id);

		//Push and pull
		if (g_iGrabData[id][GRAB_FLAGS] & CDF_IN_PULL) // Притягиваем
			grab_do_pull(id);
		
		else if (g_iGrabData[id][GRAB_FLAGS] & CDF_IN_PUSH) // Отдаляем
			grab_do_push(id);
		
		if (iTarget > MaxClients)
			grab_think(id);
	}
		
	return PLUGIN_CONTINUE;
}

//Grabs the client and teleports them to the admin
public force_grab(id) // public force_grab(id, level, cid)
{
	if(IsNotSetBit(g_iBitUserGrab, id)) return PLUGIN_HANDLED;
	
	new arg[33];
	read_argv(1, arg, 32);

	new targetid = cmd_target(id, arg, 1);

	if (!targetid) return PLUGIN_HANDLED;
	
	if (grab_is_grabbed(targetid, id)) return PLUGIN_HANDLED;

	if (IsNotSetBit(g_iBitUserAlive, targetid))
	{
		client_print_color(id, 0, "%s Игрок ^4%n ^1мертв.", g_szGrabPrefix, targetid);
		return PLUGIN_HANDLED;
	}
	if (id == targetid)
	{
		client_print_color(id, 0, "%s Нельзя брать себя грабом.", g_szGrabPrefix);
		return PLUGIN_HANDLED;
	}

	SetBit(g_iBitGrabUse, id);
	check_player_pre_think();
	
	//Safe to tp target to aim spot?
	new Float:tmpvec[3], Float:orig[3], Float:torig[3], Float:trace_ret[3];
	new bool:safe = false, i;
	
	velocity_by_aim(id, GRAB_MIN_DIST, tmpvec);

	for (new j = 1; j < 11 && !safe; j++) {
		torig[0] = orig[0] + tmpvec[i] * j;
		torig[1] = orig[1] + tmpvec[i] * j;
		torig[2] = orig[2] + tmpvec[i] * j;
		
		grab_traceline(tmpvec, torig, id, trace_ret);
		
		if (get_distance_f(trace_ret, torig)) break;
				
		engfunc(EngFunc_TraceHull, torig, torig, 0, HULL_HUMAN, 0, 0);
		if (!get_tr2(0, TR_StartSolid) && !get_tr2(0, TR_AllSolid) && get_tr2(0, TR_InOpen))
			safe = true;
	}
	
	//Still not safe? Then find another safe spot somewhere around the grabber
	pev(id, pev_origin, orig);
	new try[3];
	orig[2] += 2;
	while (try[2] < 3 && !safe) {
		for (i = 0; i < 3; i++) {
			switch (try[i]) {
				case 0 : torig[i] = orig[i] + (i == 2 ? 80 : 40);
				case 1 : torig[i] = orig[i];
				case 2 : torig[i] = orig[i] - (i == 2 ? 80 : 40);
			}
		}
		grab_traceline(tmpvec, torig, id, trace_ret);
		
		engfunc(EngFunc_TraceHull, torig, torig, 0, HULL_HUMAN, 0, 0);
		if (!get_tr2(0, TR_StartSolid) && !get_tr2(0, TR_AllSolid)
		&& get_tr2(0, TR_InOpen) && !get_distance_f(trace_ret, torig))
			safe = true;
		
		try[0]++;
		if (try[0] == 3) {
			try[0] = 0;
			try[1]++;
			if (try[1] == 3) {
				try[1] = 0;
				try[2]++;
			}
		}
	}

	if (safe) 
	{
		set_pev(targetid, pev_origin, torig);
		grab_set_grabbed(id, targetid);
	} 
	else 
	{
		//client_print(id, print_chat, "[AMXX]  !safe");
		client_print_color(id, 0, "%s Не удается взять игрока %n.", g_szGrabPrefix, targetid);
		ClearBit(g_iBitGrabUse, id);
		check_player_pre_think();
	}
	return PLUGIN_HANDLED;
}


/*==================================================
 #			<- GRAB <-
====================================================*/