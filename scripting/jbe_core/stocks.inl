/*===== -> Стоки -> =====*///{

#if defined DEBUG
	#define debug_print(%1) log_amx("[DEBUG] %s:%i: %s", __FILE__, __LINE__, %1);
#else
	#define debug_print(%1)
#endif

stock jbe_info_center_msg(pPlayer, const szMessage[], any:...)
{
	static szBuffer[190];

	if (numargs() > 2)
		vformat(szBuffer, charsmax(szBuffer), szMessage, 3);
	else copy(szBuffer, charsmax(szBuffer), szMessage);

	set_hudmessage(127, 127, 127, -1.0, 0.7, 0, 6.0, 2.0, 0.1, 0.1, -1);
	ShowSyncHudMsg(pPlayer, g_iSyncInfoCenter, szBuffer);
}

// fix to %s on old non-steam clients print_center
stock client_print_center(pPlayer, szMessage[], any:...)
{
	static szBuffer[190];
	if (numargs() > 2)
		vformat(szBuffer, charsmax(szBuffer), szMessage, 3);
	else
		copy(szBuffer, charsmax(szBuffer), szMessage);

	if (pPlayer)
	{
		message_begin(MSG_ONE_UNRELIABLE, MsgId_TextMsg, _, pPlayer);
		write_byte(print_center);
		write_string(szBuffer);
		message_end();
	}
	else
	{
		message_begin(MSG_BROADCAST, MsgId_TextMsg);
		write_byte(print_center);
		write_string(szBuffer);
		message_end();
	}

	return PLUGIN_HANDLED;
}

stock jbe_get_players_num(iType = 0)
{
	switch (iType)
	{
		case NUM_ALL_PLAYERS: 
			return g_iPlayersNum[TEAM_UNASSIGNED] + g_iPlayersNum[TEAM_PRISONER] 
				+ g_iPlayersNum[TEAM_GUARD] + g_iPlayersNum[TEAM_SPECTATOR];
		
		case NUM_ALIVE_ALL: 
			return g_iAlivePlayersNum[TEAM_PRISONER] + g_iAlivePlayersNum[TEAM_GUARD];
		
		case NUM_PRISONER_GUARD: 
			return g_iPlayersNum[TEAM_PRISONER] + g_iPlayersNum[TEAM_GUARD];
		
		case NUM_PRISONER: return g_iPlayersNum[TEAM_PRISONER];
		
		case NUM_GUARD: return g_iPlayersNum[TEAM_GUARD];
		
		case NUM_ALIVE_PRISONER: return g_iAlivePlayersNum[TEAM_PRISONER];
		
		case NUM_ALIVE_GUARD: return g_iAlivePlayersNum[TEAM_GUARD];
		
		case NUM_DEAD_PRISONER: 
			return g_iPlayersNum[TEAM_PRISONER] - g_iAlivePlayersNum[TEAM_PRISONER];
		
		case NUM_DEAD_GUARD: 
			return g_iPlayersNum[TEAM_GUARD] - g_iAlivePlayersNum[TEAM_PRISONER];
	}

	return -1;
}

// на уведмления в чат "Начальник" или "Админ" сделал чет
funcPref(id) return id == g_iChiefId ? g_szChiefPrefix : g_szAdminPrefix;

stock fnGetUserIp(id, bool:bNoPort = true) {
	static szIp[MAX_USERIP_LENGTH];
	get_user_ip(id, szIp, charsmax(szIp), bNoPort);
	return szIp;
}

stock fnGetUserAuthID(id) {
	static szAuth[MAX_AUTHID_LENGTH];
	get_user_authid(id, szAuth, charsmax(szAuth));
	return szAuth;
}

stock register_clcmdex(szCommand[], szHandler[], iCmd = 0) 
{
	#define MAX_CMD_SIZE 24

	new const szRegisters[][] = { 
		"say /", "say_team /",
		"say !", "say_team !",
		"say .", "say_team ."
	};
	
	new szLine[MAX_CMD_SIZE * 2 + 11];
	new szTranslited[MAX_CMD_SIZE * 2 + 1];

	translite_string(szTranslited, charsmax(szTranslited), szCommand);

	if (iCmd) register_clcmd(szCommand, szHandler);

	for (new i; i < sizeof szRegisters; i++) 
	{
		formatex(szLine, charsmax(szLine), "%s%s", szRegisters[i], szCommand);
		register_clcmd(szLine, szHandler);
	
		formatex(szLine, charsmax(szLine), "%s%s", szRegisters[i], szTranslited);
		register_clcmd(szLine, szHandler);
	}
}

/* Рандомное копирование массива
 * inArray - входящий массив, inSize его размер
 * outArray - куда копируем, outSize его размер
 * второй массив размером такой же или меньше,
 * если больше остаток массива будет пустой
 */
stock array_copy_array_random(inArray[], inSize, outArray[], outSize)
{
	new iLast, iRandom, iFound;

	iLast = inSize - 1;

	while (iLast != -1 && iFound != outSize)
	{
		iRandom = random_num(0, iLast);

		outArray[iFound++] = inArray[iRandom];

		if (outArray[iRandom] != iLast)
		{
			outArray[iRandom] = outArray[iLast];
		}

		iLast--; // 3
	}

	return iFound;
}

stock UTIL_ShowRadar(iPlayer, pPlayer, iCoords[3])
{
	message_begin(MSG_ONE_UNRELIABLE, MsgId_HostageAdd, {0,0,0}, iPlayer);
	write_byte(iPlayer);
	write_byte(pPlayer);
	write_coord(iCoords[0]);
	write_coord(iCoords[1]);
	write_coord(iCoords[2]);
	message_end();

	message_begin(MSG_ONE_UNRELIABLE, MsgId_HostageDel, {0,0,0}, iPlayer);
	write_byte(pPlayer);
	message_end();
}

// конверт минут в строку
stock minutes_to_str(iMinutes)
{
	new szTime[32];
	new iHour = iMinutes / 60;
	if (iHour > 0)
	{
		iMinutes = (iMinutes - (iHour * 60));
		if (iMinutes > 0)
			formatex(szTime, charsmax(szTime), "%d час. %d мин.", iHour, iMinutes);
		else
			formatex(szTime, charsmax(szTime), "%d час.", iHour);
	}
	else
		formatex(szTime, charsmax(szTime), "%d мин.", iMinutes);
	
	return szTime;
}

