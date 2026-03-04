#include "EffectSystem.h"
#include <cstdlib>
#include <ctime>

// === effects ===
#include "SensitivityEffect.h"
#include "SensitivityHighEffect.h"
#include "InvertWASDEffect.h"
#include "InvertMouseEffect.h"
#include "OverlayErase.h"
#include "HalfScreenCover.h"
#include "BlurEffect.h"
#include "FpsEffect.h"
#include "Window.h"

// ======================================================
// GLOBAL STATE
// ======================================================

static HWND g_overlay = nullptr;

static SystemState g_state = SystemState::Off;
static EffectType  g_currentEffect = EffectType::None;

// тайминги
static DWORD g_stateStartTime = 0; // для Waiting / Rolling
static DWORD g_effectStartTime = 0; // для Effect
static DWORD g_effectDuration = 0;

static const DWORD ROULETTE_INTERVAL = 45000; // 60 сек ожидание
static const DWORD ROLLING_DURATION = 5000;  // 5 сек рулетка

// ======================================================
// INTERNAL
// ======================================================

static void StartRandomEffect();
static void StopInternalEffect();

static DWORD GetEffectDuration(EffectType type) {
    switch (type) {
    case EffectType::Erase:              return 30000;
    case EffectType::Blur:               return 20000;
    case EffectType::HalfScreenCover:    return 30000;
    case EffectType::DisableSensitivity: return 10000;
    case EffectType::SensitivityHigh:    return 15000;
    case EffectType::InvertWASD:          return 35000;
    case EffectType::InvertMouse:         return 35000;
    case EffectType::FpsLow:              return 15000;
    default:                             return 0;
    }
}

// ======================================================
// INIT
// ======================================================

void InitEffectSystem(HWND overlayHwnd) {
    g_overlay = overlayHwnd;
    srand((unsigned)time(nullptr));
}

// ======================================================
// CONTROL
// ======================================================

void StartSystem() {
    StopInternalEffect();
    g_state = SystemState::Waiting;
    g_stateStartTime = GetTickCount();
}

void StopSystem() {
    StopInternalEffect();
    g_state = SystemState::Off;
}

// ======================================================
// STATE ACCESS
// ======================================================

SystemState GetSystemState() {
    return g_state;
}

EffectType GetCurrentEffect() {
    return g_currentEffect;
}

bool IsEffectAlive() {
    return g_state == SystemState::Effect;
}

// ======================================================
// UPDATE (ЕДИНСТВЕННЫЙ МОЗГ)
// ======================================================

void UpdateEffectSystem() {
    DWORD now = GetTickCount();

    if (g_state == SystemState::Off)
        return;

    // -------------------------
    // EFFECT ACTIVE
    // -------------------------
    if (g_state == SystemState::Effect) {

        if (now - g_effectStartTime >= g_effectDuration) {
            StopInternalEffect();
            g_state = SystemState::Waiting;
            g_stateStartTime = now;
        }

        return; // 🔥 НИЧЕГО БОЛЬШЕ НЕ ДЕЛАЕМ
    }

    // -------------------------
    // WAITING → ROLLING
    // -------------------------
    if (g_state == SystemState::Waiting &&
        now - g_stateStartTime >= (ROULETTE_INTERVAL - ROLLING_DURATION))
    {
        g_state = SystemState::Rolling;
        g_stateStartTime = now;

        ShowWindow(g_overlay, SW_SHOWNA);          // 👈 ВАЖНО
        SetOverlayMousePassThrough(g_overlay, true);

        return;
    }

    // -------------------------
    // ROLLING → EFFECT
    // -------------------------
    if (g_state == SystemState::Rolling &&
        now - g_stateStartTime >= ROLLING_DURATION)
    {
        StartRandomEffect();
        return;
    }
}

// ======================================================
// EFFECT CONTROL (INTERNAL)
// ======================================================

