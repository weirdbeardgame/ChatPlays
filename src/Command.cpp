#include "Command.h"
#include <algorithm>

bool Command::Parse(std::string cmd, const char *delim)
{
    std::string command = strtok(cmd.data(), delim);
    // std::map<std::string, CommandTypes>::iterator it = Commands.find(command);
}