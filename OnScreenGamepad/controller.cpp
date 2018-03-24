#include "controller.h"

std::map<PVIGEM_CLIENT, Controller*> Controller::_clientToGampead;

void Controller::added_callback(PVIGEM_CLIENT client, PVIGEM_TARGET target, VIGEM_ERROR error)
{
    _clientToGampead.at(client)->added(client, target, error);
}

void Controller::added(PVIGEM_CLIENT client, PVIGEM_TARGET target, VIGEM_ERROR error)
{
    if (!VIGEM_SUCCESS(error))
        _onFatalError(AppException::ControllerException, error);
}

void Controller::connect()
{
    auto error = vigem_connect(_client);
    if (!VIGEM_SUCCESS(error))
    {
        throw ControllerException(error);
    }

    vigem_target_add_async(_client, _target, added_callback);
}

void Controller::keyDown(USHORT button)
{
    _report.wButtons |= button;

    //DS4_REPORT report = { 0 };
    //DS4_REPORT_INIT(&report);
    //report.wButtons = _buttons;
    
    auto result = vigem_target_ds4_update(_client, _target, _report);
    if (!VIGEM_SUCCESS(result))
        throw ControllerException(result);
}

void Controller::keyUp(USHORT button)
{
    _report.wButtons &= ~button;

    //DS4_REPORT report = { 0 };
    //DS4_REPORT_INIT(&report);
    //report.wButtons = _buttons;

    auto result = vigem_target_ds4_update(_client, _target, _report);
    if (!VIGEM_SUCCESS(result))
        throw ControllerException(result);
}

const char* ControllerException::what() const
{
    switch (_error)
    {
    case VIGEM_ERROR_BUS_NOT_FOUND:
        return "Controller subsytem: Bus not found";

    case VIGEM_ERROR_NO_FREE_SLOT:
        return "Controller subsytem: No free slot";

    case VIGEM_ERROR_INVALID_TARGET:
        return "Controller subsytem: Invalid target";

    case VIGEM_ERROR_REMOVAL_FAILED:
        return "Controller subsytem: Removal failed";

    case VIGEM_ERROR_ALREADY_CONNECTED:
        return "Controller subsytem: Already connected";

    case VIGEM_ERROR_TARGET_UNINITIALIZED:
        return "Controller subsytem: Target uninitialized";

    case VIGEM_ERROR_TARGET_NOT_PLUGGED_IN:
        return "Controller subsytem: Not plugged in";

    case VIGEM_ERROR_BUS_VERSION_MISMATCH:
        return "Controller subsytem: Bus version mismatch";

    case VIGEM_ERROR_BUS_ACCESS_FAILED:
        return "Controller subsytem: Bus access failed";

    case VIGEM_ERROR_CALLBACK_ALREADY_REGISTERED:
        return "Controller subsytem: Callback already registered";

    case VIGEM_ERROR_CALLBACK_NOT_FOUND:
        return "Controller subsytem: Callback not found";

    case VIGEM_ERROR_BUS_ALREADY_CONNECTED:
        return "Controller subsytem: Bus already connected";

    default:
        return "Controller subsytem: Unknown error";
    }
}
