#pragma once
#include <filesystem>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>

namespace fs = std::filesystem;

class Connect
{
    int sock;
    sockaddr_in sockIn;
    hostent* host;
    addrinfo info, * infoP;
    timeval tv;
    fd_set set;

    std::thread th;

public:
    bool open(const char* hostName, const char* port);
    bool openSockFile(fs::path socket, char slot);
    char* recieve();
    bool sendAll(std::string buf);
    inline int sendBytes(const char* val, int siz)
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
    std::string parseCommand(std::string s);
    bool isConnected();
    void disconnect();
};
