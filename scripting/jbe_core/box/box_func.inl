// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

#include "jbe_core\box\box_menu.inl"

public FakeMeta_UpdateClientData_Post(id, iSendWeapons, CD_Handle)
{
	if (g_iBoxingGame && IsSetBit(g_iBitUserBoxing, id))
	{
		new iWeaponAnim = get_cd(CD_Handle, CD_WeaponAnim);
		switch(iWeaponAnim)
		{
			case 4, 5:
			{
				switch(g_iBoxingTypeKick[id])
				{
					case 0: set_cd(CD_Handle, CD_WeaponAnim, 4);
					case 1: set_cd(CD_Handle, CD_WeaponAnim, 5);
					case 2: set_cd(CD_Handle, CD_WeaponAnim, 2);
				}
			}
			case 6, 7: if(g_iBoxingTypeKick[id] == 4) set_cd(CD_Handle, CD_WeaponAnim, 1);
		}
	}
}

/*===== -> Бокс -> =====*///{

jbe_boxing_set_gloves_model(id, iTeam)
{
	new iTarget, iBody;
	get_user_aiming(id, iTarget, iBody);
	if (iTeam > -1)
	{
		if (jbe_is_user_valid(iTarget) 
		&& g_iUserTeam[iTarget] == TEAM_PRISONER && IsSetBit(g_iBitUserAlive, iTarget)
		&& IsNotSetBit(g_iBitUserFree, iTarget) && IsNotSetBit(g_iBitUserWanted, iTarget)
		&& IsNotSetBit(g_iBitUserSoccer, iTarget) && IsNotSetBit(g_iBitUserDuel, iTarget))
		{
			if (!g_iBoxingGame) jbe_boxing_enable_all();

			if (IsNotSetBit(g_iBitUserBoxing, iTarget))
			{
				SetBit(g_iBitUserBoxing, iTarget);
				ExecuteHamB(Ham_Player_ResetMaxSpeed, iTarget);
				ClearBit(g_iBitClothingType, iTarget);

				set_entvar(iTarget, var_health, 100.0);
				set_entvar(iTarget, var_gravity, 1.0);
				set_member(iTarget, m_bloodColor, -1);
			}
			g_iBoxingUserTeam[iTarget] = iTeam;
			if (get_user_weapon(iTarget) != CSW_KNIFE)
			{
				engclient_cmd(iTarget, "weapon_knife");
			}
			else
			{
				new iActiveItem = get_member(iTarget, m_pActiveItem);
				if (iActiveItem > 0)
				{
					ExecuteHamB(Ham_Item_Deploy, iActiveItem);
					UTIL_WeaponAnimation(iTarget, 3);
				}
			}
		}
		else
			client_print_color(id, print_team_blue, "%s ^1Наведите ^4прицел на игрока^1, \
				и нажмите соответствующею кнопку чтобы ^3дать перчатки^1.", g_szChatPrefix);
	}
	else
	{
		if (jbe_is_user_valid(iTarget) && IsSetBit(g_iBitUserBoxing, iTarget))
		{
			ClearBit(g_iBitUserBoxing, iTarget);
			ExecuteHamB(Ham_Player_ResetMaxSpeed, iTarget);
			new iActiveItem = get_member(iTarget, m_pActiveItem);
			if (iActiveItem > 0)
			{
				ExecuteHamB(Ham_Item_Deploy, iActiveItem);
				UTIL_WeaponAnimation(iTarget, 3);
			}
			set_entvar(iTarget, var_health, 100.0);
			set_member(iTarget, m_bloodColor, 247);
			if (!g_iBitUserBoxing) jbe_boxing_disable_all();
		}
		else
			client_print_color(id, print_team_blue, "%s ^1Наведите ^4прицел на игрока^1, \
				и нажмите соответствующею кнопку чтобы ^3забрать перчатки^1.", g_szChatPrefix);		
	}
}

jbe_boxing_enable_all()
{
	for (new i = 0; i < charsmax(g_iHamHookItemDeploy); i++)
		EnableHamForward(g_iHamHookItemDeploy[i]);
	
	g_iFakeMetaUpdateClientData = register_forward(FM_UpdateClientData, 
		"FakeMeta_UpdateClientData_Post", 1);

	jbe_set_name_mode("Бокс");
	g_iChiefGameType = GAME_BOXING;
	g_iBoxingGame = BOXING_WAIT; //	g_bBoxingStatus = true;
}

jbe_boxing_disable_all()
{
	for (new i; i < charsmax(g_iHamHookItemDeploy); i++)
		DisableHamForward(g_iHamHookItemDeploy[i]);
	
	for (new iPlayer = 1; iPlayer <= MaxClients; iPlayer++)
	{
		if (IsSetBit(g_iBitUserBoxing, iPlayer))
		{
			ClearBit(g_iBitUserBoxing, iPlayer);
			ExecuteHamB(Ham_Player_ResetMaxSpeed, iPlayer);
			set_member(iPlayer, m_bloodColor, 247);
			new iActiveItem = get_member(iPlayer, m_pActiveItem);
			if (iActiveItem > 0)
			{
				ExecuteHamB(Ham_Item_Deploy, iActiveItem);
				UTIL_WeaponAnimation(iPlayer, 3);
			}
		}
	}

	jbe_set_name_mode("");
	g_iBoxingGame = BOXING_NOT; //	g_iBoxingGame = 0; //	g_bBoxingStatus = false;
	g_iChiefGameType = GAME_NONE;
	unregister_forward(FM_UpdateClientData, g_iFakeMetaUpdateClientData, 1);
}

