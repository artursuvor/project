#include "InvertMouseEffect.h"
#include "InputSequencer.h"

// 🔧 значения подставь реальные из игры
static const wchar_t* INVERT_ON = L"mouse_inverty true";
static const wchar_t* INVERT_OFF = L"mouse_inverty false";

void StartInvertMouseEffect() {

    // кладём команду в буфер
    SetClipboardText(INVERT_ON);

    // открыть консоль
    PressF7();

    // вставить команду
    PressCtrlV();

    // подтвердить
    PressEnter();

    // закрыть консоль
    PressF7();
}

void StopInvertMouseEffect() {

    // вернуть обычную мышь
    SetClipboardText(INVERT_OFF);

    PressF7();
    PressCtrlV();
    PressEnter();
    PressF7();
}
