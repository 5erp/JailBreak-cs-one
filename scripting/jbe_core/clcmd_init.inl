/*===== -> Консольные команды -> =====*///{
clcmd_init()
{
	for (new i, szBlockCmd[][] = {"jointeam", "joinclass"}; i < sizeof szBlockCmd; i++)
		register_clcmd(szBlockCmd[i], "ClCmd_Block");
		
	new szBuyCommands[][] = {
		"usp", "glock", "deagle", "p228", "elites", "fn57", "m3", "xm1014", "mp5", "tmp", "p90",
		"mac10", "ump45", "ak47", "galil", "famas", "sg552", "m4a1", "aug", "scout", "awp", "g3sg1",
		"sg550", "m249", "vest", "vesthelm", "flash", "hegren", "sgren", "defuser", "nvgs", "shield",
		"primammo", "secammo", "km45", "9x19mm", "nighthawk", "228compact", "fiveseven", "12gauge", "autoshotgun",
		"mp", "c90", "cv47", "defender", "clarion", "krieg552", "bullpup", "magnum", "d3au1", "krieg550",
		"buy", "buyequip", "cl_autobuy", "cl_rebuy", "cl_setautobuy", "cl_setrebuy", "client_buy_open" };
	for (new j; j < sizeof szBuyCommands; j++)
		register_clcmd(szBuyCommands[j], "ClCmd_Buy");
	
	register_clcmd("chooseteam", "ClCmd_ChooseTeam");
	register_clcmd("nightvision", "ClCmd_NightVision");
	register_clcmd("menuselect", "ClCmd_MenuSelect");
	register_clcmd("buyammo1", "ClCmd_BuyAmmoPrimary");
	register_clcmd("buyammo2", "ClCmd_BuyAmmoSecondary");
	register_clcmd("money_transfer", "ClCmd_MoneyTransfer");
	register_clcmdex("menu", "ClCmd_ChooseTeam", 1);

	register_clcmd("radio1", "ClCmd_Radio1"); // Одежда КТ
	register_clcmd("radio2", "ClCmd_Radio2"); // Достать нож
	register_clcmd("radio3", "ClCmd_Radio3"); // Обыск и Открытие дверей
	register_clcmd("drop", "ClCmd_Drop");

//	new szRadioCommands[][] = {
//		"coverme", "takepoint",
//	};
//	for (new l; l < sizeof szRadioCommands; l++)
//		register_clcmd(szRadioCommands[l], "ClCmd_RadioBlock");

	register_clcmd("coverme","ClCmd_RadioBlock");
	register_clcmd("takepoint","ClCmd_RadioBlock");
	register_clcmd("holdpos","ClCmd_RadioBlock");
	register_clcmd("regroup","ClCmd_RadioBlock");
	register_clcmd("followme","ClCmd_RadioBlock");
	register_clcmd("takingfire","ClCmd_RadioBlock");
	register_clcmd("go","ClCmd_RadioBlock");
	register_clcmd("fallback","ClCmd_RadioBlock");
	register_clcmd("sticktog","ClCmd_RadioBlock");
	register_clcmd("getinpos","ClCmd_RadioBlock");
	register_clcmd("stormfront","ClCmd_RadioBlock");
	register_clcmd("report","ClCmd_RadioBlock");
	register_clcmd("roger","ClCmd_RadioBlock");
	register_clcmd("enemyspot","ClCmd_RadioBlock");
	register_clcmd("needbackup","ClCmd_RadioBlock");
	register_clcmd("sectorclear","ClCmd_RadioBlock");
	register_clcmd("inposition","ClCmd_RadioBlock");
	register_clcmd("reportingin","ClCmd_RadioBlock");
	register_clcmd("getout","ClCmd_RadioBlock");
	register_clcmd("negative","ClCmd_RadioBlock");
	register_clcmd("enemydown","ClCmd_RadioBlock");

	register_clcmdex("adminka", "ClCmd_InfoBuy", 1); // say /adminka
	register_clcmdex("admin", "ClCmd_InfoBuy", 1); // say /admin
	register_clcmdex("vip", "ClCmd_InfoBuy", 1); // say /vip
	register_clcmdex("vipka", "ClCmd_InfoBuy", 1); // say /vipka
	
	register_clcmdex("priv", "ClCmd_PrivilegiesMenu", 1); // say /priv
	register_clcmdex("rs", "ClCmd_ResetScore"); // say /rs
	register_clcmdex("stuck", "ClCmd_Stuck"); // say /stuck
	register_clcmdex("lr", "ClCmd_Lr"); // say /lr
	register_clcmdex("cam", "ClCmd_Cam"); // say /cam
	
	register_clcmdex("setskin", "ClCmd_SetSkin", 1); // меню установки скинов say /setskin
	register_clcmdex("ss", "ClCmd_SetSkin", 1); // меню установки скинов say /ss
	register_clcmdex("weapon", "ClCmd_GrWeapon"); // /weapon
	
	register_concmd("amx_reloadadmins", "CocCmd_ReloadAdmin", ADMIN_RCON);
	register_concmd("amx_kick", "cmdKick", ADMIN_KICK, "<name or #userid> [reason]");

	/* Paint */
	register_clcmd("+paint", "ClCmd_PaintOn");
	register_clcmd("-paint", "ClCmd_PaintOff");

	register_clcmdex("cmds", "ClCmd_MotdCmds");
	register_clcmdex("rules", "ClCmd_ShowRules"); // /rules
	register_clcmdex("hi", "ClCmd_Hello"); // say /hi
	
	register_clcmdex("anim", "ClCmd_Anim"); // say /anim
	register_clcmdex("offhats", "ClCmd_OffCostumes"); // say /offhats
	
	// команды dev
	register_clcmd("get_my_origin", "dev_cmd_get_origin", ADMIN_IMMUNITY, "user origin, angles, v_angle");
	register_clcmd("get_entity_id", "dev_cmd_get_entity_id"); //
	register_clcmd("use_entity_id", "dev_cmd_use_entity_id", ADMIN_IMMUNITY); //

	register_clcmd("sqll", "sqll");
}

