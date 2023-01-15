/*===== -> Двери в тюремных камерах -> =====*///{***
door_init()
{
	g_aDoorList = ArrayCreate();
	new iEntity[2], Float:vecOrigin[3], szClassName[32], szTargetName[32];
	
	while ((iEntity[0] = engfunc(EngFunc_FindEntityByString, iEntity[0], "classname", "info_player_deathmatch")))
	{
		pev(iEntity[0], pev_origin, vecOrigin);

		// 200.0 - уровень охвата сферы в которой ищутся двери
		while((iEntity[1] = engfunc(EngFunc_FindEntityInSphere, iEntity[1], vecOrigin, 200.0)))
		{
			if (!pev_valid(iEntity[1])) continue;

			pev(iEntity[1], pev_classname, szClassName, charsmax(szClassName));
			
			if (szClassName[5] != 'd' && szClassName[6] != 'o' && szClassName[7] != 'o' && szClassName[8] != 'r')
				continue;
			
			if (pev(iEntity[1], pev_iuser1) == IUSER1_DOOR_KEY)
				continue;
			
			pev(iEntity[1], pev_targetname, szTargetName, charsmax(szTargetName));

			if (TrieKeyExists(g_tButtonList, szTargetName))
			{
				set_pev(iEntity[1], pev_iuser1, IUSER1_DOOR_KEY);
				ArrayPushCell(g_aDoorList, iEntity[1]);
				fm_set_kvd(iEntity[1], szClassName, "spawnflags", "0");
				fm_set_kvd(iEntity[1], szClassName, "wait", "-1");
			}
		}
	}

	fix_map_door_file_read();

	g_iDoorListSize = ArraySize(g_aDoorList);
}

fix_map_door_file_read()
{
	new szCfgDir[64], szCfgFile[128], szMapName[64];

	get_localinfo("amxx_configsdir", szCfgDir, charsmax(szCfgDir));
	get_mapname(szMapName, charsmax(szMapName));

	formatex(szCfgFile, charsmax(szCfgFile), "%s/jb_engine/fix_doors/%s.ini", szCfgDir, szMapName);

	new fp = fopen(szCfgFile, "rt");
	if (!fp) return -1;

	new szBuffer[5];
	new szClassName[32];
	new iEntity;
	new iCurrentID;

	if (MaxClients < MAX_PLAYERS)
		iCurrentID = MAX_PLAYERS - MaxClients;

	while (!feof(fp))
	{
		fgets(fp, szBuffer, charsmax(szBuffer));
		trim(szBuffer);
		
		if (!szBuffer[0] || szBuffer[0] == ';'/* || szBuffer[17]*/)
			continue;

		iEntity = str_to_num(szBuffer) - iCurrentID;

		pev(iEntity, pev_classname, szClassName, charsmax(szClassName));
		set_pev(iEntity, pev_iuser1, IUSER1_DOOR_KEY);
		ArrayPushCell(g_aDoorList, iEntity);
		fm_set_kvd(iEntity, szClassName, "spawnflags", "0");
		fm_set_kvd(iEntity, szClassName, "wait", "-1");
	}

	return fclose(fp);
}
/*===== <- Двери в тюремных камерах <- =====*///}