#include "jbe_core\shop\shop_menus.inl"
#include "jbe_core\shop\shop_items.inl"

shop_init()
{
	register_menucmd(register_menuid("Show_ShopPrisonersMenu"), MENU_ALL_KEYS, "Handle_ShopPrisonersMenu");
	register_menucmd(register_menuid("Show_ShopGuardMenu"), MENU_ALL_KEYS, "Handle_ShopGuardMenu");
	register_menucmd(register_menuid("Show_SkinMenu"), MENU_ALL_KEYS, "Handle_SkinMenu");
	register_menucmd(register_menuid("Show_PrankPrisonerMenu"), MENU_ALL_KEYS, "Handle_PrankPrisonerMenu");

	register_clcmdex("shop", "ClCmd_Shop", 1); // say /shop
}

public ClCmd_Shop(id)
{
	if (is_not_shop_menu(id))
		return PLUGIN_HANDLED;

	jbe_shop_open_menu(id);

	return PLUGIN_HANDLED;
}

is_not_shop_menu(id) // if (is_not_shop_menu(id)) return PLUGIN_HANDLED;
{
	if (IsNotSetBit(g_iBitUserAlive, id))
		client_print_color(id, id, "%s Мертвым ^4Магазин ^1недоступен!", g_szChatPrefix);
	
	else if (g_iDayMode == DAY_RESTART || g_iDayMode == DAY_DAYMODE || IsSetBit(g_iBitUserDuel, id))
		client_print_color(id, id, "%s Сейчас ^4Магазин ^1недоступен!", g_szChatPrefix);

	else if (g_iGlobalBlock[GB_SHOP])
		client_print_color(id, id, "%s ^1Магазин ^4заблокирован %s^1!", g_szChatPrefix, global_get_name_block(GB_SHOP));

	else
		return false;

	return true;
}

#define MIN_ALL_PL_FD	5	// мин зеков чтоб купить fd
#define MIN_ALIVE_PL_FD	4	// мин живых зеков чтоб взять фд

bool:is_not_shop_freeday(id)
{
	if (g_iPlayersNum[TEAM_PRISONER] < MIN_ALL_PL_FD)
		client_print_color(id, print_team_default, "%s Выходной можно взять если на сервере от %d зеков!", g_szChatPrefix, MIN_ALL_PL_FD);
	
	else if (g_iAlivePlayersNum[TEAM_PRISONER] < MIN_ALIVE_PL_FD)
		client_print_color(id, print_team_default, "%s Выходной можно взять если живо от %d зеков!", g_szChatPrefix, MIN_ALIVE_PL_FD);
	
	else if (g_fRoundStartTime + 60.0 < get_gametime())
		client_print_color(id, print_team_default, "%s Прошло более минуты с старта раунда, нельзя взять выходной.", g_szChatPrefix);
	
	else if (g_iChiefGameType)
		client_print_color(id, print_team_default, "%s Нельзя брать выходной в игры.", g_szChatPrefix);

	else if (IsSetBit(g_iBitUserWanted, id))
		client_print_color(id, print_team_default, "%s В розыске не доступно.", g_szChatPrefix);

	else if (g_iDayMode != DAY_NORMAL) { /*_*/ }

	else if (g_bRoundEnd) { /*_*/ }

	else
		return false;
	
	return true;
}

jbe_is_shop_time_close(id)
{
	if (g_iUserTeam[id] == TEAM_GUARD)
		return false;

	if (g_iDayMode != DAY_DEATHMATCH && (g_iBitOpenShopInfo
	|| get_gametime() - g_fRoundStartTime > float(g_eShopCvars[SHOP_OPEN_TIME])))
	{
		if (IsNotSetBit(g_iBitOpenShopInfo, id))
		{
			SetBit(g_iBitOpenShopInfo, id);
			client_print_color(id, id, "%s ^1Прошло ^4%s ^1с начала раунда, основной магазин закрыт.",
				g_szChatPrefix, seconds_to_str(g_eShopCvars[SHOP_OPEN_TIME]));
		}
		return true;
	}
		
	return false;
}

jbe_shop_open_menu(id)
{
	switch (g_iUserTeam[id])
	{
		case TEAM_PRISONER: return Show_ShopPrisonersMenu(id);
		case TEAM_GUARD: return Show_ShopGuardMenu(id);
	}
	return PLUGIN_HANDLED;
}