stock seconds_to_str(iSeconds)
{
	new iHour = iSeconds / 60 / 60;
	iSeconds -= iHour * 60 * 60;
	
	new iMinutes = iSeconds / 60;
	iSeconds -= iMinutes * 60;
	
	new szTime[32];
	new iLen;

	if (iHour) 
		iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%d час.", iHour);

	if (iMinutes)
		iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%s%d мин.", iLen ? " " : "", iMinutes);
	
	if (iSeconds)
		iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%s%d сек.", iLen ? " " : "", iSeconds);

	return szTime;
}

stock seconds_to_str__gg(iSeconds, bool:bFull = false)
{
	new iHour = iSeconds / 60 / 60;
	iSeconds -= iHour * 60 * 60;
	
	new iMinutes = iSeconds / 60;
	iSeconds -= iMinutes * 60;
	
	new szTime[32];
	new iLen;

	if (iHour) {
		if (bFull) {
			if  (iHour > 20 && iHour % 10 == 1 || iHour == 1)
				iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%d час", iHour);
			
			
			else if ((iHour > 20 && iHour % 10 => 2 && iHour % 10 <= 4) || iHour >= 2 && iHour <= 4)
				iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%d часа", iHour);
			
			else
				iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%d часов", iHour);
		}
		else
			iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%d час.", iHour);
	}

	if (iMinutes) {
		if (bFull) {
			if  (iMinutes > 20 && iMinutes % 10 == 1 || iMinutes == 1)
				iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%s%d минута", iLen ? " " : "", iMinutes);
			
			else if (iMinutes > 20 && iMinutes % 10 => 2 && iMinutes % 10 <= 4 || iMinutes >= 2 && iMinutes <= 4)
				iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%s%d минуты", iLen ? " " : "", iMinutes);
			
			else
				iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%s%d минут", iLen ? " " : "", iMinutes);
		}
		else
			iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%s%d мин.", iLen ? " " : "", iMinutes);
	}

	if (iSeconds) {
		if (bFull) {
			if  (iSeconds > 20 && iSeconds % 10 == 1 || iSeconds == 1)
				iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%s%d секунда", iLen ? " " : "", iSeconds);
			
			else if (iSeconds > 20 && iSeconds % 10 => 2 && iSeconds % 10 <= 4 || iSeconds >= 2 && iSeconds <= 4)
				iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%s%d секунды", iLen ? " " : "", iSeconds);
			
			else
				iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%s%d секунд", iLen ? " " : "", iSeconds);
		}
		else
			iLen += formatex(szTime[iLen], charsmax(szTime) - iLen, "%s%d сек.", iLen ? " " : "", iSeconds);
	}

	return szTime;
}


stock hours_to_str(iHour)
{
	new iDay = iHour / 24;
	iHour -= iDay * 24;

	new szTime[32];
	new iLen;

	if (iDay)
		iLen += formatex(szTime[iLen], charsmax(szTime) - iLen,
			"%d д.", iDay);

	if (iHour)
		iLen += formatex(szTime[iLen], charsmax(szTime) - iLen,
			"%d час.", iHour);

	return szTime;
}

/* С поддержкой отрицательных значений */
stock bool:is_str_numex(const sString[])
{
	new i = 0;

	if (sString[i] == '-')
		++i;

	while (sString[i] && isdigit(sString[i]))
		++i;
	
	return sString[i] == 0 && i != 0;
}

// Есть ли у игрока оружие (нож и грены не в счет)
stock bool:is_user_weapons(pPlayer)
{
	new iWeaponsId[32], iNum;
	const iBitWeapons = (1<<CSW_GLOCK18|1<<CSW_USP|1<<CSW_P228|1<<CSW_DEAGLE|1<<CSW_ELITE
		|1<<CSW_FIVESEVEN|1<<CSW_M3|1<<CSW_XM1014|1<<CSW_MAC10|1<<CSW_TMP|1<<CSW_MP5NAVY
		|1<<CSW_UMP45|1<<CSW_P90|1<<CSW_GALIL|1<<CSW_FAMAS|1<<CSW_AK47|1<<CSW_M4A1|1<<CSW_SCOUT
		|1<<CSW_SG552|1<<CSW_AUG|1<<CSW_AWP|1<<CSW_G3SG1|1<<CSW_SG550|1<<CSW_M249);
	
	get_user_weapons(pPlayer, iWeaponsId, iNum);

	for (new i; i < iNum; i++)
	{
		if (iBitWeapons & (1<<iWeaponsId[i]))
			return true;
	}

	return false;
}

stock rg_set_user_noclip(index, noclip)
{
	set_entvar(index, var_movetype, noclip ? MOVETYPE_NOCLIP : MOVETYPE_WALK);
	return 1;
}

stock rg_get_user_noclip(id)
{
	return get_entvar(id, var_movetype) == MOVETYPE_NOCLIP ? 1 : 0;
}

// by neygomon	(dev-cs.ru)
stock rg_give_item_ex(id, weapon[], GiveType:type = GT_APPEND, ammount = 0)
{
	rg_give_item(id, weapon, type);
	if (ammount)
	{
		rg_set_user_bpammo(id, rg_get_weapon_info(weapon, WI_ID), ammount);
	}
}

stock get_aiming_position(pPlayer, Float:vecReturn[3])
{
	new Float:vecOrigin[3], Float:vecViewOfs[3], Float:vecAngle[3], Float:vecForward[3];
	get_entvar(pPlayer, var_origin, vecOrigin);
	get_entvar(pPlayer, var_view_ofs, vecViewOfs);
	xs_vec_add(vecOrigin, vecViewOfs, vecOrigin);
	get_entvar(pPlayer, var_v_angle, vecAngle);
	engfunc(EngFunc_MakeVectors, vecAngle);
	global_get(glb_v_forward, vecForward);
	xs_vec_mul_scalar(vecForward, 8192.0, vecForward); // Max World Size
	xs_vec_add(vecOrigin, vecForward, vecForward);
	engfunc(EngFunc_TraceLine, vecOrigin, vecForward, DONT_IGNORE_MONSTERS, pPlayer, 0);
	get_tr2(0, TR_vecEndPos, vecReturn);
}

