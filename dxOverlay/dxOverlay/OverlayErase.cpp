#include "OverlayErase.h"
#include "EffectSystem.h"
#include <vector>
#include <Windows.h>

// -------------------------------
// Данные erase-эффекта
// -------------------------------

struct Hole {
    int x, y, r;
};

static std::vector<Hole> g_holes;
static const int ERASE_RADIUS = 35;

// -------------------------------
// Главная функция erase-эффекта
// -------------------------------

bool HandleErase(HWND hwnd, UINT msg, WPARAM wParam) {

    // ❗ Проверяем состояние ТОЛЬКО через EffectSystem
    if (GetCurrentEffect() != EffectType::Erase)
        return false;

    switch (msg) {

    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE:

        // Стираем только если:
        // 1) зажата ЛКМ
        // 2) эффект ещё жив
        if ((wParam & MK_LBUTTON) && IsEffectAlive()) {

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
            InvalidateRect(hwnd, nullptr, FALSE);
        }

        return true;

    case WM_PAINT: {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &ps.rcPaint, brush);
        DeleteObject(brush);

        EndPaint(hwnd, &ps);
        return true;
    }
    }

    return false;
}

void ResetErase(HWND hwnd) {
    g_holes.clear();
    SetWindowRgn(hwnd, nullptr, TRUE); // 🔥 ВАЖНО
}


