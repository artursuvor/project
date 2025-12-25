#include <Windows.h>
#include "Window.h"

#include "EffectSystem.h"
#include "OverlayErase.h"
#include "HalfScreenCover.h"
#include "BlurEffect.h"
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

// ======================================================
// ROULETTE DATA
// ======================================================

struct RouletteItem {
    EffectType type;
    const wchar_t* name;
};

struct RouletteIcon {
    EffectType type;
    HBITMAP bitmap;
};



enum class RouletteVisualState {
    Hidden,
    Spinning,
    HoldResult,
    FadeOut
};

static RouletteVisualState g_visualState = RouletteVisualState::Hidden;
static DWORD g_visualStateStart = 0;
static BYTE g_fadeAlpha = 255;
static int g_finalIndex = -1;

static RouletteItem g_items[] = {
    { EffectType::Erase,              L"ERASE" },
    { EffectType::DisableSensitivity, L"NO SENS" },
    { EffectType::SensitivityHigh,    L"HIGH SENS" },
    { EffectType::InvertWASD,         L"INVERT WASD" },
    { EffectType::InvertMouse,        L"INVERT MOUSE" },
    { EffectType::HalfScreenCover,    L"HALF SCREEN" },
    { EffectType::Blur,               L"BLUR" },
    { EffectType::FpsLow,             L"FPS LOW" },
};
static const int ITEM_COUNT = sizeof(g_items) / sizeof(g_items[0]);

static RouletteIcon g_icons[ITEM_COUNT];

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

static HBITMAP LoadBitmapFromExeDir(const wchar_t* relativePath)
{
    wchar_t exeDir[MAX_PATH];
    GetModuleFileNameW(nullptr, exeDir, MAX_PATH);
    PathRemoveFileSpecW(exeDir);

    wchar_t fullPath[MAX_PATH];
    wsprintfW(fullPath, L"%s\\%s", exeDir, relativePath);

    HBITMAP bmp = (HBITMAP)LoadImageW(
        nullptr,
        fullPath,
        IMAGE_BITMAP,
        0, 0,
        LR_LOADFROMFILE | LR_CREATEDIBSECTION
    );

#ifdef _DEBUG
    if (!bmp) {
        DWORD err = GetLastError();
        wchar_t buf[512];
        wsprintfW(buf,
            L"FAILED TO LOAD BMP\n\n%s\n\nGetLastError = %lu",
            fullPath, err);
        MessageBoxW(nullptr, buf, L"BMP ERROR", MB_ICONERROR);
    }
#endif

    return bmp;
}

static void LoadRouletteIcons()
{
    g_icons[0] = { EffectType::Erase,
        LoadBitmapFromExeDir(L"assets\\icons\\erase.bmp") };

    g_icons[1] = { EffectType::DisableSensitivity,
        LoadBitmapFromExeDir(L"assets\\icons\\nosens.bmp") };

    g_icons[2] = { EffectType::SensitivityHigh,
        LoadBitmapFromExeDir(L"assets\\icons\\highsens.bmp") };

    g_icons[3] = { EffectType::InvertWASD,
        LoadBitmapFromExeDir(L"assets\\icons\\invert_wasd.bmp") };

    g_icons[4] = { EffectType::InvertMouse,
        LoadBitmapFromExeDir(L"assets\\icons\\invert_mouse.bmp") };

    g_icons[5] = { EffectType::HalfScreenCover,
        LoadBitmapFromExeDir(L"assets\\icons\\half_screen.bmp") };

    g_icons[6] = { EffectType::Blur,
        LoadBitmapFromExeDir(L"assets\\icons\\blur.bmp") };

    g_icons[7] = { EffectType::FpsLow,
        LoadBitmapFromExeDir(L"assets\\icons\\fps_low.bmp") };
}


// ======================================================
// ROULETTE VISUAL STATE
// ======================================================

static bool  g_previewRoulette = false;
static int   g_spinIndex = 0;
static DWORD g_lastSpinTick = 0;
// ======================================================
// OVERLAY HWND
// ======================================================

static HWND g_hwndOverlay = nullptr;

HWND GetOverlayWindow() {
    return g_hwndOverlay;
}

// ======================================================
// ROULETTE DRAW (LEFT → RIGHT, 5 ITEMS)
// ======================================================

