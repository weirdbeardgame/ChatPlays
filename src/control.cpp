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
    driverVersion = c.driverVersion;
    mappedControls = c.mappedControls;
    controllerName = c.controllerName;
}

Emit::Emit()
{
    control = json
    {
        "Emit" , 
        {
            {"commands", commands},
            {"controller", controller},
        }
    };
}

Emit::Emit(json j)
{
    from_json(j, *this);
}

void Emit::save(json &j, bool isDefault)
{
    if (isDefault)
    {
        Emit e = Emit();
        j.push_back(e.control);
    }
    else
    {
        j.push_back(control);
    }
}

void Emit::initalConfig()
{
    // List devices and select one to save.
    std::vector<Controller> controlSelect(15);
    uint32_t* supportedEvents;
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
                    if (createControl.controllerName != std::string() || createControl.controllerName != "")
                    {
                        controlSelect.push_back(createControl);
                    }
                    else
                    {
                        continue;
                    }
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

        supportedEvents  = new uint32_t[EV_MAX];
        uint32_t* buttonTemp = new uint32_t[KEY_MAX];

        while(polling)
        {
            ioctl(controller.fd, EVIOCGBIT(0, sizeof(supportedEvents)), supportedEvents[0]);

            for (uint32_t evType = 0; evType < EV_MAX; evType++)
            {
                printf("evType: 0x%2x\n", evType);
                if (((supportedEvents[0] >> evType) & 0x1) != EV_REP)
                {
                    ioctl(controller.fd, EVIOCGBIT(evType, EV_MAX), supportedEvents[evType]);
                    switch(evType)
                    {
                        case EV_SYN:
                            std::cout << "Sync Event" << std::endl;
                            break;

                            case EV_ABS:
                                for (int i = 0; i < ABS_MAX; i++)
                                {
                                    input_absinfo absTemp;
                                    ioctl(controller.fd, EVIOCGABS(i), absTemp);
                                    if (absTemp.maximum > 0)
                                    {
                                        controller.abs.push_back(absTemp);
                                    }
                                }

                                for (int i = 0; i < 4; i++)
                                {
                                    input_event ev;
                                    bool map = true;
                                    std::cout << "Map: " << controlNames[(Buttons)i] << ": ";
                                    while (map)
                                    {
                                        int err = read(controller.fd, &ev, sizeof(input_event));
                                        if (err < 0)
                                        {
                                            std::cerr << "Failed read: " << strerror(-err) << std::endl;
                                        }
                                        if (ev.type == EV_ABS)
                                        {
                                            printf("ev: 0x%02x", ev.code + '\n');
                                            controller.mappedControls.emplace((Buttons)i, ev);
                                            map = false;
                                        }
                                        else
                                        {
                                            continue;
                                        }
                                    }
                                }

                                break;

                            case EV_KEY:
                                for(std::size_t index = 3; index < controlNames.size(); index++)
                                {
                                    input_event ev;
                                    bool map = true;
                                    while (map)
                                    {
                                        std::cout << "Map " << controlNames[(Buttons)index] << ": ";
                                        int err = read(controller.fd, &ev, sizeof(input_event));
                                        if (err < 0)
                                        {
                                            std::cerr << "Failed read: " << strerror(-err) << std::endl;
                                        }
                                        if (ev.type == EV_KEY)
                                        {
                                            printf("ev: 0x%02x", ev.code + '\n');
                                            controller.mappedControls.emplace((Buttons)index, ev);
                                            map = false;
                                        }
                                        else
                                        {
                                            continue;
                                        }
                                    }
                                }
                                break;

                            case EV_FF:
                                std::cout << "Rumble goes brrrr" << std::endl;
                                break;

                            default:
                                continue;
                                break;
                }
            }
        }
        delete[] buttonTemp;
        delete[] supportedEvents;
        if(controller.mappedControls.size() > 0 && controller.abs.size() > 0)
        {
            polling = false;
        }
        else
        {
            std::cerr << "Controller polling error!" << std::endl;
            return;
        }
    }

    controller.uniqueID = libevdev_get_uniq(controller.dev);
    controller.driverVersion = libevdev_get_driver_version(controller.dev);
    }
}
bool Emit::CreateController()
{
    dev = libevdev_new();
    libevdev_set_name(dev, controller.controllerName.c_str());
    libevdev_enable_event_type(dev, EV_ABS);
    libevdev_enable_event_type(dev, EV_KEY);
    for (int i = 0; i < controller.mappedControls.size(); i++)
    {
        if (i <= 7)
        {
            libevdev_enable_event_code(dev, EV_ABS, controller.mappedControls[(Buttons)i].type, &controller.abs[i]);
        }
        else if (i > 7)
        {
            libevdev_enable_event_code(dev, EV_KEY, controller.mappedControls[(Buttons)i].type, NULL);
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
        emitCode = moveABS(controller.mappedControls[keyCode].code, controller.mappedControls[keyCode].value, 4095);
        break;
    case Buttons::DOWN:
        // Dpad Down
        emitCode = moveABS(controller.mappedControls[keyCode].code, controller.mappedControls[keyCode].value, 4095);
        break;

    case Buttons::LEFT:
        // Dpad Left
        emitCode = moveABS(controller.mappedControls[keyCode].code, controller.mappedControls[keyCode].value, 4095);
        break;

    case Buttons::RIGHT:
        // Dpad Right
        emitCode = moveABS(controller.mappedControls[keyCode].code, controller.mappedControls[keyCode].value, 4095);
        break;

    case Buttons::EXIT: 
        emitCode = Close();
        return emitCode;
        break;
    }
    emitCode = pressBtn(controller.mappedControls[keyCode].code);
    return emitCode;
}

bool Emit::Close()
{
    libevdev_uinput_destroy(uidev);
    libevdev_free(dev);
    return 0;
}

void to_json(json& j, const input_absinfo& c)
{
    j = json
    {
        "input_absinfo",
        {
            {"value", c.value},
            {"flat", c.flat},
            {"fuzz", c.fuzz},
            {"maximum", c.maximum},
            {"minimum", c.minimum},
            {"resolution", c.resolution}
        }
    };
}

void from_json(const json& j, input_absinfo& c)
{
    j.at("value").get_to(c.value);
    j.at("flat").get_to(c.flat);
    j.at("fuzz").get_to(c.fuzz);
    j.at("maximum").get_to(c.maximum);
    j.at("minimum").get_to(c.minimum);
    j.at("resolution").get_to(c.resolution);
}

void to_json(json& j, const timeval& t)
{
    j = json
    {
        {"tv_sec", t.tv_sec},
        {"tv_usec", t.tv_usec},
    };
}

void from_json(const json& j, timeval& t)
{
    j.at("tv_sec").get_to(t.tv_sec);
    j.at("tv_usec").get_to(t.tv_usec);
}


void to_json(json& j, const input_event& e)
{
    j = json
    {
        {"type", e.type},
        {"code", e.code},
        {"value", e.value},
        {"time", e.time},
    };
}

void from_json(const json& j, input_event& e)
{
    j.at("type").get_to(e.type);
    j.at("code").get_to(e.code);
    j.at("value").get_to(e.value);
    j.at("time").get_to(e.time);
}


void to_json(json& j, const Emit& c)
{
    j = json
    {
        "ControlInfo" , 
        {
            {"commands", c.commands},
            {"controller", c.controller},
        }
    };
}

void from_json(const json& j, Emit& c)
{
    j.at("commands").get_to(c.commands);
    j.at("controller").get_to(c.controller);
}

void to_json(json& j, const Controller& c)
{
    j = json
    {
        "Controller",
        {
            {"fd", c.fd},
            {"uniqueID", c.uniqueID},
            {"driverVersion", c.driverVersion},
            {"controllerName", c.controllerName},
            {"mappedControls", c.mappedControls},
        }
    };
    for (int i = 0; i < c.abs.size(); i++)
    {
        j.push_back(c.abs[i]);
    }
}

void from_json(const nlohmann::json& j, Controller& c)
{
    j.at("fd").get_to(c.fd);
    j.at("uniqueID").get_to(c.uniqueID);
    j.at("driverVersion").get_to(c.driverVersion);
    j.at("controllerName").get_to(c.controllerName);
    j.at("mappedControls").get_to(c.mappedControls);
}
