#include "BlurEffect.h"
#include <Windows.h>

// 🔹 СОСТОЯНИЕ ЭФФЕКТА (ТОЛЬКО ДЛЯ BLUR)
static int   g_lineCount = 0;
static DWORD g_lastGrow = 0;

void StartBlurEffect(HWND hwnd) {
    g_lineCount = 10;                  // старт
    g_lastGrow = GetTickCount64();
    InvalidateRect(hwnd, nullptr, TRUE);
}

void StopBlurEffect(HWND hwnd) {
    g_lineCount = 0;
    InvalidateRect(hwnd, nullptr, TRUE);
}

// 👉 даём доступ WM_PAINT
int GetBlurLineCount() {
    return g_lineCount;
}

// 👉 обновление из WM_TIMER
void UpdateBlurEffect() {
    DWORD now = GetTickCount64();

    if (now - g_lastGrow >= 2) {   // скорость роста
        g_lineCount += 1;
        if (g_lineCount > 250)
            g_lineCount = 250;

        g_lastGrow = now;
    }
}
