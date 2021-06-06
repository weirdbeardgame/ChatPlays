#include "control.h"

bool Control::CreateController()
{
    // Z
    init = new input_absinfo();
    init->flat = 63;
    init->fuzz = 3;
    init->maximum = 1023;
    init->minimum = 0;
    abs.push_back(init);

    // DPad X, Y
    init = new input_absinfo();
    init->value = 0;
    init->maximum = 1;
    init->minimum = -1;
    abs.push_back(init);

    init = new input_absinfo();
    init->value = 0;
    init->maximum = 1;
    init->minimum = -1;
    abs.push_back(init);

    dev = libevdev_new();
    libevdev_set_name(dev, "8BitDo SN30 Pro+");
    libevdev_enable_event_type(dev, EV_ABS);
    libevdev_enable_event_type(dev, EV_KEY);
    int type = buttonCodes[0];
    for (int i = 0; i < buttonCodes.size(); i++)
    {
        if (i <= 8)
        {
            libevdev_enable_event_code(dev, EV_ABS, buttonCodes[i], abs[i]);
        }
        else if (i >= 8)
        {
            libevdev_enable_event_code(dev, EV_KEY, buttonCodes[i], NULL);
        }
    }
    fd = open("/dev/uinput", O_RDWR);
    int rc = libevdev_uinput_create_from_device(dev, fd, &uidev);
    if (rc < 0)
    {
        std::cout << "ERR Bad FD: " << fd << " : " << strerror(errno) << std::endl;
        return false;
    }
    else 
    {
        return true;
    }
}

bool Control::emit(std::string keyCode)
{
    int emitCode = 0;
    if (keyCode == "UP")
    {
        // Dpad Up
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_Y, 65535);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        sleep (1);
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_Y, 4095);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    }

    if (keyCode == "DOWN")
    {
        // Dpad Down
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_Y, 0);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        sleep (1);
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_Y, 4095);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    }

    if (keyCode == "LEFT")
    {
        // Dpad Left
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_X, 0);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        sleep (1);
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_X, 4095);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    }

    if (keyCode == "RIGHT")
    {
        // Dpad Right
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_X, 65535);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        sleep (1);
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_X, 4095);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    }

    if (keyCode == "A")
    {
        emitCode = libevdev_uinput_write_event(uidev, EV_KEY, BTN_SOUTH, 1);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        sleep (1);
        emitCode = libevdev_uinput_write_event(uidev, EV_KEY, BTN_SOUTH, 0);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    }

    if (keyCode == "B")
    {
        emitCode = libevdev_uinput_write_event(uidev, EV_KEY, BTN_EAST, 1);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        sleep (1);
        emitCode = libevdev_uinput_write_event(uidev, EV_KEY, BTN_EAST, 0);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    }

    if (keyCode == "START")
    {
        std::cout << "START PRESSED" << std::endl;
        emitCode = libevdev_uinput_write_event(uidev, EV_KEY, BTN_TR, 1);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        sleep (1);
        emitCode = libevdev_uinput_write_event(uidev, EV_KEY, BTN_TR, 0);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    }

    if (keyCode == "Exit")
    {
        emitCode = Close();
        return emitCode;
    }
    if (emitCode != 0)
    {
        std::cout << "Code: " << emitCode << std::endl;
    }
    return emitCode;
}

bool Control::Close()
{
    libevdev_uinput_destroy(uidev);
    libevdev_free(dev);
    return 0;
}
