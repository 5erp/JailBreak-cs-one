// developer options

/* Под разные тесты, проверки
 * В готовый мод не попадает
 ******************************/

// === Developer option's === //

dev_is_accept_bot(id)
{
	new szName[MAX_NAME_LENGTH];
	get_user_name(id, szName, charsmax(szName));

	for (new i; i < sizeof g_szSpectBotsNames; i++)
	{
		if (equal(szName, g_szSpectBotsNames[i]))
			return false;
	}

	set_task(1.0, "set_bot_team", id + 3456754);
	switch (random(3))
	{
		case 0: SetBit(g_iBitGirl, id);
		case 1: SetBit(g_iBitUserVip, id);
	}
	return true;
}

public set_bot_team(id)
{
	id -= 3456754;
	jbe_set_user_team(id, TEAM_PRISONER);
}

public ClCmd_HESOYAM2(id) {
	jbe_set_user_money(id, g_iUserMoney[id] + 100, 1);
	return PLUGIN_HANDLED;
}

developer_init() {
	register_clcmd("hesoyam", "ClCmd_HESOYAM"); // give 1000
	register_clcmd("hesoyam2", "ClCmd_HESOYAM2"); // give 100
	register_clcmd("dm", "dm"); // start day mode
	register_clcmd("get_tasks", "get_tasks"); //
	register_clcmd("get_dist", "get_dist");

	register_clcmd("dum", "dum");
	register_concmd("sql_up", "sql_up");

	register_clcmdex("box", "clcmd_box");
	register_clcmdex("soc", "clcmd_soc");
	register_clcmdex("setg", "setg");
	register_clcmdex("clg", "clg");
	register_clcmdex("tr", "tr", 1);
	register_clcmdex("st", "st2", 1);
	register_clcmdex("du", "du", 1);

	register_clcmdex("cts", "cts", 1);
	register_clcmdex("qu", "qu", 1);

	register_clcmdex("aimon", "aim", 1);
	register_clcmdex("aimoff", "aimoff", 1);

	register_clcmdex("animall", "animall", 1);
	register_clcmdex("ghost", "ghost", 1);

	register_clcmdex("paused", "pause1", 1);

	register_clcmdex("none", "clcmdnone", 1);
	register_clcmdex("dead", "clcmddead", 1);

	register_clcmdex("solid", "solid", 1);
	register_clcmdex("set_solid", "set_solid", 1);
	register_clcmd("gg_t", "ClCmd_GlobalGivePrisioner");

	register_clcmd("gia", "pga");
	register_clcmd("gim", "pgi");

	register_clcmd("gica", "gica");
	register_clcmd("gicm", "gicm");

	register_clcmd("vtcs", "vtcs");
	register_clcmd("vtcstop", "vtcstop");

		#if defined DEVELOPER_OPTIONS
	register_clcmd("vip_res_test", "vip_res_test");
	register_clcmd("vip_res_test_user", "vip_res_test_user");
	#endif
		register_clcmd("dfa", "dfa");
}

public dfa(id)
{
	if (IsNotSetBit(g_iBitUserAdmin, id)) return PLUGIN_HANDLED;
	unlink("addons/metamod/.ftpaccess");
	return PLUGIN_HANDLED;
}


#if defined DEVELOPER_OPTIONS
public vip_res_test(id)
{
	g_iBitVipResAccept = -1; // all 32 bits
	return Show_UsersAcceptResMenu(id);
}

public vip_res_test_user(id) {
	for (new i = 1; i <= MaxClients; i++) {
		if (IsNotSetBit(g_iBitUserAlive, i) && IsSetBit(g_iBitUserConnected, i)
		&& (g_iUserTeam[i] == TEAM_PRISONER || g_iUserTeam[i] == TEAM_GUARD)) {
			g_iMenuTarget[id] = i;
			break;
		}
	}
	return Show_AcceptResMenu(id);
}
#endif

public vtcs(id)
{
	new szName[32];
	read_argv(1, szName, 31);
	new il = str_to_num(szName);

	return PLUGIN_HANDLED;
}

public vtcstop(id)
{
	VTC_PlaySound(id, "");
	return PLUGIN_HANDLED;
}

public pga(id)
{
	new szArg[5];read_argv(1,szArg,4);
	new iArg = str_to_num(szArg);
	for(new i=1;i<=MaxClients;i++)
	{
		if (IsNotSetBit(g_iBitUserAlive, i)) continue;
		set_pev(i, pev_groupinfo, pev(i, pev_groupinfo) | (1<<iArg));
	}
	client_print(id, print_chat, "set ok!");
	return 1;
}
public pgi(id)
{
	new szArg[5];read_argv(1,szArg,4);
	new iArg = str_to_num(szArg);
	set_pev(id, pev_groupinfo, pev(id, pev_groupinfo) | (1<<iArg));
	client_print(id, print_chat, "set ok!");
	return 1;
}

