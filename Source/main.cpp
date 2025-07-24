#include "main.hpp"

HHOOK g_hook = nullptr;
HWND g_hwnd = nullptr;
HWND g_test = nullptr;

void SwitchCaps::SwitchLanguage(void)
{
    keybd_event(VK_MENU, 0, 0, 0);
    keybd_event(VK_SHIFT, 0, 0, 0);
    keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
}

void SwitchCaps::ToggleCapsLock(void)
{
    keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}

void SwitchCaps::CreateDummyWindow(HINSTANCE hInstance)
{
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

    if (g_test != nullptr)
    {
        ShowWindow(g_test, SW_HIDE);
    }
}

void SwitchCaps::DestroyDummyWindow(void)
{
    if (g_test != nullptr)
    {
        DestroyWindow(g_test);
        g_test = nullptr;
    }
}

bool SwitchCaps::RegisterMainWindowClass(HINSTANCE hInstance)
{
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_NAME;

    ATOM atom = RegisterClassA(&wc);
    return atom != 0;
}

static SwitchCaps sw;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        KBDLLHOOKSTRUCT *p = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);
        if (wParam == WM_KEYDOWN && p->vkCode == VK_CAPITAL)
        {
            SHORT shiftState = GetAsyncKeyState(VK_SHIFT);
            if (shiftState & 0x8000)
            {
                sw.ToggleCapsLock();
                return 1;
            }
            else
            {
                sw.SwitchLanguage();
                return 1;
            }
        }
    }
    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_COPYDATA)
    {
        if (g_test == nullptr)
            sw.CreateDummyWindow(GetModuleHandle(NULL));

        // COPYDATASTRUCT *cds = reinterpret_cast<COPYDATASTRUCT *>(lParam);

        int result = MessageBoxW(g_test, L"SwitchCaps is already running.\nDo you want to close it?", L"SwitchCaps", MB_ICONQUESTION | MB_YESNO | MB_TOPMOST);
        if (result == IDYES)
        {
            PostQuitMessage(0);
        }
        return 1;
    }

    switch (msg)
    {
    case WM_DESTROY:
    {
        sw.DestroyDummyWindow();
        PostQuitMessage(0);
        return 0;
    }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    HWND other = FindWindowA(WINDOW_NAME, NULL);
    if (other)
    {
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

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hook)
    {
        UnhookWindowsHookEx(g_hook);
        g_hook = nullptr;
    }

    return 0;
}