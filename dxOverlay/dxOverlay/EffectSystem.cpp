/**
 * @file EffectSystem.cpp
 * @brief Реализация системы управления эффектами
 * 
 * Основной модуль приложения, который:
 * 1. Управляет переходами между состояниями
 * 2. Контролирует тайминги для рулетки и эффектов
 * 3. Инициализирует/останавливает нужные эффекты
 */

#include "EffectSystem.h"
#include "Config.h"
#include <cstdlib>
#include <ctime>

// === Effect headers ===
#include "SensitivityEffect.h"
#include "SensitivityHighEffect.h"
#include "InvertWASDEffect.h"
#include "InvertMouseEffect.h"
#include "OverlayErase.h"
#include "HalfScreenCover.h"
#include "BlurEffect.h"
#include "FpsEffect.h"
#include "Window.h"

// ============================================================================
// GLOBAL STATE
// ============================================================================

/** Окно оверлея для отрисовки эффектов */
static HWND g_overlay = nullptr;

/** Текущее состояние системы */
static SystemState g_state = SystemState::Off;

/** Тип активного эффекта */
static EffectType g_currentEffect = EffectType::None;

/** Время начала текущего состояния (для Waiting / Rolling) */
static DWORD g_stateStartTime = 0;

/** Время начала активного эффекта */
static DWORD g_effectStartTime = 0;

/** Длительность текущего эффекта в миллисекундах */
static DWORD g_effectDuration = 0;

/** Флаг превью рулетки (без активации эффекта) */
static bool g_previewRoulette = false;

// ============================================================================
// ROULETTE VISUAL STATE
// ============================================================================

/** Состояние визуализации рулетки */
static RouletteVisualState g_visualState = RouletteVisualState::Hidden;

/** Время начала текущего визуального состояния рулетки */
static DWORD g_visualStateStart = 0;

/** Альфа-канал для затухания (0-255) */
static BYTE g_fadeAlpha = 255;

/** Финальный выбранный индекс рулетки */
static int g_finalIndex = -1;

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

static void StartRandomEffect();
static void StopInternalEffect();
static void ActivateEffect(EffectType type);


// ============================================================================
// EFFECT DURATIONS MAPPING
// ============================================================================

/**
 * @brief Получить длительность эффекта в миллисекундах
 * @param type Тип эффекта
 * @return Длительность в ms или 0 если неизвестный тип
 */
static DWORD GetEffectDuration(EffectType type) {
    switch (type) {
    case EffectType::Erase:              return EFFECT_DURATION_ERASE_MS;
    case EffectType::Blur:               return EFFECT_DURATION_BLUR_MS;
    case EffectType::HalfScreenCover:    return EFFECT_DURATION_HALF_SCREEN_MS;
    case EffectType::DisableSensitivity: return EFFECT_DURATION_DISABLE_SENSITIVITY_MS;
    case EffectType::SensitivityHigh:    return EFFECT_DURATION_HIGH_SENSITIVITY_MS;
    case EffectType::InvertWASD:         return EFFECT_DURATION_INVERT_WASD_MS;
    case EffectType::InvertMouse:        return EFFECT_DURATION_INVERT_MOUSE_MS;
    case EffectType::FpsLow:             return EFFECT_DURATION_LOW_FPS_MS;
    default:                             return 0;
    }
}

// ============================================================================
// INITIALIZATION
// ============================================================================

void InitEffectSystem(HWND overlayHwnd) {
    g_overlay = overlayHwnd;
    srand((unsigned)time(nullptr));
    g_state = SystemState::Off;
    g_currentEffect = EffectType::None;
}

// ============================================================================
// SYSTEM CONTROL
// ============================================================================

void StartSystem() {
    StopInternalEffect();
    g_state = SystemState::Waiting;
    g_stateStartTime = GetTickCount();
}

void StopSystem() {
    StopInternalEffect();
    g_state = SystemState::Off;
}

// ============================================================================
// STATE QUERIES
// ============================================================================

