#include <poll.h>
#include <chrono>
#include <functional>
#include <condition_variable>
#include "Linux/Uinput.h"

Emit::Emit()
{
    memset(&usetup, 0, sizeof(usetup));
    memset(&deviceID, 0, sizeof(deviceID));

    deviceID.bustype = 0x3;
    deviceID.vendor = 0x45e;
    deviceID.product = 0x28e;
    deviceID.version = 0x114;
    usetup.id = deviceID;

    memcpy(usetup.name, deviceName, UINPUT_MAX_NAME_SIZE);
}

bool Emit::AttachController()
{
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0)
    {
        printf("Open Error! \n");
        isActive = false;
    }
    else
    {
        if (ioctl(fd, UI_SET_EVBIT, EV_ABS) < 0)
        {
            printf("IoCtrl EVBit Error\n");
            return false;
        }
        if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
        {
            printf("IoCtrl EVBit Error\n");
            return false;
        }

        for (int i = 0; i < buttonCodes.size(); i++)
        {
            if (ioctl(fd, UI_SET_KEYBIT, buttonCodes[Buttons(i)]) < 0)
            {
                printf("IoCtrl KEY Error\n");
                return false;
            }
        }

        if (ioctl(fd, UI_DEV_SETUP, &usetup) < 0)
        {
            std::cout << "Device setup error: " << fd << " : " << strerror(errno) << std::endl;
            isActive = false;
            return false;
        }

        if (ioctl(fd, UI_DEV_CREATE) < 0)
        {
            std::cout << "ERR Bad FD: " << fd << " : " << strerror(errno) << std::endl;
            isActive = false;
            return false;
        }
        sleep(1);
        isActive = true;
        controllerID = 1;
    }
    return isActive;
}

void Emit::DisconnectController()
{
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}

int Emit::PressButton(Buttons button)
{
    input_event input;
    input.type = EV_KEY;
    input.code = buttonCodes[button];
    input.value = 1;
    input.time.tv_sec = 0;
    input.time.tv_usec = 0;
    if (write(fd, &input, sizeof(input)) < 0)
    {
        std::cout << "Input Write Error: " << strerror(errno) << std::endl;
        return -1;
    }
    memset(&input, 0, sizeof(input));

    input.type = EV_SYN;
    input.code = SYN_REPORT;
    input.value = 0;
    input.time.tv_sec = 0;
    input.time.tv_usec = 0;
    if (write(fd, &input, sizeof(input)) < 0)
    {
        std::cout << "Input Write Error: " << strerror(errno) << std::endl;
        return -1;
    }
    return 0;
}

int Emit::ReleaseButton(Buttons button)
{
    input_event input;
    input.type = EV_KEY;
    input.code = buttonCodes[button];
    input.value = 0;
    input.time.tv_sec = 0;
    input.time.tv_usec = 0;
    if (write(fd, &input, sizeof(input)) < 0)
    {
        std::cout << "Input Write Error: " << strerror(errno) << std::endl;
        return -1;
    }
    input.type = EV_SYN;
    input.code = SYN_REPORT;
    input.value = 0;
    input.time.tv_sec = 0;
    input.time.tv_usec = 0;
    if (write(fd, &input, sizeof(input)) < 0)
    {
        std::cout << "Input Write Error: " << strerror(errno) << std::endl;
        return -1;
    }

    return 0;
}

int Emit::MoveABS(ABS abs, float moveAxis)
{
    input_event input;
    input.type = EV_ABS;
    input.code = Abs[abs];
    input.value = moveAxis;
    input.time.tv_sec = 0;
    input.time.tv_usec = 0;
    if (write(fd, &input, sizeof(input)) < 0)
    {
        std::cout << "Input Write Error: " << strerror(errno) << std::endl;
        return -1;
    }
    memset(&input, 0, sizeof(input));

    input.type = EV_SYN;
    input.code = SYN_REPORT;
    input.value = 0;
    input.time.tv_sec = 0;
    input.time.tv_usec = 0;
    if (write(fd, &input, sizeof(input)) < 0)
    {
        std::cout << "Input Write Error: " << strerror(errno) << std::endl;
        return -1;
    }

    return 0;
}

int Emit::ResetABS(ABS abs)
{
    input_event input;
    input.type = EV_ABS;
    input.code = Abs[abs];
    input.value = 0;
    input.time.tv_sec = 0;
    input.time.tv_usec = 0;
    if (write(fd, &input, sizeof(input)) < 0)
    {
        std::cout << "Input Write Error: " << strerror(errno) << std::endl;
        return -1;
    }
    memset(&input, 0, sizeof(input));

    input.type = EV_SYN;
    input.code = SYN_REPORT;
    input.value = 0;
    input.time.tv_sec = 0;
    input.time.tv_usec = 0;
    if (write(fd, &input, sizeof(input)) < 0)
    {
        std::cout << "Input Write Error: " << strerror(errno) << std::endl;
        return -1;
    }
    return 0;
}

bool Emit::SendInput(Buttons btn, ABS abs, float axis, int heldFor)
{
    int emitCode = 0;

    if (btn != Buttons::B_CLEAR)
    {
        emitCode = PressButton(btn);
        sleep(heldFor);
        emitCode = ReleaseButton(btn);
    }
    if (abs != ABS::CLEAR)
    {
        emitCode = MoveABS(abs, axis);
        sleep(heldFor);
        emitCode = ResetABS(abs);
    }
    return emitCode;
}

Emit::~Emit()
{
    DisconnectController();
}