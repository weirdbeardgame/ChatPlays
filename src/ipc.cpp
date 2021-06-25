#include "ipc.h"

bool IPC::open(fs::path socketPath, char* slot)
{
    socketPath.replace_extension(".sock");
    return connect.open(socketPath.c_str(), slot);
}

std::string IPC::gameTitle()
{

}
