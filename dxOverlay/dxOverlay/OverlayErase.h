#pragma once

#include <Windows.h>

/**
 * @file OverlayErase.h
 * @brief Эффект "стирания" частей оверлея
 * 
 * Позволяет создавать прозрачные отверстия в черном оверлее
 * путем нажатия левой кнопки мыши
 */

/**
 * @brief Обработать событие мыши для эффекта стирания
 * @param hwnd Handle окна оверлея
 * @param msg Сообщение Windows (WM_LBUTTONDOWN, WM_MOUSEMOVE, WM_PAINT)
 * @param wParam Параметры сообщения
 * @return true если сообщение было обработано
 * 
 * Вызывается из message loop окна оверлея для интерактивности
 */
bool HandleErase(HWND hwnd, UINT msg, WPARAM wParam);

/**
 * @brief Сбросить эффект (очистить все отверстия)
 * @param hwnd Handle окна оверлея
 * 
 * Должна быть вызвана перед завершением эффекта
 */
void ResetErase(HWND hwnd);
