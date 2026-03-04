/**
 * @file SensitivityEffect.cpp
 * @brief Эффект отключения чувствительности мыши
 *
 * Использует консоль игры для установки sensitivity = 0.0
 */

#include "SensitivityEffect.h"
#include "InputSequencer.h"
#include "Config.h"

// ============================================================================
// EFFECT IMPLEMENTATION
// ============================================================================

void StartSensitivityEffect() {
    // Отправляем команду в буфер обмена
    SetClipboardText(SENSITIVITY_DISABLE_CMD);

    // Открываем консоль, вставляем и выполняем команду
    PressF7();
    PressCtrlV();
    PressEnter();
    PressF7();
}

void StopSensitivityEffect() {
    // Восстанавливаем оригинальное значение через консоль
    SetClipboardText(SENSITIVITY_NORMAL_CMD);

    PressF7();
    PressCtrlV();
    PressEnter();
    PressF7();
}
