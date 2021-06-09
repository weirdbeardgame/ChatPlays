#pragma once
#include <iostream>
#include <stdio.h>
#include <netdb.h>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>


class Connect
{
    private:
    int sock;
    sockaddr_in sockIn;
    hostent *host;
    addrinfo info, *infoP;
    timeval tv;
    fd_set set;

    public:
    bool open(const char* hostName, char* port);
    bool recieve(std::string &buff);
    bool sendAll(std::string buf);
    std::string parseCommand(std::string s);
    bool isConnected();
    void disconnect();
};