#pragma once
#include <filesystem>
#include <Windows.h>
#include <iostream>
#include <cstdint>
#include "json.hpp"
#include <map>
#include <vjoyinterface.h>

enum Buttons { UP, DOWN, LEFT, RIGHT, A, B, X, Y, START, SELECT, L1, R1, L2, R2, L3, R3, EXIT, CLEAR };

using json = nlohmann::json;

namespace fs = std::filesystem;

static std::map<Buttons, uint32_t> buttonPos =
{
    {A, 0},
    {B, 1},
    {X, 2},
    {Y, 3},
    {START, 4},
    {SELECT, 5},
    {L1, 6},
    {R1, 7},
    {L2, 8},
    {R2, 9},
    {L3, 10},
    {R3, 11}
};

static std::map<std::string, Buttons> commands
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

struct axisData
{
private:
    long axis[4];
public:
    void set(long x, long y, long rx, long ry)
    {
        axis[0] = x;
        axis[1] = y;
        axis[2] = rx;
        axis[3] = ry;
    }

    long get(int i)
    {
        return axis[i];
    }
};

class Emit
{
private:
    int dStat;
    _JOYSTICK_POSITION_V3 controller;
public:
    Emit();
    Emit(json j);

    void initalConfig();
    Buttons& GetCommands(std::string key);

    json control;
    void save(json& j, bool isDefault = false);
    friend void to_json(nlohmann::json& j, const Emit& p);
    friend void from_json(const nlohmann::json& j, Emit& p);

    int CreateController();
    bool emit(Buttons& cmd);
    int moveABS(axisData axis);
    int resetABS();
    int pressBtn(Buttons& btn);
    int releaseBtn(Buttons& btn);
};