jbe_buy_respawn(id)
{
	if (IsSetBit(g_iBitUserAlive, id))
		return client_print_color(id, print_team_blue, "%s Вы живы!", g_szChatPrefix);

	if (IsSetBit(g_iBitBuyRespawn, id))
		return client_print_color(id, print_team_blue, "%s Вы уже покупали возрождение в этом раунде.", g_szChatPrefix);

	if (g_iAlivePlayersNum[TEAM_PRISONER] < g_eAllCvars[RESPAWN_PLAYER_NUM])
		return client_print_color(id, print_team_blue, "%s Мало живых заключенных.", g_szChatPrefix);

	else if (g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY)
		return 0;
	
	if (g_iGlobalBlock[GB_BUY_RESPAWN])
		return client_print_color(id, print_team_blue, "%s ^1Покупка возрождения заблокированна ^4%s^1.", g_szChatPrefix, global_get_name_block(GB_BUY_RESPAWN));
	
	else if (jbe_is_wanted())
		return client_print_color(id, print_team_blue, "%s ^1Недоступно в розыск.", g_szChatPrefix);
	
	else if (g_iUserTeam[id] == TEAM_SPECTATOR || g_iUserTeam[id] == TEAM_UNASSIGNED)
		return 0;

	else if (is_shief_global_game())
		return client_print_color(id, print_team_blue, "%s ^1Недоступно в игры.", g_szChatPrefix);

	if (get_gametime() - g_fRoundStartTime > (cvar_mp_roundtime * 60.0))
		return client_print_color(id, print_team_blue, "%s Прошло %d минут со старта раунда, возрождение невозможно.", g_szChatPrefix, cvar_mp_roundtime);

	if (g_iUserMoney[id] < g_eShopCvars[BUY_RESPAWN])
		return client_print_color(id, print_team_blue, "%s Недостаточно денег для покупки возрождения.", g_szChatPrefix);

	jbe_set_user_money(id, g_iUserMoney[id] - g_eShopCvars[BUY_RESPAWN], 1);

	rg_round_respawn(id);
	SetBit(g_iBitBuyRespawn, id);
	
	if (g_iUserTeam[id] == TEAM_PRISONER)
		set_entvar(id, var_health, 50.0);

	respawm_set_effects(id);

	client_print_color(0, id, "%s ^3%n ^1купил возрождение. ^4(respawn)", g_szChatPrefix, id);

	return PLUGIN_HANDLED;
}

// ограбление магазина
jbe_rob_shop(id)
{
	if (g_iUserLevel[id] < ROB_SHOP_MIN_LVL)
		client_print_color(id, print_team_blue, "%s Маловат ты еще ^3(доступно с %d уровня)", 
			g_szRobShopPrefix, ROB_SHOP_MIN_LVL);

	else if (IsSetBit(g_iBitRobShop, id))
		client_print_color(id, print_team_blue, "%s Вы уже пытались ограбить магазин.", g_szRobShopPrefix);
	
	else if (g_bRobShop)
		client_print_color(id, print_team_blue, "%s Магазин ограблен, ждите следующий раунд.", g_szRobShopPrefix);
	
	else if ((((abs(g_iRobShopNum - 1) / 3) + 1) > g_iAlivePlayersNum[TEAM_PRISONER]) || g_bRoundEnd)
		client_print_color(id, print_team_blue, "%s Из-за неспокойной обстановки магазин закрыт.", g_szRobShopPrefix);
	
	else if (g_iDayMode != DAY_NORMAL || g_iLastPnId)
		client_print_color(id, print_team_blue, "%s Магазин закрыт на выходной.", g_szRobShopPrefix);
	
	else if (random(11) > 7)
	{
		new iMoney = random_num(5, 30);
		g_bRobShop = true;
		jbe_set_user_money(id, g_iUserMoney[id] + iMoney, 1);
		client_print_color(0, print_team_red, "%s ^4%n ^1ограбил магазин и получил ^4%d^3$^1.",
			g_szRobShopPrefix, id, iMoney);
	}

	else
	{
		jbe_add_user_wanted(id);
		SetBit(g_iBitRobShop, id);
		g_iRobShopNum++;
		emit_sound(id, CHAN_AUTO, sound_prison_riot, 0.5, ATTN_NORM, 0, PITCH_LOW);
		client_print_color(0, print_team_red, "%s ^4%n ^1попытался ограбить магазин и теперь ^3разыскивается^1.",
			g_szRobShopPrefix, id);
	}
	
	return PLUGIN_HANDLED;
}

