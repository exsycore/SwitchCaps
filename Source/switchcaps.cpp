#include "switchcaps.hpp"
#include "main.hpp"
#include "../resource.h"

bool capslock_var = false;
bool g_bypassHook = false;

void SwitchCaps::SwitchLanguage(void) {
    keybd_event(VK_MENU, 0, 0, 0);
    keybd_event(VK_SHIFT, 0, 0, 0);
    keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
}

void SwitchCaps::ToggleCapsLock(void) {
    capslock_var = !capslock_var;
    keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}

void SwitchCaps::CreateDummyWindow(HINSTANCE hInstance) {
    if (g_test != nullptr)
        return;

    g_test = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        L"STATIC",
        NULL,
        WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT,
        0, 0,
        NULL, NULL, hInstance, NULL);

    if (g_test != nullptr) {
        ShowWindow(g_test, SW_HIDE);
    }
}

void SwitchCaps::DestroyDummyWindow(void) {
    if (g_test != nullptr) {
        DestroyWindow(g_test);
        g_test = nullptr;
    }
}

bool SwitchCaps::RegisterMainWindowClass(HINSTANCE hInstance) {
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_NAME;

    ATOM atom = RegisterClassA(&wc);
    return atom != 0;
}

void SwitchCaps::AddTrayIcon(HWND hwnd) {
    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = (HICON)LoadImageW(
        GetModuleHandleW(nullptr),
        MAKEINTRESOURCEW(IDI_ICON1),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON),
        LR_DEFAULTCOLOR
    );
    wcscpy_s(nid.szTip, L"SwitchCaps");

    Shell_NotifyIconW(NIM_ADD, &nid);
}

void SwitchCaps::RemoveTrayIcon(HWND hwnd) {
    NOTIFYICONDATA nid = {0};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;

    Shell_NotifyIcon(NIM_DELETE, &nid);
}