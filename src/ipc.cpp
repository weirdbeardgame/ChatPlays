#include "ipc.h"
#include <memory.h>

bool IPC::open(fs::path socketPath, uint8_t slot)
{
    socketPath.replace_extension(".sock");
    return connect.openSockFile(socketPath.string().c_str(), char(slot));
}

std::string IPC::gameTitle()
{
    return " ";
}

uint8_t IPC::read8(int pos)
{
    // extract value out of buffer and return that
    //uint32_t *temp = static_cast<uint32_t*>(&buffer[pos]);
    uint8_t value = 0;

    // Host Endianess
    //memcpy(&value, &temp, sizeof(value));
    return value;
}

uint16_t IPC::read16(int pos)
{
    uint32_t temp = static_cast<uint32_t>(buffer[pos]);
    uint16_t value = static_cast<uint16_t>(temp);
    return value;
}

uint32_t IPC::read32(int pos)
{
    return static_cast<uint32_t>(buffer[pos]);
}

uint64_t IPC::read64(int pos)
{
    uint32_t temp = static_cast<uint32_t>(buffer[pos]);
    uint64_t val = temp;
    return val;
}

void IPC::write8(uint8_t val)
{
    connect.sendBytes<uint8_t>(val, sizeof(val));
}

void IPC::write16(uint16_t val)
{
    connect.sendBytes<uint16_t>(val, sizeof(val));
}

void IPC::write32(uint32_t val)
{
    connect.sendBytes<uint32_t>(val, sizeof(val));
}

void IPC::write64(uint64_t val)
{
    connect.sendBytes<uint64_t>(val, sizeof(val));
}