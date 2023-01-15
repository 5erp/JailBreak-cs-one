// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

#include "jbe_core\p_models\p_models_menu.inl"

/*===== -> Персональные модели -> =====*///{

personal_models_init()
{
	register_menucmd(register_menuid("Show_PersonalModelsMenu"),
		(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Handle_PersonalModelsMenu");

	register_clcmdex("models", "ClCmd_Model", 1); // say /models
}

public ClCmd_Model(id)
{
	if (IsSetBit(g_iBitUserModelsPack, id) || g_iPersonalModelsUserSize[id])
	{
		return Show_PersonalModelsMenu(id);
	}
	return ClCmd_InfoBuy(id);
}

is_not_personal_models_menu(id)
{
	if (IsNotSetBit(g_iBitUserModelsPack, id) && !g_iPersonalModelsUserSize[id])
		ClCmd_InfoBuy(id);

	else if (g_iPersonalModelsAllSize <= 1) {}

	else return false;

	return true;
}

jbe_personal_models_read_file()
{
	g_aDataPersonalModels = ArrayCreate(DATA_PERSONAL_MODELS);
	
	new szCfgDir[64], szCfgFile[128];
	get_localinfo("amxx_configsdir", szCfgDir, charsmax(szCfgDir));
	formatex(szCfgFile, charsmax(szCfgFile), "%s/jb_engine/personal_models.ini", szCfgDir);
	
	new fp = fopen(szCfgFile, "rt");
	
	if (!fp) {
		log_amx("File ^"addons/amxmodx/configs/jb_engine/personal_models.ini^" not found");
		return -1;
	}
	
	new szBuffer[256];
	new szModel[64];
	new aDataPersonalModels[DATA_PERSONAL_MODELS];

	// 0 - model none
	ArrayPushArray(g_aDataPersonalModels, aDataPersonalModels);
	
	while (!feof(fp))
	{
		fgets(fp, szBuffer, charsmax(szBuffer));
		trim(szBuffer);

		if (!szBuffer[0] || szBuffer[0] == ';') continue;

		parse(szBuffer, 
			aDataPersonalModels[FLAG_PM],		charsmax(aDataPersonalModels[FLAG_PM]),
			aDataPersonalModels[MODEL_PM],		charsmax(aDataPersonalModels[MODEL_PM]),
			aDataPersonalModels[SUB_MODEL_PM],	charsmax(aDataPersonalModels[SUB_MODEL_PM]),
			aDataPersonalModels[NAME_PM],		charsmax(aDataPersonalModels[NAME_PM])
		);

		formatex(szModel, charsmax(szModel), "models/player/%s/%s.mdl",
			aDataPersonalModels[MODEL_PM], aDataPersonalModels[MODEL_PM]);

		if (file_exists(szModel))
		{
			ArrayPushArray(g_aDataPersonalModels, aDataPersonalModels);
			engfunc(EngFunc_PrecacheModel, szModel);
		}
		else
		{
			new szDate[9], szLogFile[64];
			get_time("%Y/%m/%d", szDate, charsmax(szDate));
			formatex(szLogFile, charsmax(szLogFile), "jb_engine_%s.log", szDate);
			log_to_file(szLogFile, "[Personal models] Not File: %s", szModel);
		}
	}
	g_iPersonalModelsAllSize = ArraySize(g_aDataPersonalModels);
	
	for (new i = 1; i <= MaxClients; i++) // создание аррай под всех игроков под модели
		g_aPersonalModelsUser[i] = ArrayCreate();

	return fclose(fp);
}

get_user_personal_models(id)
{
	if (IsSetBit(g_iBitUserModelsPack, id))
	{
		g_iPersonalModel[id] = -1;
		return;
	}
#if !defined DEVELOPER_OPTIONS
	else
	{
		new AdminReason[33];
		if (!cmsapi_is_admin_active(id, AdminReason, charsmax(AdminReason), _))
			return;

		if (!g_aPersonalModelsUser[id])
			return;
		
		new aDataPersonalModels[DATA_PERSONAL_MODELS];
		for (new i; i < g_iPersonalModelsAllSize; i++)
		{
			ArrayGetArray(g_aDataPersonalModels, i, aDataPersonalModels);
		//	iFlag & aDataPersonalModels[FLAG_PM][0]
		//	iFlag & aDataPersonalModels[FLAG_PM][1]
		//	a_pm1
			if (cmsapi_get_user_services(id, "", aDataPersonalModels[FLAG_PM], 0))
			{
				ArrayPushCell(g_aPersonalModelsUser[id], i);
				
				if (!g_iPersonalModel[id])
					g_iPersonalModel[id] = i;
			}
		}

		if (g_aPersonalModelsUser[id])
			g_iPersonalModelsUserSize[id] = ArraySize(g_aPersonalModelsUser[id]);
		else
			g_iPersonalModelsUserSize[id] = 0;
	}
#endif
	
	// Если не нашли модель игрока (зачем? :D)
//	if (!g_iPersonalModel[id])
//		g_iPersonalModel[id] = -1;
}

/*===== <- Персональные модели <- =====*///{