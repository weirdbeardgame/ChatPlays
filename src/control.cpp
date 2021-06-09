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
        if (i <= 7)
        {
            libevdev_enable_event_code(dev, EV_ABS, buttonCodes[i], abs[i]);
        }
        else if (i > 7)
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

int Control::pressBtn(int button)
{
    int emitCode = 0;

    emitCode = libevdev_uinput_write_event(uidev, EV_KEY, button, 1);
    if (emitCode < 0)
    {
        std::cerr << "PRESS ERROR: " << strerror(errno) << std::endl;
        return -1;
    }

    emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    if (emitCode < 0)
    {
        std::cerr << "PRESS ERROR: " << strerror(errno) << std::endl;
        return -1;
    }

    sleep(1);
    releaseBtn(button);

    return emitCode;
}

int Control::releaseBtn(int button)
{
    int emitCode = 0;
    emitCode = libevdev_uinput_write_event(uidev, EV_KEY, button, 0);
    if (emitCode < 0)
    {
        std::cerr << "PRESS ERROR: " << strerror(errno) << std::endl;
        return -1;
    }
    emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    if (emitCode < 0)
    {
        std::cerr << "PRESS ERROR: " << strerror(errno) << std::endl;
        return -1;
    }
    return emitCode;
}

int Control::moveABS(int ABS, int moveAxis, int flat)
{
    int emitCode = 0;
    emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS, moveAxis);

    if (emitCode < 0)
    {
        std::cerr << "PRESS ERROR: " << strerror(errno) << std::endl;
        return -1;
    }

    emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);

    if (emitCode < 0)
    {
        std::cerr << "PRESS ERROR: " << strerror(errno) << std::endl;
        return -1;
    }

    sleep(1);
    resetABS(ABS, flat);
    return emitCode;
}

int Control::resetABS(int ABS, int flat)
{
    int emitCode = 0;
    emitCode = libevdev_uinput_write_event(uidev, EV_ABS, ABS, flat);

    if (emitCode < 0)
    {
        std::cerr << "PRESS ERROR: " << strerror(errno) << std::endl;
        return -1;
    }

    emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);

    if (emitCode < 0)
    {
        std::cerr << "PRESS ERROR: " << strerror(errno) << std::endl;
        return -1;
    }

    return emitCode;
}

Buttons Control::GetCommands(std::string key)
{
    if (commands.find(key) != commands.end())
    {
        std::cout << "Command found" << std::endl;
        return commands[key];
    }
    else
    {
        return Buttons(-1);
    }
}


bool Control::emit(Buttons keyCode)
{
    int emitCode = 0;
    switch(keyCode)
    {
    case Buttons::UP:
        emitCode = moveABS(ABS_Y, 65535, 4095);
        break;
    case Buttons::DOWN:
        // Dpad Down
        emitCode = moveABS(ABS_Y, 0, 4095);
        break;

    case Buttons::LEFT:
        // Dpad Left
        emitCode = moveABS(ABS_X, 0, 4095);
        break;

    case Buttons::RIGHT:
        // Dpad Right
        emitCode = moveABS(ABS_X, 65535, 4095);
        break;

    case Buttons::A:
        emitCode = pressBtn(BTN_SOUTH);
        break;

    case Buttons::B:
        emitCode = pressBtn(BTN_EAST);
        break;

    case Buttons::X:
        emitCode = pressBtn(BTN_WEST);
        break;

    case Buttons::Y:
        emitCode = pressBtn(BTN_NORTH);
        break;

    case Buttons::START:
        emitCode = pressBtn(BTN_TR);
        break;

    case Buttons::SELECT:
        emitCode = pressBtn(BTN_TL);
        break;

    case Buttons::EXIT: 
        emitCode = Close();
        return emitCode;
        break;
    }
    return emitCode;
}

bool Control::Close()
{
    libevdev_uinput_destroy(uidev);
    libevdev_free(dev);
    return 0;
}
