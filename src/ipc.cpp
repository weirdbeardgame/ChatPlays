#include "ipc.h"

bool IPC::open(fs::path socketPath, int slot)
{
    socketPath.replace_extension(".sock");
    return connect.openSockFile(socketPath.string().c_str(), slot);
}

std::string IPC::gameTitle()
{
    return " ";
}


uint8_t IPC::read8()
{
    
}
