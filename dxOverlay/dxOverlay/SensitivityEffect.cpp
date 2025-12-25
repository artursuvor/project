#include "SensitivityEffect.h"
#include "InputSequencer.h"

// Значение, к которому возвращаемся
static const wchar_t* ORIGINAL_SENSITIVITY = L"sensitivity 0.2";

void StartSensitivityEffect() {
    // кладём команду в буфер
    SetClipboardText(L"sensitivity 0.0");

    // открыть консоль
    PressF7();

    // вставить команду
    PressCtrlV();

    // подтвердить
    PressEnter();

    // закрыть консоль
    PressF7();
}

void StopSensitivityEffect() {
    // вернуть старое значение
    SetClipboardText(L"sensitivity 0.2");

    PressF7();
    PressCtrlV();
    PressEnter();
    PressF7();
}
