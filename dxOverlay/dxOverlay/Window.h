#pragma once
#include <Windows.h>

bool CreateMainWindow(HINSTANCE hInstance);
void RunMessageLoop();

HWND GetOverlayWindow();

// Overlay helpers
void SetOverlayMousePassThrough(HWND hwnd, bool passThrough);
void SetOverlayColorkey(HWND hwnd);
void SetOverlayAlpha(HWND hwnd);
void StartPreviewRoulette();
void StopPreviewRoulette();
