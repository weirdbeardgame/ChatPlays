#pragma once
#include <iostream>
#include <stdio.h>
#include "control.h"
#include "connect.h"

class Twitch
{
    private:
    std::string address = "irc.chat.twitch.tv";
    // IRC Ping Pong game!
    std::string pong = "PONG :tmi.twitch.tv";
    std::string password = "oauth:h0afs2o2pu7idj4feqs3krdroovobv";
    Connect connection;
    Control controller;
    std::string buffer;
    public:
    bool login();
    // bool open(std::string address for other service integration like discord?
    // recieve commands from chat and parse
    bool update(); 
    void exit();

};