stock fm_set_kvd(pEntity, const szClassName[], const szKeyName[], const szValue[])
{
	set_kvd(0, KV_ClassName, szClassName);
	set_kvd(0, KV_KeyName, szKeyName);
	set_kvd(0, KV_Value, szValue);
	set_kvd(0, KV_fHandled, 0);
	return dllfunc(DLLFunc_KeyValue, pEntity, 0);
}

stock drop_user_weapons(pPlayer, iType)
{
	new iWeaponsId[32], iNum;
	get_user_weapons(pPlayer, iWeaponsId, iNum);
	if(iType) iType = (1<<CSW_GLOCK18|1<<CSW_USP|1<<CSW_P228|1<<CSW_DEAGLE|1<<CSW_ELITE|1<<CSW_FIVESEVEN);
	else iType = (1<<CSW_M3|1<<CSW_XM1014|1<<CSW_MAC10|1<<CSW_TMP|1<<CSW_MP5NAVY|1<<CSW_UMP45|1<<CSW_P90
	|1<<CSW_GALIL|1<<CSW_FAMAS|1<<CSW_AK47|1<<CSW_M4A1|1<<CSW_SCOUT|1<<CSW_SG552|1<<CSW_AUG|1<<CSW_AWP
	|1<<CSW_G3SG1|1<<CSW_SG550|1<<CSW_M249);
	for(new i; i < iNum; i++)
	{
		if(iType & (1<<iWeaponsId[i]))
		{
			new szWeaponName[24];
			get_weaponname(iWeaponsId[i], szWeaponName, charsmax(szWeaponName));
			engclient_cmd(pPlayer, "drop", szWeaponName);
		}
	}
}

stock UTIL_ScreenFade(pPlayer, iDuration, iHoldTime, iFlags, iRed, iGreen, iBlue, iAlpha, iReliable = 0)
{
	switch(pPlayer)
	{
		case 0:
		{
			message_begin(iReliable ? MSG_ALL : MSG_BROADCAST, MsgId_ScreenFade);
			write_short(iDuration);
			write_short(iHoldTime);
			write_short(iFlags);
			write_byte(iRed);
			write_byte(iGreen);
			write_byte(iBlue);
			write_byte(iAlpha);
			message_end();
		}
		default:
		{
			engfunc(EngFunc_MessageBegin, iReliable ? MSG_ONE : MSG_ONE_UNRELIABLE, MsgId_ScreenFade, {0.0, 0.0, 0.0}, pPlayer);
			write_short(iDuration);
			write_short(iHoldTime);
			write_short(iFlags);
			write_byte(iRed);
			write_byte(iGreen);
			write_byte(iBlue);
			write_byte(iAlpha);
			message_end();
		}
	}
}

stock UTIL_ScreenShake(pPlayer, iAmplitude, iDuration, iFrequency, iReliable = 0)
{
	engfunc(EngFunc_MessageBegin, iReliable ? MSG_ONE : MSG_ONE_UNRELIABLE, MsgId_ScreenShake, {0.0, 0.0, 0.0}, pPlayer);
	write_short(iAmplitude);
	write_short(iDuration);
	write_short(iFrequency);
	message_end();
}

stock UTIL_WeaponAnimation(pPlayer, iAnimation)
{
	set_pev(pPlayer, pev_weaponanim, iAnimation);
	engfunc(EngFunc_MessageBegin, MSG_ONE_UNRELIABLE, SVC_WEAPONANIM, {0.0, 0.0, 0.0}, pPlayer);
	write_byte(iAnimation);
	write_byte(0);
	message_end();
}

stock UTIL_PlayerAnimation(pPlayer, const szAnimation[]) // Спасибо большое KORD_12.7
{
	new iAnimDesired, Float:flFrameRate, Float:flGroundSpeed, bool:bLoops;
	if((iAnimDesired = lookup_sequence(pPlayer, szAnimation, flFrameRate, bLoops, flGroundSpeed)) == -1)
		iAnimDesired = 0;
	new Float:flGametime = get_gametime();
	set_entvar(pPlayer, var_frame, 0.0);
	set_entvar(pPlayer, var_framerate, 1.0);
	set_entvar(pPlayer, var_animtime, flGametime); // fAnimTime
	set_entvar(pPlayer, var_sequence, iAnimDesired);
	set_member(pPlayer, m_fSequenceLoops, bLoops);
	set_member(pPlayer, m_fSequenceFinished, 0);
	set_member(pPlayer, m_flFrameRate, flFrameRate);
	set_member(pPlayer, m_flGroundSpeed, flGroundSpeed);
	set_member(pPlayer, m_flLastEventCheck, flGametime);
	set_member(pPlayer, m_Activity, ACT_RANGE_ATTACK1);
	set_member(pPlayer, m_IdealActivity, ACT_RANGE_ATTACK1);
	set_member(pPlayer, m_flLastFired, flGametime);
}

stock CREATE_BEAMCYLINDER(Float:vecOrigin[3], iRadius, pSprite, iStartFrame = 0, iFrameRate = 0, iLife, iWidth, iAmplitude = 0, iRed, iGreen, iBlue, iBrightness, iScrollSpeed = 0)
{
	engfunc(EngFunc_MessageBegin, MSG_PVS, SVC_TEMPENTITY, vecOrigin, 0);
	write_byte(TE_BEAMCYLINDER);
	engfunc(EngFunc_WriteCoord, vecOrigin[0]);
	engfunc(EngFunc_WriteCoord, vecOrigin[1]);
	engfunc(EngFunc_WriteCoord, vecOrigin[2]);
	engfunc(EngFunc_WriteCoord, vecOrigin[0]);
	engfunc(EngFunc_WriteCoord, vecOrigin[1]);
	engfunc(EngFunc_WriteCoord, vecOrigin[2] + 32.0 + iRadius * 2);
	write_short(pSprite);
	write_byte(iStartFrame);
	write_byte(iFrameRate); // 0.1's
	write_byte(iLife); // 0.1's
	write_byte(iWidth);
	write_byte(iAmplitude); // 0.01's
	write_byte(iRed);
	write_byte(iGreen);
	write_byte(iBlue);
	write_byte(iBrightness);
	write_byte(iScrollSpeed); // 0.1's
	message_end();
}

