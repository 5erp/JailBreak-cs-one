Show_VipMenu(id)
{	
	new iKeys = (1<<9), iAlive = IsSetBit(g_iBitUserAlive, id);
	menu_title("Вип Меню^n^n");
	
	if (!iAlive && g_iVip[id][DV_RESPAWN] && !jbe_is_wanted()
	&& g_iAlivePlayersNum[TEAM_PRISONER] >= g_eAllCvars[RESPAWN_PLAYER_NUM]
	&& (g_iUserTeam[id] == TEAM_PRISONER || g_iUserTeam[id] == TEAM_GUARD))
	{
		menu_item("\y1. \wВоскреснуть \r[%d]^n", g_iVip[id][DV_RESPAWN]);
		iKeys |= (1<<0);
	}
	else if (!g_iVip[id][DV_RESPAWN] || iAlive)
		menu_item("\y1. \dВоскреснуть^n");
	else
		menu_item("\y1. \dВоскреснуть \r[%d]^n", g_iVip[id][DV_RESPAWN]);
	
	if (iAlive && g_iVip[id][DV_HEALTH] && IsNotSetBit(g_iBitUserBoxing, id) && get_user_health(id) < 100)
	{
		menu_item("\y2. \wПодлечиться \r[%d]^n", g_iVip[id][DV_HEALTH]);
		iKeys |= (1<<1);
	}
	else if (!g_iVip[id][DV_HEALTH] || !iAlive)
		menu_item("\y2. \dПодлечиться^n");
	else
		menu_item("\y2. \dПодлечиться \r[%d]^n", g_iVip[id][DV_HEALTH]);
	
	if (g_iVip[id][DV_MONEY] >= g_eAllCvars[VIP_MONEY_ROUND])
	{
		menu_item("\y3. \wПолучить \r%d\y$^n", g_eAllCvars[VIP_MONEY_NUM]); // , g_eAllCvars[VIP_MONEY_ROUND]
		iKeys |= (1<<2);
	}
	else menu_item("\y3. \dПолучить %d$ \r[жди %d]^n", g_eAllCvars[VIP_MONEY_NUM], g_eAllCvars[VIP_MONEY_ROUND] - g_iVip[id][DV_MONEY]);
	
	if (iAlive && g_iVip[id][DV_INVISIBLE] >= g_eAllCvars[VIP_INVISIBLE] && g_iUserTeam[id] == TEAM_GUARD)
	{
		menu_item("\y4. \wНевидимость^n");
		iKeys |= (1<<3);
	}
	else if (g_iUserTeam[id] != TEAM_GUARD || !iAlive)
		menu_item("\y4. \dНевидимость \r[CT]^n");
	else if (g_iVip[id][DV_INVISIBLE] == 0 && g_eUserRendering[id][RENDER_STATUS]
		&& (pev(id, pev_renderfx) != kRenderFxNone || pev(id, pev_rendermode) != kRenderNormal))
	{
		menu_item("\y4. \wСнять Невидимость^n");
		iKeys |= (1<<3);
	}
	else menu_item("\y4. \dНевидимость \r[CT] [жди %d]^n", g_eAllCvars[VIP_INVISIBLE] - g_iVip[id][DV_INVISIBLE]);
	
	if (iAlive && g_iVip[id][DV_HP_AP] >= g_eAllCvars[VIP_HP_AP_ROUND] && (get_user_health(id) < 250 || get_user_armor(id) < 250))
	{
		menu_item("\y5. \w250HP и 250 AP^n"); // , g_eAllCvars[VIP_HP_AP_ROUND]
		iKeys |= (1<<4);
	}
	else if ((get_user_health(id) >= 250 && get_user_armor(id) >= 250) || !iAlive)
		menu_item("\y5. \d250HP и 250 AP^n");
	else menu_item("\y5. \d250HP и 250 AP \r[жди %d]^n", g_eAllCvars[VIP_HP_AP_ROUND] - g_iVip[id][DV_HP_AP]);

	if (iAlive && g_iVip[id][DV_GHOST] >= g_eAllCvars[VIP_GHOST_ROUND])
	{
		menu_item("\y6. \wПризрак^n");
		iKeys |= (1<<5);
	}
	else if (!iAlive)
		menu_item("\y6. \dПризрак^n");
	else menu_item("\y6. \dПризрак \r[жди %d]^n", g_eAllCvars[VIP_GHOST_ROUND] - g_iVip[id][DV_GHOST]);
	
	if (iAlive && g_iChiefId == id && g_iVip[id][DV_GOD] >= g_eAllCvars[VIP_GOD_ROUND])
	{
		menu_item("\y7. \wБесмертие^n");
		iKeys |= (1<<6);
	}
	else if (g_iChiefId != id || !iAlive)
		menu_item("\y7. \dБесмертие \r[Simon]^n");
	else menu_item("\y7. \dБесмертие \r[жди %d]^n", g_eAllCvars[VIP_GOD_ROUND] - g_iVip[id][DV_GOD]);
	
	if (iAlive && g_iVip[id][DV_FOOTSTEPS] >= g_eAllCvars[VIP_FOOTSTEPS_ROUND])
	{
		menu_item("\y8. \wБесшумные шаги^n");
		iKeys |= (1<<7);
	}
	else if (!iAlive)
		menu_item("\y8. \dБесшумные шаги^n");
	else menu_item("\y8. \dБесшумные шаги \r[жди %d]^n", g_eAllCvars[VIP_FOOTSTEPS_ROUND] - g_iVip[id][DV_FOOTSTEPS]);
	
	if (g_iVip[id][DV_RES_PLAYERS] && !jbe_is_wanted() && g_iAlivePlayersNum[TEAM_PRISONER] >= g_eAllCvars[RESPAWN_PLAYER_NUM])
	{
		menu_item("\y9. \wВозродить игрока \r[%d]^n", g_iVip[id][DV_RES_PLAYERS]);
		iKeys |= (1<<8);
	}
	else if (!g_iVip[id][DV_RES_PLAYERS] || g_iAlivePlayersNum[TEAM_PRISONER] < g_eAllCvars[RESPAWN_PLAYER_NUM])
		menu_item("\y9. \dВозродить игрока^n");
	else menu_item("\y9. \dВозродить игрока \r[%d]^n", g_iVip[id][DV_RES_PLAYERS]);
	
	menu_item("^n\y0. \w%L", id, "JBE_MENU_EXIT");
	return menu_show(id, iKeys, "Show_VipMenu");
}

