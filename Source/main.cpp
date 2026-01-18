#include "main.hpp"

HHOOK g_hook = nullptr;
HWND g_hwnd = nullptr;
HWND g_test = nullptr;

#include "../resource.h"
#include "switchcaps.hpp"

static SwitchCaps sw;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (g_bypassHook)
        return CallNextHookEx(g_hook, nCode, wParam, lParam);

    if (nCode == HC_ACTION) {
        auto* p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        if (wParam == WM_KEYDOWN && p->vkCode == VK_CAPITAL) {
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                sw.ToggleCapsLock();
            } else {
                sw.SwitchLanguage();
            }
            return 1;
        }
    }
    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COPYDATA) {
        if (g_test == nullptr)
            sw.CreateDummyWindow(GetModuleHandle(NULL));

        int result = MessageBoxW(g_test, L"SwitchCaps is already running.\nDo you want to close it?", L"SwitchCaps", MB_ICONQUESTION | MB_YESNO | MB_TOPMOST);
        if (result == IDYES) {
            PostQuitMessage(0);
        }
        return 1;
    }

    switch (msg) {
        case WM_DESTROY: {
            sw.RemoveTrayIcon(hwnd);
            sw.DestroyDummyWindow();
            PostQuitMessage(0);
            return 0;
        }
        
        case WM_TRAYICON: {
            if (lParam == WM_LBUTTONUP)
                MessageBoxW(hwnd, L"SwitchCaps running", L"SwitchCaps", MB_ICONQUESTION | MB_OK);
            else if (lParam == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);

                HMENU hMenu = CreatePopupMenu();
                AppendMenuW(hMenu, MF_STRING, IDM_SWITCH, L"Switch language");

                if (!capslock_var) AppendMenuW(hMenu, MF_STRING, IDM_CAPSLOCK, L"Toggle CAPSLOCK");
                else AppendMenuW(hMenu, MF_STRING, IDM_CAPSLOCK, L"UnToggle CAPSLOCK");

                AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
                AppendMenuW(hMenu, MF_STRING, IDM_EXIT,   L"Exit");

                SetForegroundWindow(hwnd);

                TrackPopupMenu(
                    hMenu,
                    TPM_RIGHTBUTTON | TPM_BOTTOMALIGN,
                    pt.x,
                    pt.y,
                    0,
                    hwnd,
                    nullptr
                );

                DestroyMenu(hMenu);
                // PostQuitMessage(0);
            }
            return 0;
        }

        case WM_COMMAND: {
            g_bypassHook = true;
            switch (LOWORD(wParam)) {
                case IDM_SWITCH: {
                    sw.SwitchLanguage();
                    break;
                }
                case IDM_CAPSLOCK: {
                    // capslock_var = !capslock_var;
                    sw.ToggleCapsLock();
                    break;
                }

                case IDM_EXIT: {
                    PostQuitMessage(0);
                    break;
                }
            }
            g_bypassHook = false;
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    HWND other = FindWindowA(WINDOW_NAME, NULL);
    if (other) {
        COPYDATASTRUCT cds = {0};
        SendMessage(other, WM_COPYDATA, 0, (LPARAM)&cds);
        return 0;
    }

    if (!sw.RegisterMainWindowClass(hInstance))
        return 1;

    g_hwnd = CreateWindowA(WINDOW_NAME, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
    if (!g_hwnd)
        return 1;

    g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (!g_hook)
        return 1;

    sw.AddTrayIcon(g_hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hook) {
        UnhookWindowsHookEx(g_hook);
        g_hook = nullptr;
    }
    return 0;
}