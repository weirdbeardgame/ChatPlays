#include "control.h"

Controller::Controller()
{
    fd = 0;
    dev = nullptr;
    driverVersion = 0;
    uniqueID = std::string();
    controllerName = std::string();
}

Controller::Controller(const Controller& c)
{
    fd = c.fd;
    dev = c.dev;
    abs = c.abs;
    uniqueID = c.uniqueID;
    eventPath = c.eventPath;
    buttonCodes = c.buttonCodes;
    driverVersion = c.driverVersion;
    MappedControls = c.MappedControls;
    controllerName = c.controllerName;
}

ControlInfo::ControlInfo()
{
    control = json
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

void ControlInfo::initalConfig()
{
    // List devices and select one to save.
    std::vector<Controller> controlSelect(15);
    uint32_t* buttonList;
    for (auto &entry: fs::directory_iterator("/dev/input"))
    {
        Controller createControl = Controller();
        std::string temp = entry.path().filename();

        if (temp.compare(0, 5, "event") == 0)
        {
            std::cout << "Temp: " << temp << std::endl;
            createControl.fd = open(entry.path().c_str(), O_RDWR);
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
                else
                {
                    printf("Invalid FD \n");
                    close(createControl.fd);
                    return;
                }
            }
            if (createControl.dev != nullptr)
            {
                if (libevdev_has_event_type(createControl.dev, EV_KEY) && libevdev_has_event_type(createControl.dev, EV_ABS))
                {
                    createControl.eventPath = createControl.eventPath / temp;
                    createControl.controllerName = libevdev_get_name(createControl.dev);
                    controlSelect.push_back(createControl);
                }
            }
        }
        else
        {
            continue;
        }
    }

    bool polling = true;
    int j = -1;
    if (controlSelect.size() > 0)
    {
        for (int i = 0; i < controlSelect.size(); i++)
        {
            std::cout << "Avalible Controllers " << i << ": " <<  controlSelect[i].controllerName << std::endl;
        }

        if (j < 0)
        {
            std::cout << "> ";
            std::cin >> j;
        }

        // Does EvDev have a method for detecting the currently selected ABS codes? Each controller will be different!
        controller = controlSelect[j];

        buttonList  = new uint32_t[EV_MAX];

        while(polling)
        {
            ioctl(controller.fd, EVIOCGBIT(0, sizeof(buttonList)), buttonList[0]);

            for (uint32_t keyCode = 0; keyCode < EV_MAX; keyCode++)
            {
                if (((buttonList[0] >> keyCode) & 0x1) != EV_REP)
                {
                    ioctl(controller.fd, EVIOCGBIT(keyCode, sizeof(buttonList)), buttonList[keyCode]);
                    if ((*buttonList >> keyCode) & 0x1)
                    {
                        switch(keyCode)
                        {
                            case EV_SYN:
                                std::cout << "Sync Event" << std::endl;
                                keyCode += 1;
                                break;

                            case EV_ABS:
                                std::cout << libevdev_event_code_get_name(buttonList[keyCode], EV_ABS) << std::endl;
                                controller.abs.push_back(libevdev_get_abs_info(controller.dev, buttonList[keyCode]));
                                break;

                            case EV_KEY:
                                std::cout << libevdev_event_code_get_name(buttonList[keyCode], EV_KEY) << std::endl;
                                controller.buttonCodes.push_back(buttonList[keyCode]);
                                break;

                            case EV_FF:
                                std::cout << "Rumble goes brrrr" << std::endl;
                                break;

                            default:
                                std::cerr << "No ABS or KEY FOUND" << std::endl;
                                continue;
                                break;
                    }
                }
            }
        }
    }

        /*for(std::size_t index = 0; index < controller.buttonCodes.size(); index++)
        {
            input_event ev;
            std::cout << "Map " << controlNames[(Buttons)index] << ": ";
            int err = libevdev_next_event(controller.dev, LIBEVDEV_READ_FLAG_NORMAL | LIBEVDEV_READ_FLAG_BLOCKING, &ev);
            if (err < 0)
            {
                std::cerr << "Err detecting event code: " << std::endl;
            }
            controller.MappedControls.emplace((Buttons)index, ev.code);
        }*/

        controller.uniqueID = libevdev_get_uniq(controller.dev);
        controller.driverVersion = libevdev_get_driver_version(controller.dev);
    }
}
bool Emit::CreateController()
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

int Emit::pressBtn(int button)
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

int Emit::releaseBtn(int button)
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

int Emit::moveABS(int ABS, int moveAxis, int flat)
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

int Emit::resetABS(int ABS, int flat)
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

Buttons Emit::GetCommands(std::string key)
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


bool Emit::emit(Buttons keyCode)
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

bool Emit::Close()
{
    libevdev_uinput_destroy(uidev);
    libevdev_free(dev);
    return 0;
}
