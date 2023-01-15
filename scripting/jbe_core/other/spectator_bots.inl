// Spectator Bots

#define SPEC_BOT_PL_NUM 16 // игроков для как бота

stock const g_szSpectBotsNames[][] = {
	"Наш сайт: CS-ONE.RU",
	"BUY VIP|ADMIN: cs-one.ru/buy",
	"FREE VIP: cs-one.ru/freecase",
	"Tвoй 6oнyc: cs-one.ru/bonus",
	"VK group: vk.com/CsOneRu",
//	"TG chat: t.me/csonetg",
	"Discord: cs-one.ru/ds",
	"Помощь: cs-one.ru/support",
	"CS-One.Ru | vk.com/CsOneRu"
};


stock spectbot_check(bool:bBotsKick = false)
{
#define SPEC_BOT_PL_NUM 16

	static iSpecBot;

	if (iSpecBot == -1)
		return;

	if (bBotsKick)
	{
		server_cmd("kick #%d", get_user_userid(iSpecBot));
		iSpecBot = -1;
		return;
	}
	
	new iPlNum = g_iPlayersNum[0] + g_iPlayersNum[1] + g_iPlayersNum[2] + g_iPlayersNum[3];
	if (iPlNum > SPEC_BOT_PL_NUM && iSpecBot)
	{
		server_cmd("kick #%d", get_user_userid(iSpecBot));
		iSpecBot = 0;
	}
	else if (!iSpecBot && iPlNum <= SPEC_BOT_PL_NUM)
	{
		iSpecBot = engfunc(EngFunc_CreateFakeClient, g_szSpectBotsNames[ random( sizeof(g_szSpectBotsNames) ) ]);
		if (0 < iSpecBot)
		{
			dllfunc(MetaFunc_CallGameEntity, "player", iSpecBot);
			set_pev(iSpecBot, pev_flags, FL_FAKECLIENT);
			set_pev(iSpecBot, pev_model, "");
			set_pev(iSpecBot, pev_viewmodel2, "");
			set_pev(iSpecBot, pev_modelindex, 0);
			set_pev(iSpecBot, pev_deadflag, DEAD_DEAD);
		}
		else
		{
			iSpecBot = 0;
			return;
		}
	}

	if (iSpecBot)
	{
	//	set_pdata_int(iSpecBot, m_bHasChangeTeamThisRound, true);
	//	set_pdata_int(iSpecBot, m_iSpawnCount, 1);
		
		if (IsSetBit(g_iBitUserAlive, iSpecBot) || is_user_alive(iSpecBot))
		{
			server_cmd("kick #%d", get_user_userid(iSpecBot));
			iSpecBot = 0;
			return;
		}

		engclient_cmd(iSpecBot, "jointeam", "6");
		if (get_member(iSpecBot, m_iTeam) != 3)
		{
			server_cmd("kick #%d", get_user_userid(iSpecBot));
			iSpecBot = 0;
		}
	}
}