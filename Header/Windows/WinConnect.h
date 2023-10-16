#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <filesystem>
#include "Connect.h"

namespace fs = std::filesystem;

#pragma comment(lib, "Ws2_32.lib")

class Socket : public Connect
{
    int sock;
    sockaddr_in sockIn;
    hostent *host;
    addrinfo info, *infoP;
    timeval tv;
    fd_set set;
    std::thread th;

public:
    virtual bool Open(const char *hostName, const char *port) override;
    virtual char *Recieve() override;
    virtual bool Send(std::string buf, int siz) override;
    std::string ParseCommand(std::string s);
    bool isConnected();
    void Disconnect();
};