public sqll(id)
{
	if (IsNotSetBit(g_iBitUserAdmin, id))
		return PLUGIN_HANDLED;
	load_user_mysql(id + TASK_LOAD_PLAYER_SQL);
	client_print(id, print_console, "Load start");
	return PLUGIN_HANDLED;
}

public ClCmd_RadioBlock(id) return PLUGIN_HANDLED;

public ClCmd_OffCostumes(id)
{
	InvertBit(g_iBitOffCostumes, id);
	jbe_reset_user_groupinfo(id);

	client_print_color(id, print_team_blue,
		"%s ^1Вы ^3%s ^1видимость костюмов.", 
		g_szChatPrefix, IsSetBit(g_iBitOffCostumes, id) ? "отключили" : "включили");
	
	return PLUGIN_HANDLED;
}

public ClCmd_Cam(id)
{
	if (IsNotSetBit(g_iBitUser3DView, id))
		return client_print_color(id, id, "%s Ошибка, у вас не включен вид от 3-го лица.", g_szChatPrefix);

	jbe_set_user_view(id, 0);

	client_print_color(0, id, "%s ^3%n ^1отключил вид от 3-го лица.", g_szChatPrefix, id);

	return PLUGIN_HANDLED;
}

public ClCmd_MotdCmds(id) {
	return show_motd(id, g_szMotdCmds);
}

public ClCmd_TreatPrisonerMenu(id)
{
	if (g_iChiefId == id || IsSetBit(g_iBitUserAdmin, id))
	{
		Show_TreatPrisonerMenu(id, _, 1);
	}
	else
	{
		ClCmd_InfoBuy(id);
	}
	
	return PLUGIN_HANDLED;
}

public ClCmd_InfoBuy(id)
{
	client_print_color(id, print_team_blue, "%s ^1Купить привилегии ^3онлайн ^1можно на нашем сайте: ^4CS-One.Ru", g_szChatPrefix);
	client_print_color(id, print_team_blue, "%s ^4Buy ^3Admin ^1/ ^3Vip ^1/ ^3Hook ^1/ ^3Grab ^1: ^4CS-One.Ru", g_szChatPrefix);
	return PLUGIN_HANDLED;
}

public ClCmd_Block(id) return PLUGIN_HANDLED;

public ClCmd_ChooseTeam(id)
{
	if (jbe_menu_blocked(id))
		return PLUGIN_HANDLED;

	switch (g_iUserTeam[id])
	{
		case 1, 2:
		{
			if (IsSetBit(g_iBitOpenMainMenu, id))
			{
				show_menu(id, 0, "^n");
				jbe_informer_offset_down(id);
				return PLUGIN_HANDLED;
			}
				
			Show_MainMenu(id);
		}
		default: Show_ChooseTeamMenu(id, 0);
	}
	return PLUGIN_HANDLED;
}

public ClCmd_MenuSelect(id)
{
	if (IsNotSetBit(g_iBitUserConnected, id))
		return;

	new iMenuOld, iMenuNew;
	if (player_menu_info(id, iMenuOld, iMenuNew) && iMenuNew > -1)
		return;

	jbe_informer_offset_down(id);
}

