#include "twitch.h"

TwitchInfo::TwitchInfo(nlohmann::json j)
{
    from_json(j, *this);
}

void TwitchInfo::to_json(nlohmann::json& j, const TwitchInfo& p)
{
    j
    = nlohmann::json
    { 
        {"userName", p.userName}, 
        {"oauthToken", oathToken}, 
        {"channelName", channelName}
    };
}

void TwitchInfo::from_json(const nlohmann::json& j, TwitchInfo& p)
{
    j.at("userName").get_to(userName);
    j.at("oauthToken").get_to(oathToken);
    j.at(channelName).get_to(channelName);
}

bool Twitch::login()
{
    controller.CreateController();
    if (connection.open(address.c_str(), "6697"))
    {
        if (!connection.sendAll("PASS " + setting.oathToken + "\r\n"))
        {
            return false;
        }

        if (!connection.sendAll("NICK " + setting.userName + "\r\n"))
        {
            return false;
        }
    }

    if (!connection.recieve(buffer))
    {
        return false;
    }

    if (!isJoined)
    {
        if (!connection.sendAll("JOIN #" + setting.channelName + "\r\n"))
        {
            return false;
        }

        if (!connection.sendAll("CAP REQ :twitch.tv/membership"))
        {
            return false;
        }

        isJoined = true;
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
            std::cout << "PING RECIEVED" << std::endl;
            connection.sendAll(pong);
        }

        if(controller.emit(controller.GetCommands(com)))
        {
            continue;
        }

        std::flush(std::cout);
    }
    return true;
}
