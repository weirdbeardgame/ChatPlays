#pragma once
#include <iostream>
#include <stdio.h>
#ifdef __linux__
#include "Linux/connect.h"
#elif _WIN32
#include "Windows/winConnect.h"
#endif

#include "json.hpp"
#include "message.h"
#include "json.hpp"

using json = nlohmann::json;

struct TwitchInfo
{
    std::string userName;
    std::string oauthToken;
    std::string channelName;

    json twitch;

    TwitchInfo();
    TwitchInfo(nlohmann::json &j);

    TwitchInfo *InitalConfig();
    void Save(nlohmann::json &j, bool isDefault = false);
    void Load(nlohmann::json &j);

    friend void to_json(nlohmann::json &j, const TwitchInfo &p);
    friend void from_json(const nlohmann::json &j, TwitchInfo &p);
};

class Twitch
{
private:
    std::string buffer;
    std::string address = "irc.chat.twitch.tv";
    std::string pong = "PONG :tmi.twitch.tv\r\n";

    Connect connection;

    Message *queue;
    TwitchInfo settings;

    bool isJoined = false;

public:
    Twitch() = default;
    Twitch(Message *q)
    {
        queue = q;
    }
    bool login(Message *q, TwitchInfo *s);

    void StartTwitchThread(Message *q, TwitchInfo *s);

    bool ParseCommand(std::string command);

    bool update();
    // void exit();
};