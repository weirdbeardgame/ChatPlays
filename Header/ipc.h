#pragma once
#ifdef __linux__ 
#include "Linux/control.h"
#include "Linux/connect.h"
#elif _WIN32
#include "Windows/winConnect.h"
#include "Windows/control.h"
#endif

class IPC
{
    private:
    Connect connect;
    std::vector<uint32_t> mem;
    public:
    bool open(fs::path socketPath, int slot);

    uint8_t read8();
    uint16_t read16();
    uint32_t read32();
    uint64_t read64();

    std::string gameTitle();
};