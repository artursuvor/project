/**
 * @file OverlayErase.cpp
 * @brief Эффект "стирания" части экрана (прозрачные отверстия)
 *
 * Позволяет пользователю "стирать" части оверлея нажатием ЛКМ,
 * создавая прозрачные отверстия в черном оверлее
 */

#include "OverlayErase.h"
#include "EffectSystem.h"
#include "Config.h"
#include <vector>
#include <Windows.h>

// ============================================================================
// ERASE STATE
// ============================================================================

/** Список всех созданных отверстий */
static std::vector<EraseHole> g_holes;

// ============================================================================
// EFFECT IMPLEMENTATION
// ============================================================================

bool HandleErase(HWND hwnd, UINT msg, WPARAM wParam) {
    // Проверяем, что эффект Erase активен
    if (GetCurrentEffect() != EffectType::Erase)
        return false;

    switch (msg) {

    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE: {
        // Создаем новое отверстие при нажатой ЛКМ
        if ((wParam & MK_LBUTTON) && IsEffectAlive()) {
            POINT cursorPos;
            GetCursorPos(&cursorPos);

            // Добавляем отверстие в список
            g_holes.push_back({
                cursorPos.x,
                cursorPos.y,
                ERASE_EFFECT_RADIUS_PX
            });

            // Создаем регион окна с отверстиями
            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);

            HRGN region = CreateRectRgn(0, 0, screenWidth, screenHeight);

            // Вычитаем все отверстия из региона
            for (const auto& hole : g_holes) {
                HRGN holeRgn = CreateEllipticRgn(
                    hole.x - hole.radius,
                    hole.y - hole.radius,
                    hole.x + hole.radius,
                    hole.y + hole.radius
                );
                CombineRgn(region, region, holeRgn, RGN_DIFF);
                DeleteObject(holeRgn);
            }

            // Применяем регион и перерисовываем
            SetWindowRgn(hwnd, region, TRUE);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return true;
    }

    case WM_PAINT: {
        // Перерисовываем черный фон оверлея
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &ps.rcPaint, blackBrush);
        DeleteObject(blackBrush);

        EndPaint(hwnd, &ps);
        return true;
    }

    default:
        return false;
    }
}

void ResetErase(HWND hwnd) {
    // Очищаем все отверстия
    g_holes.clear();

    // Восстанавливаем прямоугольный регион окна (без отверстий)
    SetWindowRgn(hwnd, nullptr, TRUE);
}


