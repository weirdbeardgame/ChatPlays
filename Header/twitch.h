#pragma once
#include <iostream>
#include <stdio.h>
#include "control.h"
#include "connect.h"
#include "json.hpp"

using json = nlohmann::json;

struct TwitchInfo
{
    std::string userName;
    std::string oauthToken;
    // This will typically be your twitch username unless you're trying to join someone elses channel
    std::string channelName;
    json twitch;

    TwitchInfo();
    TwitchInfo(nlohmann::json &j);
    void save(nlohmann::json &j, bool isDefault = false);
    friend void to_json(nlohmann::json& j, const TwitchInfo& p);
    friend void from_json(const nlohmann::json& j, TwitchInfo& p);
};

class Twitch
{
    private:
    TwitchInfo setting;
    std::string address = "irc.chat.twitch.tv";
    std::string pong = "PONG :tmi.twitch.tv";
    Connect connection;
    Control controller;
    std::string buffer;
    bool isJoined;
    public:
    bool login();
    // bool open(std::string address for other service integration like discord?
    // recieve commands from chat and parse
    bool update(); 
    void exit();

};