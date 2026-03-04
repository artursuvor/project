#pragma once

#include <Windows.h>

/**
 * @file Window.h
 * @brief Управление главным окном оверлея и его визуализацией
 *
 * Отвечает за:
 * - Создание и управление окном оверлея поверх всех окон
 * - Message loop приложения
 * - Отрисовку рулетки и эффектов
 * - Вспомогательные функции для работы с окном оверлея
 */

// ============================================================================
// WINDOW INITIALIZATION
// ============================================================================

/**
 * @brief Создать главное окно оверлея
 * @param hInstance Handle экземпляра приложения
 * @return true если окно успешно создано
 *
 * Создает полноэкранное прозрачное окно поверх всех остальных
 */
bool CreateMainWindow(HINSTANCE hInstance);

/**
 * @brief Запустить главный message loop приложения
 *
 * Блокирует выполнение до закрытия приложения.
 * Обрабатывает все Windows сообщения и обновляет эффекты.
 */
void RunMessageLoop();

/**
 * @brief Получить handle главного окна оверлея
 * @return Handle окна или nullptr если окно не создано
 */
HWND GetOverlayWindow();

// ============================================================================
// OVERLAY WINDOW HELPERS
// ============================================================================

/**
 * @brief Установить режим "pass-through" для окна оверлея
 * @param hwnd Handle окна оверлея
 * @param passThrough true для пропускания кликов мышью через оверлей
 *
 * Если passThrough=true, клики проходят в окна под оверлеем.
 * Если passThrough=false, окно захватывает все клики.
 */
void SetOverlayMousePassThrough(HWND hwnd, bool passThrough);

/**
 * @brief Установить прозрачный цвет окна оверлея
 * @param hwnd Handle окна оверлея
 *
 * Использует цвет RGB(255, 0, 255) как прозрачный
 */
void SetOverlayColorkey(HWND hwnd);

/**
 * @brief Установить альфа-канал для всего окна
 * @param hwnd Handle окна оверлея
 *
 * Используется для глобальной прозрачности оверлея
 */
void SetOverlayAlpha(HWND hwnd);

// ============================================================================
// ROULETTE PREVIEW (DEPRECATED - use EffectSystem instead)
// ============================================================================

/**
 * @deprecated Используйте Dev_StartPreviewRoulette() из EffectSystem
 */
void StartPreviewRoulette();

/**
 * @deprecated Используйте Dev_StopPreviewRoulette() из EffectSystem
 */
void StopPreviewRoulette();