static void StartRandomEffect() {
    StopInternalEffect();

    static EffectType pool[] = {
        EffectType::Erase,
        EffectType::DisableSensitivity,
        EffectType::SensitivityHigh,
        EffectType::InvertWASD,
        EffectType::InvertMouse,
        EffectType::HalfScreenCover,
        EffectType::Blur,
        EffectType::FpsLow
    };

    int index = rand() % (sizeof(pool) / sizeof(pool[0]));
    g_currentEffect = pool[index];

    g_effectStartTime = GetTickCount();
    g_effectDuration = GetEffectDuration(g_currentEffect);
    g_state = SystemState::Effect;

    HWND hwnd = g_overlay;

    ShowWindow(hwnd, SW_SHOWNA);
    SetOverlayColorkey(hwnd);

    bool passThrough = true;
    if (g_currentEffect == EffectType::Erase ||
        g_currentEffect == EffectType::HalfScreenCover)
        passThrough = false;

    SetOverlayMousePassThrough(hwnd, passThrough);

    switch (g_currentEffect) {
    case EffectType::Blur:                StartBlurEffect(hwnd); break;
    case EffectType::DisableSensitivity:  StartSensitivityEffect(); break;
    case EffectType::SensitivityHigh:     StartSensitivityHighEffect(); break;
    case EffectType::InvertWASD:           StartInvertWASD(); break;
    case EffectType::InvertMouse:          StartInvertMouseEffect(); break;
    case EffectType::FpsLow:               StartFpsLowEffect(); break;
    case EffectType::Erase:
    case EffectType::HalfScreenCover:
        break;
    default:
        break;
    }
}

static void StopInternalEffect() {
    HWND hwnd = g_overlay;

    switch (g_currentEffect) {
    case EffectType::DisableSensitivity: StopSensitivityEffect(); break;
    case EffectType::SensitivityHigh:    StopSensitivityHighEffect(); break;
    case EffectType::InvertWASD:          StopInvertWASD(); break;
    case EffectType::InvertMouse:         StopInvertMouseEffect(); break;
    case EffectType::Erase:               ResetErase(hwnd); break;
    case EffectType::HalfScreenCover:     ResetHalfScreenCover(); break;
    case EffectType::Blur:                StopBlurEffect(hwnd); break;
    case EffectType::FpsLow:              StopFpsLowEffect(); break;
    default: break;
    }

    g_currentEffect = EffectType::None;
    ShowWindow(hwnd, SW_HIDE);
    SetOverlayMousePassThrough(hwnd, true);
}

// ======================================================
// DEV / DEBUG API
// ======================================================

void Dev_StopEffect() {
    StopInternalEffect();
    g_state = SystemState::Waiting;
    g_stateStartTime = GetTickCount();
}

void Dev_ForceEffect(EffectType type) {
    StopInternalEffect();

    g_currentEffect = type;
    g_effectStartTime = GetTickCount();
    g_effectDuration = GetEffectDuration(type);
    g_state = SystemState::Effect;

    HWND hwnd = g_overlay;

    ShowWindow(hwnd, SW_SHOWNA);
    SetOverlayColorkey(hwnd);

    bool passThrough = true;
    if (type == EffectType::Erase || type == EffectType::HalfScreenCover)
        passThrough = false;

    SetOverlayMousePassThrough(hwnd, passThrough);

    switch (type) {
    case EffectType::Blur:                StartBlurEffect(hwnd); break;
    case EffectType::DisableSensitivity:  StartSensitivityEffect(); break;
    case EffectType::SensitivityHigh:     StartSensitivityHighEffect(); break;
    case EffectType::InvertWASD:           StartInvertWASD(); break;
    case EffectType::InvertMouse:          StartInvertMouseEffect(); break;
    case EffectType::FpsLow:               StartFpsLowEffect(); break;
    case EffectType::Erase:
    case EffectType::HalfScreenCover:
        break;
    default:
        break;
    }
}

void Dev_ForceRoulette() {
    StopInternalEffect();
    g_state = SystemState::Rolling;
    g_stateStartTime = GetTickCount();
}

static bool g_previewRoulette = false;

void Dev_StartPreviewRoulette() {
    if (g_state != SystemState::Off)
        return;

    g_previewRoulette = true;
    g_state = SystemState::Rolling;
    g_stateStartTime = GetTickCount();
}

void Dev_StopPreviewRoulette() {
    g_previewRoulette = false;
    g_state = SystemState::Off;
}

bool IsPreviewRoulette() {
    return g_previewRoulette;
}
