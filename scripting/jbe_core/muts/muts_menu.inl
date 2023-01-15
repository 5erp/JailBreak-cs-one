//

Show_MutsMenu(id, iPos = 0)
{

}

public Handle_MutsMenu(id, key)
{

}

show_muts_menu(id, pos = 0)
{
	if (pos < 0) return PLUGIN_HANDLED;

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsNotSetBit(g_iBitUserConnected, i))
			continue;


	}


	return show_list_menu(id, "show_muts_menu");
}

public handle_muts_menu(id, key)
{
	switch (key)
	{
		case 8:
		case 9:
	}


}