#pragma once
#include <Windows.h>

enum class EffectType {
    None,
    Erase,
    DisableSensitivity,
    SensitivityHigh,
    InvertWASD,
    InvertMouse,
    HalfScreenCover,
    Blur,
    FpsLow
};

enum class SystemState {
    Off,
    Waiting,
    Rolling,
    Effect
};

void InitEffectSystem(HWND overlayHwnd);

// Control
void StartSystem();
void StopSystem();

// Update
void UpdateEffectSystem();

// State access (для overlay)
SystemState GetSystemState();
EffectType GetCurrentEffect();
bool IsEffectAlive();

// DEV / DEBUG
void Dev_StopEffect();
void Dev_ForceEffect(EffectType type);
void Dev_ForceRoulette();

// Preview roulette (NO EFFECT)
void Dev_StartPreviewRoulette();
void Dev_StopPreviewRoulette();
bool IsPreviewRoulette();
