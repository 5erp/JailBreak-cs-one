/*===== -> Модели -> =====*///{

models_precache()
{
	jbe_precache_model_player_cvar("model_prisoner", PRISONER);
	jbe_precache_model_player_cvar("model_guard", GUARD);
	jbe_precache_model_player_cvar("model_chief", CHIEF);
	jbe_precache_model_player_cvar("model_prisoner_girl", PRISONER_GIRL);
	jbe_precache_model_player_cvar("model_guard_girl", GUARD_GIRL);
	jbe_precache_model_player_cvar("model_chief_girl", CHIEF_GIRL);

	jbe_precache_model_cvar("model_p_hand", model_p_hand);
	jbe_precache_model_cvar("model_v_hand", model_v_hand);
	
	jbe_precache_model_cvar("model_p_baton", model_p_baton);
	jbe_precache_model_cvar("model_v_baton", model_v_baton);
	
	jbe_precache_model_cvar("model_v_boxing_gloves_red", model_v_boxing_gloves_red);
	jbe_precache_model_cvar("model_p_boxing_gloves_red", model_p_boxing_gloves_red);
	jbe_precache_model_cvar("model_v_boxing_gloves_blue", model_v_boxing_gloves_blue);
	jbe_precache_model_cvar("model_p_boxing_gloves_blue", model_p_boxing_gloves_blue);
	
	jbe_precache_model_cvar("model_p_balisong", model_p_balisong);
	jbe_precache_model_cvar("model_v_balisong", model_v_balisong);
	jbe_precache_model_cvar("model_v_baseballbat", model_v_baseballbat);
	jbe_precache_model_cvar("model_p_baseballbat", model_p_baseballbat);
	jbe_precache_model_cvar("model_v_syringe", model_v_syringe);
	
	jbe_precache_model_cvar("model_ball", model_ball);
	jbe_precache_model_cvar("model_v_hand_ball", model_v_hand_ball);

	jbe_precache_model_cvar("model_p_bracelets", model_p_bracelets);
	jbe_precache_model_cvar("model_v_bracelets", model_v_bracelets);

	g_pModelGlass = engfunc(EngFunc_PrecacheModel, "models/glassgibs.mdl");
	engfunc(EngFunc_PrecacheModel, "models/rpgrocket.mdl"); // cam
}
/*===== <- Модели <- =====*///}

/*===== -> Звуки -> =====*///{
sounds_precache()
{
	jbe_precache_sound_cvar("sound_hand_hit", sound_hand_hit);
	jbe_precache_sound_cvar("sound_hand_slash", sound_hand_slash);
	jbe_precache_sound_cvar("sound_hand_deploy", sound_hand_deploy);

	jbe_precache_sound_cvar("sound_baton_deploy", sound_baton_deploy);
	jbe_precache_sound_cvar("sound_baton_hitwall", sound_baton_hitwall);
	jbe_precache_sound_cvar("sound_baton_slash", sound_baton_slash);
	jbe_precache_sound_cvar("sound_baton_stab", sound_baton_stab);
	jbe_precache_sound_cvar("sound_baton_hit", sound_baton_hit);

	jbe_precache_sound_cvar("sound_bounce_ball", sound_bounce_ball);
	jbe_precache_sound_cvar("sound_grab_ball", sound_grab_ball);
	jbe_precache_sound_cvar("sound_kick_ball", sound_kick_ball);
	jbe_precache_sound_cvar("sound_whitle_start", sound_whitle_start);
	jbe_precache_sound_cvar("sound_whitle_end", sound_whitle_end);

	jbe_precache_sound_cvar("sound_gloves_hit", sound_gloves_hit);
	jbe_precache_sound_cvar("sound_super_hit", sound_super_hit);
	jbe_precache_sound_cvar("sound_gong", sound_gong);

	jbe_precache_sound_cvar("sound_grenade_frost_explosion", sound_grenade_frost_explosion);
	jbe_precache_sound_cvar("sound_freeze_player", sound_freeze_player);
	jbe_precache_sound_cvar("sound_defrost_player", sound_defrost_player);

	jbe_precache_sound_cvar("sound_balisong_deploy", sound_balisong_deploy);
	jbe_precache_sound_cvar("sound_balisong_hitwall", sound_balisong_hitwall);
	jbe_precache_sound_cvar("sound_balisong_slash", sound_balisong_slash);
	jbe_precache_sound_cvar("sound_balisong_hit", sound_balisong_hit);
	jbe_precache_sound_cvar("sound_syringe_use", sound_syringe_use);

	jbe_precache_sound_cvar("sound_prison_riot", sound_prison_riot);
	jbe_precache_sound_cvar("sound_hook", sound_hook);
	jbe_precache_sound_cvar("sound_freeday_end", sound_freeday_end);
	
	plugins_get_cvar_str("sound_duel_ready", sound_duel_ready);
	if (file_exists(sound_duel_ready))
		precache_generic(sound_duel_ready);
	else
		sound_duel_ready = "blip.wav", precache_sound("blip.wav");

//	for (new i; i <= 10; i++)
//		engfunc(EngFunc_PrecacheSound, fmt("jb_engine/countdown/%d.wav", i));

	new szDirName[SOUND_STRING_SIZE];
	plugins_get_cvar_str("sound_countdown_dir", szDirName);

	for (new i; i <= 10; i++)
	{
		plugins_get_cvar_str("sounds_countdown", sounds_countdown[i], i + 1);
		format(sounds_countdown[i], charsmax(sounds_countdown[]), "%s%s", szDirName, sounds_countdown[i]);

		if (file_exists(fmt("sound/%s", sounds_countdown[i])))
			precache_sound(sounds_countdown[i]);
		else
		{
			log_amx("File not found 'sound/%s'", sounds_countdown[i]);
			sounds_countdown[i] = "blip.wav", precache_sound("blip.wav");
		}
	}
}
/*===== <- Звуки <- =====*///}


