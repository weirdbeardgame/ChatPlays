#pragma once
#include "connect.h"

class IPC
{
    private:
    Connect connect;
     std::vector<uint32_t> mem;
    public:
    bool open(fs::path socketPath, char* slot);

    uint8_t read8();
    uint16_t read16();
    uint32_t read32();
    uint64_t read64();

    std::string gameTitle();
};