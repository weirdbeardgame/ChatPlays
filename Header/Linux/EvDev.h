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
#include <iostream>
#include <filesystem>

#include <linux/uinput.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

#include "json.hpp"
#include "message.h"

#include "Controller.h"

using json = nlohmann::json;

namespace fs = std::filesystem;

static std::map<ABS, std::string> absName{
    {ABS::L_X_AXIS, "Left stick X"},
    {ABS::L_Y_AXIS, "Left stick Y"},
    {ABS::R_X_AXIS, "Right stick X"},
    {ABS::R_Y_AXIS, "Right stick Y"},
    {ABS::R2, "R2"},
    {ABS::L2, "L2"}};

static std::map<Buttons, std::string> buttonName{
    {Buttons::A, "A"},
    {Buttons::B, "B"},
    {Buttons::X, "X"},
    {Buttons::Y, "Y"},
    {Buttons::START, "Start"},
    {Buttons::SELECT, "Select"},
    {Buttons::R1, "R1"},
    {Buttons::L1, "L1"},
    {Buttons::R3, "R3"},
    {Buttons::L3, "L3"}};

static std::array<uint32_t, 100> bl{
    KEY_BACK,
    KEY_HOMEPAGE,
    KEY_UP,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_DOWN,
    KEY_NEXT,
    KEY_PREVIOUS,
    BTN_0,
    BTN_1,
    BTN_2,
    BTN_3,
    BTN_4,
    BTN_5,
    BTN_6,
    BTN_7,
    BTN_8,
    BTN_9};

static std::vector<uint32_t> button_list =
    {
        KEY_BACK,
        KEY_HOMEPAGE,
        KEY_UP,
        KEY_LEFT,
        KEY_RIGHT,
        KEY_DOWN,
        KEY_NEXT,
        KEY_PREVIOUS,
        BTN_0,
        BTN_1,
        BTN_2,
        BTN_3,
        BTN_4,
        BTN_5,
        BTN_6,
        BTN_7,
        BTN_8,
        BTN_9,
        BTN_LEFT,
        BTN_RIGHT,
        BTN_MIDDLE,
        BTN_SIDE,
        BTN_EXTRA,
        BTN_FORWARD,
        BTN_BACK,
        BTN_TASK,
        BTN_JOYSTICK,
        BTN_TRIGGER,
        BTN_THUMB,
        BTN_THUMB2,
        BTN_TOP,
        BTN_TOP2,
        BTN_PINKIE,
        BTN_BASE,
        BTN_BASE2,
        BTN_BASE3,
        BTN_BASE4,
        BTN_BASE5,
        BTN_BASE6,
        BTN_DEAD,
        BTN_A,
        BTN_B,
        BTN_C,
        BTN_X,
        BTN_Y,
        BTN_Z,
        BTN_TL,
        BTN_TR,
        BTN_TL2,
        BTN_TR2,
        BTN_SELECT,
        BTN_START,
        BTN_MODE,
        BTN_THUMBL,
        BTN_THUMBR,
        BTN_TOOL_PEN,
        BTN_TOOL_RUBBER,
        BTN_TOOL_BRUSH,
        BTN_TOOL_PENCIL,
        BTN_TOOL_AIRBRUSH,
        BTN_TOOL_FINGER,
        BTN_TOOL_MOUSE,
        BTN_TOOL_LENS,
        BTN_TOOL_QUINTTAP,
        BTN_TOUCH,
        BTN_STYLUS,
        BTN_STYLUS2,
        BTN_TOOL_DOUBLETAP,
        BTN_TOOL_TRIPLETAP,
        BTN_TOOL_QUADTAP,
        BTN_GEAR_DOWN,
        BTN_GEAR_UP,
        BTN_DPAD_UP,
        BTN_DPAD_DOWN,
        BTN_DPAD_LEFT,
        BTN_DPAD_RIGHT,
        BTN_TRIGGER_HAPPY,
        BTN_TRIGGER_HAPPY1,
        BTN_TRIGGER_HAPPY2,
        BTN_TRIGGER_HAPPY3,
        BTN_TRIGGER_HAPPY4,
        BTN_TRIGGER_HAPPY5,
        BTN_TRIGGER_HAPPY6,
        BTN_TRIGGER_HAPPY7,
        BTN_TRIGGER_HAPPY8,
        BTN_TRIGGER_HAPPY9,
        BTN_TRIGGER_HAPPY10,
        BTN_TRIGGER_HAPPY11,
        BTN_TRIGGER_HAPPY12,
        BTN_TRIGGER_HAPPY13,
        BTN_TRIGGER_HAPPY14,
        BTN_TRIGGER_HAPPY15,
        BTN_TRIGGER_HAPPY16,
        BTN_TRIGGER_HAPPY17,
        BTN_TRIGGER_HAPPY18,
        BTN_TRIGGER_HAPPY19,
        BTN_TRIGGER_HAPPY20,
        BTN_TRIGGER_HAPPY21,
        BTN_TRIGGER_HAPPY22,
        BTN_TRIGGER_HAPPY23,
        BTN_TRIGGER_HAPPY24,
        BTN_TRIGGER_HAPPY25,
        BTN_TRIGGER_HAPPY26,
        BTN_TRIGGER_HAPPY27,
        BTN_TRIGGER_HAPPY28,
        BTN_TRIGGER_HAPPY29,
        BTN_TRIGGER_HAPPY30,
        BTN_TRIGGER_HAPPY31,
        BTN_TRIGGER_HAPPY32,
        BTN_TRIGGER_HAPPY33,
        BTN_TRIGGER_HAPPY34,
        BTN_TRIGGER_HAPPY35,
        BTN_TRIGGER_HAPPY36,
        BTN_TRIGGER_HAPPY37,
        BTN_TRIGGER_HAPPY38,
        BTN_TRIGGER_HAPPY39,
        BTN_TRIGGER_HAPPY40,
};

