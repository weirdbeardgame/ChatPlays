#include "twitch.h"

bool Twitch::login()
{
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
    return true;
}

bool Twitch::update()
{

    if (!connection.sendAll("JOIN #weirdbeardgame\n"))
    {
        return false;
    }

    if (connection.recieve(buffer))
    {
        std::string out (buffer.begin(), buffer.end());

        if (out.find("PING :tmi.twitch.tv"))
        {
            connection.sendAll(pong);
        }

        if (out.find("up"))
        {
            controller.emit(UP);
        }

        if (out.find("down"))
        {
            controller.emit(DOWN);
        }

        if (out.find("left"))
        {
            controller.emit(LEFT);
        }

        if (out.find("right"))
        {
            controller.emit(RIGHT);
        }

        if (out.find("a"))
        {
            controller.emit(A);
        }

        if (out.find("b"))
        {
            controller.emit(B);
        }

        if (out.find("x"))
        {
            controller.emit(X);
        }

        if (out.find("y"))
        {
            controller.emit(Y);
        }

        if (out.find("start"))
        {
            controller.emit(START);
        }

        if (out.find("select"))
        {
            controller.emit(SELECT);
        }

        std::cout << out << std::endl;
        out.clear();
    }
}
