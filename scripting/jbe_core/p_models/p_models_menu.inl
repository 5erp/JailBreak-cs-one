// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

Show_PersonalModelsMenu(id, iPos = 0)
{
	if (iPos < 0 || is_not_personal_models_menu(id))
		return PLUGIN_HANDLED;

	new aDataPersonalModels[DATA_PERSONAL_MODELS];
	new iCount = IsSetBit(g_iBitUserModelsPack, id) ? g_iPersonalModelsAllSize : g_iPersonalModelsUserSize[id] - 1;
	
	if (!menu_list_title("Ваши модели", iPos, iCount))
		menul_item_block("Нет моделей");

	menu_item("^n");

	if (!iPos && menu_list_count()) // menu_list_count - hotfix :(
		menu_list_item("\y1. %sСнять модель^n", g_iPersonalModel[id] == -1 ? "\d" : "\w");
	
	if (IsSetBit(g_iBitUserModelsPack, id))
	{
		while (menu_list_count())
		{
			ArrayGetArray(g_aDataPersonalModels, menu_list_countid(), aDataPersonalModels);
			menu_list_item("\y%d. %s%s^n", menu_list_num(), 
				g_iPersonalModel[id] == menu_list_countid() ? "\d" : "\w",
				aDataPersonalModels[NAME_PM]);
		}
	}
	else
	{
		new iPersonalModel;
		while (menu_list_count())
		{
			iPersonalModel = ArrayGetCell(g_aPersonalModelsUser[id], menu_list_countid());
			ArrayGetArray(g_aDataPersonalModels, iPersonalModel, aDataPersonalModels);
			menu_list_item("\y%d. %s%s^n", menu_list_num(),
				g_iPersonalModel[id] == iPersonalModel ? "\d" : "\w",
				aDataPersonalModels[NAME_PM]);
		}
	}

	return menu_list_show(id, "Show_PersonalModelsMenu");
}

public Handle_PersonalModelsMenu(id, iKey)
{
	switch (iKey) {
		case 8: return Show_PersonalModelsMenu(id, ++g_iMenuPosition[id]);
		case 9: return Show_PersonalModelsMenu(id, --g_iMenuPosition[id]);
	}

	if (!iKey && !g_iMenuPosition[id])
	{
		if (g_iPersonalModel[id] == -1)
			client_print_color(id, print_team_red, "%s ^1У вас уже ^3снята ^1личная модель.", g_szChatPrefix);
		else
		{
			g_iPersonalModel[id] = -1;
			if (IsSetBit(g_iBitUserAlive, id) && IsNotSetBit(g_iBitUserSoccer, id) && g_iDayMode != DAY_DAYMODE)
				jbe_default_player_model(id);
			client_print_color(id, 0, "%s ^1Вы сняли личную модель.", g_szChatPrefix);
		}
		return PLUGIN_HANDLED;
	}

	new iNum = menu_list_itemid(id, iKey);
	new aDataPersonalModels[DATA_PERSONAL_MODELS], iModel;

	if (IsSetBit(g_iBitUserModelsPack, id))
		iModel = iNum;//- 1;
	else
		iModel = ArrayGetCell(g_aPersonalModelsUser[id], iNum);// - 1);
	
	ArrayGetArray(g_aDataPersonalModels, iModel, aDataPersonalModels);
	
	if (g_iPersonalModel[id] == iModel)// + 1)
		client_print_color(id, 0, "%s ^1У вас уже выбрана модель ^4%s", g_szChatPrefix, aDataPersonalModels[NAME_PM]);
	else
	{
		g_iPersonalModel[id] = iModel;// + 1;
		client_print_color(id, 0, "%s ^1Вы выбрали модель ^4%s", g_szChatPrefix, aDataPersonalModels[NAME_PM]);
		if (IsSetBit(g_iBitUserAlive, id) && IsNotSetBit(g_iBitUserSoccer, id) && g_iDayMode != DAY_DAYMODE)
			jbe_default_player_model(id);
	}
	return PLUGIN_HANDLED;
}
