stock const g_chConvert[94 * 2 char] = {
	0x2100D0AD, 0x2300D0B6, 0x25003F00, 0xD18D2800, 0x29002A00, 0x2B00D0B1, 0x2D00D18E, 0x2E003000,
	0x31003200, 0x33003400, 0x35003600, 0x37003800, 0x3900D096, 0xD0B6D091, 0x3D00D0AE, 0x2C002200,
	0xD0A4D098, 0xD0A1D092, 0xD0A3D090, 0xD09FD0A0, 0xD0A8D09E, 0xD09BD094, 0xD0ACD0A2, 0xD0A9D097,
	0xD099D09A, 0xD0ABD095, 0xD093D09C, 0xD0A6D0A7, 0xD09DD0AF, 0xD1855C00, 0xD18A3A00, 0x5F00D191,
	0xD184D0B8, 0xD181D0B2, 0xD183D0B0, 0xD0BFD180, 0xD188D0BE, 0xD0BBD0B4, 0xD18CD182, 0xD189D0B7,
	0xD0B9D0BA, 0xD18BD0B5, 0xD0B3D0BC, 0xD186D187, 0xD0BDD18F, 0xD0A52F00, 0xD0AAD081
};

stock register_clcmdex(szCommand[], szHandler[], iCmd = 0) {
#define MAX_CMD_SIZE 24
	new const szRegisters[][] = { "say /", "say_team /", "say !", "say_team !", "say .", "say_team ."};
	new szLine[MAX_CMD_SIZE * 2 + 11];
	// команды по русски))
	new sConvertStr[MAX_CMD_SIZE * 2 + 1], nLen;
	for (new n; szCommand[n] != EOS; n++) {
		if ('!' <= szCommand[n] <= '~') {
			sConvertStr[nLen++] = g_chConvert{(szCommand[n] - '!') * 2};
			if (g_chConvert{(szCommand[n] - '!') * 2 + 1} != EOS)
				sConvertStr[nLen++] = g_chConvert{(szCommand[n] - '!') * 2 + 1};
		} else sConvertStr[nLen++] = szCommand[n];
	} sConvertStr[nLen] = EOS;
	
	if (iCmd) register_clcmd(szCommand, szHandler);
	for (new i; i < sizeof szRegisters; i++) {
		formatex(szLine, charsmax(szLine), "%s%s", szRegisters[i], szCommand);
		register_clcmd(szLine, szHandler);
		
		formatex(szLine, charsmax(szLine), "%s%s", szRegisters[i], sConvertStr);
		register_clcmd(szLine, szHandler);
	}
}