public ClCmd_MoneyTransfer(id, iTarget, iMoney)
{
	if (IsSetBit(g_iBitUserDuel, id))
	{
		client_print_color(id, 0, "%s Дождитесь окончания дуэли для перевода денег.", g_szChatPrefix);
		return PLUGIN_HANDLED;
	}

	if (is_user_cmd_flood(id, FLOOD_MONEY, 20.0, 40.0))
		return PLUGIN_HANDLED;

	if (!iTarget)
	{
		new szArg1[3], szArg2[7];
		read_argv(1, szArg1, charsmax(szArg1));
		read_argv(2, szArg2, charsmax(szArg2));
		if (!is_str_num(szArg1) || !is_str_num(szArg2))
		{
			client_print_color(id, 0, "^1Вы ввели ^3неверные параметры ^1операции, попробуйте ещё раз.");
			return PLUGIN_HANDLED;
		}
		iTarget = str_to_num(szArg1);
		iMoney = str_to_num(szArg2);
	}
	if (id == iTarget || !jbe_is_user_valid(iTarget) || IsNotSetBit(g_iBitUserConnected, iTarget))
		client_print_color(id, 0, "^3Недействительный индекс ^1получателя, попробуйте ещё раз.");
	
	else if (g_iUserMoney[id] < iMoney)
	{
		client_print_color(id, 0, "%s ^1На вашем счету ^3недостаточно средств ^1для перевода.", g_szChatPrefix);
		UTIL_BlinkAcct(id, 5);
	}
	else if (iMoney <= 0)
		client_print_color(id, 0, "%s ^3Минимальная ^1сумма для перевода^4 1$^1.", g_szChatPrefix);
	else
	{
		jbe_set_user_money(iTarget, g_iUserMoney[iTarget] + iMoney, 1);
		jbe_set_user_money(id, g_iUserMoney[id] - iMoney, 1);
		client_print_color(0, id, "%s ^3%n ^1перевёл ^4%d$ ^1на счёт ^4%n^1.", g_szChatPrefix, id, iMoney, iTarget);
	}
	return PLUGIN_HANDLED;
}

public ClCmd_Radio1(id)
{
	if (g_iUserTeam[id] == TEAM_PRISONER && IsSetBit(g_iBitClothingGuard, id))
	{
		if (IsSetBit(g_iBitUserSoccer, id) || IsSetBit(g_iBitUserBoxing, id))
			client_print_color(id, id, "%s ^1Одежда охранника ^3заблокирована ^1во время проведения игр.", g_szChatPrefix);
		else
		{
			if (IsSetBit(g_iBitClothingType, id))
			{
				jbe_default_player_model(id);
				client_print_color(id, id, "%s ^1Вы ^3сняли ^1одежду охранника.", g_szChatPrefix);
			}
			else
			{
				jbe_set_player_guard_model(id);
				client_print_color(id, id, "%s ^1Вы ^3одели ^1одежду охранника.", g_szChatPrefix);
			}
			InvertBit(g_iBitClothingType, id);
		}
	}
	return PLUGIN_HANDLED;
}

public ClCmd_Radio2(id)
{
	if (g_iUserTeam[id] == TEAM_PRISONER && get_user_weapon(id) == CSW_KNIFE
	&& (IsSetBit(g_iBitBalisong, id) || IsSetBit(g_iBitBasaballbat, id) || IsSetBit(g_iBitGuardBaton, id)))
	{
		if (IsSetBit(g_iBitUserSoccer, id) || IsSetBit(g_iBitUserBoxing, id) || IsSetBit(g_iBitUserDuel, id))
		{
			client_print_color(id, id, "%s ^1Использование оружия ^3запрещено ^1во время проведения игр.", g_szChatPrefix);
			return PLUGIN_HANDLED;
		}

		if (Float:get_member(id, m_flNextAttack) < 0.1)
		{
			new iActiveItem = get_member(id, m_pActiveItem);
			if (iActiveItem > 0)
			{
				InvertBit(g_iBitWeaponStatus, id);
				ExecuteHamB(Ham_Item_Deploy, iActiveItem);
				UTIL_WeaponAnimation(id, 3);
			}
		}
	}
	return PLUGIN_HANDLED;
}