SystemState GetSystemState() {
    return g_state;
}

EffectType GetCurrentEffect() {
    return g_currentEffect;
}

bool IsEffectAlive() {
    return g_state == SystemState::Effect;
}

DWORD GetEffectRemainingTime() {
    if (g_state != SystemState::Effect)
        return 0;

    DWORD elapsed = GetTickCount() - g_effectStartTime;
    if (elapsed >= g_effectDuration)
        return 0;

    return g_effectDuration - elapsed;
}

// ============================================================================
// ROULETTE STATE QUERIES
// ============================================================================

RouletteVisualState GetRouletteVisualState() {
    return g_visualState;
}

int GetRouletteCurrentIndex() {
    // TODO: вычислить текущий индекс на основе g_visualStateStart
    return 0;
}

int GetRouletteFinalIndex() {
    return g_finalIndex;
}

BYTE GetRouletteFadeAlpha() {
    return g_fadeAlpha;
}

// ============================================================================
// UPDATE LOGIC (MAIN STATE MACHINE)
// ============================================================================

/**
 * @brief Главный update цикла - управляет переходами между состояниями
 * 
 * Состояния:
 * - Off: ничего не делаем
 * - Waiting: ждем ROULETTE_WAIT_TIME_MS перед запуском рулетки
 * - Rolling: рулетка вращается ROULETTE_SPIN_DURATION_MS
 * - Effect: активный эффект работает GetEffectDuration(type) миллисекунд
 */
void UpdateEffectSystem() {
    if (g_state == SystemState::Off)
        return;

    DWORD now = GetTickCount();

    // =========================================
    // EFFECT STATE: контролируем длительность
    // =========================================
    if (g_state == SystemState::Effect) {
        if (now - g_effectStartTime >= g_effectDuration) {
            StopInternalEffect();
            g_state = SystemState::Waiting;
            g_stateStartTime = now;
        }
        return;
    }

    // =========================================
    // WAITING → ROLLING: запустить рулетку
    // =========================================
    if (g_state == SystemState::Waiting) {
        DWORD timeUntilRolling = ROULETTE_WAIT_TIME_MS - ROULETTE_SPIN_DURATION_MS;
        
        if (now - g_stateStartTime >= timeUntilRolling) {
            g_state = SystemState::Rolling;
            g_stateStartTime = now;
            
            // Показываем окно оверлея для визуализации рулетки
            ShowWindow(g_overlay, SW_SHOWNA);
            SetOverlayMousePassThrough(g_overlay, true);
            g_visualState = RouletteVisualState::Spinning;
            g_visualStateStart = now;
        }
        return;
    }

    // =========================================
    // ROLLING → EFFECT: выбрать эффект
    // =========================================
    if (g_state == SystemState::Rolling) {
        if (now - g_stateStartTime >= ROULETTE_SPIN_DURATION_MS) {
            if (!g_previewRoulette) {
                StartRandomEffect();
            } else {
                // В режиме превью просто скрываем рулетку
                g_state = SystemState::Off;
                ShowWindow(g_overlay, SW_HIDE);
            }
        }
        return;
    }
}

// ============================================================================
// EFFECT EXECUTION
// ============================================================================

/**
 * @brief Запустить случайный эффект из пула доступных
 */
static void StartRandomEffect() {
    StopInternalEffect();

    // Пул доступных эффектов (исключаем None)
    static const EffectType EFFECT_POOL[] = {
        EffectType::Erase,
        EffectType::DisableSensitivity,
        EffectType::SensitivityHigh,
        EffectType::InvertWASD,
        EffectType::InvertMouse,
        EffectType::HalfScreenCover,
        EffectType::Blur,
        EffectType::FpsLow
    };

    constexpr int POOL_SIZE = sizeof(EFFECT_POOL) / sizeof(EFFECT_POOL[0]);
    int randomIndex = rand() % POOL_SIZE;
    g_finalIndex = randomIndex;

    ActivateEffect(EFFECT_POOL[randomIndex]);
}

