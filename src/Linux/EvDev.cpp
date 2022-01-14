#include <poll.h>
#include <chrono>
#include <functional>
#include <condition_variable>
#include "Linux/EvDev.h"

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

bool EvDevDevice::pollEvent(uint32_t evType, uint32_t keycode)
{
    int val = 0;

    if (libevdev_fetch_event_value(dev, evType, keycode, &val))
    {
        if (val == 1)
        {
            std::cout << "Event 1" << std::endl;

            if (libevdev_fetch_event_value(dev, evType, keycode, &val))
            {
                std::cout << " Code: " << libevdev_event_code_get_name(evType, keycode) << std::endl;
                std::cout << "Value: " << val << std::endl;
                if (val == 0)
                {
                    std::cout << "Event 2" << std::endl;
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}

void Emit::CreateController(bool manual)
{
    if (controller->AttachController())
    {
        if (manual)
        {
            // run manual control loop.
        }
    }
}

bool EvDevDevice::AttachController()
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
        }
        if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
        {
            printf("IoCtrl EVBit Error\n");
        }

        for (int i = 0; i < buttonCodes.size(); i++)
        {
            if (ioctl(fd, UI_SET_KEYBIT, buttonCodes[Buttons(i)]) < 0)
            {
                printf("IoCtrl KEY Error\n");
            }
        }
        if (ioctl(fd, UI_DEV_CREATE) < 0)
        {
            std::cout << "ERR Bad FD: " << fd << " : " << strerror(errno) << std::endl;
            isActive = false;
        }
        sleep(1);
        isActive = true;
        // Count of attached players
        // controllerID = ;
    }
    return isActive;
}

void EvDevDevice::DisconnectController()
{
    libevdev_uinput_destroy(uidev);
    libevdev_free(dev);
    close(fd);
}

int EvDevDevice::PressButton(Buttons button)
{
    int emitCode = 0;

    emitCode = libevdev_uinput_write_event(uidev, EV_KEY, buttonCodes[button], 1);
    emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    sleep(1);
    ReleaseButton(button);
}

int EvDevDevice::ReleaseButton(Buttons button)
{
    int emitCode = 0;

    emitCode = libevdev_uinput_write_event(uidev, EV_KEY, button, 0);
    emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    return emitCode;
}

int EvDevDevice::MoveABS(ABS abs, float moveAxis)
{
    int emitCode = 0;

    emitCode = libevdev_uinput_write_event(uidev, EV_ABS, abs, moveAxis);
    emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    return emitCode;
}

int EvDevDevice::ResetABS(ABS abs)
{
    int emitCode = 0;
    emitCode = libevdev_uinput_write_event(uidev, EV_ABS, abs, 0);
    emitCode = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    return emitCode;
}

bool EvDevDevice::SendInput(Buttons b, ABS a, float axis, int heldFor)
{
    int emitCode = 0;
    emitCode = PressButton(b);
    sleep(heldFor);
    emitCode = ReleaseButton(b);
    emitCode = MoveABS(a, axis);
    sleep(heldFor);
    return emitCode;
}

bool Emit::Close()
{
    controller->DisconnectController();
    return 0;
}

void to_json(nlohmann::json &j, const input_absinfo &abs)
{
    j["flat"] = abs.flat;
    j["fuzz"] = abs.fuzz;
    j["value"] = abs.value;
    j["minimum"] = abs.minimum;
    j["maximum"] = abs.maximum;
    j["resolution"] = abs.resolution;
}

void from_json(const nlohmann::json &j, input_absinfo &abs)
{
    j["flat"].get_to(abs.flat);
    j["fuzz"].get_to(abs.fuzz);
    j["value"].get_to(abs.value);
    j["minimum"].get_to(abs.minimum);
    j["maximum"].get_to(abs.maximum);
    j["resolution"].get_to(abs.resolution);
}

void to_json(nlohmann::json &j, const EvDevDevice &c)
{
    j[1]["abs"] = c.abs;
    j[1]["buttonCodes"] = c.buttonCodes;
    j[1]["controllerName"] = c.controllerName;
}

void from_json(const nlohmann::json &j, EvDevDevice &c)
{
    j[1]["abs"].get_to(c.abs);
    j[1]["buttonCodes"].get_to(c.buttonCodes);
}

void to_json(nlohmann::json &j, const Emit &p)
{
    // j[1]["controller"] = p.controller;
}

void from_json(const nlohmann::json &j, Emit &p)
{
    // j[1]["controller"].get_to(p.controller);
}