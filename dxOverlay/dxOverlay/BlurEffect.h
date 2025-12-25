#pragma once
#include <Windows.h>

void StartBlurEffect(HWND hwnd);
void StopBlurEffect(HWND hwnd);

// 👇 для WM_PAINT
int GetBlurLineCount();

// 👇 для WM_TIMER
void UpdateBlurEffect();
