#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <functional>
#include <iostream>
#include <system_error>

#include "resource.h"
#include "controller.h"

HWND createMainWindow(HINSTANCE instance, int initialWindowState);
INT_PTR CALLBACK dialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void displayException(std::exception& ex);
BOOL handleDialogMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
void handleFatalError(HWND mainWindow, AppException exception, long data);

static Controller* _controller;

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE _, LPWSTR commandLineArgs, int initialWindowState)
{
    try {
        auto mainWindow = createMainWindow(instance, initialWindowState);

        auto handleError = std::bind(handleFatalError, mainWindow, std::placeholders::_1, std::placeholders::_2);

        auto controller = Controller(handleError);
        _controller = &controller;
        controller.connect();

        MSG msg;

        while (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return static_cast<int>(msg.wParam);
    }
    catch (std::exception& ex)
    {
        displayException(ex);
        return -1;
    }
}

HWND createMainWindow(HINSTANCE instance, int initialWindowState)
{
    auto dialogHandle = CreateDialogW(instance, MAKEINTRESOURCE(IDD_GAMEPAD), 0, dialogProc);
    if (!dialogHandle)
    {
        auto ec = std::error_code(GetLastError(), std::system_category());
        throw std::system_error(ec, "InitInstance");
    }

    ShowWindow(dialogHandle, initialWindowState);

    return dialogHandle;
}

INT_PTR CALLBACK dialogProc(HWND window,  
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    try
    {
        return handleDialogMessage(window, message, wParam, lParam);
    }
    catch (std::exception& ex)
    {
        displayException(ex);
        ExitProcess(-1);
    }
}

static WNDPROC _originalHandleButtonMessage;

LRESULT CALLBACK handleButtonMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    try
    {
        switch (message)
        {
        case WM_LBUTTONDOWN:
            std::cout << "button down" << std::endl;
            _controller->keyDown(XUSB_GAMEPAD_A);
            break;

        case WM_LBUTTONUP:
            std::cout << "button down" << std::endl;
            _controller->keyUp(XUSB_GAMEPAD_A);
            break;

        }
    }
    catch (std::exception& ex)
    {
        displayException(ex);
        ExitProcess(-1);
    }

    return CallWindowProcW(_originalHandleButtonMessage, window, message, wParam, lParam);
}


BOOL handleDialogMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        auto leftButton = GetDlgItem(window, IDC_LEFT);
        auto orig = SetWindowLongPtrW(leftButton, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(handleButtonMessage));
        _originalHandleButtonMessage = reinterpret_cast<WNDPROC>(orig);
        //_controller->connect();
        return TRUE;

    }
    case WM_COMMAND:
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
            switch (LOWORD(wParam)) {
            case IDC_LEFT:
                break;
            case IDC_RIGHT:
                break;
            }
        case BN_PUSHED:
            std::cout << "pushed" << std::endl;
            break;
        case BN_UNPUSHED:
            std::cout << "unpushed" << std::endl;
            break;
        }
        
        return TRUE;

    case static_cast<UINT>(AppMessage::FatalException):
        switch (static_cast<AppException>(wParam))
        {
        case AppException::ControllerException:
            throw ControllerException(static_cast<VIGEM_ERROR>(lParam));
        default:
            throw std::exception("Unknown fatal exception message");
        }

    case WM_DESTROY:
        PostQuitMessage(0);
        return TRUE;

    case WM_CLOSE:
    {
        auto leftButton = GetDlgItem(window, IDC_LEFT);
        SetWindowLongPtrW(leftButton, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(_originalHandleButtonMessage));

        DestroyWindow(window);
        return TRUE;
    }
        
    }

    return FALSE;
}

void handleFatalError(HWND mainWindow, AppException exception, long data)
{
    PostMessageW(mainWindow, static_cast<UINT>(AppMessage::FatalException), static_cast<WPARAM>(exception), data);
}

void displayException(std::exception& ex)
{
    auto s = std::string(ex.what());
    auto ws = std::wstring(s.begin(), s.end());
    MessageBoxW(nullptr, ws.c_str(), L"On-Screen Gamepad crashed :(", MB_OK);
}
