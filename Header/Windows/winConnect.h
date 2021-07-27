#pragma once
#include <filesystem>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

class Connect
{
    int sock;
    sockaddr_in sockIn;
    hostent* host;
    addrinfo info, * infoP;
    timeval tv;
    fd_set set;

public:
    bool open(const char* hostName, char port);
    bool openSockFile(fs::path socket, char slot);
    char* read();
    bool sendAll(std::string buf);
    bool httpGet();
    bool httpPost();
    std::string parseCommand(std::string s);
    bool isConnected();
    void disconnect();
};
