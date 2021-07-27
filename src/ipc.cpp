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

uint8_t IPC::read8()
{
    // extract value out of buffer and return that
    uint32_t temp = static_cast<uint32_t>(buffer[strlen(buffer)]);
    uint8_t value = static_cast<uint8_t>(temp); // I'm assuming this is very wrong but will do for now...
    return value;
}

uint16_t IPC::read16()
{
    uint32_t temp = static_cast<uint32_t>(buffer[strlen(buffer)]);
    uint16_t value = static_cast<uint16_t>(temp); // Same as above
    return value;
}

uint32_t IPC::read32()
{
    return static_cast<uint32_t>(buffer[strlen(buffer)]);
}