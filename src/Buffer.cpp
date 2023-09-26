#include "Buffer.h"

void Buffer::Enqueue(char *str)
{
    buf.push_back(str);
}

std::string Buffer::Dequeue()
{
    std::string rtn = buf.back();
    buf.pop_back();
    return rtn;
}

std::string Buffer::GetCommand()
{
    if (commands.empty())
    {
        return " ";
    }

    std::string rtn = commands.front();
    commands.pop();
    return rtn;
}