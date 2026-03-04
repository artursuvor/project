#pragma once

#include <Windows.h>

/**
 * @file Types.h
 * @brief Определение всех типов данных, используемых в приложении
 *
 * Централизованное место для enum'ов и структур
 */

// ============================================================================
// EFFECT TYPES
// ============================================================================

/**
 * @enum EffectType
 * @brief Все доступные эффекты в системе
 */
enum class EffectType {
    None,                   ///< Нет активного эффекта
    Erase,                  ///< Стирающие отверстия на экране
    DisableSensitivity,     ///< Отключение чувствительности мыши
    SensitivityHigh,        ///< Максимальная чувствительность мыши
    InvertWASD,             ///< Инверсия клавиш движения
    InvertMouse,            ///< Инверсия движения мыши
    HalfScreenCover,        ///< Закрытие половины экрана
    Blur,                   ///< Размытие экрана
    FpsLow                  ///< Понижение FPS
};

// ============================================================================
// SYSTEM STATES
// ============================================================================

/**
 * @enum SystemState
 * @brief Состояние системы эффектов
 */
enum class SystemState {
    Off,        ///< Система отключена
    Waiting,    ///< Ожидание перед запуском рулетки
    Rolling,    ///< Рулетка вращается (выбирается случайный эффект)
    Effect      ///< Активный эффект в процессе
};

// ============================================================================
// ROULETTE VISUAL STATE
// ============================================================================

/**
 * @enum RouletteVisualState
 * @brief Состояние визуализации рулетки
 */
enum class RouletteVisualState {
    Hidden,     ///< Рулетка не видна
    Spinning,   ///< Рулетка вращается
    HoldResult, ///< Показываем выбранный результат
    FadeOut     ///< Затухание рулетки
};

// ============================================================================
// ROULETTE DATA STRUCTURES
// ============================================================================

/**
 * @struct RouletteItem
 * @brief Элемент рулетки (эффект + название)
 */
struct RouletteItem {
    EffectType type;        ///< Тип эффекта
    const wchar_t* name;    ///< Текстовое название эффекта
};

/**
 * @struct RouletteIcon
 * @brief Иконка эффекта
 */
struct RouletteIcon {
    EffectType type;        ///< Тип эффекта
    HBITMAP bitmap;         ///< Загруженная битмап иконки
};

// ============================================================================
// VISUAL EFFECTS DATA STRUCTURES
// ============================================================================

/**
 * @struct EraseHole
 * @brief Одно стирающее отверстие в эффекте Erase
 */
struct EraseHole {
    int x;                  ///< X координата центра
    int y;                  ///< Y координата центра
    int radius;             ///< Радиус отверстия в пиксельах
};

// ============================================================================
// CONTROL WINDOW IDS
// ============================================================================

/**
 * @enum ControlWindowButtonId
 * @brief ID всех кнопок в контрольном окне для удобства обращения
 */
enum ControlWindowButtonId {
    // System control
    BTN_START_SYSTEM = 1001,
    BTN_STOP_SYSTEM = 1002,

    // Effect control
    BTN_FORCE_ROULETTE = 1100,
    BTN_STOP_EFFECT = 1101,

    // Individual effect buttons
    BTN_ERASE = 1200,
    BTN_BLUR = 1201,
    BTN_HALF_SCREEN = 1202,
    BTN_INVERT_WASD = 1203,
    BTN_INVERT_MOUSE = 1204,
    BTN_LOW_FPS = 1205,
    BTN_DISABLE_SENSITIVITY = 1206,
    BTN_HIGH_SENSITIVITY = 1207,

    // Preview
    BTN_PREVIEW_ROULETTE = 1300,
    BTN_STOP_PREVIEW = 1301
};

// ============================================================================
// TIMER IDS
// ============================================================================

/**
 * @enum TimerId
 * @brief ID'шники таймеров для окна
 */
enum TimerId {
    TIMER_EFFECT_UPDATE = 1000,
    TIMER_BLUR_UPDATE = 2000,
    TIMER_DELAY = 5000
};

// ============================================================================
// PENDING ACTIONS (for UI)
// ============================================================================

/**
 * @enum PendingAction
 * @brief Все возможные действия, которые можно запланировать в контрольном окне
 */
enum class PendingAction {
    None,

    // System control
    StartSystem,
    StopSystem,

    // Roulette control
    ForceRoulette,
    StopEffect,

    // Individual effects
    ForceErase,
    ForceBlur,
    ForceHalf,
    ForceInvertWASD,
    ForceInvertMouse,
    ForceFpsLow,
    ForceSensitivityDisable,
    ForceSensitivityHigh,

    // Roulette preview
    PreviewRoulette,
    StopPreviewRoulette
};