static void DrawRoulette(HDC hdc, int screenW, int screenH) {

    const int baseSize = 200;
    const int finalSize = 240;
    const int spacing = 30;
    const int visible = 5;
    const int radius = 24;

    int centerX = screenW / 2;
    int centerY = screenH / 5;

    int totalWidth = visible * baseSize + (visible - 1) * spacing;
    int startX = centerX - totalWidth / 2;

    for (int i = 0; i < visible; i++) {

        int index = g_visualState == RouletteVisualState::Spinning
            ? (g_spinIndex + i) % ITEM_COUNT
            : (g_spinIndex + i) % ITEM_COUNT; // сейчас ок, но если захочешь подкрутить — точка тут

        bool selected = (i == visible / 2);
        bool isFinal = selected && g_visualState != RouletteVisualState::Spinning;

        int size = isFinal ? finalSize : baseSize;

        int x = startX + i * (baseSize + spacing);
        if (isFinal)
            x -= (finalSize - baseSize) / 2;

        int y = centerY - size / 2;

        RECT r{ x, y, x + size, y + size };

        COLORREF bg = selected ? RGB(255, 200, 0) : RGB(70, 70, 70);

        HBRUSH brush = CreateSolidBrush(bg);
        HPEN pen = CreatePen(PS_SOLID, selected ? 4 : 2, RGB(255, 255, 255));

        HGDIOBJ oldBrush = SelectObject(hdc, brush);
        HGDIOBJ oldPen = SelectObject(hdc, pen);

        RoundRect(hdc, r.left, r.top, r.right, r.bottom, radius, radius);

        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(brush);
        DeleteObject(pen);

        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);

        HBITMAP icon = g_icons[index].bitmap;
        if (icon) {

            HDC memDC = CreateCompatibleDC(hdc);
            HGDIOBJ oldBmp = SelectObject(memDC, icon);

            BITMAP bmp{};
            GetObject(icon, sizeof(bmp), &bmp);

            int iconSize = isFinal ? finalSize - 20 : baseSize - 20;
            int ix = r.left + (r.right - r.left - iconSize) / 2;
            int iy = r.top + (r.bottom - r.top - iconSize) / 2;

            StretchBlt(
                hdc,
                ix, iy,
                iconSize, iconSize,
                memDC,
                0, 0,
                bmp.bmWidth, bmp.bmHeight,
                SRCCOPY
            );

            SelectObject(memDC, oldBmp);
            DeleteDC(memDC);
        }

    }
}

