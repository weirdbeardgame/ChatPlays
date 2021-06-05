#include "control.h"

bool Control::init()
{
    dev = libevdev_new();
    libevdev_set_name(dev, "8BitDo SN30 Pro+");
    // Joystick L, R
    libevdev_enable_event_type(dev, EV_ABS);
    // If type is EV_ABS, data points to a struct input_absinfo.
    // Left Stick
    libevdev_enable_event_code(dev, EV_ABS, ABS_X, abs);
    libevdev_enable_event_code(dev, EV_ABS, ABS_Y, abs);
    // Right Stick
    libevdev_enable_event_code(dev, EV_ABS, ABS_RX, abs);
    libevdev_enable_event_code(dev, EV_ABS, ABS_RY, abs);
    // R2
    libevdev_enable_event_code(dev, EV_ABS, ABS_RZ, abs);
    // L2
    libevdev_enable_event_code(dev, EV_ABS, ABS_Z, abs);
    // DpadX
    libevdev_enable_event_code(dev, EV_ABS, ABS_HAT0X, abs);
    // Dpad Y
    libevdev_enable_event_code(dev, EV_ABS, ABS_HAT0Y, abs);
    // Buttons
    libevdev_enable_event_type(dev, EV_KEY);
    // A or X
    libevdev_enable_event_code(dev, EV_KEY, BTN_SOUTH, NULL);
    // B or O
    libevdev_enable_event_code(dev, EV_KEY, BTN_EAST, NULL);
    // X or square
    libevdev_enable_event_code(dev, EV_KEY, BTN_C, NULL);
    // Y or Triangle
    libevdev_enable_event_code(dev, EV_KEY, BTN_NORTH, NULL);
    // L1
    libevdev_enable_event_code(dev, EV_KEY, BTN_WEST, NULL);
    // R1
    libevdev_enable_event_code(dev, EV_KEY, BTN_Z, NULL);
    // Start
    libevdev_enable_event_code(dev, EV_KEY, BTN_TR, NULL);
    // Select
    libevdev_enable_event_code(dev, EV_KEY, BTN_TL, NULL);
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
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_Y, 1);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        sleep (1);
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_Y, 0);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    }

    if (keyCode == "DOWN")
    {
        // Dpad Down
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_Y, -1);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        sleep (1);
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_Y, 0);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    }

    if (keyCode == "LEFT")
    {
        // Dpad Left
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_X, -1);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        sleep (1);
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_X, 0);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    }

    if (keyCode == "RIGHT")
    {
        // Dpad Right
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_X, 1);
        emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        sleep (1);
        emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS_X, 0);
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

    if (keyCode == "Start")
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
