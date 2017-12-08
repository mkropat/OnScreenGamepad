#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <system_error>

#include "resource.h"

void createMainWindow(HINSTANCE instance, int initialWindowState);
INT_PTR CALLBACK dialogProc(HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam);

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE _, LPWSTR commandLineArgs, int initialWindowState)
{
    try {
        createMainWindow(instance, initialWindowState);

        MSG msg;

        while (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return (int)msg.wParam;
    }
    catch (std::system_error& ex)
    {
        std::string s = std::string(ex.what());
        std::wstring ws(s.begin(), s.end());
        MessageBoxW(nullptr, ws.c_str(), L"On-Screen Gamepad crashed :(", MB_OK);

        return -1;
    }
}

void createMainWindow(HINSTANCE instance, int initialWindowState)
{
    HWND dialogHandle = CreateDialogW(instance, MAKEINTRESOURCE(IDD_GAMEPAD), 0, dialogProc);
    if (!dialogHandle)
    {
        std::error_code ec(GetLastError(), std::system_category());
        throw std::system_error(ec, "InitInstance");
    }

    ShowWindow(dialogHandle, initialWindowState);
}

INT_PTR CALLBACK dialogProc(HWND hwnd,  
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_LEFT:
            break;
        case IDC_RIGHT:
            break; 
        }
        return TRUE;

    case WM_DESTROY:
        PostQuitMessage(0);
        return TRUE;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return TRUE;
    }

    return FALSE;
}
