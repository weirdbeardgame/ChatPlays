#pragma once
#include <map>
#include <mutex>
#include <queue>
#include <vector>
#include <thread>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <linux/uinput.h>

#include "message.h"

#include "Controller.h"

namespace fs = std::filesystem;

// Do note these are static incase they need to be directly refrenced elsewhere
static std::map<Buttons, uint32_t> buttonCodes = {
    {A, BTN_SOUTH},
    {B, BTN_EAST},
    {X, BTN_WEST},
    {Y, BTN_NORTH},
    {START, BTN_START},
    {SELECT, BTN_SELECT},
    {L1, BTN_THUMBL},
    {R1, BTN_THUMBR},
    {L3, BTN_TL},
    {R3, BTN_TR}};

static std::map<ABS, uint32_t> Abs{
    {L_X_AXIS, ABS_X},
    {L_X_AXIS, ABS_Y},
    {R_X_AXIS, ABS_RX},
    {R_Y_AXIS, ABS_RY},
    {HAT_X, ABS_HAT0X},
    {HAT_Y, ABS_HAT0Y}};

// This is supposed to represent the actual controller in memory as it's created.
// It can hold it's own assigned controller data and mappings
struct Emit : Controller
{
    // All of this data is intended to create as convincing a device as possible
    char deviceName[UINPUT_MAX_NAME_SIZE] = "Microsoft X-Box 360 pad";
    input_id deviceID;

    int fd;
    // Controller struct pointers
    uinput_setup usetup;
    input_event ev;
    timeval timer;

    bool isActive = false;

    bool isMapped = false;

    Emit();
    int SetAbs(int type, int val, int min, int max, int fuzz, int flat, int resolution);
    bool AttachController() override;
    int PressButton(Buttons b) override;
    int ReleaseButton(Buttons b) override;
    int MoveABS(ABS abs, float axis) override;
    int ResetABS(ABS abs) override;
    void DisconnectController() override;
    bool SendInput(Buttons b = B_CLEAR, ABS a = ABS::CLEAR, float axis = 0, int heldFor = 0) override;
    ~Emit();
};