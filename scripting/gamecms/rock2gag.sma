/*
*	Rock to Ban
*	  Every player are able to rock the vote to ban somebody.
*
*	Last update:
*	  12/05/2012
*
	Credits:
*	 Alka for his 'Vote Ban' plugin http://forums.alliedmods.net/showthread.php?t=96586
*	  I got the idea and took part of his code
*
*	This file is provided as is (no warranties).
*
*	Переделка плагина Rock to Ban (AUTHOR "Safety1st") под плагин GameCMS_Gagmanager
*/

#include <amxmodx>
#include <gamecms5>

#define PLUGIN "From Rock to Ban"
#define VERSION "1.02"
#define AUTHOR "Safety1st++"

// customizable parameters
#define MAX_PLAYERS 32
#define IMMUNITY_FLAG (ADMIN_IMMUNITY)	/* flags 'a' & 'd', separate by '|' */
new gszPrefix[] = "[VoteGAG]"	// set prefix for all chat messages; for example ">"
const VOTEGAG_NEED = 4	// how many players are required to enable voteban function
// end of customizable parameters

new giVotedPlayers[MAX_PLAYERS + 1]	// player's voteban targets
new giVotes[MAX_PLAYERS + 1]	// count of votes for ban that player
new giVoted[MAX_PLAYERS + 1]	// count of votes for ban by that player

// macro; %1 - variable being modified, %2 - player id
#define CheckFlag(%1,%2)	( %1 &   ( 1 << (%2-1) ) )
#define AddFlag(%1,%2)		( %1 |=  ( 1 << (%2-1) ) )
#define RemoveFlag(%1,%2)	( %1 &= ~( 1 << (%2-1) ) )

enum _:Labels {
	CVAR_PERCENT = 0,

	CVAR_BANTIME,
	CVAR_LIMIT,
	CVAR_REASON,
	CVAR_LOG
}
new pCvar[Labels]

enum _:LogRecords {
	UNVOTE = 0,
	VOTE
}

new gszLogRecords[LogRecords][] = {
	"Player '%s' has canceled vote for banning '%s'",
	"Player '%s' has been voted for banning by '%s'"
}

public plugin_init() {
	register_plugin( PLUGIN, VERSION, AUTHOR )
	register_dictionary( "rock2gag.txt" )
	register_cvar( "rock2gag", VERSION, FCVAR_SERVER|FCVAR_SPONLY|FCVAR_UNLOGGED )

	register_saycmd( "votegag", "VoteGagMenu" )

	pCvar[CVAR_PERCENT] = register_cvar( "votegag_percent", "60" )
	pCvar[CVAR_BANTIME] = register_cvar( "votegag_time", "20" )
	pCvar[CVAR_LIMIT] = register_cvar( "votegag_limit", "3" )
	pCvar[CVAR_REASON] = register_cvar( "votegag_reason", "Заблочен игроками" )
	pCvar[CVAR_LOG] = register_cvar( "votegag_log", "1" )
	
	server_cmd("exec addons/amxmodx/configs/amxx.cfg");
}

register_saycmd( saycommand[], function[] ) {
	new szTemp[64]
	formatex( szTemp, charsmax(szTemp), "say %s", saycommand )
	register_clcmd( szTemp, function )
	formatex( szTemp, charsmax(szTemp), "say_team %s", saycommand )
	register_clcmd( szTemp, function )
	formatex( szTemp, charsmax(szTemp), "say /%s", saycommand )
	register_clcmd( szTemp, function )
	formatex( szTemp, charsmax(szTemp), "say .%s", saycommand )
	register_clcmd( szTemp, function )
	formatex( szTemp, charsmax(szTemp), "say_team /%s", saycommand )
	register_clcmd( szTemp, function )
	formatex( szTemp, charsmax(szTemp), "say_team .%s", saycommand )
	register_clcmd( szTemp, function )
}