stock CREATE_BREAKMODEL(Float:vecOrigin[3], Float:vecSize[3] = {16.0, 16.0, 16.0}, Float:vecVelocity[3] = {25.0, 25.0, 25.0}, iRandomVelocity, pModel, iCount, iLife, iFlags)
{
	engfunc(EngFunc_MessageBegin, MSG_PVS, SVC_TEMPENTITY, vecOrigin, 0);
	write_byte(TE_BREAKMODEL);
	engfunc(EngFunc_WriteCoord, vecOrigin[0]);
	engfunc(EngFunc_WriteCoord, vecOrigin[1]);
	engfunc(EngFunc_WriteCoord, vecOrigin[2] + 24);
	engfunc(EngFunc_WriteCoord, vecSize[0]);
	engfunc(EngFunc_WriteCoord, vecSize[1]);
	engfunc(EngFunc_WriteCoord, vecSize[2]);
	engfunc(EngFunc_WriteCoord, vecVelocity[0]);
	engfunc(EngFunc_WriteCoord, vecVelocity[1]);
	engfunc(EngFunc_WriteCoord, vecVelocity[2]);
	write_byte(iRandomVelocity);
	write_short(pModel);
	write_byte(iCount); // 0.1's
	write_byte(iLife); // 0.1's
	write_byte(iFlags); // BREAK_GLASS 0x01, BREAK_METAL 0x02, BREAK_FLESH 0x04, BREAK_WOOD 0x08
	message_end();
}

/* Создать спрайт ( по типу траила)
 * iLife = длина спрайта (продолжительность жизни спрайта)
 * iWidth = ширина спрайта
 * iAlpha = насыщенность цвета
*/
stock CREATE_BEAMFOLLOW(pEntity, pSptite, iLife, iWidth, iRed, iGreen, iBlue, iAlpha)
{
	message_begin(MSG_BROADCAST, SVC_TEMPENTITY);
	write_byte(TE_BEAMFOLLOW);
	write_short(pEntity);
	write_short(pSptite);
	write_byte(iLife); // 0.1's
	write_byte(iWidth);
	write_byte(iRed);
	write_byte(iGreen);
	write_byte(iBlue);
	write_byte(iAlpha);
	message_end();
}

// Создать спрайт в точке
stock CREATE_SPRITE(Float:vecOrigin[3], pSptite, iWidth, iAlpha)
{
	engfunc(EngFunc_MessageBegin, MSG_PVS, SVC_TEMPENTITY, vecOrigin, 0);
	write_byte(TE_SPRITE);
	engfunc(EngFunc_WriteCoord, vecOrigin[0]);
	engfunc(EngFunc_WriteCoord, vecOrigin[1]);
	engfunc(EngFunc_WriteCoord, vecOrigin[2]);
	write_short(pSptite);
	write_byte(iWidth);
	write_byte(iAlpha);
	message_end();
}

// Спрайт над головой
stock CREATE_PLAYERATTACHMENT(pPlayer, iHeight = 50, pSprite, iLife)
{
	message_begin(MSG_ALL, SVC_TEMPENTITY);
	write_byte(TE_PLAYERATTACHMENT);
	write_byte(pPlayer);
	write_coord(iHeight);
	write_short(pSprite);
	write_short(iLife); // 0.1's
	message_end();
}

// Удалить спрайт над головой
stock CREATE_KILLPLAYERATTACHMENTS(pPlayer)
{
	message_begin(MSG_ALL, SVC_TEMPENTITY);
	write_byte(TE_KILLPLAYERATTACHMENTS);
	write_byte(pPlayer);
	message_end();
}

// пушистые няши бля
stock CREATE_SPRITETRAIL(Float:vecOrigin[3], pSprite, iCount, iLife, iScale, iVelocityAlongVector, iRandomVelocity)
{
	engfunc(EngFunc_MessageBegin, MSG_PVS, SVC_TEMPENTITY, vecOrigin, 0);
	write_byte(TE_SPRITETRAIL);
	engfunc(EngFunc_WriteCoord, vecOrigin[0]); // start
	engfunc(EngFunc_WriteCoord, vecOrigin[1]);
	engfunc(EngFunc_WriteCoord, vecOrigin[2]);
	engfunc(EngFunc_WriteCoord, vecOrigin[0]); // end
	engfunc(EngFunc_WriteCoord, vecOrigin[1]);
	engfunc(EngFunc_WriteCoord, vecOrigin[2]);
	write_short(pSprite);
	write_byte(iCount);
	write_byte(iLife); // 0.1's
	write_byte(iScale);
	write_byte(iVelocityAlongVector);
	write_byte(iRandomVelocity);
	message_end();
}

// Отрисовать спрайт куда смотрит игрок (для хука)
stock CREATE_BEAMENTPOINT(pEntity, Float:vecOrigin[3], pSprite, iStartFrame = 0, iFrameRate = 0, iLife, iWidth, iAmplitude = 0, iRed, iGreen, iBlue, iBrightness, iScrollSpeed = 0)
{
	message_begin(MSG_BROADCAST, SVC_TEMPENTITY);
	write_byte(TE_BEAMENTPOINT);
	write_short(pEntity);
	engfunc(EngFunc_WriteCoord, vecOrigin[0]);
	engfunc(EngFunc_WriteCoord, vecOrigin[1]);
	engfunc(EngFunc_WriteCoord, vecOrigin[2]);
	write_short(pSprite);
	write_byte(iStartFrame);
	write_byte(iFrameRate); // 0.1's
	write_byte(iLife); // 0.1's
	write_byte(iWidth);
	write_byte(iAmplitude); // 0.01's
	write_byte(iRed);
	write_byte(iGreen);
	write_byte(iBlue);
	write_byte(iBrightness);
	write_byte(iScrollSpeed); // 0.1's
	message_end();
}

// Удалить спрайт(траил) (что идет за мячом)
stock CREATE_KILLBEAM(pEntity)
{
	message_begin(MSG_ALL, SVC_TEMPENTITY);
	write_byte(TE_KILLBEAM);
	write_short(pEntity);
	message_end();
}

stock UTIL_UserScoreMessage(pPlayer, iFrags, iDeaths)
{
	message_begin(MSG_ALL, MsgId_ScoreInfo);
	write_byte(pPlayer);	// игрок
	write_short(iFrags);	// фраги
	write_short(iDeaths); 	// смерти
	write_short(0);			//
	write_short(g_iUserTeam[pPlayer]);	// команда игрока
	message_end();
}

