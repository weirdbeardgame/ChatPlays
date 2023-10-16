#pragma once
#include <iostream>
#include <stdio.h>
#ifdef __linux__
#include "Linux/LinuxSock.h"
#elif _WIN32
#include "Windows/WinConnect.h"
#endif
#include "Settings.h"
#include "Buffer.h"

class Twitch
{
private:
    std::string buffer;
    std::string address = "irc.chat.twitch.tv";
    std::string pong = "PONG :tmi.twitch.tv\r\n";

    std::vector<std::string>
        Commands{
            "join",
            "Disconnect",
            "ban",
            "unban",
            "clear",
            "subscribers",
            "subscribersoff"};

    Connect *connection;

    Buffer *queue;
    SettingsData settings;

    bool isJoined = false;

public:
    Twitch() = default;
    bool login(SettingsData *s);

    void StartTwitchThread(SettingsData *s);

    bool ParseCommand(std::string command);

    bool update();
    // void exit();
};