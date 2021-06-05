#include <iostream>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "control.h"

class Twitch
{
    private:
    int sock;
    int portno;
    sockaddr_in sockIn;
    hostent *host;
    addrinfo info, *infoP;
    std::string address = "wss://irc-ws.chat.twitch.tv";
    // IRC Ping Pong game!
    std::string repsonse = "PONG :tmi.twitch.tv";
    std::string portString;
    Control controller;
    std::string outputBuffer;
    public:
    bool open();
    // bool open(std::string address for other service integration like discord?
    // recieve commands from chat and parse
    bool input(); 
    void close();

};