/*
	Функции выдачи предметов магазина
*/

jbe_shop_balisong(id)
{
	if (IsSetBit(g_iBitBalisong, id))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[BALISONG] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[BALISONG], 1);
		ClearBit(g_iBitBasaballbat, id);
		ClearBit(g_iBitGuardBaton, id);
		
		ClearBit(g_iBitWeaponStatus, id);

		SetBit(g_iBitBalisong, id);

		client_print_color(id, print_team_blue, "%s ^1Чтобы ^3вытащить оружие ^1нажмите ^4'x'^1, чтобы ^3спрятать оружие ^1повторно нажмите ^4'x'^1.", g_szChatPrefix);
		ClCmd_Radio2(id);
	}

	return PLUGIN_HANDLED;
}

jbe_shop_grenades(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (user_has_weapon(id, CSW_FLASHBANG) && user_has_weapon(id, CSW_SMOKEGRENADE))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[GRENADES] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[GRENADES], 1);
		rg_give_item(id, "weapon_hegrenade");
		rg_give_item(id, "weapon_flashbang");
		
	}

	return PLUGIN_HANDLED;
}

jbe_shop_usp(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (user_has_weapon(id, CSW_USP))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[USP] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[USP], 1);
		drop_user_weapons(id, 1);
		rg_give_item(id, "weapon_usp");
	}

	return PLUGIN_HANDLED;
}

jbe_shop_baseballbat(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (IsSetBit(g_iBitBasaballbat, id))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[BASABALLBAT] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[BASABALLBAT], 1);
		ClearBit(g_iBitBalisong, id);
		ClearBit(g_iBitGuardBaton, id);
		ClearBit(g_iBitWeaponStatus, id);

		SetBit(g_iBitBasaballbat, id);

		client_print_color(id, print_team_blue, "%s ^1Чтобы ^3вытащить оружие ^1нажмите ^4'x'^1, чтобы ^3спрятать оружие ^1повторно нажмите ^4'x'^1.", g_szChatPrefix);
		ClCmd_Radio2(id);
	}

	return PLUGIN_HANDLED;
}

jbe_shop_guard_baton(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (IsSetBit(g_iBitGuardBaton, id))
		return client_print_center(id, "Уже есть!");

	if (IsNotSetBit(g_iBitGuardBaton, id) && g_eShopCvars[GUARD_BATON] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[GUARD_BATON], 1);
		ClearBit(g_iBitBalisong, id);
		ClearBit(g_iBitBasaballbat, id);
		
		ClearBit(g_iBitWeaponStatus, id);

		SetBit(g_iBitGuardBaton, id);

		client_print_color(id, print_team_blue, "%s ^1Чтобы ^3вытащить оружие ^1нажмите ^4'x'^1, чтобы ^3спрятать оружие ^1повторно нажмите ^4'x'^1.", g_szChatPrefix);
		ClCmd_Radio2(id);
	}

	return PLUGIN_HANDLED;
}

jbe_shop_frostnade(id)
{
	if (user_has_weapon(id, CSW_SMOKEGRENADE) && IsSetBit(g_iBitFrostNade, id))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[FROSTNADE] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[FROSTNADE], 1);
		SetBit(g_iBitFrostNade, id);
		check_frostnade_fwd();
		rg_give_item(id, "weapon_smokegrenade");
		SetBit(g_iBitBuyFrostNade, id);
	}

	return PLUGIN_HANDLED;
}

jbe_shop_latchkey(id)
{
	if (IsSetBit(g_iBitLatchkey, id))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[LATCHKEY] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[LATCHKEY], 1);
		SetBit(g_iBitLatchkey, id);
		client_print_color(id, id, "%s ^1Чтобы использовать ^3отмычку^1, подойдите к двери и нажмите ^4'c'^1.", g_szChatPrefix);
	}

	return PLUGIN_HANDLED;
}

jbe_shop_kokain(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (IsSetBit(g_iBitKokain, id))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[KOKAIN] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[KOKAIN], 1);
		SetBit(g_iBitKokain, id);
		jbe_set_syringe_model(id);
		client_print_color(id, print_team_blue, "%s ^1Побочные эффекты ^3кокаина^1: получаете в ^3два раза ^1меньше урона.", g_szChatPrefix);
		set_task(2.8, "jbe_remove_syringe_model", id+TASK_REMOVE_SYRINGE);	
	}

	return PLUGIN_HANDLED;
}

