
// кол-во последних получателей в меню
#define LAST_PM_TARGETS_NUM		6

// храним последних получателей (LAST_PM_TARGETS_NUM)
new g_PM_LastTargets[MAX_PLAYERS + 1][LAST_PM_TARGETS_NUM];

// выбранный получатель для текущего сообщения
new g_PM_CurrentTarget[MAX_PLAYERS + 1];

// бит заблокированых отправителей
new g_iBitBlockedPM[MAX_PLAYERS + 1];

// Выключенны приват сообщения
new g_iBitUserOffPM;