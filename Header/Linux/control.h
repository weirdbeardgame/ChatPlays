#pragma once
#include <map>
#include <queue>
#include <vector>
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

enum Buttons {UP, DOWN, LEFT, RIGHT, A, B, X, Y, START, SELECT, L1, R1, L2, R2, L3, R3, EXIT};

using json = nlohmann::json;

namespace fs = std::filesystem;

static std::map<Buttons, std::string> controlNames = 
{
    {UP, "UP"},
    {DOWN, "DOWN"},
    {LEFT, "LEFT"},
    {RIGHT, "RIGHT"},
    {A, "A"},
    {B, "B"},
    {X, "X"},
    {Y, "Y"},
    {START, "START"},
    {SELECT, "SELECT"},
    {L1, "L1"},
    {R1, "R1"},
    {L2, "L2"},
    {R2, "R2"},
    {L3, "L3"},
    {R3, "R3"}
};

struct Controller
{
    Controller();
    Controller(const Controller& c);

    fs::path eventPath = "/dev/input";
    std::string controllerName;
    std::map<Buttons, input_event> mappedControls;
    std::vector<input_absinfo> abs;
    std::string uniqueID;
    int driverVersion;

    int fd;
    struct libevdev *dev;

    friend void to_json(nlohmann::json& j, const Controller& p);
    friend void from_json(const nlohmann::json& j, Controller& p);

};

class Emit
{
    private:
    int fd = 0;
    int maxInput = 0;

    struct libevdev *dev;
    struct uinput_user_dev device;
    struct uinput_setup usetup;
    struct libevdev_uinput *uidev;
    struct input_absinfo* init;

    Controller controller;
    std::map<std::string, Buttons> commands
    {
        {"UP", UP},
        {"DOWN", DOWN},
        {"LEFT", LEFT},
        {"RIGHT", RIGHT},
        {"A", A},
        {"B", B},
        {"X", X},
        {"Y", Y},
        {"START", START},
        {"SELECT", SELECT},
        {"L1", L1},
        {"R1", R1},
        {"L2", L2},
        {"R2", R2},
        {"L3", L3},
        {"R3", R3},
        {"Exit", EXIT}
    };
    std::queue<Buttons>controlQueue;
    public:
    Emit();
    Emit(json j);

    void initalConfig();
    Buttons GetCommands(std::string key);

    json control;
    void save(json &j, bool isDefault = false);
    friend void to_json(nlohmann::json& j, const Emit& p);
    friend void from_json(const nlohmann::json& j, Emit& p);

    bool CreateController();
    bool emit(Buttons cmd);
    int moveABS(int ABS, int moveAxis, int flat);
    int resetABS(int ABS, int flatAxis);
    int pressBtn(int btn);
    int releaseBtn(int btn);
    bool Close();
};