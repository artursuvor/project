#include <Windows.h>
#include "ControlWindow.h"
#include "EffectSystem.h"

// --------------------
// Control window handle
// --------------------
static HWND g_hwndControl = nullptr;

HWND GetControlWindow() {
    return g_hwndControl;
}

// --------------------
// Delay config
// --------------------
#define DELAY_TIMER_ID  5000
#define ACTION_DELAY_MS 3000

// --------------------
// Button IDs
// --------------------
#define BTN_START_SYSTEM   1001
#define BTN_STOP_SYSTEM    1002

#define BTN_FORCE_ROULETTE 1100
#define BTN_STOP_EFFECT    1101

#define BTN_ERASE          1200
#define BTN_BLUR           1201
#define BTN_HALF           1202
#define BTN_INVERT_WASD    1203
#define BTN_INVERT_MOUSE   1204
#define BTN_LOW_FPS        1205
#define BTN_PREVIEW_ROULETTE 1300
#define BTN_STOP_PREVIEW     1301


// --------------------
// Pending action
// --------------------
enum class PendingAction {
    None,
    StartSystem,
    StopSystem,
    ForceRoulette,
    StopEffect,
    ForceErase,
    ForceBlur,
    ForceHalf,
    ForceInvertWASD,
    ForceInvertMouse,
    ForceFpsLow,
    PreviewRoulette,
    StopPreviewRoulette
};

static PendingAction g_pendingAction = PendingAction::None;

// --------------------
// Execute pending action
// --------------------
static void ExecutePendingAction() {

    switch (g_pendingAction) {
    case PendingAction::StartSystem:        StartSystem(); break;
    case PendingAction::StopSystem:         StopSystem(); break;
    case PendingAction::ForceRoulette:      Dev_ForceRoulette(); break;
    case PendingAction::StopEffect:         Dev_StopEffect(); break;
    case PendingAction::ForceErase:         Dev_ForceEffect(EffectType::Erase); break;
    case PendingAction::ForceBlur:          Dev_ForceEffect(EffectType::Blur); break;
    case PendingAction::ForceHalf:          Dev_ForceEffect(EffectType::HalfScreenCover); break;
    case PendingAction::ForceInvertWASD:    Dev_ForceEffect(EffectType::InvertWASD); break;
    case PendingAction::ForceInvertMouse:   Dev_ForceEffect(EffectType::InvertMouse); break;
    case PendingAction::ForceFpsLow:        Dev_ForceEffect(EffectType::FpsLow); break;
    case PendingAction::PreviewRoulette:     Dev_StartPreviewRoulette(); break;
    case PendingAction::StopPreviewRoulette: Dev_StopPreviewRoulette(); break;


    default: break;
    }

    g_pendingAction = PendingAction::None;
}

// --------------------
// Window procedure
// --------------------
LRESULT CALLBACK ControlWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

    case WM_COMMAND:
    {
        // если уже есть ожидающее действие — игнор
        if (g_pendingAction != PendingAction::None)
            return 0;

        switch (LOWORD(wParam)) {

        case BTN_START_SYSTEM:   g_pendingAction = PendingAction::StartSystem; break;
        case BTN_STOP_SYSTEM:    g_pendingAction = PendingAction::StopSystem; break;
        case BTN_FORCE_ROULETTE: g_pendingAction = PendingAction::ForceRoulette; break;
        case BTN_STOP_EFFECT:    g_pendingAction = PendingAction::StopEffect; break;

        case BTN_ERASE:          g_pendingAction = PendingAction::ForceErase; break;
        case BTN_BLUR:           g_pendingAction = PendingAction::ForceBlur; break;
        case BTN_HALF:           g_pendingAction = PendingAction::ForceHalf; break;
        case BTN_INVERT_WASD:    g_pendingAction = PendingAction::ForceInvertWASD; break;
        case BTN_INVERT_MOUSE:   g_pendingAction = PendingAction::ForceInvertMouse; break;
        case BTN_LOW_FPS:        g_pendingAction = PendingAction::ForceFpsLow; break;
        case BTN_PREVIEW_ROULETTE:g_pendingAction = PendingAction::PreviewRoulette;  break;
        case BTN_STOP_PREVIEW: g_pendingAction = PendingAction::StopPreviewRoulette; break;


        default:
            return 0;
        }

        // стартуем таймер
        SetTimer(hwnd, DELAY_TIMER_ID, ACTION_DELAY_MS, nullptr);
        return 0;
    }

    case WM_TIMER:
        if (wParam == DELAY_TIMER_ID) {
            KillTimer(hwnd, DELAY_TIMER_ID);
            ExecutePendingAction();
        }
        return 0;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// --------------------
