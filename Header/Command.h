#pragma once
#include <iostream>
#include <string.h>
#include "PineClient.h"
#include "Controller.h"
#include "twitch.h"

enum CommandTypes
{
    TWITCH,
    CONTROLLER,
    PINE_CMD
};

class Command
{

private:
    Controller *controls;
    Twitch twitch;
    PineClient pine;

public:
    bool Parse(std::string cmd, const char *delim);
};