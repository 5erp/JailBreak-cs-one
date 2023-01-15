// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

/********************************
*	Shared Functions Header File
*********************************/

/*==================================================
 #			-> Персональные модели ->
====================================================*/

#define PERSONAL_MODEL_NONE	-1
#define PERSONAL_MODEL_HIDE	0

enum _:DATA_PERSONAL_MODELS
{
	FLAG_PM[16], 
	MODEL_PM[64], 
	SUB_MODEL_PM[4], 
	NAME_PM[48] 
};

// Все модели с конфига
new Array:g_aDataPersonalModels;

// все модели игрока по id с g_aDataPersonalModels
new Array:g_aPersonalModelsUser[MAX_PLAYERS + 1];

// кол-во всех моделей
new g_iPersonalModelsAllSize;

// текущая модель игрока за т и кт (-1 если снял)
new g_iPersonalModel[MAX_PLAYERS + 1];

// Кол-во моделей у игрока (для цикла в меню)
new g_iPersonalModelsUserSize[MAX_PLAYERS + 1];

// Пак персональных моделей
new g_iBitUserModelsPack;


/*==================================================
 #			<- Персональные модели <-
====================================================*/