// Create control window
// --------------------
bool CreateControlWindow(HINSTANCE hInstance) {

    WNDCLASS wc{};
    wc.lpfnWndProc = ControlWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"DX_CONTROL";

    RegisterClass(&wc);

    g_hwndControl = CreateWindow(
        wc.lpszClassName,
        L"Chaos Controller (DEV)",
        WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
        200, 200, 420, 360,
        nullptr, nullptr, hInstance, nullptr
    );

    // --- SYSTEM ---
    CreateWindow(L"BUTTON", L"START SYSTEM", WS_CHILD | WS_VISIBLE,
        20, 20, 160, 30, g_hwndControl, (HMENU)BTN_START_SYSTEM, hInstance, nullptr);

    CreateWindow(L"BUTTON", L"STOP SYSTEM", WS_CHILD | WS_VISIBLE,
        200, 20, 160, 30, g_hwndControl, (HMENU)BTN_STOP_SYSTEM, hInstance, nullptr);

    CreateWindow(L"BUTTON", L"FORCE ROULETTE (3s)", WS_CHILD | WS_VISIBLE,
        20, 60, 160, 30, g_hwndControl, (HMENU)BTN_FORCE_ROULETTE, hInstance, nullptr);

    CreateWindow(L"BUTTON", L"STOP EFFECT (3s)", WS_CHILD | WS_VISIBLE,
        200, 60, 160, 30, g_hwndControl, (HMENU)BTN_STOP_EFFECT, hInstance, nullptr);

    // --- EFFECTS ---
    int y = 110;
    CreateWindow(L"BUTTON", L"ERASE", WS_CHILD | WS_VISIBLE,
        20, y, 160, 28, g_hwndControl, (HMENU)BTN_ERASE, hInstance, nullptr);

    CreateWindow(L"BUTTON", L"BLUR", WS_CHILD | WS_VISIBLE,
        200, y, 160, 28, g_hwndControl, (HMENU)BTN_BLUR, hInstance, nullptr);

    y += 34;
    CreateWindow(L"BUTTON", L"HALF SCREEN", WS_CHILD | WS_VISIBLE,
        20, y, 160, 28, g_hwndControl, (HMENU)BTN_HALF, hInstance, nullptr);

    CreateWindow(L"BUTTON", L"INVERT WASD", WS_CHILD | WS_VISIBLE,
        200, y, 160, 28, g_hwndControl, (HMENU)BTN_INVERT_WASD, hInstance, nullptr);

    y += 34;
    CreateWindow(L"BUTTON", L"INVERT MOUSE", WS_CHILD | WS_VISIBLE,
        20, y, 160, 28, g_hwndControl, (HMENU)BTN_INVERT_MOUSE, hInstance, nullptr);

    CreateWindow(L"BUTTON", L"FPS LOW", WS_CHILD | WS_VISIBLE,
        200, y, 160, 28, g_hwndControl, (HMENU)BTN_LOW_FPS, hInstance, nullptr);

    // --- PREVIEW ---
    CreateWindow(
        L"BUTTON", L"PREVIEW ROULETTE",
        WS_CHILD | WS_VISIBLE,
        20, y + 40, 160, 30,
        g_hwndControl,
        (HMENU)BTN_PREVIEW_ROULETTE,
        hInstance, nullptr
    );

    CreateWindow(
        L"BUTTON", L"STOP PREVIEW",
        WS_CHILD | WS_VISIBLE,
        200, y + 40, 160, 30,
        g_hwndControl,
        (HMENU)BTN_STOP_PREVIEW,
        hInstance, nullptr
    );


    ShowWindow(g_hwndControl, SW_SHOW);
    return true;
}