public client_disconnected(id) {
	static iPlayers[32], iPlayersNum, i, iPlayer

	// check whether there were votes for ban by that player
	if ( giVoted[id] ) {
		get_players( iPlayers, iPlayersNum, "ch" )	// skip bots and HLTV
		for ( i = 0; i < iPlayersNum; i++ ) {
			iPlayer = iPlayers[i]
			if ( CheckFlag( giVotedPlayers[id], iPlayer ) )
				giVotes[iPlayer]--
		}
		giVotedPlayers[id] = 0
		giVoted[id] = 0
	}

	// check whether there were votes for ban that player
	if ( giVotes[id] ) {
		get_players( iPlayers, iPlayersNum, "ch" )	// skip bots and HLTV
		for ( i = 0; i < iPlayersNum; i++ ) {
			iPlayer = iPlayers[i]
			if ( CheckFlag( giVotedPlayers[iPlayer], id ) ) {
				RemoveFlag( giVotedPlayers[iPlayer], id )
				giVotes[id]--
				giVoted[iPlayer]--
			}
			if ( !giVotes[id] )
				break
		}
		giVotes[id] = 0	// just in case
	}
}

public VoteGagMenu(id) {
	static iPlayers[32], iPlayersNum, i, iPlayer

	get_players( iPlayers, iPlayersNum, "ch" )	// skip bots and HLTV
	if ( iPlayersNum < VOTEGAG_NEED ) {
		ColorPrint( id, "^4%s %L", gszPrefix, id, "VOTEGAG_NEEDX", VOTEGAG_NEED )
		return PLUGIN_HANDLED
	}

	// create and show menu
	new szTempString[64], szName[32], szInfo[3]
	formatex( szTempString, charsmax(szTempString), "%L\y:", id, "VOTEGAG_MENU" )
	new iMenu = menu_create( szTempString, "MenuHandle", .ml = 1 )
	new iCallback = menu_makecallback( "CallbackMenu" )
	menu_setprop( iMenu, MPROP_NUMBER_COLOR, "\r" )
	menu_setprop( iMenu, MPROP_EXIT, MEXIT_ALL )
	formatex( szTempString, charsmax(szTempString), "%L", id, "VOTEGAG_EXIT" )
	menu_setprop( iMenu, MPROP_EXITNAME, szTempString )
	formatex( szTempString, charsmax(szTempString), "%L", id, "VOTEGAG_NEXT" )
	menu_setprop( iMenu, MPROP_NEXTNAME, szTempString )
	formatex( szTempString, charsmax(szTempString), "%L", id, "VOTEGAG_BACK" )
	menu_setprop( iMenu, MPROP_BACKNAME, szTempString )

	new iPercent
	for ( i = 0; i < iPlayersNum; i++ ) {
		iPlayer = iPlayers[i]
		get_user_name( iPlayer, szName, 31 )
		if ( get_user_flags(iPlayer) & IMMUNITY_FLAG ) {
			// don't set info[] if player has immunity
			menu_additem( iMenu, szName, "", .callback = iCallback )
		}
		else {
			iPercent = get_percent( giVotes[iPlayer], iPlayersNum )
			if ( giVotes[iPlayer] && CheckFlag( giVotedPlayers[id], iPlayer ) )
				formatex( szTempString, charsmax(szTempString), "%s \d(\r%d%%\d) \y%L", szName, iPercent, id, "VOTEGAG_VOTED" )
			else
				formatex( szTempString, charsmax(szTempString), "%s \d(\r%d%%\d)", szName, iPercent )
			num_to_str( iPlayer, szInfo, charsmax(szInfo) )
			menu_additem( iMenu, szTempString, szInfo, .callback = iCallback )
		}
	}

	menu_display( id, iMenu )

	return PLUGIN_CONTINUE
}

public CallbackMenu( id, menu, item ) {
	new access, info[3], callback, szTempString[64]
	menu_item_getinfo( menu, item, access, info, charsmax(info), szTempString, charsmax(szTempString), callback )

	if ( !info[0] )
		// player has immunity
		return ITEM_DISABLED

	if ( str_to_num(info) == id )
		return ITEM_DISABLED

	return ITEM_ENABLED
}

