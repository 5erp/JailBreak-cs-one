// ￿ - вставляем это в инклуди чтоб нормально читался UTF-8

/********************************
*	Shared Functions Header File
*********************************/

/*** Причины отсидки :D ***/
new g_iSizePrisonReason1, g_iSizePrisonReason2;

new const g_szPrisonReason1[][] = { // Соверщенное действие 
	"угон",	"кражу",	"убийство",	"изнасилование",	"ограбление",
	"совращение",	"взлом",	"продажу",	"изобритение",	"куплю",
	"обман",	"поимку",	"порчу",  "обмен", "удушение", "эволюцию",
    "передачу",  "распространение", "преследование", "призание",
	"активность", "воспитание", "профессию", "обсуждение",
	"предательство", "выиграш в карты у"
};

new const g_szPrisonReason2[][] = { // Предмет над которым совершилось действие
	"жигули",	"ботинка",	"собаки",	"унитаза",	"сервера",	"стула",
	"вантуза",	"дивана",	"пачки сигарет",	"пивка",	"холодильника",
	"ролтона",	"пикинеса",	"рептилоида",	"котейки",	"бабульки",	"чебурашки",
	"соседа",	"мента",	"бродяги",	"барыги",	"Морти",	"Тимати",
	"закона",	"Дианы Шурыгиной",	"Фейса",	"Вити из АКА",	"Кизару",	
	"Илона Маска",	"Дейенерис",	"Ким Чен Ына",	"Кендрика Ламара",
	"Гомера Симпсона",	"Джонни Деппа",	"Брэда Питта",  "мохнатки",  "креветки", 
	"тапка",  "руки",  "хозяйства", "казино", "Начальника"
};
/*** Причины отсидки :D ***/