public ClCmd_Radio3(id)
{
	if (IsNotSetBit(g_iBitUserAlive, id))
		return PLUGIN_HANDLED;

	if (g_iUserTeam[id] == TEAM_GUARD || IsSetBit(g_iBitLatchkey, id)
	|| g_iDayMode == DAY_DAYMODE || g_iDayMode == DAY_DEATHMATCH || g_iLastPnId == id)
	{
		new iTarget, iBody;
		new iDistance = g_iUserTeam[id] == TEAM_PRISONER ? 30 : 60;
		get_user_aiming(id, iTarget, iBody, iDistance);

		if (jbe_is_user_valid(iTarget) && IsSetBit(g_iBitUserAlive, iTarget)
		&& g_iUserTeam[id] == TEAM_GUARD)
		{
			if (g_iChiefId != id && IsNotSetBit(g_iBitUserVip, id))
				client_print_color(id, id, "%s ^1Обыскивать может только ^4Начальник ^1и ^4VIP^1.", g_szChatPrefix);
			
			else if (g_iUserTeam[iTarget] != TEAM_PRISONER)
				client_print_color(id, id, "%s ^1Обыскивать можно только ^4заключённых^1.", g_szChatPrefix);
			
			else if (g_iDayMode != DAY_NORMAL)
				client_print_color(id, id, "%s ^1Обыскивать можно только в ^4обычный день^1.", g_szChatPrefix);
			
			else
			{
				new iWeapon = pev(iTarget, pev_weapons);
				if (iWeapon &= ~(1<<CSW_HEGRENADE|1<<CSW_SMOKEGRENADE|1<<CSW_FLASHBANG|1<<CSW_KNIFE|1<<31))
					client_print_color(id, print_team_red,
						"%s ^1У заключённого ^3обнаружено ^1огнестрельное оружие.", g_szChatPrefix);

				else
					client_print_color(id, print_team_blue,
						"%s ^1У заключённого ^3отсутствует ^1огнестрельное оружие.", g_szChatPrefix);
				
				client_print_color(iTarget, id, "%s ^4%n ^1Обыскал вас на предмет оружия!", g_szChatPrefix, id);
				UTIL_PlayerAnimation(iTarget, "so_am_I"); // анимация для обыска :D
			}
		}
		else if (pev_valid(iTarget) == 2)  // && !jbe_is_user_valid(iTarget)
		{
			new szClassName[32];
			pev(iTarget, pev_classname, szClassName, charsmax(szClassName));
			if (szClassName[5] == 'd' && szClassName[6] == 'o' && szClassName[7] == 'o' && szClassName[8] == 'r')
			{
				dllfunc(DLLFunc_Use, iTarget, id);
				client_cmd(id, "spk buttons/latchlocked1.wav"); // звук использования отмычки
				ClearBit(g_iBitLatchkey, id);
			}
		}
		else
		{
			switch (g_iUserTeam[id])
			{
				case TEAM_PRISONER:
					client_print_color(id, id, "%s ^1Ошибка использования ^3отмычки^1, дверь не найдена.", g_szChatPrefix);
			
				case TEAM_GUARD:
					client_print_color(id, id, "%s ^1Чтобы ^4обыскать заключенного ^1или ^4открыть дверь^1, \
						подойдите и ^3наведите прицел^1.", g_szChatPrefix);
			}
		}
	}
	return PLUGIN_HANDLED;	
}

public ClCmd_Drop(id)
{
	if (IsSetBit(g_iBitGrabUse, id))
	{
		if (g_iGrabData[id][GRABBED] > 0)
		{
			new Float:vecVelocity[3];
			velocity_by_aim(id, GRAB_THROW_FORCE, vecVelocity);
			set_entvar(g_iGrabData[id][GRABBED], var_velocity, vecVelocity);
			ClCmd_GrabOff(id);
			return PLUGIN_HANDLED;
		}
	}
	
	else if (g_iChiefGameType)
	{
		if (BIT(g_iChiefGameType) & CHIEF_ALL_GLOBAL_GAMES)
			return PLUGIN_HANDLED;
	}
	
	else if (IsSetBit(g_iBitUserDuel, id))
		return PLUGIN_HANDLED;
	
	return PLUGIN_CONTINUE;
}

public ClCmd_PrivilegiesMenu(id)
{
	return Show_PrivilegiesMenu(id);
}

public ClCmd_ResetScore(id) {
	if (is_user_cmd_flood(id))
		return PLUGIN_HANDLED;
	
	if (IsSetBit(g_iBitUserConnected, id)) {
		client_cmd(id, "spk buttons/blip1.wav");
		client_print_color(id, print_team_red, "%s ^1Ваш ^3счет ^1успешно обнулен.", g_szChatPrefix);
		set_entvar(id, var_frags, 0.0);
		set_member(id, m_iDeaths, 0);
		UTIL_UserScoreMessage(id, 0, 0);
	}
	return PLUGIN_HANDLED;
}

public ClCmd_NightVision(id)
{
	if (IsSetBit(g_iBitBlockMenu, id))
		return PLUGIN_HANDLED;
	
	if (g_iUserTeam[id] == TEAM_SPECTATOR || g_iUserTeam[id] == TEAM_UNASSIGNED)
		return Show_ChooseTeamMenu(id, 0);

	if (g_iDuelStatus == DS_READY && IsNotSetBit(g_iBitUserBet, id)
	&& g_iDuelUsersId[0] != id && g_iDuelUsersId[1] != id)
		return Show_BetMenu(id);

	if (id == g_iChiefId)
		return Show_ChiefMenu(id);

	if (id == g_iLastPnId)
		return Show_LastPrisonerMenu(id);

	if (IsSetBit(g_iBitUserAlive, id))
		return Show_RankBonusMenu(id);

	if (g_iDayMode == DAY_NORMAL || g_iDayMode == DAY_FREEDAY)
		return Show_DeathUserMenu(id);

	return PLUGIN_HANDLED;
}

