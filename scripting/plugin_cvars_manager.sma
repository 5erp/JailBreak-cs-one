/**
	v1.1	
			* Добавлен натив plugins_load_cvars - загрузка своих конфигов
			* Добавлен CLEAR_DATA_LOADS - очищать ли конфиги после полной загрузки
	v1.2	
			* Убрано определение типа квара '_s' и '_f', строки обязательно оборачиваються в " " (кавычки)
			* Квар может содержать разные типы аргументов (строка, дробное, целое)
 */

#define CLEAR_DATA_LOADS // очищать ли конфиги после полной загрузки
//#define DEBUG

#include <amxmodx>

#define CVARS_MANAGER_KEY_STRING			128		// Длинна Названия квара
#define CVARS_MANAGER_VALUE_STRING			512		// Длинна Значения квара

// File Config
new const g_szPluginCvarsFile[] = "config_plugins.ini";

// Задаем переменные
new g_szKey[CVARS_MANAGER_KEY_STRING], g_szValue[CVARS_MANAGER_VALUE_STRING];

// Trie Handle
new Trie:g_TrieCvarsInt, Trie:g_TrieCvarsFloat, Trie:g_TrieCvarsString;

/*===============================================================================================================================================
																		[Plugin Precache]
================================================================================================================================================*/
public plugin_precache()
{
	register_plugin("PCM", "1.2 Beta", "SISA/Serp");

	// Задаем переменные
	new szConfigFile[128], szCfgDir[64], szMapName[32];

	// Создаем ассоциативные Trie массивы
	g_TrieCvarsInt = TrieCreate();
	g_TrieCvarsFloat = TrieCreate();
	g_TrieCvarsString = TrieCreate();

	// Находим название Карты
	get_mapname(szMapName, charsmax(szMapName));

	// Получаем путь к папке с Конфигами
	get_localinfo("amxx_configsdir", szCfgDir, sizeof(szCfgDir));

	// Добавляем название файла к названию Попки с конфигами 
	formatex(szConfigFile, charsmax(szConfigFile), "%s/%s", szCfgDir, g_szPluginCvarsFile);

	// Если файл отсутствует
	if(!file_exists(szConfigFile))
		return;

	// Получаем все данные по Кварам
	Get_All_Cvars(szConfigFile);

	// Добавляем к этому пути папку с конфигами карт
	formatex(szConfigFile, charsmax(szConfigFile), "%s/maps/config_%s.ini", szCfgDir, szMapName);

	// Если файл отсутствует
	if(!file_exists(szConfigFile))
		return;

	// Повторно Получаем все данные по Кварам
	Get_All_Cvars(szConfigFile);
}

/*===============================================================================================================================================
																		[Natives]
================================================================================================================================================*/
public plugin_natives()
{
	register_native("plugins_get_cvar_int", "native_get_cvar_int", 0);
	register_native("plugins_get_cvar_str", "native_get_cvar_str", 0);
	register_native("plugins_get_cvar_float", "native_get_cvar_float", 0);
	register_native("plugins_load_cvars", "native_load_cvars", 0);
}

// zp_get_cvar_int
public native_get_cvar_int(iPlugin, iParams)
{
	// Задаем статики
	static iValue, iArgNum;

	// Получаем название запрашиваемого Квара
	get_string(1, g_szKey, charsmax(g_szKey));

	// Находим номер аргумента для многомерного массива
	iArgNum = get_param(2);

	// Если задан номер Аргумента
	if(iArgNum)
		formatex(g_szKey, charsmax(g_szKey), "%s%i", g_szKey, iArgNum);	// Формируем название Ключа

	// Получаем значение квара
	if(!TrieGetCell(g_TrieCvarsInt, g_szKey, iValue))
	{
		// Выводим информацию о не существующем кваре
		log_amx("Failed load cvar: %s", g_szKey);
		return -1;
	}

	// Возвращаем значение Квара
	return iValue;
}

// zp_get_cvar_str
public native_get_cvar_str(iPlugin, iParams)
{
	// Задаем статики
	static iArgNum;

	// Получаем название запрашиваемого Квара
	get_string(1, g_szKey, charsmax(g_szKey));

	// Находим номер аргумента для многомерного массива
	iArgNum = get_param(3);

	// Если задан номер Аргумента
	if(iArgNum)
		formatex(g_szKey, charsmax(g_szKey), "%s%i", g_szKey, iArgNum);	// Формируем название Ключа

	// Получаем значение квара
	if(!TrieGetString(g_TrieCvarsString, g_szKey, g_szValue, charsmax(g_szValue)))
	{
		// Выводим информацию о не существующем кваре
		log_amx("Failed load cvar: %s", g_szKey);
		return -1;
	}

	// Возвращаем Значение квара
	set_string(2, g_szValue, charsmax(g_szValue));

	// Возвращаем положительное значение и факт нахождения данных в массиве
	return 1;
}

