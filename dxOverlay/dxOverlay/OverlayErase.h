#pragma once
#include <Windows.h>

// возвращает true, если сообщение обработано
bool HandleErase(HWND hwnd, UINT msg, WPARAM wParam);
void ResetErase(HWND hwnd);
