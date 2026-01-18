#ifndef SWITCHCAPS_HPP
#define SWITCHCAPS_HPP

#include <Windows.h>

#define WM_TRAYICON (WM_USER + 1)

extern bool capslock_var;
extern bool g_bypassHook;

struct SwitchCaps {
    void SwitchLanguage(void);
    void ToggleCapsLock(void);
    void CreateDummyWindow(HINSTANCE hInstance);
    void DestroyDummyWindow(void);
    bool RegisterMainWindowClass(HINSTANCE hInstance);
    void AddTrayIcon(HWND hwnd);
    void RemoveTrayIcon(HWND hwnd);
};

#endif
