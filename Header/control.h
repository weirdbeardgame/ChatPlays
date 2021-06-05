#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <vector>
#include <string.h>
#include <iostream>
#include <linux/uinput.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

class Control
{
    private:
    std::vector<int>buttonCodes = 
    {
        EV_ABS,
        ABS_X,
        ABS_Y,
        ABS_RX,
        ABS_RY,
        ABS_RZ,
        ABS_Z,
        ABS_HAT0X,
        ABS_HAT0Y,
        EV_KEY,
        BTN_SOUTH,
        BTN_EAST,
        BTN_C,
        BTN_NORTH,
        BTN_WEST,
        BTN_Z,
        BTN_TR,
        BTN_TL
    };
    struct std::vector<input_absinfo*> abs;

    int fd = 0;
    struct libevdev *dev;
    struct uinput_user_dev device;
    struct uinput_setup usetup;
    struct libevdev_uinput *uidev;
    struct input_absinfo* init;

    public:
    bool CreateController();
    bool emit(std::string cmd);
    bool Close();
};