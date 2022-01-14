#include <poll.h>
#include <chrono>
#include <functional>
#include <condition_variable>

#include "Linux/control.h"

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
        control = *this;
        j += control;
    }
}

input_event Controller::pollEvent(pollfd* fds)
{
    int err = 0;
    input_event event;

    bool isReady = poll(fds, sizeof(fds), timer.tv_sec);

    if (isReady)
    {
        err = read(fd, &event, sizeof(event));
        // Means the struct was filled proper and we have an event
        if (err == sizeof(event))
        {
            return event;
        }
    }
}

void Emit::listControllers(pollfd* fds)
{
    int i = 0;
    int file = 0;

    libevdev* dev;

    // List devices and select one to save.
    for (auto const &entry: fs::directory_iterator("/dev/input"))
    {
        std::string temp = entry.path().filename();

        if (temp.compare(0, 5, "event") == 0)
        {
            // fd doesn't carry through and get's lost. Read is literally polling blank yet somewhat initalized data
            //std::cout << "Temp: " << temp << std::endl;
            file = open(entry.path().c_str(), O_RDONLY);
            int err = libevdev_new_from_fd(file, &dev);
            if (err < 0)
            {
                // If it's just a bad file descriptor, don't bother logging, but otherwise, log it.
                if (err != -9)
                {
                    printf("Failed to connect to device at %s, the error was: %s", entry.path(), strerror(-err));
                    libevdev_free(dev);
                    close(fd);
                }
                else
                {
                    printf("Invalid FD \n");
                    close(fd);
                    return;
                }
            }

            if (libevdev_has_event_type(dev, EV_KEY) && libevdev_has_event_type(dev, EV_ABS))
            {
                fds[i].fd = file;
                // Since it's being closed anyways. Just tell it to fuck off ourselves. We can reopen in the lower config sections
                close(file);
                libevdev_free(dev);
                controlSelect.push_back(entry.path());
            }
        }
        i += 1;
    }
}

void Emit::PrintControllers()
{
    int i = 0;
    libevdev *dev;
    for(auto &path: controlSelect)
    {
        int fdTemp = open(path.string().c_str(), O_RDONLY);
        if (fdTemp >= 0)
        {
            int err = libevdev_new_from_fd(fdTemp, &dev);
            std::cout << i << ": " << libevdev_get_name(dev) << std::endl;
            close(fdTemp);
            libevdev_free(dev);
            i += 1;
        }
    }
}

Controller Emit::selectController()
{
    int j = 0;
    Controller control;

    PrintControllers();

    if (controlSelect.size() > 0)
    {
        // We check negative to make sure j only gets entered into once
        // And because index 0 would be a valid index
        std::cout << "> ";
        std::cin >> j;

        control.eventPath = controlSelect[j];

        // Start to create the actual controller device in here
        control.fd = open(control.eventPath.c_str(), O_RDONLY);

        if (control.fd < 0)
        {
            std::cerr << "Err: " << control.fd << std::endl;
        }

        int err = libevdev_new_from_fd(control.fd, &control.dev);

        if (err < 0)
        {
            std::cerr << "Err Generating evdev device" << std::endl;
            return Controller();
        }

        control.driverVersion = libevdev_get_driver_version(control.dev);
        control.controllerName = libevdev_get_name(control.dev);
        control.uniqueID = libevdev_get_uniq(control.dev);
    }
    return control;
}

Emit* Emit::InitalConfig()
{
    pollfd fds[10];
    listControllers(fds);
    controller = selectController();
    bool isMapped = false;

    // Think about this for a second. There's a constant flow of data.
    // What do you actually NEED from that constant flow to be able to play back the controller when serialized.
    // Cause this aint it dumbass.

    /*for (int i = 0; i < controlNames.size(); i++)
    {
        std::cout << " Configure: " << controlNames[(Buttons)i];
        while(!isMapped)
        {
            controller.ev = controller.pollEvent(fds);

            // This checks if were mapping to UP DOWN LEFT RIGHT etc.
            if ((Buttons)i < Buttons::A && controller.ev.type == EV_ABS)
            {
                input_absinfo absTemp;
                ioctl(controller.fd, EVIOCGABS(controller.ev.code), absTemp);

                std::cout << " Code: " << libevdev_event_code_get_name(controller.ev.type, controller.ev.code) << std::endl;
                controller.abs.try_emplace(controller.ev.code, absTemp);
                controller.mappedControls.emplace((Buttons)i, controller.ev);
                isMapped = true;
            }
            if ((Buttons)i >= Buttons::A && controller.ev.type == EV_KEY)
            {
                std::cout << " Code: " << libevdev_event_code_get_name(controller.ev.type, controller.ev.code) << std::endl;
                controller.mappedControls.emplace((Buttons)i, controller.ev);
                isMapped = true;
            }
            else
            {
                controller.ev = controller.pollEvent(fds);
            }
        }
        isMapped = false;
    }*/

    return this;
}

bool Emit::CreateController(Message* q)
{
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0)
    {
        printf ("Open Error! \n");
        isActive = false;
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
            isActive = false;
        }
        sleep(1);
        isActive = true;
    }
    return isActive;
}

void Emit::ControllerThread(Message* q, Emit settings, bool manual)
{
    if (!isActive)
    {
        CreateController(q);
    }

    while(isActive)
    {
        if (manual)
        {
            std::string cmd = std::string();
            std::cout << "Please Enter a command: ";
            std::cin >> cmd;
            emit(q, GetCommands(cmd));
        }
        else if (!manual)
        {
            std::string cmd = q->dequeue();
            emit(q, GetCommands(cmd));
        }
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

bool Emit::emit(Message* q, Buttons keyCode)
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
    libevdev_free(controller.dev);
    return 0;
}

void to_json(nlohmann::json& j, const input_absinfo& abs)
{
    j["flat"] = abs.flat;
    j["fuzz"] = abs.fuzz;
    j["value"] = abs.value;
    j["minimum"] = abs.minimum;
    j["maximum"] = abs.maximum;
    j["resolution"] = abs.resolution;
}

void from_json(const nlohmann::json& j, input_absinfo& abs)
{
    j["flat"].get_to(abs.flat);
    j["fuzz"].get_to(abs.fuzz);
    j["value"].get_to(abs.value);
    j["minimum"].get_to(abs.minimum);
    j["maximum"].get_to(abs.maximum);
    j["resolution"].get_to(abs.resolution);
}

void to_json(nlohmann::json& j, const Controller& c)
{
    j[1]["abs"] = c.abs;
    j[1]["buttonCodes"] = c.buttonCodes;
    j[1]["controllerName"] = c.controllerName;

}

void from_json(const nlohmann::json& j, Controller& c)
{
    j[1]["abs"].get_to(c.abs);
    j[1]["buttonCodes"].get_to(c.buttonCodes);
}

void to_json(nlohmann::json& j, const Emit& p)
{
    j[1]["commands"] = p.commands;
    j[1]["controller"] = p.controller;
}

void from_json(const nlohmann::json& j, Emit& p)
{
    j[1]["commands"].get_to(p.commands);
    j[1]["controller"].get_to(p.controller);
}