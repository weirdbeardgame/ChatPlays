#pragma once
#include <iostream>
#include <map>

enum ABS
{
    L_X_AXIS,
    L_Y_AXIS,
    R_X_AXIS,
    R_Y_AXIS,
    HAT_X,
    HAT_Y,
    L2,
    R2,
    CLEAR
};
enum Buttons
{
    A,
    B,
    X,
    Y,
    START,
    SELECT,
    L1,
    R1,
    L3,
    R3,
    DPAD_UP,
    DPAD_DOWN,
    DPAD_LEFT,
    DPAD_RIGHT,
    B_CLEAR
};

static std::map<std::string, ABS> absName{
    {"Left stick X", ABS::L_X_AXIS},
    {"Left stick Y", ABS::L_Y_AXIS},
    {"Right stick X", ABS::R_X_AXIS},
    {"Right stick Y", ABS::R_Y_AXIS},
    {"Dpad X", ABS::HAT_X},
    {"Dpad Y", ABS::HAT_Y},
    {"R2", ABS::R2},
    {"L2", ABS::L2}};

static std::map<std::string, Buttons> buttonName{
    {"A", Buttons::A},
    {"B", Buttons::B},
    {"X", Buttons::X},
    {"Y", Buttons::Y},
    {"Start", Buttons::START},
    {"Select", Buttons::SELECT},
    {"R1", Buttons::R1},
    {"L1", Buttons::L1},
    {"R3", Buttons::R3},
    {"L3", Buttons::L3}};

class Controller
{

protected:
    int controllerID;

public:
    virtual bool AttachController() = 0;
    virtual int PressButton(Buttons b) = 0;
    virtual int ReleaseButton(Buttons b) = 0;
    virtual bool SendInput(Buttons b, ABS a = ABS::CLEAR, float axis = 0, int heldFor = 0) = 0;
    virtual int MoveABS(ABS abs, float axis) = 0;
    virtual int ResetABS(ABS abs) = 0;
    virtual void DisconnectController() = 0;
};