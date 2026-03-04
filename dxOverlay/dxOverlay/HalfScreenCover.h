#pragma once

#include <Windows.h>

/**
 * @file HalfScreenCover.h
 * @brief Эффект закрытия половины экрана
 *
 * Закрывает левую половину экрана черным оверлеем,
 * оставляя видимой только правую половину
 */

/**
 * @brief Обработать событие WM_PAINT для эффекта половины экрана
 * @param hwnd Handle окна оверлея
 * @param msg Сообщение Windows (обычно WM_PAINT)
 * @return true если сообщение было обработано
 */
bool HandleHalfScreenCover(HWND hwnd, UINT msg);

/**
 * @brief Сбросить эффект (очистить черный оверлей)
 *
 * Очистка происходит автоматически через SetOverlayColorkey
 */
void ResetHalfScreenCover();