jbe_boxing_game_start(pPlayer)
{
	new iPlayers;
	for (new iPlayer = 1; iPlayer <= MaxClients; iPlayer++)
	{
		if (IsSetBit(g_iBitUserBoxing, iPlayer))
			iPlayers++;
	}
	if (iPlayers < 2)
	{
		client_print_color(pPlayer, 0, "%s ^1Недостаточно игроков для начала боя, ^4раздайте перчатки^1!", g_szChatPrefix);
	}
	else
	{
		g_iBoxingGame = BOXING_RUN; //	g_iBoxingGame = 1;
		emit_sound(pPlayer, CHAN_AUTO, sound_gong, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	}
}

jbe_boxing_game_team_start(pPlayer)
{
	new iPlayersRed, iPlayersBlue;
	for (new iPlayer = 1; iPlayer <= MaxClients; iPlayer++)
	{
		if (IsSetBit(g_iBitUserBoxing, iPlayer))
		{
			switch (g_iBoxingUserTeam[iPlayer])
			{
				case 0: iPlayersRed++;
				case 1: iPlayersBlue++;
			}
		}
	}

	if (iPlayersRed < 2 || iPlayersBlue < 2)
	{
		client_print_color(pPlayer, 0, "%s ^1Недостаточно игроков для начала боя \
			^3(минимум по 2 игрока в команде)^1, ^4раздайте перчатки^1!", g_szChatPrefix);
	}
	else
	{
		g_iBoxingGame = BOXING_RUN_TEAM; //	g_iBoxingGame = 2;
		emit_sound(pPlayer, CHAN_AUTO, sound_gong, VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	}
}

jbe_boxing_game_end()
{
	for (new iPlayer = 1; iPlayer <= MaxClients; iPlayer++)
	{
		if (IsSetBit(g_iBitUserBoxing, iPlayer))
		{
			set_pev(iPlayer, pev_health, 100.0);
		}
	}
	rg_send_audio(0, sound_gong);
	g_iBoxingGame = BOXING_WAIT; // g_iBoxingGame = 0;
}

jbe_boxing_divide_team()
{
	new iPlayersNum;
	g_iBoxingGame = BOXING_WAIT; // чтоб при выдаче были перчатки

	for (new iPlayer = 1, iTeam; iPlayer <= MaxClients; iPlayer++)
	{
		if (g_iUserTeam[iPlayer] == TEAM_PRISONER
		&& IsSetBit(g_iBitUserAlive, iPlayer) && IsNotSetBit(g_iBitUserFree, iPlayer)
		&& IsNotSetBit(g_iBitUserWanted, iPlayer) && IsNotSetBit(g_iBitUserSoccer, iPlayer)
		&& IsNotSetBit(g_iBitUserBoxing, iPlayer) && IsNotSetBit(g_iBitUserDuel, iPlayer))
		{
			SetBit(g_iBitUserBoxing, iPlayer);
			ExecuteHamB(Ham_Player_ResetMaxSpeed, iPlayer);
			set_pev(iPlayer, pev_health, 100.0);
			set_member(iPlayer, m_bloodColor, -1);
			set_pev(iPlayer, pev_gravity, 1.0);
			g_iBoxingUserTeam[iPlayer] = iTeam;

			if (get_user_weapon(iPlayer) != CSW_KNIFE)
				 engclient_cmd(iPlayer, "weapon_knife");
			else
			{
				new iActiveItem = get_member(iPlayer, m_pActiveItem);
				if (iActiveItem > 0)
				{
					ExecuteHamB(Ham_Item_Deploy, iActiveItem);
					UTIL_WeaponAnimation(iPlayer, 3);
				}
			}
			iTeam = !iTeam;
			iPlayersNum++;
		}
	}

	if (!iPlayersNum)
	{
		if (g_iBitUserBoxing)
			client_print_color(g_iChiefId, print_team_default, "%s Вы уже раздали перчатки.", g_szChatPrefix);
		else
		{
			if (g_iChiefGameType == GAME_BOXING)
				jbe_boxing_disable_all();

			g_iBoxingGame = BOXING_NOT;
			client_print_color(g_iChiefId, print_team_default, "%s Некому раздать перчатки!", g_szChatPrefix);
		}
	}
	else if (!g_iChiefGameType)
	{
		jbe_boxing_enable_all();
	}
}

jbe_boxing_gloves_model(pPlayer, iTeam)
{
	switch (iTeam)
	{
		case 0: jbe_boxing_gloves_red(pPlayer);
		case 1: jbe_boxing_gloves_blue(pPlayer);
	}
}

jbe_boxing_gloves_red(pPlayer)
{
	static iszViewModel, iszWeaponModel;
	if (iszViewModel || (iszViewModel = engfunc(EngFunc_AllocString, model_v_boxing_gloves_red)))
		set_pev_string(pPlayer, pev_viewmodel2, iszViewModel);

	if (iszWeaponModel || (iszWeaponModel = engfunc(EngFunc_AllocString, model_p_boxing_gloves_red)))
		set_pev_string(pPlayer, pev_weaponmodel2, iszWeaponModel);
}

jbe_boxing_gloves_blue(pPlayer)
{
	static iszViewModel, iszWeaponModel;
	if (iszViewModel || (iszViewModel = engfunc(EngFunc_AllocString, model_v_boxing_gloves_blue)))
		set_pev_string(pPlayer, pev_viewmodel2, iszViewModel);
	
	if (iszWeaponModel || (iszWeaponModel = engfunc(EngFunc_AllocString, model_p_boxing_gloves_blue)))
		set_pev_string(pPlayer, pev_weaponmodel2, iszWeaponModel);
}

/*===== <- Бокс <- =====*///}