/*===== -> Спрайты -> =====*///{
sprites_precache()
{
	new szBuffer[64];
	plugins_get_cvar_str("sprite_wave", szBuffer);
	g_pSpriteWave = engfunc(EngFunc_PrecacheModel, szBuffer);

	plugins_get_cvar_str("sprite_beam", szBuffer);
	g_pSpriteBeam = engfunc(EngFunc_PrecacheModel, szBuffer); // 2 стрелки (траил)

	plugins_get_cvar_str("sprite_ball", szBuffer);
	g_pSpriteBall = engfunc(EngFunc_PrecacheModel, szBuffer);

	plugins_get_cvar_str("sprite_lgthning", szBuffer);
	g_pSpriteLgtning = engfunc(EngFunc_PrecacheModel, szBuffer); // плазма (хук/паинт)

	plugins_get_cvar_str("sprite_richow", szBuffer);
	g_pSpriteRicho2 = engfunc(EngFunc_PrecacheModel, szBuffer);

//	g_pSpriteWave = engfunc(EngFunc_PrecacheModel, "sprites/shockwave.spr");
//	g_pSpriteBeam = engfunc(EngFunc_PrecacheModel, "sprites/arrow1.spr"); // 2 стрелки (траил)
//	g_pSpriteBall = engfunc(EngFunc_PrecacheModel, "sprites/jb_engine/ball.spr");
//	g_pSpriteLgtning = engfunc(EngFunc_PrecacheModel, "sprites/plasma.spr"); // плазма (хук/паинт)
//	g_pSpriteRicho2 = engfunc(EngFunc_PrecacheModel, "sprites/richo2.spr");
}
/*===== <- Спрайты <- =====*///}


/*===== -> Файлы -> =====*///{
files_precache()
{
	new szCfgDir[64], szCfgFile[128];
	get_localinfo("amxx_configsdir", szCfgDir, charsmax(szCfgDir));
	
	formatex(szCfgFile, charsmax(szCfgFile), "%s/jb_engine/round_sound.ini", szCfgDir);
	switch(file_exists(szCfgFile))
	{
		case 0: log_to_file("%s/jb_engine/log_error.log", "File ^"%s^" not found!", szCfgDir, szCfgFile);
		case 1: jbe_round_sound_read_file(szCfgFile);
	}

	formatex(szCfgFile, charsmax(szCfgFile), "%s/jb_engine/costumes.ini", szCfgDir);
	switch(file_exists(szCfgFile))
	{
		case 0: log_to_file("%s/jb_engine/log_error.log", "File ^"%s^" not found!", szCfgDir, szCfgFile);
		case 1: jbe_costumes_read_file(szCfgFile);
	}
}

jbe_round_sound_read_file(szCfgFile[])
{
	new fp = fopen(szCfgFile, "rt");
	if (!fp) return -1;

	g_aDataRoundSound = ArrayCreate(DATA_ROUND_SOUND);
	new aDataRoundSound[DATA_ROUND_SOUND];
	new szBuffer[128];
	
	while (!feof(fp))
	{
		fgets(fp, szBuffer, charsmax(szBuffer));
		trim(szBuffer);
		if (!szBuffer[0] || szBuffer[0] == ';') continue;
		parse(szBuffer,
			aDataRoundSound[FILE_NAME],		charsmax(aDataRoundSound[FILE_NAME]),
			aDataRoundSound[TRACK_NAME],	charsmax(aDataRoundSound[TRACK_NAME])
		);
		formatex(szBuffer, charsmax(szBuffer), "sound/CsOneRu/jb/round_sound/%s.mp3", aDataRoundSound[FILE_NAME]);
		engfunc(EngFunc_PrecacheGeneric, szBuffer);
		ArrayPushArray(g_aDataRoundSound, aDataRoundSound);
	}
	
	g_iRoundSoundSize = ArraySize(g_aDataRoundSound);
	
	return fclose(fp);
}