jbe_shop_stimulator(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (get_user_health(id) >= 200)
		return client_print_center(id, "Уже есть!");

	if (IsNotSetBit(g_iBitUserBoxing, id) && g_eShopCvars[STIMULATOR] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[STIMULATOR], 1);
		jbe_set_syringe_model(id);
		set_task(1.3, "jbe_set_syringe_health", id+TASK_REMOVE_SYRINGE);
		set_task(2.8, "jbe_remove_syringe_model", id+TASK_REMOVE_SYRINGE);
	}

	return PLUGIN_HANDLED;
}

jbe_shop_armor(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (get_user_armor(id) >= 100)
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[ARMOR] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[ARMOR], 1);
		rg_give_item(id, "item_kevlar");
	}

	return PLUGIN_HANDLED;
}

jbe_shop_cloting_guard(id)
{
	if (IsSetBit(g_iBitClothingGuard, id))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[CLOTHING_GUARD] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[CLOTHING_GUARD], 1);
		SetBit(g_iBitClothingGuard, id);
		client_print_color(id, print_team_blue, "%s ^1Чтобы одеть ^3одежду охраны ^1нажмите ^4'z' ^1чтобы снять одежду, нажмите ещё раз ^4'z'^1.", g_szChatPrefix);
	}

	return PLUGIN_HANDLED;
}

jbe_shop_hing_jump(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (IsSetBit(g_iBitHingJump, id))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[HING_JUMP] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[HING_JUMP], 1);
		SetBit(g_iBitHingJump, id);
		ham_jump_check();
	}

	return PLUGIN_HANDLED;
}

jbe_shop_fast_run(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (IsSetBit(g_iBitFastRun, id))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[FAST_RUN] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[FAST_RUN], 1);
		jbe_set_user_maxspeed(id, 400.0, true);
	}

	return PLUGIN_HANDLED;
}

jbe_shop_double_jump(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (IsSetBit(g_iBitDoubleJump, id))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[DOUBLE_JUMP] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[DOUBLE_JUMP], 1);
		SetBit(g_iBitDoubleJump, id);
		ham_jump_check();
	}

	return PLUGIN_HANDLED;
}

jbe_shop_fall_no_dmg(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (IsSetBit(g_iBitFallNoDamage, id))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[FALL_NO_DMG] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[FALL_NO_DMG], 1);
		SetBit(g_iBitFallNoDamage, id);
	}

	return PLUGIN_HANDLED;
}

jbe_shop_auto_bhop(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (IsSetBit(g_iBitAutoBhop, id))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[AUTO_BHOP] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[AUTO_BHOP], 1);
		SetBit(g_iBitAutoBhop, id);
		ham_jump_check();
	}

	return PLUGIN_HANDLED;
}

jbe_shop_double_damage(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (IsSetBit(g_iBitDoubleDamage, id))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[DOUBLE_DAMAGE] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[DOUBLE_DAMAGE], 1);
		SetBit(g_iBitDoubleDamage, id);
	}

	return PLUGIN_HANDLED;
}

jbe_shop_low_gravity(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (get_entvar(id, var_gravity) > 1.0)
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[LOW_GRAVITY] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[LOW_GRAVITY], 1);
		set_entvar(id, var_gravity, SHOP_GRAVITY_NUM); 
	}

	return PLUGIN_HANDLED;
}

jbe_shop_long_jump(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (IsSetBit(g_iBitLongJump, id))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[LONG_JUMP] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[LONG_JUMP], 1);
		SetBit(g_iBitLongJump, id);
		ham_jump_check();
		client_print_color(id, id, "%s Для длиного прыжка нажмите кнопки сесть+прыгнуть.", g_szChatPrefix);
	}

	return PLUGIN_HANDLED;
}

// Закрыть дело
jbe_shop_slose_case(id)
{
	if (IsNotSetBit(g_iBitUserWanted, id))
		return client_print_center(id, "Вы не в розыске!");

	if (IsSetBit(g_iBitUserWanted, id) && g_eShopCvars[CLOSE_CASE] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[CLOSE_CASE], 1);
		jbe_sub_user_wanted(id);
	}

	return PLUGIN_HANDLED;
}

