#include "twitch.h"

TwitchInfo::TwitchInfo()
{
    twitch = nlohmann::json
    {
        "TwitchInfo" , 
        {
            {"userName", "USERNAME HERE"}, 
            {"oauthToken", "OAUTH HERE"}, 
            {"channelName", "CHANNEL_NAME HERE"}
        }
    };
}


TwitchInfo::TwitchInfo(json &j)
{
    from_json(j, *this);
}

void TwitchInfo::save(json &j, bool isDefault)
{
    if (isDefault)
    {
        TwitchInfo t = TwitchInfo();
        j.push_back(t.twitch);
    }
    else
    {
        twitch = *this;
        j.push_back(twitch);
        
    }
}

void to_json(json& j, const TwitchInfo& p)
{
    j = nlohmann::json
    {
        "TwitchInfo" , 
        {
            {"userName", p.userName}, 
            {"oauthToken", p.oauthToken}, 
            {"channelName", p.channelName}
        }
    };
}

void from_json(const nlohmann::json& j, TwitchInfo& p)
{
    j.at("userName").get_to(p.userName);
    j.at("oauthToken").get_to(p.oauthToken);
    j.at("channelName").get_to(p.channelName);
}

bool Twitch::login()
{
    controller.CreateController();
    if (connection.open(address.c_str(), "6697"))
    {
        if (!connection.sendAll("PASS " + setting.oauthToken + "\r\n"))
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