jbe_costumes_read_file(szCfgFile[])
{
	new fp = fopen(szCfgFile, "rt");
	if (!fp) return -1;

	g_aDataCostumes = ArrayCreate(DATA_COSTUMES_FILE);
	new aDataCostumes[DATA_COSTUMES_FILE];
	new szBuffer[128];
	new szFileBuffer[64];

	aDataCostumes[COSTUME_NAME] = "Снять шапочку";
	ArrayPushArray(g_aDataCostumes, aDataCostumes);

	while (!feof(fp))
	{
		fgets(fp, szBuffer, charsmax(szBuffer));
		trim(szBuffer);
		
		if (!szBuffer[0] || szBuffer[0] == ';' || szBuffer[0] == '/')
			continue;
		
		parse(szBuffer,
			aDataCostumes[COSTUME_NAME], charsmax(aDataCostumes[COSTUME_NAME]),
			aDataCostumes[COSTUME_FILE], charsmax(aDataCostumes[COSTUME_FILE]),
			aDataCostumes[COSTUME_BODY], charsmax(aDataCostumes[COSTUME_BODY]),
			aDataCostumes[COSTUME_ACCESS], charsmax(aDataCostumes[COSTUME_ACCESS])
		);
		
		format(aDataCostumes[COSTUME_FILE], charsmax(aDataCostumes[COSTUME_FILE]), "models/csone/costumes/%s.mdl", aDataCostumes[COSTUME_FILE]);
		
		if (!equal(szFileBuffer, aDataCostumes[COSTUME_FILE]))
		{
			if (file_exists(aDataCostumes[COSTUME_FILE]))
			{
				precache_model(aDataCostumes[COSTUME_FILE]);
			}
			else
			{
				log_amx("[Costumes] File not found '%s'", szBuffer);
				continue;
			}
		}
		else
		{
			szFileBuffer = aDataCostumes[COSTUME_FILE];
		}

		ArrayPushArray(g_aDataCostumes, aDataCostumes);
	}
	
	g_iCostumesListSize = ArraySize(g_aDataCostumes);
	
	return fclose(fp);
}

/* Функция для прекеша с проверкой существования файла и логом при его отсутствии
 * szCvar - квар с которого получаем модель
 * pVar - Переменная куда записать файл
*/
jbe_precache_model_cvar(szCvar[MODEL_STRING_SIZE], pVar[MODEL_STRING_SIZE])
{
	static szBuffer[MODEL_STRING_SIZE];
	plugins_get_cvar_str(szCvar, szBuffer);

	if (file_exists(szBuffer))
	{
		precache_model(szBuffer);
		pVar = szBuffer;
	}
	else
	{
		log_amx("[JBE] File not found: '%s'", szBuffer);
		if (pVar[0] == EOS) pVar = "models/chick.mdl", precache_model("models/chick.mdl"); // курица
	}
}

// грузим модель игрока
jbe_precache_model_player_cvar(szCvar[MODEL_STRING_SIZE], iModelID)
{
	static szBuffer[MODEL_STRING_SIZE];
	plugins_get_cvar_str(szCvar, szBuffer, 1);

	new szFilePatch[64];
	formatex(szFilePatch, charsmax(szFilePatch), "models/player/%s/%s.mdl", szBuffer, szBuffer);

	if (file_exists(szFilePatch))
	{
		precache_model(szFilePatch);
		models_player[iModelID] = szBuffer;
		models_player_body[iModelID] = plugins_get_cvar_int(szCvar, 2);
	}
	else
	{
		log_amx("[JBE] File not found: '%s'", szFilePatch);
		if (models_player[iModelID][0] == EOS) models_player[iModelID] = "arctic";
	}
}

jbe_precache_sound_cvar(szCvar[MODEL_STRING_SIZE], pVar[MODEL_STRING_SIZE])
{
	static szBuffer[MODEL_STRING_SIZE];
	plugins_get_cvar_str(szCvar, szBuffer);

	if (file_exists(fmt("sound/%s", szBuffer)))
	{
		precache_sound(szBuffer);
		pVar = szBuffer;
	}
	else
	{
		log_amx("[JBE] File not found: '%s'", szBuffer);
		if (pVar[0] == EOS) pVar = "fvox/blip.wav", precache_sound("fvox/blip.wav"); // блип
	}
}

stock jbe_precache_sprite_cvar(szCvar[MODEL_STRING_SIZE], pVar[MODEL_STRING_SIZE])
{
	static szBuffer[MODEL_STRING_SIZE];
	plugins_get_cvar_str(szCvar, szBuffer);

	if (file_exists(szBuffer)) // fmt("sprites/%s", szBuffer)
	{
		pVar = szBuffer;
		return precache_model(szBuffer);
	}
	else
	{
		log_amx("[JBE] File not found: '%s'", szBuffer);
		if (pVar[0] == EOS)
		{
			pVar = "sprites/richo2.spr";
			return precache_model("sprites/richo2.spr"); // луч
		}
	}
}

/*===== <- Файлы <- =====*///}