#include "HalfScreenCover.h"
#include "EffectSystem.h"
#include <Windows.h>

static const COLORREF TRANSPARENT_COLOR = RGB(255, 0, 255);

bool HandleHalfScreenCover(HWND hwnd, UINT msg) {

    if (GetCurrentEffect() != EffectType::HalfScreenCover)
        return false;

    if (msg == WM_PAINT) {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        int screenW = GetSystemMetrics(SM_CXSCREEN);
        int screenH = GetSystemMetrics(SM_CYSCREEN);

        // 1️⃣ Заливаем весь экран прозрачным цветом
        HBRUSH clearBrush = CreateSolidBrush(TRANSPARENT_COLOR);
        RECT full{ 0, 0, screenW, screenH };
        FillRect(hdc, &full, clearBrush);
        DeleteObject(clearBrush);

        // 2️⃣ Чёрная половина экрана
        RECT halfRect{
            0,
            0,
            screenW / 2,
            screenH
        };

        HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &halfRect, blackBrush);
        DeleteObject(blackBrush);

        EndPaint(hwnd, &ps);
        return true;
    }

    return false;
}

void ResetHalfScreenCover() {
    // тут ничего не нужно — очистка делается в StopEffect()
}
