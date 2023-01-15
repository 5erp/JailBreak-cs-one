/*===== -> 'fakemeta' события -> =====*///{
fakemeta_init()
{
	TrieDestroy(g_tButtonList);
	TrieDestroy(g_tRemoveEntities);
	
	unregister_forward(FM_KeyValue, g_iFakeMetaKeyValue, true);
	unregister_forward(FM_Spawn, g_iFakeMetaSpawn, 0);

	register_forward(FM_ClientKill, "FakeMeta_ClientKill");
}

public FakeMeta_KeyValue_Post(iEntity, KVD_Handle)
{
	if (!pev_valid(iEntity))
		return;
	
	new szBuffer[32];
	
	// func_button
	get_kvd(KVD_Handle, KV_ClassName, szBuffer, charsmax(szBuffer));
	if ((szBuffer[5] != 'b' || szBuffer[6] != 'u' || szBuffer[7] != 't')
	&& (szBuffer[0] != 'b' || szBuffer[1] != 'u' || szBuffer[2] != 't'))
		return;
	
	// target
	get_kvd(KVD_Handle, KV_KeyName, szBuffer, charsmax(szBuffer));
	if (szBuffer[0] != 't' || szBuffer[1] != 'a' || szBuffer[3] != 'g')
		return;
	
	get_kvd(KVD_Handle, KV_Value, szBuffer, charsmax(szBuffer));
	TrieSetCell(g_tButtonList, szBuffer, iEntity);
}

public FakeMeta_Spawn(iEntity) 
{
	new szClassName[32];
	pev(iEntity, pev_classname, szClassName, charsmax(szClassName));

	if (TrieKeyExists(g_tRemoveEntities, szClassName))
	{
		if (szClassName[5] == 'u' && pev(iEntity, pev_iuser1) == IUSER1_BUYZONE_KEY) 
			return FMRES_IGNORED;

		forward_return(FMV_CELL, -1);
		return FMRES_SUPERCEDE;
	}

	return FMRES_IGNORED;
}

check_duel_fm_setmodel()
{
	if (g_iDuelType == DT_HEGRENADE && !g_iFakeMetaSetModelDuel)
	{
		g_iFakeMetaSetModelDuel = register_forward(FM_SetModel, "FakeMeta_SetModel_Duel", 0);
	}
	else if (!g_iDuelStatus && g_iFakeMetaSetModelDuel)
	{
		unregister_forward(FM_SetModel, g_iFakeMetaSetModelDuel, 0);
		g_iFakeMetaSetModelDuel = 0;
	}
}

public FakeMeta_SetModel_Duel(iEntity, szModel[])
{
	if (g_iDuelStatus && g_iDuelType == DT_HEGRENADE && strlen(szModel) > 10
	&& (szModel[7] == 'w' && szModel[8] == '_' && szModel[9] == 'h' && szModel[10] == 'e'))
	{
		new iOwner = pev(iEntity, pev_owner);
		if (IsSetBit(g_iBitUserDuel, iOwner))
		{
			CREATE_BEAMFOLLOW(iEntity,
				.pSptite = g_pSpriteBeam, 
				.iLife = 10,
				.iWidth = 10,
				.iRed = random_num(50, 255),
				.iGreen = random_num(50, 255),
				.iBlue = random_num(50, 255),
				.iAlpha = 200
			);
			
			rg_set_user_bpammo(iOwner, WEAPON_HEGRENADE, 100);
		}
	}
}

check_frostnade_fwd()
{
	if (g_iBitFrostNade && !g_iFakeMetaSetModel)
	{
		g_iFakeMetaSetModel = register_forward(FM_SetModel, "FakeMeta_SetModel", 0);
		EnableHamForward(g_iHamHookGrenadeTouch);
	}
	else if (!g_iBitFrostNade && g_iFakeMetaSetModel)
	{
		unregister_forward(FM_SetModel, g_iFakeMetaSetModel, 0);
		g_iFakeMetaSetModel = 0;
		DisableHamForward(g_iHamHookGrenadeTouch);
	}
}

public FakeMeta_SetModel(iEntity, szModel[])
{
	if (g_iBitFrostNade && strlen(szModel) > 10
	&& (szModel[7] == 'w' && szModel[8] == '_' && szModel[9] == 's' && szModel[10] == 'm'))
	{
		new iOwner = pev(iEntity, pev_owner);
		if (IsSetBit(g_iBitFrostNade, iOwner))
		{
			set_pev(iEntity, pev_iuser1, IUSER1_FROSTNADE_KEY);
			ClearBit(g_iBitFrostNade, iOwner);

			CREATE_BEAMFOLLOW(iEntity,
				.pSptite = g_pSpriteBeam, 
				.iLife = 10,
				.iWidth = 10,
				.iRed = 0,
				.iGreen = 110,
				.iBlue = 255,
				.iAlpha = 200
			);
		}
	}
}

public FakeMeta_ClientKill(id)
{
	if (IsSetBit(g_iBitUserGhost, id))
	{
		ClearBit(g_iBitUserGhost, id);
		set_pev(id, pev_deadflag, DEAD_NO);
		user_silentkill(id);
		jbe_reset_user_informer(id);
		show_menu(id, 0, "^n");
	}
	else if (g_bRoundEnd || g_bRestartGame)
	{
		client_print(id, print_console, " * Не доступно.");
		client_print_color(id, print_team_default, " * Не доступно.");

		return FMRES_SUPERCEDE;
	}
	else if (g_fRoundStartTime + 30.0 > get_gametime()) // g_iDayMode != DAY_DEATHMATCH && 
	{
		client_print(id, print_console, " * [CS-One.Ru] Ждите еще %0.f секунд чтоб использовать ^"kill^".",
			g_fRoundStartTime + 30.0 - get_gametime());
		
		client_print_color(id, print_team_red,
			"%s ^1Ждите еще ^4%0.f ^1секунд чтоб использовать ^3^"^4kill^3^"^1.",
			g_szChatPrefix, g_fRoundStartTime + 30.0 - get_gametime());
		
		return FMRES_SUPERCEDE;
	}
	remove_task(id + TASK_SPAWN_FIX);
	return FMRES_IGNORED;
}

/*===== <- 'fakemeta' события <- =====*///}