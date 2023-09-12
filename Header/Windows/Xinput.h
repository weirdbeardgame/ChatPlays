#pragma once
#define WIN32_LEAN_AND_MEAN

#include <map>
#include <mutex>
#include <thread>
#include <cstdint>
#include <windows.h>

#include <Xinput.h>
#include <iostream>
#include <filesystem>

#include "Controller.h"
#include "ViGEm/Client.h"

#pragma comment(lib, "setupapi.lib")

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
        {DPAD_UP, XUSB_GAMEPAD_DPAD_UP},
        {DPAD_DOWN, XUSB_GAMEPAD_DPAD_DOWN},
        {DPAD_LEFT, XUSB_GAMEPAD_DPAD_LEFT},
        {DPAD_RIGHT, XUSB_GAMEPAD_DPAD_RIGHT}};

// This needs to be a thread safe blocking class!
class Emit : public Controller
{
private:
    // I need Xinput
    PXUSB_REPORT report;
    PVIGEM_CLIENT driver;
    PVIGEM_TARGET xbox;

    // A crew vs Z crew
    int playerAmount;

public:
    Emit();

    // All the action is in here m8
    bool isActive = false;

    // This says hey! I have data bitch! Stop!
    std::mutex m;

    bool AttachController() override;

    bool SendInput(Buttons cmd, ABS a = ABS::CLEAR, float axis = 0, int heldFor = 0) override;

    int MoveABS(ABS abs, float axis) override;
    int ResetABS(ABS abs) override;

    int PressButton(Buttons b) override;
    int ReleaseButton(Buttons b) override;

    void DisconnectController() override;

    inline void SetPlayerAmount(int i)
    {
        if (i > 0)
        {
            playerAmount = i;
        }
    }

    ~Emit();
};