public ClCmd_Buy(id)
{
	new szUserInfo[3];
	get_user_info(id, "_vgui_menus", szUserInfo, charsmax(szUserInfo));

	if (!szUserInfo[0] || str_to_num(szUserInfo) != 0)
	{
		message_begin(MSG_ONE, MsgId_BuyClose, _, id);
		message_end();

		client_print_color(id, print_team_blue,
		 "^1Введите в консоль ^3^"^4setinfo _vgui_menus 0^3^" \
			^1для отключения VGUI меню CS.");
	}

	if (IsSetBit(g_iBitBlockMenu, id) || is_not_shop_menu(id))
		return PLUGIN_HANDLED;

	return ClCmd_Shop(id);
}

public ClCmd_BuyAmmoPrimary(id) return cmd_user_buy_ammo(id, .bIsPrimary = true);
public ClCmd_BuyAmmoSecondary(id) return cmd_user_buy_ammo(id, .bIsPrimary = false);

stock cmd_user_buy_ammo(id, bool:bIsPrimary)
{
#define AMMO_PRIMARY 1509749160
#define AMMO_SECONDARY 67308546
	
	if ((g_iUserTeam[id] == TEAM_PRISONER && g_iDayMode != DAY_DEATHMATCH) 
	|| is_not_shop_menu(id) || g_iChiefGameType && (g_iChiefGameType == GAME_SHTURM 
	|| g_iChiefGameType == GAME_M9SO || g_iChiefGameType == GAME_GHOST))
		return PLUGIN_HANDLED;

	// Для покупки патронов охранной и зеками дезматч
	enum _:
	{
		MAXBPAMMO,
		OFFSET_AMMO,
		BUYAMMO,
		AMMOCOST,
		AMMOTYPE
	}

	new const g_iszBuyAmmo[][] = {
		{ -1, 0, -1, 0, "" }, // N--
		{ 52, _:ammo_357sig, 13, 3, "357sig" }, // p228
		{ -1, 0, -1, 0, "" }, // N--
		{ 90, _:ammo_762nato, 30, 4, "762nato" }, // scout
		{ 1, 0, -1, 0, "" }, // N--
		{ 32, _:ammo_buckshot, 8, 3, "buckshot" }, // xm1014
		{ 1, 0, -1, 0, "" }, // N--
		{ 100, _:ammo_45acp, 12, 1, "45acp" }, // mac10
		{ 90, _:ammo_556nato, 30, 3, "556nato" }, // aug
		{ 1, 0, -1, 0, "" }, // N--
		{ 120, _:ammo_9mm, 30, 2, "9mm" }, // elite
		{ 100, _:ammo_57mm, 50, 3, "57mm" }, // fiveseven
		{ 100, _:ammo_45acp, 12, 1, "45acp" }, // ump45
		{ 90, _:ammo_556nato, 30, 3, "556nato" }, // sg550
		{ 90, _:ammo_556nato, 30, 3, "556nato" }, // galil
		{ 90, _:ammo_556nato, 30, 3, "556nato" }, // famas
		{ 100, _:ammo_45acp, 12, 1, "45acp" }, // usp
		{ 120, _:ammo_9mm, 30, 2, "9mm" }, // glock18
		{ 30, _:ammo_338mag, 10, 6, "338magnum" }, // awp
		{ 120, _:ammo_9mm, 30, 2, "9mm" }, // mp5navy
		{ 200, _:ammo_556natobox, 30, 3, "556natobox" }, // m249
		{ 32, _:ammo_buckshot, 8, 3, "buckshot" }, // m3
		{ 90, _:ammo_556nato, 30, 3, "556nato" }, // m4a1
		{ 120, _:ammo_9mm, 30, 2, "9mm" }, // tmp
		{ 90, _:ammo_762nato, 30, 4, "762nato" }, // g3sg1
		{ 2, 0, -1, 0, "" }, // N--
		{ 35, _:ammo_50ae, 7, 2, "50ae" }, // deagle
		{ 90, _:ammo_556nato, 30, 3, "556nato" }, // sg552
		{ 90, _:ammo_762nato, 30, 4, "762nato" }, // ak47
		{ -1, 0, -1, 0, "" }, // N--
		{ 100, _:ammo_57mm, 50, 3, "57mm" } // p90
	};
	
//	new const SOUND_BUY[] = "items/9mmclip1.wav"; - убран звук unprecacher'ом
	new iAmmoType;

	if (bIsPrimary) iAmmoType = AMMO_PRIMARY;
	else iAmmoType = AMMO_SECONDARY;
	
	new szWeapons[32], iNum, i, iCurrentAmmo, iWeapon, bRefilled = false;
	get_user_weapons(id, szWeapons, iNum);
	new iCurrentAmmoValue, iCurrentAmmoCost, iCurrentAmmoMax;
	for (i = 0; i < iNum; i++)
	{
		iWeapon = szWeapons[i];
		if ((1<<iWeapon) & iAmmoType)
		{
			iCurrentAmmoValue = g_iszBuyAmmo[iWeapon][BUYAMMO];
			iCurrentAmmoCost = g_iszBuyAmmo[iWeapon][AMMOCOST];
			iCurrentAmmoMax = g_iszBuyAmmo[iWeapon][MAXBPAMMO];
			if (g_iUserMoney[id] < iCurrentAmmoCost)
			{
				if (get_member(id, g_iszBuyAmmo[iWeapon][OFFSET_AMMO]) < iCurrentAmmoMax)
				{
					UTIL_TextMsg(id, "#Not_Enough_Money");
					UTIL_BlinkAcct(id, 5);
				}
				return PLUGIN_HANDLED;
			}
			iCurrentAmmo = get_member(id, g_iszBuyAmmo[iWeapon][OFFSET_AMMO]);
			ExecuteHamB(Ham_GiveAmmo, id, iCurrentAmmoValue, g_iszBuyAmmo[iWeapon][AMMOTYPE], iCurrentAmmoMax);
			if (get_member(id, g_iszBuyAmmo[iWeapon][OFFSET_AMMO]) - iCurrentAmmo > 0) 
				bRefilled = true;
			break;
		}
	}
	if (!bRefilled)
	{
		for (i = 0; i < iNum; i++)
		{
			iWeapon = szWeapons[i];
			if ((1<<iWeapon) & iAmmoType)
				UTIL_TextMsg( id, "#Cannot_Carry_Anymore" );
		}
		return PLUGIN_HANDLED;
	}
	jbe_set_user_money(id, g_iUserMoney[id] - iCurrentAmmoCost, 1);
//	emit_sound(id, CHAN_ITEM, SOUND_BUY, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	return PLUGIN_HANDLED;
}

