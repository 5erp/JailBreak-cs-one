/********************************
*	Shared Functions Header File
*********************************/

#define PLAYERS_PER_PAGE		8

#define RETURN_CHIEF 1
#define RETURN_ADMIN 2

#define MENU_ALL_KEYS	1023
//#define MENU_ALL_KEYS (1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9)

/* -> Массивы для меню из игроков (и не только :D) -> */
new g_iMenuPlayers[MAX_PLAYERS + 1][MAX_PLAYERS];
new g_iMenuPosition[MAX_PLAYERS + 1];
new g_iMenuTarget[MAX_PLAYERS + 1];
new g_iMenuType[MAX_PLAYERS + 1];	// тип выбора

// кол-во пунктов выводимых в текущем меню игрока (для list menu)
new g_iMenuPerPage[MAX_PLAYERS + 1]; 

// Под упрощенное форматирование меню 
stock _menu_string[MAX_MENU_LENGTH];
stock _menu_len;

// Под упрощенный список с меню 
// (общие переменные - используются при 'отрисовке' меню)
stock _menu_list_pos;
stock _menu_list_count;
stock _menu_list_start;
stock _menu_list_end;
stock _menu_list_keys;
stock _menu_list_b;
stock _menu_list_pergage;
stock _menu_list_countid;

stock _menu_list_itemsnum;	// 7 или 8 пунктов на меню