/**
 * @file FpsEffect.cpp
 * @brief Эффект понижения FPS в игре
 *
 * Использует консоль игры для установки fps_max = 16
 */

#include "FpsEffect.h"
#include "InputSequencer.h"
#include "Config.h"

// ============================================================================
// EFFECT IMPLEMENTATION
// ============================================================================

void StartFpsLowEffect() {
    // Отправляем команду понижения FPS в буфер обмена
    SetClipboardText(FPS_LOW_CMD);

    // Открываем консоль, вставляем и выполняем команду
    PressF7();
    PressCtrlV();
    PressEnter();
    PressF7();
}

void StopFpsLowEffect() {
    // Восстанавливаем нормальный FPS
    SetClipboardText(FPS_NORMAL_CMD);

    PressF7();
    PressCtrlV();
    PressEnter();
    PressF7();
}