stock bool:is_hull_vacant(Float:vOrigin[3], iHull, id)
{
	new iTr;
	engfunc(EngFunc_TraceHull, vOrigin, vOrigin, 0, iHull, id, iTr);
	if (!get_tr2(iTr, TR_StartSolid) || !get_tr2(iTr, TR_AllSolid)) //get_tr2(iTr, TR_InOpen))
//	if (get_tr2(iTr, TR_StartSolid) || get_tr2(iTr, TR_AllSolid) || !get_tr2(iTr, TR_InOpen))
		return true;
	return false;
}

// Check if a player is stuck (credits to VEN)
stock is_player_stuck(id)
{
	static Float:originF[3];
	pev(id, pev_origin, originF);
	
	engfunc(EngFunc_TraceHull, originF, originF, 0, (pev(id, pev_flags) & FL_DUCKING) ? HULL_HEAD : HULL_HUMAN, id, 0);
	
	if (get_tr2(0, TR_StartSolid) || get_tr2(0, TR_AllSolid) || !get_tr2(0, TR_InOpen))
		return true;
	
	return false;
}

stock UTIL_DirectorMessage (
	const id, const message[], const red = 0, const green = 160, const blue = 0, 
	const Float:x = -1.0, const Float:y = 0.65, const effect = 2, const Float:fxTime = 6.0, 
	const Float:holdTime = 3.0, const Float:fadeInTime = 0.1, const Float:fadeOutTime = 1.5)
{

#define pack_color(%0,%1,%2) ( %2 + ( %1 << 8 ) + ( %0 << 16 ) )
#define write_float(%0) write_long( _:%0 )
	
	message_begin( id ? MSG_ONE : MSG_BROADCAST, SVC_DIRECTOR, .player = id );
	{
		write_byte( strlen( message ) + 31 ); // size of write_*
		write_byte( DRC_CMD_MESSAGE );
		write_byte( effect );
		write_long( pack_color( red, green, blue ) );
		write_float( x );
		write_float( y );
		write_float( fadeInTime );
		write_float( fadeOutTime );
		write_float( holdTime );
		write_float( fxTime );
		write_string( message );
	}
	message_end( );
}

// Костыль открытия дверей :D
stock open_doors_hotfix()
{
	for (new i, iDoor; i < g_iDoorListSize; i++)
	{
		iDoor = ArrayGetCell(g_aDoorList, i);
		dllfunc(DLLFunc_Use, iDoor, 0);
		dllfunc(DLLFunc_Think, iDoor);
		dllfunc(DLLFunc_Use, iDoor, 0);
	}
	g_bDoorStatus = true;
	g_bOneOpenDoors = true;
}

stock entity_set_aim(id, ent)
{
	new Float:vOrigin[3], Float:vEntOrigin[3]; 
	pev(id, pev_origin, vOrigin);
	pev(ent, pev_origin, vEntOrigin);
	
	vOrigin[0] -= vEntOrigin[0];
	vOrigin[1] -= vEntOrigin[1];
	vOrigin[2] -= vEntOrigin[2];
	
	new Float:vNewAngles[3], Float:vAimVector[3], Float:v_length;
	v_length = vector_length(vOrigin);

	vAimVector[0] = vOrigin[0] / v_length;
	vAimVector[1] = vOrigin[1] / v_length;
	vAimVector[2] = vOrigin[2] / v_length;
	
	vector_to_angle(vAimVector, vNewAngles);
	
	vNewAngles[0] *= -1;
	vNewAngles[1] += 180;
	
	set_pev(id, pev_angles, vNewAngles);
	set_pev(id, pev_angles, 1);
}

// Grab
stock UTIL_DamagePlayer(pPlayer, iArmor, iHealth, iType, vecVector[3], iReliable = 0)
{
	message_begin(iReliable ? MSG_ONE : MSG_ONE_UNRELIABLE, MsgId_Damage, _, pPlayer);
	write_byte(iArmor); //damage armor
	write_byte(iHealth); //damage health
	write_long(iType); //damage type
	write_coord(vecVector[0]); //origin[x]
	write_coord(vecVector[1]); //origin[y]
	write_coord(vecVector[2]); //origin[z]
	message_end();
}

stock CREATE_BLOODSTREAM(vecVector[3], iColIndex, iSpeed)
{
	message_begin(MSG_BROADCAST, SVC_TEMPENTITY);
	write_byte(TE_BLOODSTREAM);
	write_coord(vecVector[0]); //pos.x
	write_coord(vecVector[1]); //pos.y
	write_coord(vecVector[2] + 15); //pos.z
	write_coord(random_num(0, 255)); //vec.x
	write_coord(random_num(0, 255)); //vec.y
	write_coord(random_num(0, 255)); //vec.z
	write_byte(iColIndex); //col index
	write_byte(iSpeed); //speed
	message_end();
}

stock get_max_ammo_csw(iCSW)
{
	switch (iCSW)
	{
		case CSW_M4A1, CSW_AK47, CSW_AUG, CSW_SG552, CSW_SG550,
		CSW_MAC10, CSW_TMP, CSW_MP5NAVY, CSW_ELITE:
			return 30;
		case CSW_FAMAS, CSW_UMP45: return 25;
		case CSW_GALI: return 35;
		case CSW_M249: return 100;
		case CSW_AWP, CSW_SCOUT: return 10;
		case CSW_G3SG1, CSW_GLOCK18, CSW_FIVESEVEN: return 20;
		case CSW_XM1014, CSW_DEAGLE: return 7;
		case CSW_M3: return 8;
		case CSW_P90: return 50;
		case CSW_USP: return 12;
		case CSW_P228: return 13;
	}
	return 0;
}

stock get_max_ammo_wpnid(iWpnID)
{
	switch (iWpnID)
	{
		case WEAPON_M4A1, WEAPON_AK47, WEAPON_AUG, WEAPON_SG552, WEAPON_SG550, WEAPON_MAC10, WEAPON_TMP, WEAPON_MP5N, WEAPON_ELITE: return 30;
		case WEAPON_FAMAS, WEAPON_UMP45: return 25;
		case WEAPON_GALIL: return 35;
		case WEAPON_M249: return 100;
		case WEAPON_AWP, WEAPON_SCOUT: return 10;
		case WEAPON_G3SG1, WEAPON_GLOCK18, WEAPON_FIVESEVEN: return 20;
		case WEAPON_XM1014, WEAPON_DEAGLE: return 7;
		case WEAPON_M3: return 8;
		case WEAPON_P90: return 50;
		case WEAPON_USP: return 12;
		case WEAPON_P228: return 13;
	}
	return 0;
}