// Голос
jbe_shop_resolution_voice(id)
{
	if (IsSetBit(g_iBitUserVoice, id))
		return client_print_center(id, "Уже есть!");

	if (IsNotSetBit(g_iBitUserVoice, id) && g_eShopCvars[RESOLUTION_VOICE] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[RESOLUTION_VOICE], 1);
		SetBit(g_iBitUserVoice, id);
		reload_user_voice(id);
	}

	return PLUGIN_HANDLED;
}

// Лотерейный билет
jbe_shop_lottery_ticket(id)
{
	if (IsSetBit(g_iBitLotteryTicket, id))
		return client_print_center(id, "Уже использован билет!");

	if (g_eShopCvars[LOTTERY_TICKET] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[LOTTERY_TICKET], 1);
		SetBit(g_iBitLotteryTicket, id);

		new iPrize;
		switch (random_num(0, 7))
		{
			case 0: iPrize = 10;
			case 2: iPrize = 30;
			case 4: iPrize = 20;
			case 5: iPrize = 5;
		}

		if (iPrize)
		{
			client_print_color(id, id, "%s ^1Поздравляем! Вы выиграли в лотереи ^4%d$^1.", g_szChatPrefix, iPrize);
			jbe_set_user_money(id, g_iUserMoney[id] + iPrize, 1);
		}
		else client_print_color(id, id, "%s ^1К сожалению ваш билет оказался проигрышный.", g_szChatPrefix);
	}

	return PLUGIN_HANDLED;
}

jbe_shop_transfer_guard(id)
{
	if (jbe_is_user_guard_block(id))
	{
		client_print_color(id, 0, "%s Вам заблокирован вход за охрану! Осталось: %s",
			g_szGrBlockPrefix, seconds_to_str(g_iTimeBlockGrEnd[id] - get_systime()));
	
		if (g_iUserTeam[id] != TEAM_PRISONER)
			jbe_set_user_team(id, TEAM_PRISONER);
	
		return PLUGIN_HANDLED;
	}

	if (g_eShopCvars[TRANSFER_GUARD] <= g_iUserMoney[id])
	{
		if (jbe_set_user_team(id, TEAM_GUARD))
			jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[TRANSFER_GUARD], 1);
	}

	return PLUGIN_HANDLED;
}

jbe_shop_free_day(id)
{
	if (jbe_is_shop_time_close(id))
		return PLUGIN_HANDLED;

	if (is_not_shop_freeday(id))
		return Show_ShopPrisonersMenu(id, g_iMenuPosition[id]);

	if (IsSetBit(g_iBitUserFree, id))
		return client_print_center(id, "У вас уже выходной!");

	if (g_iDayMode == DAY_NORMAL && IsNotSetBit(g_iBitUserWanted, id) && g_eShopCvars[FREE_DAY_SHOP] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[FREE_DAY_SHOP], 1);
		jbe_add_user_free(id);
	}

	return PLUGIN_HANDLED;
}

jbe_shop_electro_costume(id)
{
	if (IsSetBit(g_iBitElectroCostume, id))
		return client_print_center(id, "Уже есть!");

	if (g_eShopCvars[ELECTRO_COSTUME] <= g_iUserMoney[id])
	{
		jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[ELECTRO_COSTUME], 1);
		SetBit(g_iBitElectroCostume, id);
		jbe_set_user_rendering(id, kRenderFxGlowShell, 255, 128, 255, kRenderNormal, 0);
		jbe_save_user_rendering(id);
		client_print_color(id, print_team_blue, "%s ^1Вы купили ^3электро костюм^1, зеки ударив вас ^4замирают^1!", g_szChatPrefix);
	}

	return PLUGIN_HANDLED;
}

jbe_shop_weapons_guard(id)
{
	if (g_eShopCvars[WEAPONS_GR] <= g_iUserMoney[id])
	{
		SetBit(g_iBitBuyWeaponsGuard, id);
		return Show_WeaponsGuardMenu(id);
	}

	return PLUGIN_HANDLED;
}