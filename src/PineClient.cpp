#include "PineClient.h"

void PineClient::Open(std::string emu, int port)
{
    emulator = new Shared(port, emu, true);
}

void PineClient::Run()
{
}