#pragma once
#include <Windows.h>

// низкоуровневый ввод
void SendKey(WORD vk, bool down);
void SendKeyPress(WORD vk);

// часто используемые комбинации
void PressCtrlV();
void PressEnter();
void PressF7();

// буфер обмена
void SetClipboardText(const wchar_t* text);
