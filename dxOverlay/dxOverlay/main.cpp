#include <Windows.h>
#include "Window.h"        // CreateMainWindow, RunMessageLoop, GetOverlayWindow
#include "ControlWindow.h" // CreateControlWindow
#include "EffectSystem.h"  // InitEffectSystem
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

ULONG_PTR g_gdiplusToken;

void InitGDIPlus() {
    Gdiplus::GdiplusStartupInput input;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &input, nullptr);
}

void ShutdownGDIPlus() {
    Gdiplus::GdiplusShutdown(g_gdiplusToken);
}

int APIENTRY wWinMain(
    HINSTANCE hInstance,
    HINSTANCE,
    LPWSTR,
    int
) {
    // 1️⃣ создаём overlay
    if (!CreateMainWindow(hInstance))
        return 0;

    // 2️⃣ инициализируем систему эффектов
    InitEffectSystem(GetOverlayWindow());

    // 3️⃣ создаём UI окно
    CreateControlWindow(hInstance);

    // 4️⃣ единый message loop
    RunMessageLoop();
    return 0;
}
