#include <chrono>
#include "Linux/control.h"
#include <condition_variable>
#include <functional>

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

bool Controller::pollEvent()
{
    int err = 0;
    err = read(fd, &ev, sizeof(ev));

    err = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL | LIBEVDEV_READ_FLAG_SYNC, &ev);
    if (err < 0)
    {
        printf("Failed read: ", strerror(errno) + '\n');
        return false;
    }
    else if (ev.type > 0)
    {
        //printf("ev: 0x%02x", ev.code + ' \n');
        return true;
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
            createControl.fd = open(entry.path().c_str(), O_RDONLY);
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
    }

    bool polled;
    std::thread th = controller.createPollThread();
    std::condition_variable condition;

    for (int i = 0; i < controlNames.size(); i++)
    {
        std::mutex m;
        std::unique_lock<std::mutex> lck(m);
        while (condition.wait_for(lck, std::chrono::seconds(5)) == std::cv_status::timeout)
        {
            switch (controller.ev.type)
            {
                case EV_ABS:
                    input_absinfo absTemp;
                    // I want code then I poll for ABS Info
                    ioctl(controller.fd, EVIOCGABS(controller.ev.code), absTemp);
                    if (absTemp.maximum > 0 && !controller.abs.contains(controller.ev.code))
                    {
                        //controller.abs.try_emplace(controller.ev.code, absTemp);
                    }
                    controller.mappedControls.try_emplace((Buttons)i, controller.ev);
                    break;

                    case EV_KEY:
                        controller.mappedControls.try_emplace((Buttons)i, controller.ev);
                        break;

                default:
                    controller.pollEvent();
                    break;
                }
        }

    }

    controller.uniqueID = libevdev_get_uniq(controller.dev);
    controller.driverVersion = libevdev_get_driver_version(controller.dev);
}

bool Emit::CreateController(Message* q, bool manualControl)
{
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0)
    {
        printf ("Open Error! \n");
        return false;
    }
    else
    {
        queue = q;

        if (ioctl(fd, UI_SET_EVBIT, EV_ABS) < 0)
        {
            printf("IoCtrl EVBit Error\n");
        }
        if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
        {
            printf("IoCtrl EVBit Error\n");
        }
        //int type = controller.buttonCodes[0];
        for (int i = 0; i < controller.buttonCodes.size(); i++)
        {
            if (i < DRIGHT)
            {
                if (ioctl(fd, UI_ABS_SETUP, &controller.abs[Buttons(i)], 0) < 0)
                {
                    printf("IoCtrl ABS Error\n");
                }
            }
            else
            {
                if (ioctl(fd, UI_SET_KEYBIT, controller.buttonCodes[Buttons(i)]) < 0)
                {
                    printf("IoCtrl KEY Error\n");
                }
            }
        }
        if (ioctl(fd, UI_DEV_CREATE) < 0)
        {
            std::cout << "ERR Bad FD: " << fd << " : " << strerror(errno) << std::endl;
            return false;
        }
        sleep(1);
        return true;
    }
}

int Emit::pressBtn(uint32_t button)
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

int Emit::releaseBtn(uint32_t button)
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

int Emit::moveABS(uint32_t ABS, int moveAxis, int flat)
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

int Emit::resetABS(uint32_t ABS, int flat)
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
        emitCode = moveABS(controller.buttonCodes[keyCode], controller.mappedControls[keyCode].value, 4095);
        break;
    case Buttons::DOWN:
        // Dpad Down
        emitCode = moveABS(controller.buttonCodes[keyCode], controller.mappedControls[keyCode].value, 4095);
        break;

    case Buttons::LEFT:
        // Dpad Left
        emitCode = moveABS(controller.buttonCodes[keyCode], controller.mappedControls[keyCode].value, 4095);
        break;

    case Buttons::RIGHT:
        // Dpad Right
        emitCode = moveABS(controller.buttonCodes[keyCode], controller.mappedControls[keyCode].value, 4095);
        break;

    case Buttons::EXIT: 
        emitCode = Close();
        return emitCode;
        break;
    }
    emitCode = pressBtn(controller.buttonCodes[keyCode]);
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
            //{"absInfo", c.abs},
        }
    };
}

void from_json(const nlohmann::json& j, Controller& c)
{
    j.at("fd").get_to(c.fd);
    j.at("uniqueID").get_to(c.uniqueID);
    j.at("driverVersion").get_to(c.driverVersion);
    j.at("controllerName").get_to(c.controllerName);
    j.at("mappedControls").get_to(c.mappedControls);
    //j.at("absInfo").get_to(c.abs);
}