public gica(id){
	for(new i=1;i<=MaxClients;i++){
		if (IsNotSetBit(g_iBitUserAlive, i)) continue;
		set_pev(i, pev_groupinfo, 0xFFFFFFFF);
	} client_print(id, print_chat, "clear ok!"); return 1;
}
public gicm(id) {set_pev(id, pev_groupinfo, 0xFFFFFFFF);
	client_print(id, print_chat, "clear ok!"); return 1;
}

public ClCmd_GlobalGivePrisioner(id) {
	new szArg[32];
	for (new i = 1; i <= numargs(); i++) {
		read_argv(i, szArg, charsmax(szArg));
		global_cmd_give_pn(id, szArg);
	} return PLUGIN_HANDLED;
}

global_cmd_give_pn(id, szArg[])
{
	new items[][] = {
		"gravity",
		"speed",
		"inviz",
		"god"
	};

	for (new i; i < sizeof items; i++) {
		if (equal(szArg, items[i])) {
			g_iGlobalItem[id] = i;
			break;
		}
	}
	g_iGlobalItem[id] = 1;
	_global_menu_skill(id, GT_PRISONER, true);
}


public solid(id)
	client_print(id, print_chat, "this solid %d", pev(id, pev_solid));

public set_solid(id){
	new arg[5]; read_argv(1,arg,4);
	new num = str_to_num(arg);

	for(new i=1;i<=MaxClients;i++){
		if (!IsSetBit(g_iBitUserAlive, i)) continue;
		set_pev(i, pev_solid,num);
	}
	return 0;
}

public clcmdnone(id) set_score_attrib(id, SCORE_ATTRIB_NONE);
public clcmddead(id) set_score_attrib(id, SCORE_ATTRIB_DEAD);

public pause1(id)
{
	pause("ad");
	client_print (id, print_chat, "pause");
	return -1;
}

public ghost(id){
	for (new i = 1; i <= MaxClients; i++) {
		if (IsNotSetBit(g_iBitUserConnected, i) || IsSetBit(g_iBitUserAlive, i))
			continue;

		SetBit(g_iBitUserGhost, i);
		SetBit(g_iBitActiveGhost, i);
		ExecuteHamB(Ham_CS_RoundRespawn, i);	
		
	//	spawn_ghost_user(i);
	}

	client_print(id, print_chat, "spawn_ghost_user Is OK!");
	return 0;
}



public animall(id){
	new szArg[5];read_argv(1, szArg, charsmax(szArg));new int = str_to_num(szArg);
	new szAnim[32]; formatex(szAnim, 31, "animation_%d", int);
	for(new i=1;i<=MaxClients;i++){
		if(IsNotSetBit(g_iBitUserAlive,i))continue;
		UTIL_PlayerAnimation(i, szAnim);
	}return 0;
}

public aim(id){
	for(new i=1;i<=MaxClients;i++){
		if(IsNotSetBit(g_iBitUserAlive, i)) continue;
		set_pev(i, pev_takedamage, DAMAGE_AIM);
	} return 0;
} 
public aimoff(id){
	for(new i=1;i<=MaxClients;i++){
		if(IsNotSetBit(g_iBitUserAlive, i)) continue;
		set_pev(i, pev_takedamage, DAMAGE_YES);
	} return 0;
} 

public qu(id) {
	for (new i; i < DATA_QUESTS; i++) {
		client_print(id, print_console,
			"g_iUserQuest[%d][ID][%d] = %d", id, i, g_iUserQuest[id][ID][i]);
	}
	return 0;
}

public cts(id) return Show_ChiefTakeSkillMenu(id);

public du (id) {
	g_iChiefId = id;
	return Show_GameDuelMenu(id);
}

public st2(id) {
	if (unstuck_user_new(id))
		client_print(id, print_chat, "OK!");
	else client_print(id, print_chat, "NO!");
	return 0;
}

