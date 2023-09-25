#pragma once
#include <iostream>
#include <string.h>
#include <map>
#include "PineClient.h"
#include "Controller.h"
#include "twitch.h"

enum CommandTypes
{
    TWITCH,
    CONTROLLER,
    PINE_CMD
};

std::map<CommandTypes, std::string>
    TwitchCommands{
        {CommandTypes::TWITCH, "join"},
        {CommandTypes::TWITCH, "disconnect"},
        {CommandTypes::TWITCH, "ban"},
        {CommandTypes::TWITCH, "unban"},
        {CommandTypes::TWITCH, "clear"},
        {CommandTypes::TWITCH, "subscribers"},
        {CommandTypes::TWITCH, "subscribersoff"}};

class Command
{

private:
    Controller *controls;
    Twitch twitch;
    PineClient pine;

public:
    bool Parse(std::string cmd);
};