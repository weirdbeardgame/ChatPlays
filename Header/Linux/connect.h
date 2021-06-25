#pragma once
#include <filesystem>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <iostream>
#include <stdio.h>
#include <netdb.h>
#include <vector>

namespace fs = std::filesystem;

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
    bool openSockFile(fs::path socket, int slot);
    bool recieve(std::string &buff);
    bool sendAll(std::string buf);
    bool httpGet();
    bool httpPost();
    std::string parseCommand(std::string s);
    bool isConnected();
    void disconnect();
};