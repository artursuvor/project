#include "FpsEffect.h"
#include "InputSequencer.h"

// что считаем "нормой"
static const wchar_t* FPS_NORMAL = L"fps_max 128";

void StartFpsLowEffect() {

    // ставим низкий FPS
    SetClipboardText(L"fps_max 16");

    PressF7();      // открыть консоль
    PressCtrlV();   // вставить
    PressEnter();   // выполнить
    PressF7();      // закрыть
}

void StopFpsLowEffect() {

    // возвращаем нормальный FPS
    SetClipboardText(FPS_NORMAL);

    PressF7();
    PressCtrlV();
    PressEnter();
    PressF7();
}
