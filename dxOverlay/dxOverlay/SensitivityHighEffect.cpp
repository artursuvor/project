#include "SensitivityHighEffect.h"
#include "InputSequencer.h"

// ⚠️ должно совпадать с тем, что реально стоит в игре
static const wchar_t* ORIGINAL_SENSITIVITY = L"sensitivity 0.2";

void StartSensitivityHighEffect() {

    SetClipboardText(L"sensitivity 7.0");

    PressF7();
    PressCtrlV();
    PressEnter();
    PressF7();
}

void StopSensitivityHighEffect() {

    SetClipboardText(ORIGINAL_SENSITIVITY);

    PressF7();
    PressCtrlV();
    PressEnter();
    PressF7();
}
