#include "twitch.h"

bool Twitch::login()
{
    controller.CreateController();
    if (connection.open(address.c_str(), "6667"))
    {
        if (!connection.sendAll("PASS " + password + "\n"))
        {
            return false;
        }

        if (!connection.sendAll("NICK weirdbeardgame\n"))
        {
            return false;
        }
    }

    if (!connection.recieve(buffer))
    {
        return false;
    }

    if (!connection.sendAll("JOIN #weirdbeardgame\n"))
    {
        return false;
    }

    return true;
}

bool Twitch::update()
{
    while (connection.isConnected())
    {
        if (!connection.recieve(buffer))
        {
            return false;
        }
        std::cout << "Update" << std::endl;
        std::cout << buffer << std::endl;

        std::string com = connection.parseCommand(buffer);

        if (buffer.find("PING :tmi.twitch.tv"))
        {
            connection.sendAll(pong);
        }

        if(controller.emit(controller.GetCommands(com)))
        {
            continue;
        }

        std::flush(std::cout);
        //buffer.clear();
    }
    return true;
}
