#pragma once
#include <iostream>
#include <vector>

#ifdef __linux__
#include "Linux/Uinput.h"
#elif _WIN32
#include "Windows/Xinput.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#endif

// #include "settings.h"
#include "pine.h"
#include "twitch.h"

using namespace PINE;

// ToDo: Write your own implementation of PINE for whatever game or emulator software you're using
class PineClient
{
    // This represents the active emulator. Use polymorphisim. There's class PCSX2, and class RPCS3 etc.
    Shared *emulator;

    std::vector<Shared::BatchCommand> RecievedPackets;

public:
    void Open(std::string emu, int port);
    bool IsEmulatorOpen() { return emulator != nullptr; }

    uint8_t Read8(uint32_t adr);
    uint16_t Read16(uint32_t adr);
    uint32_t Read32(uint32_t adr);
    uint64_t Read64(uint32_t adr);

    // ToDo, need to actually pack struct instead of writing address
    template <typename T>
    bool WriteStruct(uint32_t adr, T st, int size)
    {
    }

    void Write8(uint32_t adr, uint8_t val);
    void Write16(uint32_t adr, uint16_t val);
    void Write32(uint32_t adr, uint32_t val);
    void Write64(uint32_t adr, uint64_t val);

    // Need to fix reading junk data
    template <typename T>
    T ReadStruct(uint32_t adr)
    {
        uint32_t *bytes;
        int amtToRead;
        T structToReturn;
        while (amtToRead < sizeof(T))
        {
            *bytes = emulator->Read<uint32_t>(adr);
            bytes++;
            amtToRead += 4;
        }
        structToReturn = *reinterpret_cast<T *>(bytes);
        return structToReturn;
    }
};
