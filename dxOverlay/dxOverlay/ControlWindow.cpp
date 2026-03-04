/**
 * @file ControlWindow.cpp
 * @brief Реализация DEV панели управления
 * 
 * Предоставляет окно с кнопками для тестирования эффектов
 */

#include <Windows.h>
#include "ControlWindow.h"
#include "EffectSystem.h"
#include "Types.h"
#include "Config.h"

// ============================================================================
// WINDOW STATE
// ============================================================================

/** Handle окна контроля */
static HWND g_hwndControl = nullptr;

/** Действие, ожидающее выполнения после задержки */
static PendingAction g_pendingAction = PendingAction::None;

HWND GetControlWindow() {
    return g_hwndControl;
}

/**
 * @brief Получить handle окна контроля
 */

// ============================================================================
// PENDING ACTION EXECUTOR
// ============================================================================

/**
 * @brief Выполнить запланированное действие
 * 
 * Вызывается из таймера после задержки ACTION_DELAY_MS
 */
static void ExecutePendingAction() {
    switch (g_pendingAction) {
    // System control
    case PendingAction::StartSystem:            StartSystem(); break;
    case PendingAction::StopSystem:             StopSystem(); break;
    
    // Roulette control
    case PendingAction::ForceRoulette:          Dev_ForceRoulette(); break;
    case PendingAction::StopEffect:             Dev_StopEffect(); break;
    
    // Individual effects
    case PendingAction::ForceErase:             Dev_ForceEffect(EffectType::Erase); break;
    case PendingAction::ForceBlur:              Dev_ForceEffect(EffectType::Blur); break;
    case PendingAction::ForceHalf:              Dev_ForceEffect(EffectType::HalfScreenCover); break;
    case PendingAction::ForceInvertWASD:        Dev_ForceEffect(EffectType::InvertWASD); break;
    case PendingAction::ForceInvertMouse:       Dev_ForceEffect(EffectType::InvertMouse); break;
    case PendingAction::ForceFpsLow:            Dev_ForceEffect(EffectType::FpsLow); break;
    case PendingAction::ForceSensitivityDisable:Dev_ForceEffect(EffectType::DisableSensitivity); break;
    case PendingAction::ForceSensitivityHigh:   Dev_ForceEffect(EffectType::SensitivityHigh); break;
    
    // Roulette preview
    case PendingAction::PreviewRoulette:        Dev_StartPreviewRoulette(); break;
    case PendingAction::StopPreviewRoulette:    Dev_StopPreviewRoulette(); break;
    
    default:
        break;
    }

    // Очищаем действие после выполнения
    g_pendingAction = PendingAction::None;
}

// ============================================================================
// WINDOW PROCEDURE
// ============================================================================

/**
 * @brief Message handler для окна контроля
 */
LRESULT CALLBACK ControlWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    case WM_COMMAND: {
        // Игнорируем клик, если уже есть ожидающее действие
        // (это предотвращает спам)
        if (g_pendingAction != PendingAction::None)
            return 0;

        // Определяем какую кнопку нажали и устанавливаем соответствующее действие
        switch (LOWORD(wParam)) {
        // System control buttons
        case BTN_START_SYSTEM:          g_pendingAction = PendingAction::StartSystem; break;
        case BTN_STOP_SYSTEM:           g_pendingAction = PendingAction::StopSystem; break;
        
        // Roulette control buttons
        case BTN_FORCE_ROULETTE:        g_pendingAction = PendingAction::ForceRoulette; break;
        case BTN_STOP_EFFECT:           g_pendingAction = PendingAction::StopEffect; break;
        
        // Individual effect buttons
        case BTN_ERASE:                 g_pendingAction = PendingAction::ForceErase; break;
        case BTN_BLUR:                  g_pendingAction = PendingAction::ForceBlur; break;
        case BTN_HALF_SCREEN:           g_pendingAction = PendingAction::ForceHalf; break;
        case BTN_INVERT_WASD:           g_pendingAction = PendingAction::ForceInvertWASD; break;
        case BTN_INVERT_MOUSE:          g_pendingAction = PendingAction::ForceInvertMouse; break;
        case BTN_LOW_FPS:               g_pendingAction = PendingAction::ForceFpsLow; break;
        case BTN_DISABLE_SENSITIVITY:   g_pendingAction = PendingAction::ForceSensitivityDisable; break;
        case BTN_HIGH_SENSITIVITY:      g_pendingAction = PendingAction::ForceSensitivityHigh; break;
        
        // Roulette preview buttons
        case BTN_PREVIEW_ROULETTE:      g_pendingAction = PendingAction::PreviewRoulette; break;
        case BTN_STOP_PREVIEW:          g_pendingAction = PendingAction::StopPreviewRoulette; break;

        default:
            return 0;
        }

        // Стартуем таймер - действие выполнится после задержки
        SetTimer(hwnd, TimerId::TIMER_DELAY, ACTION_DELAY_MS, nullptr);
        return 0;
    }

    case WM_TIMER:
        if (wParam == TimerId::TIMER_DELAY) {
            KillTimer(hwnd, TimerId::TIMER_DELAY);
            ExecutePendingAction();
        }
        return 0;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

// ============================================================================
// WINDOW CREATION & BUTTON LAYOUT
// ============================================================================

/**
 * @brief Вспомогательная функция для создания кнопки
 * @param hInstance Handle приложения
 * @param parent Handle родительского окна
 * @param text Текст на кнопке
 * @param id ID кнопки
 * @param x X координата
 * @param y Y координата
 * @param width Ширина кнопки
 * @param height Высота кнопки
 */
