#pragma once

/**
 * @file Config.h
 * @brief Централизованная конфигурация всех констант приложения
 *
 * Все магические числа и строки вынесены сюда для удобства настройки
 */

// ============================================================================
// WINDOW CONFIGURATION
// ============================================================================

/** Имя класса окна оверлея */
constexpr wchar_t OVERLAY_CLASS_NAME[] = L"DXOverlay_OverlayWindow";

/** Заголовок окна оверлея */
constexpr wchar_t OVERLAY_WINDOW_TITLE[] = L"DX Overlay";

/** Имя класса контрольного окна */
constexpr wchar_t CONTROL_CLASS_NAME[] = L"DXOverlay_ControlWindow";

/** Заголовок контрольного окна */
constexpr wchar_t CONTROL_WINDOW_TITLE[] = L"Chaos Controller (DEV)";

// ============================================================================
// TIMING CONFIGURATION (milliseconds)
// ============================================================================

/** Время ожидания перед автозапуском рулетки */
constexpr DWORD ROULETTE_WAIT_TIME_MS = 45000;  // 45 секунд

/** Длительность вращения рулетки */
constexpr DWORD ROULETTE_SPIN_DURATION_MS = 5000;  // 5 секунд

/** Время затухания после выбора эффекта */
constexpr DWORD FADEOUT_DURATION_MS = 1000;  // 1 секунда

// ============================================================================
// EFFECT DURATIONS (milliseconds)
// ============================================================================

constexpr DWORD EFFECT_DURATION_ERASE_MS = 30000;              // 30 сек
constexpr DWORD EFFECT_DURATION_BLUR_MS = 20000;               // 20 сек
constexpr DWORD EFFECT_DURATION_HALF_SCREEN_MS = 30000;        // 30 сек
constexpr DWORD EFFECT_DURATION_DISABLE_SENSITIVITY_MS = 10000; // 10 сек
constexpr DWORD EFFECT_DURATION_HIGH_SENSITIVITY_MS = 15000;    // 15 сек
constexpr DWORD EFFECT_DURATION_INVERT_WASD_MS = 35000;         // 35 сек
constexpr DWORD EFFECT_DURATION_INVERT_MOUSE_MS = 35000;        // 35 сек
constexpr DWORD EFFECT_DURATION_LOW_FPS_MS = 15000;             // 15 сек

// ============================================================================
// VISUAL EFFECTS CONFIGURATION
// ============================================================================

/** Радиус стирающего отверстия в пиксельах */
constexpr int ERASE_EFFECT_RADIUS_PX = 35;

/** Прозрачный цвет для эффектов (magenta) */
constexpr COLORREF TRANSPARENT_COLOR = RGB(255, 0, 255);

// ============================================================================
// INPUT SEQUENCER CONFIGURATION
// ============================================================================

/** Задержка между нажатиями клавиш (для безопасности) */
constexpr DWORD KEY_PRESS_DELAY_MS = 50;

// ============================================================================
// GAME COMMANDS FOR SENSITIVITY/FPS EFFECTS
// ============================================================================

/** Команда консоли для отключения чувствительности мыши */
constexpr wchar_t SENSITIVITY_DISABLE_CMD[] = L"sensitivity 0.0";

/** Команда консоли для восстановления чувствительности */
constexpr wchar_t SENSITIVITY_NORMAL_CMD[] = L"sensitivity 0.2";

/** Команда консоли для максимальной чувствительности */
constexpr wchar_t SENSITIVITY_HIGH_CMD[] = L"sensitivity 7.0";

/** Команда консоли для понижения FPS */
constexpr wchar_t FPS_LOW_CMD[] = L"fps_max 16";

/** Команда консоли для нормального FPS */
constexpr wchar_t FPS_NORMAL_CMD[] = L"fps_max 128";

// ============================================================================
// CONTROL WINDOW UI CONFIGURATION
// ============================================================================

/** Задержка перед выполнением действия при клике на кнопку (anti-spam) */
constexpr DWORD ACTION_DELAY_MS = 3000;  // 3 секунды

/** Начальная позиция контрольного окна (X) */
constexpr int CONTROL_WINDOW_X = 200;

/** Начальная позиция контрольного окна (Y) */
constexpr int CONTROL_WINDOW_Y = 200;

/** Ширина контрольного окна */
constexpr int CONTROL_WINDOW_WIDTH = 420;

/** Высота контрольного окна */
constexpr int CONTROL_WINDOW_HEIGHT = 360;

/** Ширина кнопки */
constexpr int BUTTON_WIDTH = 160;

/** Высота кнопки */
constexpr int BUTTON_HEIGHT = 30;

/** Расстояние между кнопками по Y */
constexpr int BUTTON_SPACING_Y = 34;

/** Левый отступ */
constexpr int MARGIN_LEFT = 20;

/** Верхний отступ */
constexpr int MARGIN_TOP = 20;

/** Расстояние между колонками кнопок */
constexpr int COLUMN_SPACING_X = 180;

// ============================================================================
// DEBUG / CONTROL CONSTANTS
// ============================================================================

/** Интервал обновления системы эффектов (кадров в сек) */
constexpr DWORD UPDATE_INTERVAL_MS = 16;  // ~60 FPS
