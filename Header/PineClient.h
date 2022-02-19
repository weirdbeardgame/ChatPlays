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

using namespace PINE;

// ToDo: Write your own implementation of PINE for whatever game or emulator software you're using
class PineClient
{
private:
	// This represents the active emulator. Use polymorphisim. There's class PCSX2, and class RPCS3 etc.
	Shared *emulator;

	// ToDo Add Tas.

	// This is a map of potential commands and functions that execute them or. You can use a switch case in the run function to parse the command.
	std::map<std::string, void*> commands;
public:
	void StartPineThread();
	void Run();
};

