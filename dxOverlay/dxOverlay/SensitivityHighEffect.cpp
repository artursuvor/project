/**
 * @file SensitivityHighEffect.cpp
 * @brief Эффект максимальной чувствительности мыши
 *
 * Использует консоль игры для установки sensitivity = 7.0
 */

#include "SensitivityHighEffect.h"
#include "InputSequencer.h"
#include "Config.h"

// ============================================================================
// EFFECT IMPLEMENTATION
// ============================================================================

void StartSensitivityHighEffect() {
    // Отправляем команду максимальной чувствительности
    SetClipboardText(SENSITIVITY_HIGH_CMD);

    // Открываем консоль, вставляем и выполняем команду
    PressF7();
    PressCtrlV();
    PressEnter();
    PressF7();
}

void StopSensitivityHighEffect() {
    // Восстанавливаем нормальное значение
    SetClipboardText(SENSITIVITY_NORMAL_CMD);

    PressF7();
    PressCtrlV();
    PressEnter();
    PressF7();
}
