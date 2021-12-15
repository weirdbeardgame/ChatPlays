#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <filesystem>
#include <Xinput.h>
#include <ViGEm/Client.h>
#include <iostream>
#include <cstdint>
#include <map>

#include "message.h"
#include "json.hpp"

#pragma comment(lib, "setupapi.lib")

// Note that "EXIT" Stands for the bot exit command
enum Buttons { UP, DOWN, LEFT, RIGHT, DUP, DDOWN, DLEFT, DRIGHT, L2, R2, A, B, X, Y, START, SELECT, L1, R1, L3, R3, EXIT, CLEAR };

using json = nlohmann::json;

namespace fs = std::filesystem;

static std::map<Buttons, uint16_t> buttonPos =
{
    {A, XUSB_GAMEPAD_A},
    {B, XUSB_GAMEPAD_B},
    {X, XUSB_GAMEPAD_X},
    {Y, XUSB_GAMEPAD_Y},
    {START, XUSB_GAMEPAD_START},
    {SELECT, XUSB_GAMEPAD_BACK},
    {L1, XUSB_GAMEPAD_LEFT_SHOULDER},
    {R1, XUSB_GAMEPAD_RIGHT_SHOULDER},
    {L3, XUSB_GAMEPAD_LEFT_THUMB},
    {R3, XUSB_GAMEPAD_RIGHT_THUMB},
    {DUP, XUSB_GAMEPAD_DPAD_UP},
    {DDOWN, XUSB_GAMEPAD_DPAD_DOWN},
    {DLEFT, XUSB_GAMEPAD_DPAD_LEFT},
    {DRIGHT, XUSB_GAMEPAD_DPAD_RIGHT}
};

// What I need is a default settings
// Note that this array will dictate the order of the map as well
const std::array<std::string, 21> commandEnumList =
{
    "UP",
    "DOWN",
    "LEFT",
    "RIGHT",
    "A",
    "B",
    "X",
    "Y",
    "START",
    "SELECT",
    "L1",
    "R1",
    "L2",
    "R2",
    "L3",
    "R3",
    "Exit",
    "DUP",
    "DDOWN",
    "DLEFT",
    "DRIGHT"
};

static std::map<std::string, Buttons> defaultCommands
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
    {"Exit", EXIT},
    {"DUP", DUP},
    {"DDOWN", DDOWN},
    {"DLEFT", DLEFT},
    {"DRIGHT", DRIGHT}
};

struct axisData
{
private:
    short axis[4];
public:
    void set(short x, short y, short rx, short ry)
    {
        axis[0] = x;
        axis[1] = y;
        axis[2] = rx;
        axis[3] = ry;
    }

    short get(int i)
    {
        return axis[i];
    }
};

class Emit
{
private:
    // I need Xinput
    PXUSB_REPORT report;
    PVIGEM_CLIENT driver;
    PVIGEM_TARGET xbox;

    Buttons cmd = Buttons::CLEAR;
    bool emitFail = false;

    Message* queue = nullptr;

    std::map<std::string, Buttons> commands;

public:
    Emit() = default;
    Emit(Message* q)
    {
        queue = q;
    }
    Emit(json j, Message* q);

    // Note I make it include itself in the case of there's something real already.
    Emit* InitalConfig();
    Buttons& GetCommands(std::string key);

    // All the action is in here m8
    bool isActive = false;

    json control;
    void save(json& j, bool isDefault = false);
    friend void to_json(nlohmann::json& j, const Emit& p);
    friend void from_json(const nlohmann::json& j, Emit& p);

    int CreateController(Message* q, Emit settings);
    void emit(Message* q, Buttons cmd);
    void poll(Message* q, Emit settings, bool manual);
    void moveABS(axisData& axis);
    void resetABS();
    void pressBtn(Buttons& btn);
    void releaseBtn(Buttons& btn);
};