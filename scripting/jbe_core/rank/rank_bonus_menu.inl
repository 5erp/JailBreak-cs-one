// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

Show_RankBonusMenu(id)
{
	if (is_not_rankbonus_menu(id)) return PLUGIN_HANDLED;
	
	new szReason[RANK_ITEM_REASON_STR];

	menu_title("Ранг бонус^n\dСлед. погоняло %s | осталось %d опыта^n",
	g_iszRankExpName[g_iUserLevel[id] + 1][RANK_NAME],
	g_iszRankExpName[g_iUserLevel[id] + 1][RANK_EXP] - g_iUserExp[id]);
	
	if (g_iUserLevel[id] <= 2)
	{
		if (g_iRankBonus[id][RB_MONEY] >= g_eRankBonusRound[RB_MONEY])
			menu_item("\y1. \wБосяцкий подгон^n");
		else menu_item("\y1. \dБосяцкий подгон \r[%d round]^n", 
			g_eRankBonusRound[RB_MONEY] - g_iRankBonus[id][RB_MONEY]);
	}
	else
	{
		if (jbe_is_access_rank_item(id, RB_MONEY, RB_TYPE_ALL, szReason))
			menu_item("\y1. \wВымутить бабла^n");
		else menu_item("\y1. \dВымутить бабла \r[%s]^n", szReason);
	}

	if (jbe_is_access_rank_item(id, RB_HEALTH, RB_TYPE_ALL, szReason))
		menu_item("\y2. \wСтимулятор^n");
	else menu_item("\y2. \dСтимулятор \r[%s]^n", szReason);

	if (jbe_is_access_rank_item(id, RB_GRENADES, RB_TYPE_PN, szReason))
		menu_item("\y3. \wУкрасть грены^n");
	else menu_item("\y3. \dУкрасть грены \r[%s]^n", szReason);

	if (jbe_is_access_rank_item(id, RB_FREEDAY, RB_TYPE_PN, szReason))
		menu_item("\y4. \wВыходной^n");
	else menu_item("\y4. \dВыходной \r[%s]^n", szReason);

	if (jbe_is_access_rank_item(id, RB_GLOCK, RB_TYPE_PN, szReason))
		menu_item("\y5. \wНамутить глок^n");
	else menu_item("\y5. \dНамутить глок \r[%s]^n", szReason);

	if (jbe_is_access_rank_item(id, RB_EL_COSTUME, RB_TYPE_GR, szReason))
		menu_item("\y6. \wЭлеткро костюм^n");
	else menu_item("\y6. \dЭлеткро костюм \r[%s]^n", szReason);

	if (jbe_is_access_rank_item(id, RB_KOKAIN, RB_TYPE_GR, szReason))
		menu_item("\y7. \wКокаин^n");
	else menu_item("\y7. \dКокаин \r[%s]^n", szReason);

	if (jbe_is_access_rank_item(id, RB_GOD, RB_TYPE_CHIEF, szReason))
		menu_item("\y8. \wБессмертие^n");
	else menu_item("\y8. \dБессмертие \r[%s]^n", szReason);

	menu_item("^n\y9. \wНазад");
	menu_item("^n\y0. \wВыход");

	return menu_show(id, (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9), "Show_RankBonusMenu");
}

public Handle_RankBonusMenu(id, iKey)
{
	if (is_not_rankbonus_menu(id)) return PLUGIN_HANDLED;

	switch (iKey)
	{
		case 0:
		{
			if (g_iUserLevel[id] <= 2)
			{
				if (g_iRankBonus[id][RB_MONEY] >= g_eRankBonusRound[RB_MONEY])
				{
					jbe_set_user_money(id, g_iUserMoney[id] + 20, 1);
					client_print_color(0, id, "%s ^1Новичек ^3%n ^1Взял ^4Босяцкий подгон 20$^1.", 
						g_szRankPrefix, id);
					g_iRankBonus[id][RB_MONEY] = 0;
				}
				else
					client_print_color(id, print_team_default, "%s Будет доступно через %d раундов.",
						g_szChatPrefix, g_eRankBonusRound[RB_MONEY] - g_iRankBonus[id][RB_MONEY]);
			}
			else
			{
				if (is_access_rank_item_nandler(id, RB_MONEY, RB_TYPE_ALL))
				{
					new iMoney = random_num(4, 15);
					jbe_set_user_money(id, g_iUserMoney[id] + iMoney, 1);
					client_print_color(0, id, "%s ^3%n ^1Вымутил ^4%d$^1.", 
						g_szRankPrefix, id, iMoney);
				}
			}
		}
		case 1:
		{
			if (is_access_rank_item_nandler(id, RB_HEALTH, RB_TYPE_ALL))
			{
				jbe_set_syringe_model(id);
				set_task(1.3, "jbe_set_syringe_health", id+TASK_REMOVE_SYRINGE);
				set_task(2.8, "jbe_remove_syringe_model", id+TASK_REMOVE_SYRINGE);
				client_print_color(0, id, "%s ^3%n ^1Взял стимулятор ^4[200HP]^1.", g_szRankPrefix, id);
			}
		}
		case 2:
		{
			if (is_access_rank_item_nandler(id, RB_GRENADES, RB_TYPE_PN))
			{
				rg_give_item(id, "weapon_hegrenade");
				rg_give_item(id, "weapon_smokegrenade");
				rg_give_item(id, "weapon_flashbang");
				client_print_color(0, id, "%s ^3%n ^1Набрал игрушек^1.", g_szRankPrefix, id);
			}
		}
		case 3:
		{
			if (is_not_shop_freeday(id))
				return Show_RankBonusMenu(id);

			if (is_access_rank_item_nandler(id, RB_FREEDAY, RB_TYPE_PN))
			{
				jbe_add_user_free(id);
				client_print_color(0, id, "%s ^3%n ^1Добазарился с начальником ^4Выходной^1.", g_szRankPrefix, id);
			}
		}
		case 4:
		{
			if (is_access_rank_item_nandler(id, RB_GLOCK, RB_TYPE_PN))
			{
				rg_give_item(id, "weapon_glock18");
				client_print_color(0, id, "%s ^3%n ^1Вымутил игрушку^1.", g_szRankPrefix, id);
			}
		}
		case 5:
		{
			if (is_access_rank_item_nandler(id, RB_EL_COSTUME, RB_TYPE_GR))
			{
				SetBit(g_iBitElectroCostume, id);
				client_print_color(0, id, "%s ^3%n ^1Взял элеткро костюм^1.", g_szRankPrefix, id);
				client_print_color(id, print_team_default, "%s Вы взяли электро костюм, зеки ударив вас замирают!", g_szChatPrefix);
			}
		}
		case 6:
		{
			if (is_access_rank_item_nandler(id, RB_KOKAIN, RB_TYPE_GR))
			{
				SetBit(g_iBitKokain, id);
				jbe_set_syringe_model(id);
				set_task(2.8, "jbe_remove_syringe_model", id + TASK_REMOVE_SYRINGE);
				client_print_color(0, id, "%s ^3%n ^1Взял кокаин ^4[dmg/2]^1.", g_szRankPrefix, id);
			}
		}
		case 7:
		{
			if (is_access_rank_item_nandler(id, RB_GOD, RB_TYPE_CHIEF))
			{
				jbe_set_user_godmode(id, true);
				client_print_color(0, id, "%s ^3%n ^1Взял бессмертие^1.", g_szRankPrefix, id);
			}
		}
		case 8: return Show_MainMenu(id);
		case 9: return PLUGIN_HANDLED;
	}
	return PLUGIN_HANDLED;
}

