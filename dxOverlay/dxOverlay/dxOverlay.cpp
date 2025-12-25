#include <Windows.h>
#include <vector>

// ================= EFFECT SYSTEM =================

enum class EffectType {
    None,
    Erase,
    DisableLMB,
    DisableKeyboard,
    InvertMouse,
    InvertKeyboard
};

EffectType g_currentEffect = EffectType::Erase;

DWORD g_effectStartTime = 0;
DWORD g_effectDuration = 30000; // 30 секунд

// ================= ERASE DATA =================

struct Hole {
    int x, y, r;
};

std::vector<Hole> g_holes;

const int ERASE_RADIUS = 35;

// ================= UTILS =================

bool IsEffectAlive() {
    return GetTickCount() - g_effectStartTime < g_effectDuration;
}

void StopEffect(HWND hwnd) {
    g_currentEffect = EffectType::None;
    g_holes.clear();
    SetWindowRgn(hwnd, nullptr, TRUE);
    InvalidateRect(hwnd, nullptr, TRUE);
}

// ================= WINDOW PROC =================

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        // -------- ERASE EFFECT --------
    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE:
        if (g_currentEffect != EffectType::Erase)
            return 0;

        if (wParam & MK_LBUTTON && IsEffectAlive()) {
            POINT p;
            GetCursorPos(&p);

            g_holes.push_back({ p.x, p.y, ERASE_RADIUS });

            HRGN region = CreateRectRgn(
                0, 0,
                GetSystemMetrics(SM_CXSCREEN),
                GetSystemMetrics(SM_CYSCREEN)
            );

            for (auto& h : g_holes) {
                HRGN hole = CreateEllipticRgn(
                    h.x - h.r, h.y - h.r,
                    h.x + h.r, h.y + h.r
                );
                CombineRgn(region, region, hole, RGN_DIFF);
                DeleteObject(hole);
            }

            SetWindowRgn(hwnd, region, TRUE);
        }
        return 0;

        // -------- TIMER (effect lifetime) --------
    case WM_TIMER:
        if (!IsEffectAlive() && g_currentEffect != EffectType::None) {
            StopEffect(hwnd);
        }
        return 0;

        // -------- INPUT --------
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            DestroyWindow(hwnd);
            return 0;
        }
        break;

        // -------- DRAW --------
    case WM_PAINT: {
        if (g_currentEffect == EffectType::None)
            break;

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &ps.rcPaint, brush);
        DeleteObject(brush);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ================= ENTRY =================

int APIENTRY wWinMain(
    HINSTANCE hInstance,
    HINSTANCE,
    LPWSTR,
    int
) {
    WNDCLASSEX wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"DX_OVERLAY";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED,
        wc.lpszClassName,
        L"DX OVERLAY",
        WS_POPUP | WS_VISIBLE,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    SetWindowPos(
        hwnd,
        HWND_TOPMOST,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE
    );

    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

    // старт эффекта
    g_effectStartTime = GetTickCount();
    SetTimer(hwnd, 1, 16, nullptr); // тик для контроля времени

    MSG msg{};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