// ======================================================
// WINDOW PROCEDURE
// ======================================================

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    // Erase / HalfScreen могут перехватывать
    if (HandleErase(hwnd, msg, wParam))
        return 0;

    if (HandleHalfScreenCover(hwnd, msg))
        return 0;

    switch (msg) {

        // -----------------------
        // TICK
        // -----------------------
    case WM_TIMER:
    {
        UpdateEffectSystem();
        DWORD now = GetTickCount();
        bool needRedraw = false;

        // ======================
        // SPIN
        // ======================
        if (g_visualState == RouletteVisualState::Hidden &&
            (g_previewRoulette || GetSystemState() == SystemState::Rolling))
        {
            g_visualState = RouletteVisualState::Spinning;
            g_visualStateStart = now;
            g_lastSpinTick = now;
            g_fadeAlpha = 255;
            needRedraw = true;
        }

        if (g_visualState == RouletteVisualState::Spinning)
        {
            if (now - g_lastSpinTick >= 80)
            {
                g_spinIndex = (g_spinIndex + 1) % ITEM_COUNT;
                g_lastSpinTick = now;
                needRedraw = true;
            }

            // ---- EFFECT START → HOLD
            if (!g_previewRoulette &&
                GetSystemState() == SystemState::Effect)
            {
                // 🔥 фиксируем эффект (см. код выше)
                EffectType eff = GetCurrentEffect();
                for (int i = 0; i < ITEM_COUNT; i++) {
                    if (g_items[i].type == eff) {
                        g_finalIndex = i;
                        break;
                    }
                }

                g_spinIndex = (g_finalIndex - 2 + ITEM_COUNT) % ITEM_COUNT;

                g_visualState = RouletteVisualState::HoldResult;
                g_visualStateStart = now;
                needRedraw = true;
            }
        }

        // ======================
        // HOLD RESULT (2s)
        // ======================
        if (g_visualState == RouletteVisualState::HoldResult)
        {
            if (now - g_visualStateStart >= 2000)
            {
                g_visualState = RouletteVisualState::FadeOut;
                g_visualStateStart = now;
            }
            needRedraw = true;
        }

        // ======================
        // FADE OUT (0.5s)
        // ======================
        if (g_visualState == RouletteVisualState::FadeOut)
        {
            DWORD elapsed = now - g_visualStateStart;

            if (elapsed >= 500)
            {
                g_visualState = RouletteVisualState::Hidden;
                g_fadeAlpha = 255;
            }
            else
            {
                g_fadeAlpha = (BYTE)(255 - elapsed * 255 / 500);
            }

            needRedraw = true;
        }

        // ======================
        // BLUR
        // ======================
        if (GetCurrentEffect() == EffectType::Blur)
        {
            UpdateBlurEffect();
            needRedraw = true;
        }

        if (needRedraw)
            InvalidateRect(hwnd, nullptr, FALSE);

        return 0;
    }


    // -----------------------
    // RENDER
    // -----------------------
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        int w = GetSystemMetrics(SM_CXSCREEN);
        int h = GetSystemMetrics(SM_CYSCREEN);

        // ====== BACK BUFFER ======
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBmp = CreateCompatibleBitmap(hdc, w, h);
        HGDIOBJ oldBmp = SelectObject(memDC, memBmp);

        // ====== CLEAR ======
        if (GetCurrentEffect() != EffectType::Erase) {
            HBRUSH clear = CreateSolidBrush(RGB(255, 0, 255));
            RECT full{ 0, 0, w, h };
            FillRect(memDC, &full, clear);
            DeleteObject(clear);
        }

        // ====== BLUR ======
        if (GetCurrentEffect() == EffectType::Blur) {
            int count = GetBlurLineCount();
            HBRUSH dark = CreateSolidBrush(RGB(0, 0, 0));

            for (int i = 0; i < count; i++) {
                int y = rand() % h;
                RECT r{ 0, y, w, y + (rand() % 20 + 2) };
                FillRect(memDC, &r, dark);
            }
            DeleteObject(dark);
        }

        // ====== ROULETTE ======
        if (g_visualState != RouletteVisualState::Hidden) {
            DrawRoulette(memDC, w, h);
        }

        // ====== FLIP ======
        BitBlt(hdc, 0, 0, w, h, memDC, 0, 0, SRCCOPY);

        // ====== CLEANUP ======
        SelectObject(memDC, oldBmp);
        DeleteObject(memBmp);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        return 0;
    }


    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ======================================================
// CREATE OVERLAY WINDOW
// ======================================================

bool CreateMainWindow(HINSTANCE hInstance) {

    WNDCLASSEX wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"DX_OVERLAY";

    LoadRouletteIcons();

    RegisterClassEx(&wc);

    g_hwndOverlay = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_NOACTIVATE,
        wc.lpszClassName,
        L"DX OVERLAY",
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        nullptr, nullptr,
        hInstance, nullptr
    );

    if (!g_hwndOverlay)
        return false;

    ShowWindow(g_hwndOverlay, SW_SHOWNOACTIVATE);

    SetWindowPos(
        g_hwndOverlay,
        HWND_TOPMOST,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE
    );

    SetLayeredWindowAttributes(
        g_hwndOverlay,
        RGB(255, 0, 255),
        0,
        LWA_COLORKEY
    );

    SetTimer(g_hwndOverlay, 1, 16, nullptr); // ~60 FPS
    return true;
}

// ======================================================
// MESSAGE LOOP
// ======================================================

void RunMessageLoop() {
    MSG msg{};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

// ======================================================
// OVERLAY HELPERS
// ======================================================

void SetOverlayMousePassThrough(HWND hwnd, bool passThrough) {
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

    if (passThrough)
        exStyle |= WS_EX_TRANSPARENT;
    else
        exStyle &= ~WS_EX_TRANSPARENT;

    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

    SetWindowPos(
        hwnd, nullptr,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
    );
}

void SetOverlayColorkey(HWND hwnd) {
    SetLayeredWindowAttributes(
        hwnd,
        RGB(255, 0, 255),
        0,
        LWA_COLORKEY
    );
}

void SetOverlayAlpha(HWND hwnd) {
    SetLayeredWindowAttributes(
        hwnd,
        0,
        255,
        LWA_ALPHA
    );
}

// ======================================================
// PREVIEW API (CALL FROM CONTROL WINDOW)
// ======================================================

void StartPreviewRoulette() {
    g_previewRoulette = true;
    g_lastSpinTick = GetTickCount();
}

void StopPreviewRoulette() {
    g_previewRoulette = false;
    g_visualState = RouletteVisualState::Hidden;
    g_fadeAlpha = 255;
}
