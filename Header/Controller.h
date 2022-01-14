#pragma once
#include <iostream>

enum ABS { L_X_AXIS, L_Y_AXIS, R_X_AXIS, R_Y_AXIS, L2, R2, CLEAR };
enum Buttons { A, B, X, Y, START, SELECT, L1, R1, L3, R3, DPAD_UP, DPAD_DOWN, DPAD_LEFT, DPAD_RIGHT, B_CLEAR };

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