// This is supposed to represent the actual controller in memory as it's created.
// It can hold it's own assigned controller data and mappings
struct EvDevDevice : Controller
{
    // Consider all of the below a default mapping
    std::map<Buttons, uint32_t> buttonCodes;
    struct std::map<uint32_t, input_absinfo> abs;

    // All of this data is intended to create as convincing a device as possible
    std::string uniqueID;
    std::string controllerName;

    fs::path eventPath = "/dev/input";

    int fd;
    int driverVersion;

    // Controller struct pointers
    libevdev_uinput *uidev;
    input_event ev;
    libevdev *dev;
    timeval timer;

    bool isActive = false;

    bool isMapped = false;

    bool AttachController() override;
    int PressButton(Buttons b) override;
    int ReleaseButton(Buttons b) override;
    int MoveABS(ABS abs, float axis) override;
    int ResetABS(ABS abs) override;
    void DisconnectController() override;
    bool SendInput(Buttons b, ABS a = ABS::CLEAR, float axis = 0, int heldFor = 0) override;

    bool pollEvent(uint32_t evType, uint32_t keyCode);
    std::map<Buttons, uint32_t> mappedControls;

    friend void to_json(nlohmann::json &j, const Controller &p);
    friend void from_json(const nlohmann::json &j, Controller &p);
};

class Emit
{
private:
    int activePlayers;
    int fd;

    bool isActive;
    json control;

    EvDevDevice *controller;

    std::queue<Buttons>controlQueue;

    std::vector<fs::path> controlSelect;

public:
    Emit() = default;
    Emit(json j);

    void CreateController(bool manual);

    void save(json &j, bool isDefault = false);

    friend void to_json(nlohmann::json &j, const Emit &p);
    friend void from_json(const nlohmann::json &j, Emit &p);

    friend void to_json(nlohmann::json &j, const input_absinfo &p);
    friend void from_json(const nlohmann::json &j, input_absinfo &p);

    bool Close();
    bool emit(Buttons cmd);
};