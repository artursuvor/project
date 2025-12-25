#include "InputSequencer.h"
#include <Windows.h>
#include <cstring>

// ------------------------------------
// Отправка одной клавиши (down / up)
// ------------------------------------
void SendKey(WORD vk, bool down) {
    INPUT input{};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

// ------------------------------------
// Нажатие клавиши (down + up)
// ------------------------------------
void SendKeyPress(WORD vk) {
    SendKey(vk, true);
    SendKey(vk, false);
}

// ------------------------------------
// Ctrl + V
// ------------------------------------
void PressCtrlV() {
    SendKey(VK_CONTROL, true);
    SendKeyPress('V');
    SendKey(VK_CONTROL, false);
}

// ------------------------------------
// Enter
// ------------------------------------
void PressEnter() {
    SendKeyPress(VK_RETURN);
}

// ------------------------------------
// F7
// ------------------------------------
void PressF7() {
    SendKeyPress(VK_F7);
}

// ------------------------------------
// Установка текста в буфер обмена
// ------------------------------------
void SetClipboardText(const wchar_t* text) {
    if (!OpenClipboard(nullptr))
        return;

    EmptyClipboard();

    size_t size = (wcslen(text) + 1) * sizeof(wchar_t);
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, size);

    memcpy(GlobalLock(mem), text, size);
    GlobalUnlock(mem);

    SetClipboardData(CF_UNICODETEXT, mem);
    CloseClipboard();
}