/**
 * @brief Внутренняя функция активации эффекта
 */
static void ActivateEffect(EffectType type) {
    g_currentEffect = type;
    g_effectStartTime = GetTickCount();
    g_effectDuration = GetEffectDuration(type);
    g_state = SystemState::Effect;
    g_visualState = RouletteVisualState::Hidden;

    HWND hwnd = g_overlay;

    // Показываем окно и устанавливаем режим отрисовки
    ShowWindow(hwnd, SW_SHOWNA);
    SetOverlayColorkey(hwnd);

    // Определяем, нужна ли обработка мыши/клавиатуры
    bool needsInputCapture = (type == EffectType::Erase || 
                              type == EffectType::HalfScreenCover);
    SetOverlayMousePassThrough(hwnd, !needsInputCapture);

    // Инициализируем конкретный эффект
    switch (type) {
    case EffectType::Blur:                StartBlurEffect(hwnd); break;
    case EffectType::DisableSensitivity:  StartSensitivityEffect(); break;
    case EffectType::SensitivityHigh:     StartSensitivityHighEffect(); break;
    case EffectType::InvertWASD:          StartInvertWASD(); break;
    case EffectType::InvertMouse:         StartInvertMouseEffect(); break;
    case EffectType::FpsLow:              StartFpsLowEffect(); break;
    case EffectType::Erase:               break;  // Инициализация в DrawErase
    case EffectType::HalfScreenCover:     break;  // Инициализация в DrawHalfScreen
    default:
        break;
    }
}

/**
 * @brief Остановить текущий активный эффект
 */
static void StopInternalEffect() {
    if (g_currentEffect == EffectType::None)
        return;

    HWND hwnd = g_overlay;

    // Останавливаем конкретный эффект
    switch (g_currentEffect) {
    case EffectType::DisableSensitivity: StopSensitivityEffect(); break;
    case EffectType::SensitivityHigh:    StopSensitivityHighEffect(); break;
    case EffectType::InvertWASD:         StopInvertWASD(); break;
    case EffectType::InvertMouse:        StopInvertMouseEffect(); break;
    case EffectType::Erase:              ResetErase(hwnd); break;
    case EffectType::HalfScreenCover:    ResetHalfScreenCover(); break;
    case EffectType::Blur:               StopBlurEffect(hwnd); break;
    case EffectType::FpsLow:             StopFpsLowEffect(); break;
    default:
        break;
    }

    g_currentEffect = EffectType::None;
    ShowWindow(hwnd, SW_HIDE);
    SetOverlayMousePassThrough(hwnd, true);
}

// ============================================================================
// DEV / DEBUG API
// ============================================================================

void Dev_StopEffect() {
    StopInternalEffect();
    g_state = SystemState::Waiting;
    g_stateStartTime = GetTickCount();
}

void Dev_ForceEffect(EffectType type) {
    if (type == EffectType::None)
        return;

    StopInternalEffect();
    ActivateEffect(type);
}

void Dev_ForceRoulette() {
    StopInternalEffect();
    g_state = SystemState::Rolling;
    g_stateStartTime = GetTickCount();
    g_visualState = RouletteVisualState::Spinning;
    g_visualStateStart = GetTickCount();
}

void Dev_StartPreviewRoulette() {
    if (g_state != SystemState::Off)
        return;

    g_previewRoulette = true;
    g_state = SystemState::Rolling;
    g_stateStartTime = GetTickCount();
    g_visualState = RouletteVisualState::Spinning;
    g_visualStateStart = GetTickCount();
    
    ShowWindow(g_overlay, SW_SHOWNA);
    SetOverlayMousePassThrough(g_overlay, true);
}

void Dev_StopPreviewRoulette() {
    g_previewRoulette = false;
    g_state = SystemState::Off;
    g_visualState = RouletteVisualState::Hidden;
    ShowWindow(g_overlay, SW_HIDE);
}

bool IsPreviewRoulette() {
    return g_previewRoulette;
}
