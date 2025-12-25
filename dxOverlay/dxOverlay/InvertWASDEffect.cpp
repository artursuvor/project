#include "InvertWASDEffect.h"

static HHOOK g_keyboardHook = nullptr;
static bool g_active = false;

// ------------------------------------
// Low-level keyboard hook
// ------------------------------------
LRESULT CALLBACK KeyboardProc(
    int nCode,
    WPARAM wParam,
    LPARAM lParam
) {
    if (nCode == HC_ACTION && g_active) {

        KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;

        bool keyDown =
            (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool keyUp =
            (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        if (keyDown || keyUp) {

            WORD newKey = 0;

            switch (kbd->vkCode) {
            case 'W': newKey = 'S'; break;
            case 'S': newKey = 'W'; break;
            case 'A': newKey = 'D'; break;
            case 'D': newKey = 'A'; break;
            }

            if (newKey != 0) {
                INPUT input{};
                input.type = INPUT_KEYBOARD;
                input.ki.wVk = newKey;
                if (keyUp)
                    input.ki.dwFlags = KEYEVENTF_KEYUP;

                SendInput(1, &input, sizeof(INPUT));
                return 1; // ❌ блокируем оригинальную клавишу
            }
        }
    }

    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

// ------------------------------------
// Start / Stop
// ------------------------------------
void StartInvertWASD() {
    if (g_keyboardHook)
        return;

    g_active = true;

    g_keyboardHook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        KeyboardProc,
        GetModuleHandle(nullptr),
        0
    );
}

void StopInvertWASD() {
    g_active = false;

    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
}
