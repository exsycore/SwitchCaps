#ifndef MAIN_HPP
#define MAIN_HPP

#include <Windows.h>

#define WINDOW_NAME "SwitchCaps"

extern HHOOK g_hook;
extern HWND g_hwnd;
extern HWND g_test;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int);

#endif