/********************************
*	Shared Functions Header File
*********************************/

/* Сохранение настроек
=============================================*/

#define SETTING_STR_SIZE 64

// вкл сохранение настроек, если убрать то придется чет редактировать c:
#define ENABLE_SAVE_SETTING

// Под настройки
#define PRISONER_MODEL_SKINS	5 // кол-во скинов на модели зеков (без фд и розыска)
#define PRISONER_SKIN_VIP		5 // vip skin
#define GUARD_WEAPON_MENU_NUM	8 // кол-во ганов в меню кт (пунктов меню)

enum _: DATA_SAVE_SETTING
{
	SETTING_CHAT_LANG,		// Язык чата rus | eng
//	SETTING_HUD,			// Информер on | off
	SETTING_ROUND_SOUND,	// Round Sound on | off
	SETTING_DEATH_MENU,		// Меню мертвого при смерти on | off
	SETTING_OFF_PM,			// Приват сообщения off | on
	SETTING_GR_AABUY,		// Авто закуп оружия для КТ
	SETTING_GR_AATYPE,		// Выбранное оружие для авто-закупа КТ
	SETTING_GRAB_MTYPE,		// Выбранный тип меню граба
	SETTING_PN_SKIN,		// Выбранный скин за зеков
	SETTING_COSTUMES,		// id Шапки
	SETTING_PERSONAL_MODEL,	// Персональная модель

	SETTING_HOOK_SPEED,			//
	SETTING_HOOK_COLOUR,		//
	SETTING_HOOK_WIDTH,			// 
	SETTING_HOOK_AMPLITUDE,		// 
	SETTING_HOOK_TRAIL_COLOUR,	// 
	SETTING_HOOK_TRAIL_WIDTH,	// 
	SETTING_HOOK_TRAIL_LENGHT,	// 
	SETTING_HOOK_NEW_HOOK,		// 
	SETTING_HOOK_MENU_ON, 		// 

	SETTING_BUTTON_SOUND,		// g_iBitButtonSound
	SETTING_DEMO_ID,			//
};