public Handle_VipMenu(id, iKey)
{
	if (is_not_vip_menu(id))
		return PLUGIN_HANDLED;
	
	switch (iKey)
	{
		case 0:
		{
			if (IsNotSetBit(g_iBitUserAlive, id) && g_iVip[id][DV_RESPAWN] && !jbe_is_wanted()
			&& g_iAlivePlayersNum[TEAM_PRISONER] >= g_eAllCvars[RESPAWN_PLAYER_NUM])
			{
				if (g_iUserTeam[id] != TEAM_GUARD && g_iChiefId
				&& IsSetBit(g_iBitUserNightVip, id))
				{
					new iSysTime = get_systime();
					if (g_iVipResAcceptTime[id] > iSysTime)
					{
						client_print_color(id, id, "%s Не так часто, жди %s", g_szChatPrefix,
							seconds_to_str(g_iVipResAcceptTime[id] - iSysTime));
						return PLUGIN_HANDLED;
					}
					if (!g_iBitVipResAccept)
					{
						g_iMenuTarget[g_iChiefId] = id;
						Show_AcceptResMenu(g_iChiefId);
					}
					else
						Show_UsersAcceptResMenu(g_iChiefId);

					g_iVipResAcceptTime[id] = iSysTime + VIP_RES_ACCEPT_TIME; // 3 min
					SetBit(g_iBitVipResAccept, id);
					
					client_print_color(0, id, "%s ^3%n ^1хочет возродится, ждем разрешения начальника.",
						g_szChatPrefix, id);

					return PLUGIN_HANDLED;
				}

				rg_round_respawn(id);

				g_iVip[id][DV_RESPAWN]--;
				client_print_color(0, id, "%s^1[^4VIP^1] ^3%n ^1возродился.", g_szChatPrefix, id);

				respawm_set_effects(id);
			}
		}
		case 1:
		{
			if (IsSetBit(g_iBitUserWanted, id))
				return client_print_color(id, id, "%s Недоступно в розыске.", g_szChatPrefix);
	
			if (IsSetBit(g_iBitUserAlive, id) && g_iVip[id][DV_HEALTH]
			&& IsNotSetBit(g_iBitUserBoxing, id) && get_user_health(id) < 100) {
				set_pev(id, pev_health, 100.0);
				g_iVip[id][DV_HEALTH]--;
				client_print_color(0, id, "%s^1[^4VIP^1] ^3%n ^1подлечился ^4[set 100HP]", 
					g_szChatPrefix, id);				
			}
		}
		case 2: {
			jbe_set_user_money(id, g_iUserMoney[id] + g_eAllCvars[VIP_MONEY_NUM], 1);
			g_iVip[id][DV_MONEY] = 0;
			client_print_color(0, id, "%s^1[^4VIP^1] ^3%n ^1взял деньги ^4[+%d$]",
				g_szChatPrefix, id, g_eAllCvars[VIP_MONEY_NUM]);
		}
		case 3: {
			if (IsSetBit(g_iBitUserAlive, id) && g_iUserTeam[id] == TEAM_GUARD) {
				if (IsSetBit(g_iBitUserNightVip, id)) {
					client_print_color(id, id, "^1[^3Ошибка^1] Действие недоступно в ^4Night VIP^1.");
					return Show_VipMenu(id);
				}
				
				if (g_iVip[id][DV_INVISIBLE] == 0 && g_eUserRendering[id][RENDER_STATUS]
				&& (pev(id, pev_renderfx) != kRenderFxNone || pev(id, pev_rendermode) != kRenderNormal))
				{
					jbe_set_user_rendering(id, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
					jbe_save_user_rendering(id);
					g_eUserRendering[id][RENDER_STATUS] = false;
					client_print_color(0, id, "%s^1[^4VIP^1] Охранник ^3%n ^1убрал невидимость.", g_szChatPrefix, id);
				}
				else
				{
					jbe_set_user_rendering(id, kRenderFxGlowShell, 0, 0, 0, kRenderTransAlpha, 0);
					jbe_save_user_rendering(id);
					g_iVip[id][DV_INVISIBLE] = 0;
					client_print_color(0, id, "%s^1[^4VIP^1] Охранник ^3%n ^1взял невидимость.", g_szChatPrefix, id);
				}
			}
		}
		case 4: {
			if (IsSetBit(g_iBitUserWanted, id))
				return client_print_color(id, id, "%s Недоступно в розыске.", g_szChatPrefix);
				
			if(IsSetBit(g_iBitUserAlive, id)) {
				set_pev(id, pev_health, 250.0);
				rg_set_user_armor(id, 250, ARMOR_VESTHELM);
				g_iVip[id][DV_HP_AP] = 0;
				client_print_color(0, id, "%s^1[^4VIP^1] ^3%n ^1взял^4 250HP ^1и^4 250AP^1.", 
					g_szChatPrefix, id);
			}
		}
		case 5: {
			if (IsSetBit(g_iBitUserWanted, id))
				return client_print_color(id, id, "%s Недоступно в розыске.", g_szChatPrefix);
	
			if (IsSetBit(g_iBitUserAlive, id)) {
				jbe_set_user_rendering(id, kRenderFxDistort, 0, 0, 0, kRenderTransAdd, 127); // голограмма
				jbe_save_user_rendering(id);
				g_iVip[id][DV_GHOST] = 0;
				client_print_color(0, id, "%s^1[^4VIP^1] ^3%n ^1взял ^4призрака^1.", 
					g_szChatPrefix, id);
			}
		}
		case 6: {
			if (jbe_is_wanted())
				return client_print_color(id, id, "%s Недоступно во время бунта.", g_szChatPrefix);
				
			if (IsSetBit(g_iBitUserAlive, id) && g_iChiefId == id) {
				jbe_set_user_godmode(id, 1);
				g_iVip[id][DV_GOD] = 0;
				client_print_color(0, id, "%s^1[^4VIP^1] Начальник ^3%n ^1взял бессмертие.", 
					g_szChatPrefix, id);
			}
		}
		case 7: {
			if (IsSetBit(g_iBitUserWanted, id))
				return client_print_color(id, id, "%s Недоступно в розыске.", g_szChatPrefix);
	
			if (IsSetBit(g_iBitUserAlive, id))
			{
				rg_set_user_footsteps(id, true);
				g_iVip[id][DV_FOOTSTEPS] = 0;
				client_print_color(0, id, "%s^1[^4VIP^1] ^3%n ^1взял бесшумные шаги.", g_szChatPrefix, id);
			}
		}
		case 8:
		{
			if (IsSetBit(g_iBitUserNightVip, id))
			{
				client_print_color(id, id, "^1[^3Ошибка^1] Действие недоступно в ^4%s^1.", g_eAllCvars[VIP_NIGHT_PREFIX]);
				return Show_VipMenu(id);
			}
			
			if (g_iAlivePlayersNum[TEAM_PRISONER] >= g_eAllCvars[RESPAWN_PLAYER_NUM])
			{
				Show_VipRespawnMenu(id, _, 1, 1);
			}
		}
	}
	return PLUGIN_HANDLED;
}