// Стандартные мессаги начинающиеся с '#'
stock UTIL_TextMsg(iPlayer, szMessage[])
{
	message_begin(MSG_ONE_UNRELIABLE, MsgId_TextMsg, _,  iPlayer);
	write_byte(4);
	write_string(szMessage);
	message_end();
}

// Мигание денег
stock UTIL_BlinkAcct(iPlayer, BlinkAmt)
{
	message_begin(MSG_ONE_UNRELIABLE, 104, _, iPlayer);
	write_byte(BlinkAmt);
	message_end();
}

stock is_valid_user_auth(id)
{
	static szAuthId[MAX_AUTHID_LENGTH];
	get_user_authid(id, szAuthId, charsmax(szAuthId));
	
	if (equal(szAuthId, "ID_PENDING")
	|| equal(szAuthId, "STEAM_ID_LAN")
	|| equal(szAuthId, "VALVE_ID_LAN")
	|| equal(szAuthId, "UNKNOWN"))
	{
		return false;
	}
	
	return true;
}


/* Немного упрощенное форматирование меню
 * Требует _menu_string[512], _menu_len 
 *****************************************/

// Заголовок меню
stock menu_title(szItem[], any:...)
{
//	_menu_len = 0;
	if (numargs() > 1)
		_menu_len = vformat(_menu_string, charsmax(_menu_string), szItem, 2);
	else
		_menu_len = copy(_menu_string, charsmax(_menu_string), szItem);
}

// Пункт меню
stock menu_item(szItem[], any:...)
{
	if (numargs() > 1)
		_menu_len += vformat(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len, szItem, 2); //
	else
		_menu_len += copy(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len, szItem);
}

// Показ меню игроку
stock menu_show(id, iKeys = 1023, const szFunc[], iTime = -1)
{
	set_member(id, m_iMenu, 0);
	jbe_informer_offset_up(id);
	return show_menu(id, iKeys, _menu_string, iTime, szFunc);
}


/*	Упрощенный список на меню старого формата
***********************************************/
/*
 * szTitle[] - Заголовок
 * iPos - позиция в меню (страница, c 0)
 * iCount - кол-во пунктов всего 
 * iPerPage - сколько выводим пунктов в меню (до 8)
 *		(если уменьшаем кол-во, добавляя свои пункты не с списка)
 * return true / false
*/
stock menu_list_title(szTitle[], iPos, iCount, iPerPage = 8, any:...)
{
	_menu_list_pos = iPos;
	_menu_list_count = iCount;
	_menu_list_pergage = iPerPage;
	_menu_list_b = 0;
	_menu_list_keys = (1<<9);
	_menu_list_start = iPos * iPerPage;
	
	if (_menu_list_start > iCount)
		_menu_list_start = iCount;
	
	_menu_list_start = _menu_list_start - (_menu_list_start % iPerPage);
		
	_menu_list_end = _menu_list_start + iPerPage;
	
	if (_menu_list_end > iCount)
		_menu_list_end = iCount;

	_menu_list_countid = _menu_list_start;

	if (numargs() > 4)
		_menu_len = vformat(_menu_string, charsmax(_menu_string), szTitle, 5);
	else
		_menu_len = copy(_menu_string, charsmax(_menu_string), szTitle);

	new iPagesNum = (iCount / iPerPage + ((iCount % iPerPage) ? 1 : 0));
	
	if (iPagesNum > 1) // если больше 1 страницы
		_menu_len += formatex(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len,
			" \y%d/%d^n", iPos + 1, iPagesNum);
	else
		_menu_len += formatex(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len, "^n");

	if (!iPagesNum) return false;
	return true;
}

/* szItem - пункт меню */
stock menu_list_item(szItem[], any:...)
{
	_menu_list_keys |= (1<<_menu_list_b);
	_menu_list_b++;

	if (numargs() > 1)
		_menu_len += vformat(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len, szItem, 2);
	else
		_menu_len += copy(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len, szItem);
}

// пока так, хз
stock menul_item_block(szItem[], any:...)
{
	_menu_list_b++;

	if (numargs() > 1)
		_menu_len += vformat(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len, szItem, 2);
	else
		_menu_len += copy(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len, szItem);
}

/* return true если еще добавляем пункты на текущую страницу
 * while (menu_list_count()) menu_list_item("item")
 */
stock bool:menu_list_count() return _menu_list_countid++ < _menu_list_end ? true : false;

// id пункта меню для масивов
stock menu_list_countid() return _menu_list_countid - 1;

// № пункта в меню (1-8)
stock menu_list_num() return _menu_list_b + 1;

// id пункта меню для многостраничников в handler 
stock menu_list_itemid(id, iKey) return g_iMenuPosition[id] * g_iMenuPerPage[id] + iKey; // (iKey + 1); 

// для менюшек где свой пункт добавляем в конце
stock menu_list_empty_items()
{
	for (new i = _menu_list_b; i < _menu_list_pergage; i++)
	{
		_menu_len += copy(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len, "^n");
		_menu_list_b++;
	}
}
 
stock menu_list_show(id, szFunc[], iTime = -1)
{
	g_iMenuPosition[id] = _menu_list_start / _menu_list_pergage;
	g_iMenuPerPage[id] = _menu_list_pergage;

	_menu_list_pergage += g_iMenuPerPage[id] < 8 ? 8 - g_iMenuPerPage[id] : 0;

	// Чтоб меню не "скукоживалось", переносы вместо пунктов
	menu_list_empty_items();

	// Кнопки: выход, далее, назад
	if (_menu_list_end < _menu_list_count)
	{
		_menu_list_keys |= (1<<8);
		_menu_len += formatex(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len,
			"^n\y9. \w%L^n\y0. \w%L",
			id, "JBE_MENU_NEXT", id, _menu_list_pos ? "JBE_MENU_BACK" : "JBE_MENU_EXIT");
	}
	else
	{
		_menu_len += formatex(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len,
			"^n^n\y0. \w%L", id, _menu_list_pos ? "JBE_MENU_BACK" : "JBE_MENU_EXIT");
	}

	/*
	if (_menu_list_end < _menu_list_count) {
		_menu_list_keys |= (1<<7);
		_menu_len += formatex(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len,
			"^n\y8. \w%L^n", id, "JBE_MENU_NEXT");
	} else
		_menu_len += formatex(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len,
			"^n\d8. %L^n", id, "JBE_MENU_NEXT");

	if (_menu_list_pos) {
		_menu_list_keys |= (1<<8);
		_menu_len += formatex(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len,
			"\y9. \w%L^n", id, "JBE_MENU_BACK");
	} else
		_menu_len += formatex(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len,
			"\d9. %L^n", id, "JBE_MENU_BACK");

	_menu_len += formatex(_menu_string[_menu_len], charsmax(_menu_string) - _menu_len,
		"\y0. %L", id, "JBE_MENU_EXIT");
	*/

	// fix crash?
	set_member(id, m_iMenu, 0);

	jbe_informer_offset_up(id);
	return show_menu(id, _menu_list_keys, _menu_string, iTime, szFunc);
}


