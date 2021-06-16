#include "control.h"

ControlInfo::ControlInfo()
{
    control = nlohmann::json
    {
        "ControlInfo" , 
        {
            {"commands", commands},
            {"controllerName", controller.controllerName},
            {"configuredControls", controller.buttonCodes}
        }
    };
}

ControlInfo::ControlInfo(json j)
{
    from_json(j, *this);
}

void to_json(json& j, const ControlInfo& c)
{
    j = json
    {
        "ControlInfo" , 
        {
            {"commands", c.commands},
            {"controllerName", c.controller.controllerName},
            {"configuredControls", c.controller.buttonCodes}
        }
    };
}

void from_json(const json& j, ControlInfo& c)
{
    j.at("commands").get_to(c.commands);
    j.at("controllerName").get_to(c.controller.controllerName);
    j.at("configuredControls").get_to(c.controller.buttonCodes);
}

void ControlInfo::save(json &j, bool isDefault)
{
    if (isDefault)
    {
        ControlInfo c = ControlInfo();
        j.push_back(c.control);
    }
    else
    {
        j.push_back(control);
    }
}

void ControlInfo::config()
{
    std::vector<Controller> controlSelect;
    // List devices and select one to save.
    for (auto &entry: fs::directory_iterator("/dev/input"))
    {
        fs::path temp = entry.path();
        Controller createControl;
        if (temp.string().find("event"))
        {
            createControl.fd = open(temp.c_str(), O_RDWR);
            int err = libevdev_new_from_fd(createControl.fd, &createControl.dev);
            if (err < 0)
            {
                // If it's just a bad file descriptor, don't bother logging, but otherwise, log it.
                if (err != -9)
                {
                    printf("Failed to connect to device at %s, the error was: %s", entry.path(), strerror(-err));
                    libevdev_free(createControl.dev);
                    close(createControl.fd);
                }
            }
            if (libevdev_has_event_type(createControl.dev, EV_KEY) && libevdev_has_event_type(createControl.dev, EV_ABS))
            {
                createControl.controllerName = libevdev_get_name(createControl.dev);
                controlSelect.push_back(createControl);
            }
        }
    }

    bool isConfig = true;
    int i = 0;
    int j = 0;
    while(isConfig)
    {
        for (auto& entry: controlSelect)
        {
            std::cout << "Avalible Controllers: " << std::endl << i + ": " << entry.controllerName << std::endl;
            i += 1;
        }
        std::cout << "> ";
        std::cin >> j;

        // Does evDev have a method for detecting the currently selected ABS codes? Each controller will be different!
        controller = controlSelect[j];
        const input_absinfo* absX = libevdev_get_abs_info(controller.dev, ABS_X);
        const input_absinfo* absY = libevdev_get_abs_info(controller.dev, ABS_Y);

    }

    // Use evdev to grab controller info. Map controls to commands.
    controller.controllerName = libevdev_get_name(controller.dev);
}

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
