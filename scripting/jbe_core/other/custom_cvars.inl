// By SISA

public jbe_load_custom_cvars(szConfigFile[])
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

public OnAutoConfigsBuffered()
{
	// Удаляем динамические Trie массивы
	TrieDestroy(g_TrieCvarsInt);
	TrieDestroy(g_TrieCvarsFloat);
	TrieDestroy(g_TrieCvarsString);
}