#ifndef MAIN_HPP
#define MAIN_HPP

#include <Windows.h>

#define WINDOW_NAME "SwitchCaps"

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int);

struct SwitchCaps {
    void SwitchLanguage(void);
    void ToggleCapsLock(void);
    void CreateDummyWindow(HINSTANCE hInstance);
    void DestroyDummyWindow(void);
    bool RegisterMainWindowClass(HINSTANCE hInstance);
};

#endif