static inline HWND CreateButton(
    HINSTANCE hInstance,
    HWND parent,
    const wchar_t* text,
    int id,
    int x, int y,
    int width, int height
) {
    return CreateWindow(
        L"BUTTON", text,
        WS_CHILD | WS_VISIBLE,
        x, y, width, height,
        parent,
        (HMENU)(intptr_t)id,
        hInstance,
        nullptr
    );
}

bool CreateControlWindow(HINSTANCE hInstance) {
    // Регистрируем класс окна
    WNDCLASS wc{};
    wc.lpfnWndProc = ControlWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CONTROL_CLASS_NAME;

    RegisterClass(&wc);

    // Создаем главное окно контроля
    g_hwndControl = CreateWindow(
        CONTROL_CLASS_NAME,
        CONTROL_WINDOW_TITLE,
        WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
        CONTROL_WINDOW_X,
        CONTROL_WINDOW_Y,
        CONTROL_WINDOW_WIDTH,
        CONTROL_WINDOW_HEIGHT,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!g_hwndControl)
        return false;

    // =====================================================================
    // SYSTEM CONTROL BUTTONS
    // =====================================================================
    
    CreateButton(hInstance, g_hwndControl, L"START SYSTEM", BTN_START_SYSTEM,
        MARGIN_LEFT, MARGIN_TOP,
        BUTTON_WIDTH, BUTTON_HEIGHT);

    CreateButton(hInstance, g_hwndControl, L"STOP SYSTEM", BTN_STOP_SYSTEM,
        MARGIN_LEFT + COLUMN_SPACING_X, MARGIN_TOP,
        BUTTON_WIDTH, BUTTON_HEIGHT);

    CreateButton(hInstance, g_hwndControl, L"FORCE ROULETTE", BTN_FORCE_ROULETTE,
        MARGIN_LEFT, MARGIN_TOP + BUTTON_SPACING_Y,
        BUTTON_WIDTH, BUTTON_HEIGHT);

    CreateButton(hInstance, g_hwndControl, L"STOP EFFECT", BTN_STOP_EFFECT,
        MARGIN_LEFT + COLUMN_SPACING_X, MARGIN_TOP + BUTTON_SPACING_Y,
        BUTTON_WIDTH, BUTTON_HEIGHT);

    // =====================================================================
    // INDIVIDUAL EFFECT BUTTONS
    // =====================================================================
    
    int y = MARGIN_TOP + 100;

    // Row 1: Erase, Blur
    CreateButton(hInstance, g_hwndControl, L"ERASE", BTN_ERASE,
        MARGIN_LEFT, y,
        BUTTON_WIDTH, BUTTON_HEIGHT);

    CreateButton(hInstance, g_hwndControl, L"BLUR", BTN_BLUR,
        MARGIN_LEFT + COLUMN_SPACING_X, y,
        BUTTON_WIDTH, BUTTON_HEIGHT);

    // Row 2: Half Screen, Invert WASD
    y += BUTTON_SPACING_Y;
    CreateButton(hInstance, g_hwndControl, L"HALF SCREEN", BTN_HALF_SCREEN,
        MARGIN_LEFT, y,
        BUTTON_WIDTH, BUTTON_HEIGHT);

    CreateButton(hInstance, g_hwndControl, L"INVERT WASD", BTN_INVERT_WASD,
        MARGIN_LEFT + COLUMN_SPACING_X, y,
        BUTTON_WIDTH, BUTTON_HEIGHT);

    // Row 3: Invert Mouse, FPS Low
    y += BUTTON_SPACING_Y;
    CreateButton(hInstance, g_hwndControl, L"INVERT MOUSE", BTN_INVERT_MOUSE,
        MARGIN_LEFT, y,
        BUTTON_WIDTH, BUTTON_HEIGHT);

    CreateButton(hInstance, g_hwndControl, L"FPS LOW", BTN_LOW_FPS,
        MARGIN_LEFT + COLUMN_SPACING_X, y,
        BUTTON_WIDTH, BUTTON_HEIGHT);

    // Row 4: Disable Sens, High Sens
    y += BUTTON_SPACING_Y;
    CreateButton(hInstance, g_hwndControl, L"NO SENS", BTN_DISABLE_SENSITIVITY,
        MARGIN_LEFT, y,
        BUTTON_WIDTH, BUTTON_HEIGHT);

    CreateButton(hInstance, g_hwndControl, L"HIGH SENS", BTN_HIGH_SENSITIVITY,
        MARGIN_LEFT + COLUMN_SPACING_X, y,
        BUTTON_WIDTH, BUTTON_HEIGHT);

    // =====================================================================
    // ROULETTE PREVIEW BUTTONS
    // =====================================================================
    
    y += BUTTON_SPACING_Y + 10;
    CreateButton(hInstance, g_hwndControl, L"PREVIEW ROULETTE", BTN_PREVIEW_ROULETTE,
        MARGIN_LEFT, y,
        BUTTON_WIDTH, BUTTON_HEIGHT);

    CreateButton(hInstance, g_hwndControl, L"STOP PREVIEW", BTN_STOP_PREVIEW,
        MARGIN_LEFT + COLUMN_SPACING_X, y,
        BUTTON_WIDTH, BUTTON_HEIGHT);

    // Показываем окно
    ShowWindow(g_hwndControl, SW_SHOW);
    return true;
}
