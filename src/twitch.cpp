#include "twitch.h"
#include <errno.h>

void Twitch::StartTwitchThread(SettingsData *s)
{
    if (login(s))
    {
        update();
    }
}

bool Twitch::login(SettingsData *s)
{
    settings = *s;
    connection = new Socket();
    if (connection->Open(address.c_str(), "6667"))
    {
        std::string buf1 = ("PASS " + settings.oauthToken + "\r\n");

        std::cout << "Buff: " << buf1 << std::endl;
        if (connection->Send(buf1.data(), buf1.size()) <= 0)
        {
            std::cerr << "Send failed: " << strerror(errno) << std::endl;
            isJoined = false;
        }

        std::cout << "Msg: " << connection->Recieve() << std::endl;

        std::string buf2 = ("NICK " + settings.userName + "\r\n");
        std::cout << "Buff: " << buf2 << std::endl;

        if (connection->Send(buf2.c_str(), buf2.size()) <= 0)
        {
            std::cerr << "Send failed: " << strerror(errno) << std::endl;
            isJoined = false;
        }

        std::cout << "Msg: " << connection->Recieve() << std::endl;

        if (!isJoined)
        {
            std::string buf3 = ("JOIN #" + settings.channelName + "\r\n");
            std::cout << "Buff: " << buf3 << std::endl;
            if (connection->Send(buf3.c_str(), buf3.size()) < 0)
            {
                std::cerr << "Send failed: " << strerror(errno) << std::endl;
                isJoined = false;
            }

            if (std::string(connection->Recieve()).find(".tmi.twitch.tv JOIN #" + settings.channelName) != std::string::npos)
            {
                std::string buf4 = "CAP REQ :twitch.tv/membership\r\n";
                std::cout << "Buff: " << buf4 << std::endl;
                if (connection->Send(buf4.c_str(), buf4.size()) < 0)
                {
                    std::cerr << "Send failed: " << strerror(errno) << std::endl;
                    isJoined = false;
                }
                connection->Recieve();

                std::string buf5 = "CAP REQ :twitch.tv/commands\r\n";
                std::cout << "Buff: " << buf5 << std::endl;
                if (connection->Send(buf5.c_str(), buf5.size()) < 0)
                {
                    std::cerr << "Send failed: " << strerror(errno) << std::endl;
                    isJoined = false;
                }
                connection->Recieve();
                std::cout << "Channel Joined" << std::endl;
                isJoined = true;
            }
            else
            {
                std::cout << "Msg: " << connection->Recieve() << std::endl;
                isJoined = false;
            }
        }
    }
    return isJoined;
}

// Step two. Parse command and send through thread to controller or IPC alike.
bool Twitch::update()
{
    while (connection->IsConnected())
    {
        buffer += connection->Recieve();

        std::flush(std::cout);
    }
    return true;
}

// If it's a command process it! Don't push it to global thread queue needlessly! Enqueue in the recognized twitch queue of commands and push to controller.

bool Twitch::ParseCommand(std::string command)
{
    std::string com = connection->ParseCommand(buffer);

    if (buffer.find("PING :tmi.twitch.tv") != std::string::npos)
    {
        std::cout << "Pong: " << pong.c_str() << std::endl;
        if (!connection->Send(pong.c_str(), pong.size()))
        {
            std::cout << "Send Error" << std::endl;
            return false;
        }
    }

    com.clear();
    buffer.clear();

    return true;
}