public ClCmd_Stuck(id)
{
	if (IsNotSetBit(g_iBitUserAlive, id) || is_user_cmd_flood(id))
		return PLUGIN_HANDLED;

	if (IsNotSetBit(g_iBitUserBuried, id) && !rg_get_user_noclip(id)
	&& !(pev(id,pev_solid) & SOLID_NOT) && unstuck_user(id))
		client_cmd(id, "spk fvox/blip.wav");
	else
		client_print_color(id, 0, "%s ^1Вы не застряли!", g_szChatPrefix);

	return PLUGIN_HANDLED;
}

unstuck_user(id)
{
	new Float:vOrigin[3];
	get_entvar(id, var_origin, vOrigin);
	new iHull = get_entvar(id, var_flags) & FL_DUCKING ? HULL_HEAD : HULL_HUMAN;
	if (!is_hull_vacant(vOrigin, iHull, id))
	{
		new Float:vMins[3];
		new Float:vVector[3];
		new const Float:iSize[][3] = {
			{0.0, 0.0, 1.0}, {0.0, 0.0, -1.0}, {0.0, 1.0, 0.0}, {0.0, -1.0, 0.0}, {1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {-1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, -1.0, 1.0}, {1.0, 1.0, -1.0}, {-1.0, -1.0, 1.0}, {1.0, -1.0, -1.0}, {-1.0, 1.0, -1.0}, {-1.0, -1.0, -1.0},
			{0.0, 0.0, 2.0}, {0.0, 0.0, -2.0}, {0.0, 2.0, 0.0}, {0.0, -2.0, 0.0}, {2.0, 0.0, 0.0}, {-2.0, 0.0, 0.0}, {-2.0, 2.0, 2.0}, {2.0, 2.0, 2.0}, {2.0, -2.0, 2.0}, {2.0, 2.0, -2.0}, {-2.0, -2.0, 2.0}, {2.0, -2.0, -2.0}, {-2.0, 2.0, -2.0}, {-2.0, -2.0, -2.0},
			{0.0, 0.0, 3.0}, {0.0, 0.0, -3.0}, {0.0, 3.0, 0.0}, {0.0, -3.0, 0.0}, {3.0, 0.0, 0.0}, {-3.0, 0.0, 0.0}, {-3.0, 3.0, 3.0}, {3.0, 3.0, 3.0}, {3.0, -3.0, 3.0}, {3.0, 3.0, -3.0}, {-3.0, -3.0, 3.0}, {3.0, -3.0, -3.0}, {-3.0, 3.0, -3.0}, {-3.0, -3.0, -3.0},
			{0.0, 0.0, 4.0}, {0.0, 0.0, -4.0}, {0.0, 4.0, 0.0}, {0.0, -4.0, 0.0}, {4.0, 0.0, 0.0}, {-4.0, 0.0, 0.0}, {-4.0, 4.0, 4.0}, {4.0, 4.0, 4.0}, {4.0, -4.0, 4.0}, {4.0, 4.0, -4.0}, {-4.0, -4.0, 4.0}, {4.0, -4.0, -4.0}, {-4.0, 4.0, -4.0}, {-4.0, -4.0, -4.0},
			{0.0, 0.0, 5.0}, {0.0, 0.0, -5.0}, {0.0, 5.0, 0.0}, {0.0, -5.0, 0.0}, {5.0, 0.0, 0.0}, {-5.0, 0.0, 0.0}, {-5.0, 5.0, 5.0}, {5.0, 5.0, 5.0}, {5.0, -5.0, 5.0}, {5.0, 5.0, -5.0}, {-5.0, -5.0, 5.0}, {5.0, -5.0, -5.0}, {-5.0, 5.0, -5.0}, {-5.0, -5.0, -5.0}
		};
		get_entvar(id, var_mins, vMins);
		vVector[2] = vOrigin[2];
		for (new i; i < sizeof(iSize); ++i)
		{
			vVector[0] = vOrigin[0] - vMins[0] * iSize[i][0];
			vVector[1] = vOrigin[1] - vMins[1] * iSize[i][1];
			vVector[2] = vOrigin[2] - vMins[2] * iSize[i][2];
			if (is_hull_vacant(vVector, iHull, id))
			{
				set_entvar(id, var_origin, vVector);
				set_entvar(id, var_velocity, {0.0, 0.0, 0.0});
				i = sizeof(iSize);
			}
		}
		return true;
	}
	return false;
}

public ClCmd_Lr(id)
{
	if (is_not_lastprisoner_menu(id))
		return PLUGIN_HANDLED;

	return Show_LastPrisonerMenu(id);	
}

public ClCmd_Hello(id)
{
	if (IsNotSetBit(g_iBitUserAlive, id)
	|| is_user_cmd_flood(id, FLOOD_HELLO, 15.0, 35.0))
		return PLUGIN_HANDLED;

	new iTarget, iBody;
	get_user_aiming(id, iTarget, iBody, 40);

	if (jbe_is_user_valid(iTarget))
	{
		client_print_color(id, iTarget, "%s ^1Вы поприветствовали ^3%n^1.", g_szChatPrefix, iTarget);

		client_print_color(iTarget, id, "%s ^1Вас поприветствовал ^3%n^1.", g_szChatPrefix, id);
	}

	if (is_player_model_anims(id))
		UTIL_PlayerAnimation(id, "animation_1");
	else
		UTIL_PlayerAnimation(id, "ref_shoot_grenade");

	if (get_user_weapon(id) != CSW_KNIFE)
		engclient_cmd(id, "weapon_knife");

	set_entvar(id, var_viewmodel, model_v_hand);

	UTIL_WeaponAnimation(id, 8);
	set_task(4.0, "emotion_fix_anim", id + TASK_MENU_ANIM);

	return PLUGIN_HANDLED;
}

public ClCmd_SetSkin(id)
{
	if (id != g_iChiefId && IsNotSetBit(g_iBitUserAdmin, id))
		return ClCmd_InfoBuy(id);
		
	return Show_SetSkinMenu(id);
}

public ClCmd_GrWeapon(id)
{
	if (IsNotSetBit(g_iBitUserAlive, id) || g_iUserTeam[id] != TEAM_GUARD
	|| (g_iDayMode != DAY_NORMAL && g_iDayMode != DAY_FREEDAY))
		return PLUGIN_HANDLED; // PLUGIN_CONTINUE
		
	SetBit(g_iBitBuyWeaponsGuard, id);
	return Show_WeaponsGuardMenu(id);
}

public ClCmd_PaintOn(id)
{
	if (IsSetBit(g_iBitUserAdmin, id) || g_iChiefId == id) 
	{
		if (IsNotSetBit(g_iBitUserAlive, id)) 
		{
			client_print_color(id, 0, "%s Недоступно мертвым.", g_szChatPrefix);
			return PLUGIN_HANDLED;
		} 
		else if (g_iDayMode == DAY_DAYMODE) 
		{
			client_print_color(id, 0, "%s Недоступно в игры.", g_szChatPrefix);
			return PLUGIN_HANDLED;
		}
		else if (is_user_cmd_flood(id, .iType = FLOOD_PAINT, .fWhiteTime = 0.5, .fBlockTime = 1.0))
			return PLUGIN_HANDLED;

		SetBit(g_iBitUsePaint, id);
		check_player_pre_think();

		client_print(0, print_console, " ** [Paint] %n Начал рисовать", id);
	}
	else 
		ClCmd_InfoBuy(id);
	
	return PLUGIN_HANDLED;
}

public ClCmd_PaintOff(id) 
{
	if (IsNotSetBit(g_iBitUsePaint, id))
		return PLUGIN_HANDLED;
	
	ClearBit(g_iBitUsePaint, id);
	ClearBit(g_iBitPaintHolding, id);
	check_player_pre_think();

	client_print(0, print_console, " ** [Paint] %n Прекратил рисовать", id);
	
	return PLUGIN_HANDLED;
}

public ClCmd_ShowRules(id)
{
	client_print_color(id, 0, "%s ^1Правила сервера: ^4https://cs-one.ru/rules", g_szChatPrefix);
	client_print(id, print_console, " * [CS-ONE.RU] Правила сервера: https://cs-one.ru/rules");
	return show_motd(id, g_szRulesMOTD);
}


public CocCmd_ReloadAdmin() {
	if (task_exists(TASK_RELOAD_ADMINS))
		change_task(TASK_RELOAD_ADMINS, 5.0);
	else
		set_task(5.0, "fnReloadAdmins", TASK_RELOAD_ADMINS);
}
public fnReloadAdmins() {
	for (new i = 1; i <= MaxClients; i++) {
		if (IsNotSetBit(g_iBitUserConnected, i)) continue;
		reload_flags(i + TASK_RELOAD_FLAGS);
	}
	log_amx("Reload Admins");
}

public cmdKick(id, level, cid)
{
#define MAX_NAME_LENGTH 32
	if (!cmd_access(id, level, cid, 2))
		return PLUGIN_HANDLED;

	new arg[32];
	read_argv(1, arg, charsmax(arg));
	new player = cmd_target(id, arg, CMDTARGET_OBEY_IMMUNITY | CMDTARGET_ALLOW_SELF);
	
	if (!player)
		return PLUGIN_HANDLED;
	
	new szAuth[32], szAuth2[32], szName2[MAX_NAME_LENGTH],
		szName[MAX_NAME_LENGTH], userid2, szReason[32];
	
	get_user_authid(id, szAuth, charsmax(szAuth));
	get_user_authid(player, szAuth2, charsmax(szAuth2));
	get_user_name(player, szName2, charsmax(szName2));
	get_user_name(id, szName, charsmax(szName));
	userid2 = get_user_userid(player);
	read_argv(2, szReason, charsmax(szReason));
	remove_quotes(szReason);
	
	log_amx("Kick: ^"%s<%d><%s><>^" kick ^"%s<%d><%s><>^" (reason ^"%s^")",
		szName, get_user_userid(id), szAuth, szName2, userid2, szAuth2, szReason);

	show_activity_key("ADMIN_KICK_1", "ADMIN_KICK_2", szName, szName2);

	if (is_user_bot(player))
		server_cmd("kick #%d", userid2);
	else
	{
		if (szReason[0])
			server_cmd("kick #%d ^"%s^"", userid2, szReason);
		else
			server_cmd("kick #%d", userid2);
	}
	
	console_print(id, "[AMXX] Client ^"%s^" kicked", szName2);
	
	return PLUGIN_HANDLED;
}

/* Функции гл. админа
==============================================================*/

// для рандомных точек спавна
public dev_cmd_get_origin(id) {
	new Float:fOrigin[3];
	new Float:fAngles[3];
	new Float:vecAngle[3];
	
	pev(id, pev_origin, fOrigin);
	pev(id, pev_angles, fAngles);
	pev(id, pev_v_angle, vecAngle);
	
	client_print(id, print_chat, "%d %d %d %d %d %d %d %d %d", // floatround(fExp, floatround_floor);
		floatround(fOrigin[0]),
		floatround(fOrigin[1]),
		floatround(fOrigin[2]),
		
		floatround(fAngles[0]),
		floatround(fAngles[1]),
		floatround(fAngles[2]),
		
		floatround(vecAngle[0]),
		floatround(vecAngle[1]),
		floatround(vecAngle[2])
	);
	return PLUGIN_HANDLED;
}


public dev_cmd_get_entity_id(id)
{
	if (IsNotSetBit(g_iBitRconAdmin, id))
		return PLUGIN_HANDLED;

	new iEntity, iBody;
	get_user_aiming(id, iEntity, iBody); //get_user_aiming( id, iEntity, iBody, 9999);
	
	if (is_valid_ent(iEntity))
	{
		g_iMenuTarget[id] = iEntity;
		new szModel[5], szClassName[32];
		
		entity_get_string(iEntity, EV_SZ_model, szModel, charsmax(szModel));
		entity_get_string(iEntity, EV_SZ_classname, szClassName, charsmax(szClassName));
		
		client_print(id, print_chat, "entity id: %i | model: %s | name: %s | movetype %d", 
			iEntity, szModel, szClassName, pev(iEntity, pev_movetype));
	}
	return PLUGIN_HANDLED;
}

public dev_cmd_use_entity_id(id, level, cid) {
	if (!cmd_access(id, level, cid, 2))
		return PLUGIN_HANDLED;

	if (!is_valid_ent(g_iMenuTarget[id]))
		return PLUGIN_HANDLED;
	
	force_use(id, g_iMenuTarget[id]);
	
	client_print(id, print_chat, "used entity id: %i", g_iMenuTarget[id]);
	
	return PLUGIN_HANDLED;
}

public ClCmd_Anim(id) return Show_AnimMenu(id);

/*===== <- Консольные команды <- =====*///}