#pragma once
#include <iostream>

#include "settings.h"
#include "pine.h"
#include "twitch.h"

#ifdef __linux__ 
#include "Linux/control.h"
#elif _WIN32
#include "Windows/control.h"
#endif

// ToDo: Write your own implementation of PINE for whatever game or emulator software you're using
class PineClient
{

private:

public:

};

