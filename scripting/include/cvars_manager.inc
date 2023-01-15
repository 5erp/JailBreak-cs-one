/**
 * Получаем Целое значение Квара
 *
 * @param cvarname[]	Название Запрашиваемого квара
 * @param argnum		Номер аргумента для многомерного массива
 * @return				Возвращаем целое значение или -1 при отсутствии квара
 */
native plugins_get_cvar_int(const cvarname[], const argnum = 0);

/**
 * Получаем Строковое значение Квара
 *
 * @param cvarname[]	Название Запрашиваемого квара
 * @param value[]		Значение квара
 * @param argnum		Номер аргумента для многомерного массива
 * @return				Возвращаем -1 при отсутствии квара
 */
native plugins_get_cvar_str(const cvarname[], value[], const argnum = 0);

/**
 * Получаем Дробное значение Квара
 *
 * @param cvarname[]	Название Запрашиваемого квара
 * @param argnum		Номер аргумента для многомерного массива
 * @return				Возвращаем дробное значение -1.0 при отсутствии квара
 */
native Float:plugins_get_cvar_float(const cvarname[], const argnum = 0);

/**
 * Загрузка конфигов с файла
 */
native plugins_load_cvars(const file[]);