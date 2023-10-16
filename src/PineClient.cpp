#include "PineClient.h"

void PineClient::Open(std::string emu, int port)
{
    emulator = new Shared(port, emu, true);
}

void PineClient::Run()
{
}

bool PineClient::WriteStruct(uint32_t adr, void *st, size_t size)
{
    while (size > 0)
    {
        // emulator->Write()
    }

    return true;
}

template <class T>
T PineClient::ReadStruct(uint32_t adr, size_t size)
{
    uint32_t bytes;
    while ((size - bytes) > 0)
    {
    }
}
