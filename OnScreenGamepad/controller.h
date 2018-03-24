#pragma once

#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "ViGEmClient.lib")

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <exception>
#include <functional>
#include <map>
#include <string>
#include <ViGEmClient.h>

enum class AppMessage : UINT
{
    FatalException = WM_USER + 1,
};

enum class AppException : WPARAM
{
    ControllerException
};

class Controller
{
    static std::map<PVIGEM_CLIENT, Controller*> _clientToGampead;
    std::function<void(AppException, long)> _onFatalError;
    PVIGEM_CLIENT _client;
    PVIGEM_TARGET _target;
    USHORT _buttons;
    DS4_REPORT _report;

public:
    Controller(std::function<void(AppException, long)> onFatalError)
        : _buttons(0), _onFatalError(onFatalError)
    {
        _client = vigem_alloc();
        DS4_REPORT_INIT(&_report);
        _target = vigem_target_ds4_alloc();

        _clientToGampead.insert(std::pair<PVIGEM_CLIENT, Controller*>(_client, this));
    }

    void connect();

    void keyUp(USHORT button);
    void keyDown(USHORT button);

    ~Controller()
    {
        vigem_disconnect(_client);

        vigem_target_free(_target);
        vigem_free(_client);
    }

private:
    static void CALLBACK added_callback(PVIGEM_CLIENT client, PVIGEM_TARGET target, VIGEM_ERROR error);
    void added(PVIGEM_CLIENT client, PVIGEM_TARGET target, VIGEM_ERROR error);
};

class ControllerException : public std::exception
{
    VIGEM_ERROR _error; 
    std::string _message;

public:
    ControllerException(VIGEM_ERROR error) : _error(error)
    {
    }

    virtual const char* what() const throw();
};