// zp_get_cvar_float
public Float:native_get_cvar_float(iPlugin, iParams)
{
	// Задаем статики
	static Float:szfTempArray[1], iArgNum;

	// Получаем название запрашиваемого Квара
	get_string(1, g_szKey, charsmax(g_szKey));

	// Находим номер аргумента для многомерного массива
	iArgNum = get_param(2);

	// Если задан номер Аргумента
	if(iArgNum)
		formatex(g_szKey, charsmax(g_szKey), "%s%i", g_szKey, iArgNum);	// Формируем название Ключа

	// Получаем значение квара
	if(!TrieGetArray(g_TrieCvarsFloat, g_szKey, szfTempArray, sizeof(szfTempArray)))
	{
		// Выводим информацию о не существующем кваре
		log_amx("Failed load cvar: %s", g_szKey);
		return -1.0;
	}

	// Возвращаем значение Квара
	return szfTempArray[0];
}

/*===============================================================================================================================================
																		[Get All Cvars]
================================================================================================================================================*/
public native_load_cvars(iPlugin, iParams)
{
	get_string(1, g_szKey, charsmax(g_szKey));

	if (file_exists(g_szKey))
		Get_All_Cvars(g_szKey);
	
	else
		log_amx("File not found '%s'", g_szKey);
}