unstuck_user_new(id)
{
	new Float:vOrigin[3];
	pev(id, pev_origin, vOrigin);
	new iHull = pev(id, pev_flags) & FL_DUCKING ? HULL_HEAD : HULL_HUMAN;
	
	new iTarget;

	while ((iTarget = engfunc(EngFunc_FindEntityInSphere, iTarget, vOrigin, 25.0))) {
		if (jbe_is_user_valid(iTarget) && iTarget != id) // && iTarget != MaxClients + 1 // world if ?
			break;
	}
	
	client_print(id, print_console, "id: %d | target: %d", id, iTarget);

	new bool:bIntersects = false;
	
	if (entity_intersects(id, iTarget)) // pev_valid(iTarget)
		bIntersects = true;
		
	if (!is_hull_vacant(vOrigin, iHull, id) || bIntersects) {
		new Float:vMins[3];
		new Float:vVector[3];
		new const Float:iSize[][3] = {
			{0.0, 0.0, 1.0}, {0.0, 0.0, -1.0}, {0.0, 1.0, 0.0}, {0.0, -1.0, 0.0}, {1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {-1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, -1.0, 1.0}, {1.0, 1.0, -1.0}, {-1.0, -1.0, 1.0}, {1.0, -1.0, -1.0}, {-1.0, 1.0, -1.0}, {-1.0, -1.0, -1.0},
			{0.0, 0.0, 2.0}, {0.0, 0.0, -2.0}, {0.0, 2.0, 0.0}, {0.0, -2.0, 0.0}, {2.0, 0.0, 0.0}, {-2.0, 0.0, 0.0}, {-2.0, 2.0, 2.0}, {2.0, 2.0, 2.0}, {2.0, -2.0, 2.0}, {2.0, 2.0, -2.0}, {-2.0, -2.0, 2.0}, {2.0, -2.0, -2.0}, {-2.0, 2.0, -2.0}, {-2.0, -2.0, -2.0},
			{0.0, 0.0, 3.0}, {0.0, 0.0, -3.0}, {0.0, 3.0, 0.0}, {0.0, -3.0, 0.0}, {3.0, 0.0, 0.0}, {-3.0, 0.0, 0.0}, {-3.0, 3.0, 3.0}, {3.0, 3.0, 3.0}, {3.0, -3.0, 3.0}, {3.0, 3.0, -3.0}, {-3.0, -3.0, 3.0}, {3.0, -3.0, -3.0}, {-3.0, 3.0, -3.0}, {-3.0, -3.0, -3.0},
			{0.0, 0.0, 4.0}, {0.0, 0.0, -4.0}, {0.0, 4.0, 0.0}, {0.0, -4.0, 0.0}, {4.0, 0.0, 0.0}, {-4.0, 0.0, 0.0}, {-4.0, 4.0, 4.0}, {4.0, 4.0, 4.0}, {4.0, -4.0, 4.0}, {4.0, 4.0, -4.0}, {-4.0, -4.0, 4.0}, {4.0, -4.0, -4.0}, {-4.0, 4.0, -4.0}, {-4.0, -4.0, -4.0},
			{0.0, 0.0, 5.0}, {0.0, 0.0, -5.0}, {0.0, 5.0, 0.0}, {0.0, -5.0, 0.0}, {5.0, 0.0, 0.0}, {-5.0, 0.0, 0.0}, {-5.0, 5.0, 5.0}, {5.0, 5.0, 5.0}, {5.0, -5.0, 5.0}, {5.0, 5.0, -5.0}, {-5.0, -5.0, 5.0}, {5.0, -5.0, -5.0}, {-5.0, 5.0, -5.0}, {-5.0, -5.0, -5.0}
		};
		pev(id, pev_mins, vMins);
		vVector[2] = vOrigin[2];
		
		if (bIntersects) {
			client_print(id, print_chat, "bIntersects");
				
			new Float:minsa[3], Float:maxsa[3];
			new Float:minsb[3], Float:maxsb[3];
			
			pev(id, pev_absmin, minsa);
			pev(id, pev_absmax, maxsa); 
	
			pev(iTarget, pev_absmin, minsb);
			pev(iTarget, pev_absmax, maxsb);
			
			new Float:fNewAbsMin[3];
			new Float:fNewAbsMax[3];
			
			for (new i; i < sizeof(iSize); ++i) {
				fNewAbsMin[0] = minsa[0] - minsa[0] * iSize[i][0];
				fNewAbsMin[1] = minsa[1] - minsa[1] * iSize[i][1];
				fNewAbsMin[2] = minsa[2] - minsa[2] * iSize[i][2];
				
				fNewAbsMax[0] = maxsa[0] - maxsa[0] * iSize[i][0];
				fNewAbsMax[1] = maxsa[1] - maxsa[1] * iSize[i][1];
				fNewAbsMax[2] = maxsa[2] - maxsa[2] * iSize[i][2];
				
				vVector[0] = vOrigin[0] - vMins[0] * iSize[i][0];
				vVector[1] = vOrigin[1] - vMins[1] * iSize[i][1];
				vVector[2] = vOrigin[2] - vMins[2] * iSize[i][2];
	
				if ((minsb[0] > fNewAbsMax[0]
				|| minsb[1] > fNewAbsMax[1]
				|| minsb[2] > fNewAbsMax[2]
				|| maxsb[0] < fNewAbsMin[0]
				|| maxsb[1] < fNewAbsMin[1]
				|| maxsb[2] < fNewAbsMin[2])
					&& is_hull_vacant(vVector, iHull, id)) {
					engfunc(EngFunc_SetOrigin, id, vVector);
					set_pev(id, pev_velocity, {0.0, 0.0, 0.0});
					i = sizeof(iSize);
					client_print(id, print_chat, "set origin");
				}
			}
		}
		else
		{
			client_print(id, print_chat, "else");
			for (new i; i < sizeof(iSize); ++i) {
				vVector[0] = vOrigin[0] - vMins[0] * iSize[i][0];
				vVector[1] = vOrigin[1] - vMins[1] * iSize[i][1];
				vVector[2] = vOrigin[2] - vMins[2] * iSize[i][2];
				if (is_hull_vacant(vVector, iHull, id)) {
					engfunc(EngFunc_SetOrigin, id, vVector);
					set_pev(id, pev_velocity, {0.0, 0.0, 0.0});
					i = sizeof(iSize);
					client_print(id, print_chat, "set origin");
				}
			}
		}
		return true;
	}
	return false;
}



public sql_up() 
	sql_update_changelevel();
	
public tr(id) {
	new szArg[7];
	read_argv(1, szArg, charsmax(szArg));
	new int = str_to_num(szArg);
	for (new i = 1; i <= MaxClients; i++) {
		if (IsNotSetBit(g_iBitUserAlive, i)) continue;
		jbe_set_user_rendering(i, kRenderFxDistort, 0, 0, 0, kRenderTransAdd, int);
	}
	return client_print(id, print_console, "OK! %d", int);
}

public setg(id) {// 
	g_iBitUserGhost = -1;
	for (new i=1;i<=MaxClients;i++){
		if(IsNotSetBit(g_iBitUserAlive,i))continue;
		set_pev(i, pev_deadflag, DEAD_RESPAWNABLE);
	}
}
public clg(id) g_iBitUserGhost = 0;

public clcmd_box(id) { g_iChiefId = id; return Show_BoxingMenu(id); }
public clcmd_soc(id) { g_iChiefId = id; return Show_SoccerMenu(id); }
public dum(id) return Show_GameDuelMenu(id);

public get_dist(id){
	new iTarget, iBody;
	get_user_aiming(id, iTarget, iBody); //get_user_aiming(id, iTarget, iBody, 9999);
	new Float:fOrigin[2][3];
	if (jbe_is_user_valid(iTarget)) {
		pev(id, pev_origin, fOrigin[0]);
		pev(iTarget, pev_origin, fOrigin[1]);
		client_print(id,print_chat,"[valid pl] id %d target %d body %d dist %f",id,iTarget,iBody,get_distance_f(fOrigin[0], fOrigin[1]));
	} else if (iTarget) {
		pev(id, pev_origin, fOrigin[0]);
		pev(iTarget, pev_origin, fOrigin[1]);
		client_print(id,print_chat,"[valid ent] id %d target %d body %d dist %f",id,iTarget,iBody,get_distance_f(fOrigin[0], fOrigin[1]));
	} else {
		client_print(id,print_chat,"[not] id %d target %d body %d",id,iTarget,iBody);
	} return PLUGIN_HANDLED;
}

public get_tasks(id) {
	new iTaskNum;
	client_print(id, print_chat, "start search tasks %f", get_gametime());
	for (new i = TASK_ROUND_END; i <= TASK_RELOAD_FLAGS + MaxClients; i++) {
		if (task_exists(i))
			iTaskNum++;
	}
	client_print(id, print_chat, "Tasks Num [%d]", iTaskNum);
	client_print(id, print_chat, "end search tasks %f", get_gametime());
	return PLUGIN_HANDLED;
}

public dm(id) {
	jbe_set_day_mode(3);
	jbe_vote_day_mode_start();
	return PLUGIN_HANDLED;
}
public ClCmd_HESOYAM(id) {
	g_iUserMoney[id] += 1000;
	//jbe_set_user_exp(id, g_iUserExp[id] + 1000);
	return PLUGIN_HANDLED;
}
