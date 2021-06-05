#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <linux/uinput.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

class Control
{
    private:
    int fd = 0;
    struct libevdev *dev;
    struct uinput_user_dev device;
    struct uinput_setup usetup;
    struct libevdev_uinput *uidev;
    struct input_absinfo *abs;

    public:
    bool init();
    bool emit(std::string cmd);
    bool Close();
};