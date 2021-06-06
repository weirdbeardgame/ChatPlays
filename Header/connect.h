#pragma once
#include <iostream>
#include <stdio.h>
#include <netdb.h>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


class Connect
{
    private:
    int sock;
    sockaddr_in sockIn;
    hostent *host;
    addrinfo info, *infoP;

    public:
    bool open(const char* hostName, char* port);
    bool recieve(std::vector<char> &buff);
    bool sendAll(std::string buf);
    void disconnect();


};