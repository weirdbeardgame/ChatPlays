#pragma once
#include <iostream>
#include <vector>

// #include "settings.h"
#include "pine.h"
#include "twitch.h"

#ifdef __linux__
#include "Linux/Uinput.h"
#elif _WIN32
#include "Windows/Xinput.h"
#endif

using namespace PINE;

// ToDo: Write your own implementation of PINE for whatever game or emulator software you're using
class PineClient
{
private:
    // This represents the active emulator. Use polymorphisim. There's class PCSX2, and class RPCS3 etc.
    Shared *emulator;

    std::vector<Shared::BatchCommand> RecievedPackets;

public:
    void Open(std::string emu, int port);
    void Run();

    // ToDo, grab size of and subtract
    bool WriteStruct(uint32_t adr, void *st, size_t size);

    template <class T>
    T ReadStruct(uint32_t adr, size_t size);
};