public Get_All_Cvars(szConfigFile[])
{
	// Задаем переменные
	new szLineData[CVARS_MANAGER_KEY_STRING+CVARS_MANAGER_VALUE_STRING], iValueSize, Float:szfTempArray[1], iArgNum, szKeyArrayKey[CVARS_MANAGER_KEY_STRING];
//	new iKeySize;
	
	// Открываем конфиг
	new file = fopen(szConfigFile,"rt");
		
	// Прогоняем цикл по всем строкам файла до его конца
	while(file && !feof(file))
	{
		// Считываем одну строку из Файла
		fgets(file, szLineData, charsmax(szLineData));

		// Удаляем пробелы в начале и конце строки
		trim(szLineData);

		// Если это пустая строка, Комментарий или название Секции
		if(!strlen(szLineData) || szLineData[0] == ';' || szLineData[0] == '[')
			continue;

		// Получаем квар и его значение
		strtok2(szLineData, g_szKey, charsmax(g_szKey), g_szValue, charsmax(g_szValue), '=', TRIM_FULL);

		// Получаем длинну квара
	//	iKeySize = strlen(g_szKey);

		// Если это данные для многомерного массива
		if(containi(g_szValue, ",") > 0)
		{
			// Сбрасываем номер Аргумента
			iArgNum = 0;

			// Запускаем цикл по Аргументам
			while (g_szValue[0] != 0 && strtok2(g_szValue, szKeyArrayKey, charsmax(szKeyArrayKey), g_szValue, charsmax(g_szValue), ',', TRIM_FULL))
			{
				// Прибавляем Аргумент
				iArgNum++;
				
				// Получаем длинну значения квара
			//	iValueSize = strlen(g_szValue);
				iValueSize = strlen(szKeyArrayKey);

				// Если это Строка
			//	if(g_szKey[iKeySize-2] == '_' && g_szKey[iKeySize-1] == 's')
				if (szKeyArrayKey[0] == '^"' && szKeyArrayKey[iValueSize - 1] == '^"')
				{
					// Если строка слишком короткая
					if (iValueSize <= 2) continue;

					// Удаляем кавычки
					remove_quotes(szKeyArrayKey);

					// Записываем данные в Массив
					TrieSetString(g_TrieCvarsString, fmt("%s%i", g_szKey, iArgNum), szKeyArrayKey);
				#if defined DEBUG
					log_amx("[string array] %s %i %s", g_szKey, iArgNum, szKeyArrayKey);
				#endif	
					continue;
				}

				// Если это Дробное значение
			//	if(g_szKey[iKeySize-2] == '_' && g_szKey[iKeySize-1] == 'f')
				if (containi(szKeyArrayKey, ".") > 0)
				{
					// Запоминаем значение Квара
					szfTempArray[0] = str_to_float(szKeyArrayKey);
		
					// Записываем данные в Массив
					TrieSetArray(g_TrieCvarsFloat, fmt("%s%i", g_szKey, iArgNum), szfTempArray, sizeof(szfTempArray));
				#if defined DEBUG
					log_amx("[float array] %s %i %s", g_szKey, iArgNum, szfTempArray);
				#endif
					continue;
				}
		
				// Записываем данные в Массив
				TrieSetCell(g_TrieCvarsInt, fmt("%s%i", g_szKey, iArgNum), str_to_num(szKeyArrayKey));
			#if defined DEBUG
				log_amx("[cell array] %s %i %s", g_szKey, iArgNum, szKeyArrayKey);
			#endif
			}

			continue;
		}

		// Получаем длинну значения квара
		iValueSize = strlen(g_szValue);

		// Если это Строка
	//	if(g_szKey[iKeySize-2] == '_' && g_szKey[iKeySize-1] == 's')
		if (g_szValue[0] == '^"' && g_szValue[iValueSize - 1] == '^"')
		{
			// Удаляем кавычки
			remove_quotes(g_szValue);

			// Записываем данные в Массив
			TrieSetString(g_TrieCvarsString, g_szKey, g_szValue);
		#if defined DEBUG
			log_amx("[string] %s %s", g_szKey, g_szValue);
		#endif
			continue;
		}

		// Если это Дробное значение
	//	if(g_szKey[iKeySize-2] == '_' && g_szKey[iKeySize-1] == 'f')
		if (containi(g_szValue, ".") > 0)
		{
			// Запоминаем значение Квара
			szfTempArray[0] = str_to_float(g_szValue);

			// Записываем данные в Массив
			TrieSetArray(g_TrieCvarsFloat, g_szKey, szfTempArray, sizeof(szfTempArray));
		#if defined DEBUG
			log_amx("[float] %s %s", g_szKey, szfTempArray);
		#endif
			continue;
		}

		// Записываем данные в Массив
		TrieSetCell(g_TrieCvarsInt, g_szKey, str_to_num(g_szValue));
	#if defined DEBUG
		log_amx("[cell] %s %s", g_szKey, g_szValue);
	#endif
	}

	// Прекращаем чтение файла
	fclose(file);
}
/*
public Get_All_Cvars(szConfigFile[])
{
	// Задаем переменные
	new szLineData[CVARS_MANAGER_KEY_STRING+CVARS_MANAGER_VALUE_STRING], iKeySize, Float:szfTempArray[1], iArgNum, szKeyArrayKey[CVARS_MANAGER_KEY_STRING];

	// Открываем конфиг
	new file = fopen(szConfigFile,"rt");
		
	// Прогоняем цикл по всем строкам файла до его конца
	while(file && !feof(file))
	{
		// Считываем одну строку из Файла
		fgets(file, szLineData, charsmax(szLineData));

		// Удаляем пробелы в начале и конце строки
		trim(szLineData);

		// Если это пустая строка, Комментарий или название Секции
		if(!strlen(szLineData) || szLineData[0] == ';' || szLineData[0] == '[')
			continue;

		// Получаем квар и его значение
		strtok2(szLineData, g_szKey, charsmax(g_szKey), g_szValue, charsmax(g_szValue), '=', TRIM_FULL);

		// Получаем длинну квара
		iKeySize = strlen(g_szKey);

		// Если это данные для многомерного массива
		if(containi(g_szValue, ",") > 0)
		{
			// Сбрасываем номер Аргумента
			iArgNum = 0;

			// Запускаем цикл по Аргументам
			while (g_szValue[0] != 0 && strtok2(g_szValue, szKeyArrayKey, charsmax(szKeyArrayKey), g_szValue, charsmax(g_szValue), ',', TRIM_FULL))
			{
				// Прибавляем Аргумент
				iArgNum++;

				// Если это Дробное значение
				if(g_szKey[iKeySize-2] == '_' && g_szKey[iKeySize-1] == 'f')
				{
					// Запоминаем значение Квара
					szfTempArray[0] = str_to_float(szKeyArrayKey);
		
					// Записываем данные в Массив
					TrieSetArray(g_TrieCvarsFloat, fmt("%s%i", g_szKey, iArgNum), szfTempArray, sizeof(szfTempArray));
					continue;
				}
		
				// Если это Строка
				if(g_szKey[iKeySize-2] == '_' && g_szKey[iKeySize-1] == 's')
				{
					// Записываем данные в Массив
					TrieSetString(g_TrieCvarsString, fmt("%s%i", g_szKey, iArgNum), szKeyArrayKey);
					continue;
				}
		
				// Записываем данные в Массив
				TrieSetCell(g_TrieCvarsInt, fmt("%s%i", g_szKey, iArgNum), str_to_num(szKeyArrayKey));
			}

			continue;
		}

		// Если это Дробное значение
		if(g_szKey[iKeySize-2] == '_' && g_szKey[iKeySize-1] == 'f')
		{
			// Запоминаем значение Квара
			szfTempArray[0] = str_to_float(g_szValue);

			// Записываем данные в Массив
			TrieSetArray(g_TrieCvarsFloat, g_szKey, szfTempArray, sizeof(szfTempArray));
			continue;
		}

		// Если это Строка
		if(g_szKey[iKeySize-2] == '_' && g_szKey[iKeySize-1] == 's')
		{
			// Записываем данные в Массив
			TrieSetString(g_TrieCvarsString, g_szKey, g_szValue);
			continue;
		}

		// Записываем данные в Массив
		TrieSetCell(g_TrieCvarsInt, g_szKey, str_to_num(g_szValue));
	}

	// Прекращаем чтение файла
	fclose(file);
}
*/
/*===============================================================================================================================================
																		[On Auto Configs Buffered]
================================================================================================================================================*/
#if defined CLEAR_DATA_LOADS_CFD
public OnAutoConfigsBuffered()
{
	// Удаляем динамические Trie массивы
	TrieDestroy(g_TrieCvarsInt);
	TrieDestroy(g_TrieCvarsFloat);
	TrieDestroy(g_TrieCvarsString);
}
#endif