/* Paint
***********/

/* Для паинта, рисует линию*/
//stock CREATE_BEAMPOINTS
stock draw_line(
	Float:vecOrigin1[3], Float:vecOrigin2[3], pSprite, iStartFrame = 0, 
	iFrameRate = 0, iLife, iWidth, iAmplitude = 0, 
	iRed, iGreen, iBlue, iBrightness, iScrollSpeed = 0)
{
	message_begin(MSG_BROADCAST, SVC_TEMPENTITY);
	write_byte(TE_BEAMPOINTS);
	engfunc(EngFunc_WriteCoord, vecOrigin1[0]);
	engfunc(EngFunc_WriteCoord, vecOrigin1[1]); 
	engfunc(EngFunc_WriteCoord, vecOrigin1[2]);
	engfunc(EngFunc_WriteCoord, vecOrigin2[0]);
	engfunc(EngFunc_WriteCoord, vecOrigin2[1]);
	engfunc(EngFunc_WriteCoord, vecOrigin2[2]);
	write_short(pSprite);
	write_byte(iStartFrame);
	write_byte(iFrameRate);
	write_byte(iLife);
	write_byte(iWidth);
	write_byte(iAmplitude);
	write_byte(iRed);
	write_byte(iGreen);
	write_byte(iBlue);
	write_byte(iBrightness);
	write_byte(iScrollSpeed);
	message_end();
}

//from fakemeta_util.inc
stock fm_get_aim_origin(index, Float:origin[3])
{
	static Float:start[3], Float:view_ofs[3];
	get_entvar(index, var_origin, start);
	get_entvar(index, var_view_ofs, view_ofs);
	xs_vec_add(start, view_ofs, start);
	
	static Float:dest[3];
	get_entvar(index, var_v_angle, dest);
	engfunc(EngFunc_MakeVectors, dest);
	global_get(glb_v_forward, dest);
	xs_vec_mul_scalar(dest, 9999.0, dest);
	xs_vec_add(start, dest, dest);
	
	engfunc(EngFunc_TraceLine, start, dest, 0, index, 0);
	get_tr2(0, TR_vecEndPos, origin);
	
	return 1;
}

stock move_toward_client(id, Float:origin[3])
{   
	static Float:player_origin[3];
	
	get_entvar(id, var_origin, player_origin);
	
	origin[0] += (player_origin[0] > origin[0]) ? 1.0 : -1.0;
	origin[1] += (player_origin[1] > origin[1]) ? 1.0 : -1.0;
	origin[2] += (player_origin[2] > origin[2]) ? 1.0 : -1.0;
}

//Thanks AdaskoMX!
bool:is_aiming_at_sky(index)
{
	new Float:origin[3];
	fm_get_aim_origin(index, origin);

	return engfunc(EngFunc_PointContents, origin) == CONTENTS_SKY;
}

/*
register_plugin(PLUGIN, fmt("%d", valve_build_number(__DATE__)-valve_build_number("10/24/1996")), AUTHOR);
Где вычитается дата генерированная при компиляции и дата первого релиза
(для примера дата из Half-Life). и получается количество дней между этими датами.
Например в последней версии HLDS это 8308
*/
stock valve_build_number(date[])
{ 
    static mond[12] =
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    new m, d, y, b = 0;

    new mon = str_to_num(date[0]);
    for (m = 0; m < 11; m++)
    {
        if (mon == m+1)
            break;
        d += mond[m];
    }

    d += str_to_num(date[3]) - 1;
    y = str_to_num(date[6]) - 1900;
    b = d + floatround((y - 1) * 365.25, floatround_floor);

    if (((y % 4) == 0) && m > 1)
    {
        b += 1;
    }
    return b;
}

/* Проверка существования say* команды */
stock bool:IsValidSayCmd(const szCmd[])
{
    new iNumCmd = get_clcmdsnum(-1), i , szCommand[64] , iFlags , szInfo[128];

    new szSayCmd[64], szSayTeamCmd[64];
    formatex(szSayCmd, charsmax(szSayCmd), "say %s", szCmd);
    formatex(szSayTeamCmd, charsmax(szSayTeamCmd), "say_team %s", szCmd);

    for (i = 0; i < iNumCmd; i++) {
        get_clcmd(i, szCommand, charsmax(szCommand), iFlags, szInfo, charsmax(szInfo), -1);
        
        if (strcmp(szCommand, szSayCmd) == 0
        || strcmp(szCommand, szSayTeamCmd) == 0)
            return true;
    }
    return false;
}



/*===== <- Стоки <- =====*///

enum _:ScoreAttribFlags
{
    SCORE_ATTRIB_NONE   = 0,
    SCORE_ATTRIB_DEAD   = (1<<0),
    SCORE_ATTRIB_BOMB   = (1<<1),
    SCORE_ATTRIB_VIP    = (1<<2)
};

stock set_score_attrib(const pPlayer, const iFlag)
{
    message_begin(MSG_ONE, MsgId_ScoreAttrib, .player = pPlayer);
    write_byte(pPlayer);
    write_byte(iFlag);
    message_end();
}

/**
 * Flags used in hide_hud_elements()
 */
enum HideElemenentFlags (<<= 1)
{
	HideElement_None = 0,
	HideElement_Cross_Ammo_WPNList = 1,
	HideElement_Flashlight,
	HideElement_All,
	HideElement_Radar_Health_Armor,
	HideElement_Timer,
	HideElement_Money,
	HideElement_Crosshair
}

