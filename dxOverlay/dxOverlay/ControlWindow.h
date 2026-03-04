#pragma once

#include <Windows.h>

/**
 * @file ControlWindow.h
 * @brief DEV панель управления для тестирования эффектов
 *
 * Предоставляет окно с кнопками для:
 * - Запуска/остановки системы эффектов
 * - Ручного выбора конкретных эффектов
 * - Управления рулеткой
 * - Отладки
 */

/**
 * @brief Создать окно контроля эффектов
 * @param hInstance Handle экземпляра приложения
 * @return true если окно успешно создано
 *
 * Создает небольшое окно с кнопками управления.
 * Все кнопки связаны с EffectSystem API.
 */
bool CreateControlWindow(HINSTANCE hInstance);

/**
 * @brief Получить handle окна контроля
 * @return Handle окна или nullptr если окно не создано
 */
HWND GetControlWindow();
