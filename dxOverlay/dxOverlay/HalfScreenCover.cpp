/**
 * @file HalfScreenCover.cpp
 * @brief Эффект закрытия половины экрана
 *
 * Блокирует видимость половины экрана черным оверлеем
 */

#include "HalfScreenCover.h"
#include "EffectSystem.h"
#include "Config.h"
#include <Windows.h>

// ============================================================================
// EFFECT IMPLEMENTATION
// ============================================================================

bool HandleHalfScreenCover(HWND hwnd, UINT msg) {
    // Проверяем, что эффект HalfScreenCover активен
    if (GetCurrentEffect() != EffectType::HalfScreenCover)
        return false;

    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        // Сначала заливаем весь экран прозрачным цветом
        HBRUSH transparentBrush = CreateSolidBrush(TRANSPARENT_COLOR);
        RECT fullScreen{ 0, 0, screenWidth, screenHeight };
        FillRect(hdc, &fullScreen, transparentBrush);
        DeleteObject(transparentBrush);

        // Потом закрываем левую половину черной краской
        HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
        RECT leftHalf{ 0, 0, screenWidth / 2, screenHeight };
        FillRect(hdc, &leftHalf, blackBrush);
        DeleteObject(blackBrush);

        EndPaint(hwnd, &ps);
        return true;
    }

    return false;
}

void ResetHalfScreenCover() {
    // Очистка делается автоматически в StopEffect()
}