// продажа гана зеками
jbe_price_gun_prisoner(id)
{
	if ((g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY)
	|| g_iLastPnId || g_bRoundEnd || is_shief_global_game())
	{
		client_print_color(id, id, "%s Сегодня не работаем!", g_szRobShopPrefix);
		return Show_ShopPrisonersMenu(id, g_iMenuPosition[id]);
	}
	
	if (IsSetBit(g_iBitUserWanted, id))
	{
		client_print_color(id, id, "%s Ты в розыске, я не могу у тебя купить ствол!", g_szRobShopPrefix);
		return Show_ShopPrisonersMenu(id, g_iMenuPosition[id]);
	}

	if (IsSetBit(g_iBitPriceGunPn, id))
	{
		client_print_color(id, id, "%s В этом раунде ты уже продавал ствол!", g_szRobShopPrefix);
		return Show_ShopPrisonersMenu(id, g_iMenuPosition[id]);
	}

	new iWeapon = get_user_weapon(id);
	if (iWeapon == CSW_KNIFE)
	{
		client_print_color(id, id, "%s Возьмите в руки оружие которое хотите продать!", g_szChatPrefix);
		return Show_ShopPrisonersMenu(id, g_iMenuPosition[id]);
	}

	new iMoney;
	switch (iWeapon)
	{
		case CSW_AUG, CSW_GALIL, CSW_FAMAS, CSW_M4A1, CSW_AK47, CSW_SG552: iMoney = random_num(9, 12);
		case CSW_M249: iMoney = random_num(10, 12);
		case CSW_SCOUT, CSW_AWP, CSW_G3SG1, CSW_SG550: iMoney = random_num(12, 14);
		case CSW_M3, CSW_XM1014: iMoney = random_num(8, 10);
		case CSW_MAC10, CSW_UMP45, CSW_MP5NAVY, CSW_TMP, CSW_P90: iMoney = random_num(8, 11);
		case CSW_P228, CSW_ELITE, CSW_FIVESEVEN, CSW_USP, CSW_GLOCK18, CSW_DEAGLE: iMoney = random_num(5, 7);
		default:
		{
			client_print_color(id, id, "%s ^1Такое не покупаем, принеси мне нормальный ствол!", g_szRobShopPrefix);
			return Show_ShopPrisonersMenu(id, g_iMenuPosition[id]);
		}
	}

	new iItem = get_member(id, m_pActiveItem);
	if (iItem > -1 && get_member(iItem, m_Weapon_iClip) < get_max_ammo_csw(iWeapon))
	{
		client_print_color(id, id, "%s ^1Пустые пушки ненужны, приходи с полной обоймой!", g_szRobShopPrefix);
		return Show_ShopPrisonersMenu(id, g_iMenuPosition[id]);
	}

	if (random(9) < 3)
	{
		if (IsNotSetBit(g_iBitUserWanted, id))
			jbe_add_user_wanted(id);
		
		emit_sound(id, CHAN_AUTO, sound_prison_riot, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		
		client_print_color(0, id, "%s ^4%n ^1Попытался продать ствол, но его накрыли менты!",
			g_szRobShopPrefix, id);
	} 
	else 
	{
		SetBit(g_iBitPriceGunPn, id);
		new szWeaponName[24];
		get_weaponname(iWeapon, szWeaponName, charsmax(szWeaponName));
		rg_remove_item(id, szWeaponName, .removeAmmo = true);
		set_member(id, m_bHasPrimary, false); // чтоб после продажы можно было поднять ган
		jbe_set_user_money(id, g_iUserMoney[id] + iMoney, 1);
		client_print_color(0, id, "%s ^4%n ^1Удачно продал ствол и получил ^3%d^4$^1.", 
			g_szRobShopPrefix, id, iMoney);
	}
	return PLUGIN_HANDLED;
}