public MenuHandle( id, menu, item ) {
	if ( item == MENU_EXIT ) {
		menu_destroy(menu)
		return PLUGIN_HANDLED
	}

	new access, info[3], callback
	menu_item_getinfo( menu, item, access, info, charsmax(info), .callback = callback )
	menu_destroy(menu)

	new iTarget = str_to_num(info)

	if ( !is_user_connected(iTarget) ) {
		VoteGagMenu(id)
		return PLUGIN_HANDLED
	}

	if ( CheckFlag( giVotedPlayers[id], iTarget ) ) {
		RemoveFlag( giVotedPlayers[id], iTarget )
		giVoted[id]--
		giVotes[iTarget]--

		new szName[32], szTargetName[32]
		get_user_name( id, szName, 31 )
		get_user_name( iTarget, szTargetName, 31 )
		MsgToLog( gszLogRecords[UNVOTE], szName, szTargetName )
		ColorPrint( 0, "^4%s %L", gszPrefix, LANG_SERVER, "VOTEGAG_UNVOTE", szName, szTargetName )
		client_print( id, print_center, "%L", id, "VOTEGAG_CLEAR" )
		return PLUGIN_HANDLED
	}

	new iLimit = get_pcvar_num( pCvar[CVAR_LIMIT] )
	if ( giVoted[id] >= iLimit ) {
		// don't let vote too much ;)
		client_print( id, print_center, "%L", id, "VOTEGAG_LIMIT", iLimit )
		return PLUGIN_HANDLED
	}

	client_print( id, print_center, "%L", id, "VOTEGAG_SET" )
	giVoted[id]++
	giVotes[iTarget]++
	AddFlag( giVotedPlayers[id], iTarget )

	CheckVotes( iTarget, id )

	return PLUGIN_HANDLED
}

CheckVotes(target, voter)
{
	new szName[32], szTargetName[32];
	get_user_name(voter, szName, charsmax(szName));
	get_user_name(target, szTargetName, charsmax(szTargetName));

	MsgToLog(gszLogRecords[VOTE], szTargetName, szName);
	ColorPrint(0, "^4%s %L", gszPrefix, LANG_SERVER, "VOTEGAG_VOTE", szName, szTargetName);

	new iPlayers[32], iPlayersNum;
	get_players(iPlayers, iPlayersNum, "ch");	// skip bots and HLTV

	if (get_percent(giVotes[target], iPlayersNum) < get_pcvar_num(pCvar[CVAR_PERCENT]))
		return;

	new iBanTime = get_pcvar_num(pCvar[CVAR_BANTIME]);
	new szReason[256];
	get_pcvar_string(pCvar[CVAR_REASON], szReason, charsmax(szReason));

	cmsgag_add_user_block(target, 1, 1, szReason, iBanTime);
}

get_percent( value, tvalue ) {
	return floatround( floatmul( float(value) / float(tvalue) , 100.0 ) )
}

MsgToLog( szRawMessage[], any:... ) {
	if ( !get_pcvar_num( pCvar[CVAR_LOG] ) )
		return

	static szLogFile[192] = "", szTime[32], fp
	if ( !szLogFile[0] ) {
		new szLogsDir[64], szDate[16]
		get_time ( "%Y%m", szDate, charsmax(szDate) )
		get_localinfo( "amxx_logs", szLogsDir, 63 ) 
		formatex( szLogFile, charsmax(szLogFile), "%s/votegag_%s.log", szLogsDir, szDate )
	}

	new szMessage[192]
	vformat( szMessage, charsmax( szMessage ), szRawMessage, 2 )

	get_time( "%m/%d/%Y - %H:%M:%S", szTime, 31 ) 
	fp = fopen( szLogFile, "a" )
	fprintf( fp, "L %s: %s^n", szTime, szMessage )
	fclose(fp) 
}

ColorPrint( iReceiver, const szRawMessage[ ], any:... ) {
	static iMsgSayText = 0
	if( !iMsgSayText )
		iMsgSayText = get_user_msgid( "SayText" )

	new szMessage[192], iPlayers[32], iPlayersNum = 1, iPlayer
	vformat( szMessage, charsmax(szMessage), szRawMessage, 3 )
	replace_all( szMessage, charsmax(szMessage), "!n", "^1" )
	replace_all( szMessage, charsmax(szMessage), "!t", "^3" )
	replace_all( szMessage, charsmax(szMessage), "!g", "^4" )
	if ( szMessage[0] != '^1' || szMessage[0] != '^3' || szMessage[0] != '^4' )
		format( szMessage, charsmax(szMessage), "^1%s", szMessage )	// we must set initial default color if it is not provided explicitly

	if ( iReceiver )
		iPlayers[0] = iReceiver
	else {
		get_players( iPlayers, iPlayersNum, "ch" )	// skip bots and HLTV
		if ( !iPlayersNum )
			return	// don't print useless message
	}

	for ( new i = 0 ; i < iPlayersNum ; i++ ) {
		iPlayer = iPlayers[i]
		message_begin( MSG_ONE_UNRELIABLE, iMsgSayText, _, iPlayer )
		write_byte(iPlayer)	// use target player as sender to see colors at all (and his own team color for ^3)
		write_string(szMessage)
		message_end()
	}
}