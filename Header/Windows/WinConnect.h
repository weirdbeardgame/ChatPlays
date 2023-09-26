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
    virtual bool openSockFile(fs::path socket, char slot) override;
    virtual char *Recieve() override;
    virtual bool Send(std::string buf) override;
    inline int sendBytes(const char *val, int siz)
    {
        if (sock <= 0)
        {
            std::cerr << "Connection terminated" << std::endl;
            return -1;
        }

        std::cout << "Buffer: " << val << std::endl;

        int size = send(sock, val, siz, 0);
        if (size <= 0)
        {
            std::cerr << "Send Err: " << strerror(errno) << std::endl;
            return false;
        }
        std::cout << "Size: " << size << std::endl;
        return size;
    }
    bool httpGet();
    bool httpPost();
    std::string ParseCommand(std::string s);
    bool isConnected();
    void Disconnect();
};