/**
 * Hides specific elements from the HUD.
 *
 * @note The symbol + means that an additional crosshair will be drawn.
 * This crosshair looks not like the regular one, but like the one that
 * is drawn in spectator mode. You can remove this crosshair by setting
 * the "noadd" argument to "true".
 *
 * @param id            Client index or 0 for all clients
 * @param elements      HUD elements to hide. The names are self-explanatory:
 *                        HideElement_Cross_Ammo_WPNList
 *                        HideElement_Flashlight (+)
 *                        HideElement_All
 *                        HideElement_Radar_Health_Armor (+)
 *                        HideElement_Timer (+)
 *                        HideElement_Money (+)
 *                        HideElement_Crosshair
 * @param noadd         If set to false and the chosen element names have
 *                      a "+" sign, an additional crosshair will be drawn.
 * @param reliable      If true, the message will be sent via the reliable
 *                      channel, otherwise it will use the unreliable one
 *
 * @return              0 if "id" is non-zero and the client isn't connected,
 *                      1 otherwise
 */
stock hide_hud_elements(id, HideElemenentFlags:elements, bool:noadd = true, bool:reliable = true)
{
	if(id && !is_user_connected(id))
		return 0;

	static MSG_HideWeapon;

	if(!MSG_HideWeapon)
		MSG_HideWeapon = get_user_msgid("HideWeapon");

	new iDest = get_msg_destination(id, reliable);

	message_begin(iDest, MSG_HideWeapon, .player = id);
	write_byte(_:elements);
	message_end();

	if(noadd)
	{
		static MSG_Crosshair;

		if(!MSG_Crosshair)
			MSG_Crosshair = get_user_msgid("Crosshair");

		message_begin(iDest, MSG_Crosshair, .player = id);
		write_byte(0);
		message_end();
	}

	return 1;
}

stock get_msg_destination(id, bool:reliable)
{
	if(id)
		return reliable ? MSG_ONE : MSG_ONE_UNRELIABLE;

	return reliable ? MSG_ALL : MSG_BROADCAST;
}

stock rg_get_user_shield(id)
{
	return get_member(id, m_bOwnsShield);
}

// для отброса в дуэли
stock Float:EstimateDamage(Float:fPoint[3], ent, ignored) 
{
	new Float:fOrigin[3];
	new tr;
	new Float:fFraction;
	
	pev(ent, pev_origin, fOrigin);
	engfunc(EngFunc_TraceLine, fPoint, fOrigin, DONT_IGNORE_MONSTERS, ignored, tr);
	get_tr2(tr, TR_flFraction, fFraction);
	
	if (fFraction == 1.0 || get_tr2( tr, TR_pHit ) == ent)
		return 1.0;
	
	return 0.6;
}

// делаем отброс (для дуэли)
stock CreateKnockBack(iVictim, Float:vAttacker[3], Float:fMulti, Float:fRadius)
{
	new Float:vVictim[3];
	pev(iVictim, pev_origin, vVictim);

	xs_vec_sub(vVictim, vAttacker, vVictim);

	xs_vec_mul_scalar(vVictim, fMulti * 0.7, vVictim);
	xs_vec_mul_scalar(vVictim, fRadius / xs_vec_len(vVictim), vVictim);

	vVictim[2] = 0.0; // Чтоб не кидало вверх
	set_pev(iVictim, pev_velocity, vVictim);
}


stock SendWeaponList(iPlayer, const sName[], iAmmo1, iMaxAmmo1, iAmmo2, iMaxAmmo2, iSlot, iPosition, iId, iFlags)
{
    static iMsgId_WeaponList;

    if (!iMsgId_WeaponList)
        iMsgId_WeaponList = get_user_msgid("WeaponList");

    message_begin(MSG_ONE_UNRELIABLE, iMsgId_WeaponList, _, iPlayer);
    write_string(sName);
    write_byte(iAmmo1);
    write_byte(iMaxAmmo1);
    write_byte(iAmmo2);
    write_byte(iMaxAmmo2);
    write_byte(iSlot);
    write_byte(iPosition);
    write_byte(iId);
    write_byte(iFlags);
    message_end();
}

stock SendItemPickup(const id, const weaponId)
{
    static iMsgId_WeapPickup;

    if (!iMsgId_WeapPickup)
        iMsgId_WeapPickup = get_user_msgid("WeapPickup");

    message_begin(MSG_ONE_UNRELIABLE, iMsgId_WeapPickup, _, id);
    write_byte(weaponId);
    message_end();
}

// Убираем кривой синтаксис с запроса на никах
// szOutput / szName[64] - строка под имя должна быть больше, от ошибки: replace() buffer not big enough (14>=13)
stock mysql_escape_string(szOutput[], iLen)
{
	static const szReplaceIn[][] = { "\\", "\0", "\n", "\r", "\x1a", "'", "^"" };
	static const szReplaceOut[][] = { "\\\\", "\\0", "\\n", "\\r", "\Z", "\'", "\^"" };
	for (new i; i < sizeof szReplaceIn; i++)
		replace_all(szOutput, iLen, szReplaceIn[i], szReplaceOut[i]);
}

// by Mistrick
stock translite_string(string[], size, source[])
{
    static const table[][] = {
        "Э", "#", ";", "%", "?", "э", "(", ")", "*", "+", "б", "-", "ю", ".", "0", "1", "2", "3", "4",
        "5", "6", "7", "8", "9", "Ж", "ж", "Б", "=", "Ю", ",", "^"", "Ф", "И", "С", "В", "У", "А", "П",
        "Р", "Ш", "О", "Л", "Д", "Ь", "Т", "Щ", "З", "Й", "К", "Ы", "Е", "Г", "М", "Ц", "Ч", "Н", "Я",
        "х", "\", "ъ", ":", "_", "ё", "ф", "и", "с", "в", "у", "а", "п", "р", "ш", "о", "л", "д", "ь",
        "т", "щ", "з", "й", "к", "ы", "е", "г", "м", "ц", "ч", "н", "я", "Х", "/", "Ъ", "Ё"
    };
    
    new len = 0;
    for (new i = 0; source[i] != EOS && len < size; i++)
    {
        new ch = source[i];
        
        if ('"' <= ch <= '~')
        {
            ch -= '"';
            string[len++] = table[ch][0];
            if (table[ch][1] != EOS)
            {
                string[len++] = table[ch][1];
            }
        }
        else
        {
            string[len++] = ch;
        }
    }
    string[len] = EOS;
    
    return len;
}