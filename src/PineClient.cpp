#include "PineClient.h"

void PineClient::Open(std::string emu, int port)
{
    emulator = new Shared(port, emu, true);
    std::cout << "Game: " << emulator->GetGameTitle() << std::endl;
}

uint8_t PineClient::Read8(uint32_t adr)
{
    return emulator->Read<uint8_t>(adr);
}

uint16_t PineClient::Read16(uint32_t adr)
{
    return emulator->Read<uint16_t>(adr);
}

uint32_t PineClient::Read32(uint32_t adr)
{
    return emulator->Read<uint32_t>(adr);
}

uint64_t PineClient::Read64(uint32_t adr)
{
    return emulator->Read<uint64_t>(adr);
}

void PineClient::Write8(uint32_t adr, uint8_t val)
{
    emulator->Write<uint8_t>(adr, val);
}

void PineClient::Write16(uint32_t adr, uint16_t val)
{
    emulator->Write<uint16_t>(adr, val);
}

void PineClient::Write32(uint32_t adr, uint32_t val)
{
    emulator->Write<uint32_t>(adr, val);
}

void PineClient::Write64(uint32_t adr, uint64_t val)
{
    emulator->Write<uint64_t>(adr, val);
}
