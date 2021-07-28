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
    template <typename T>
    int sendBytes(T val, int siz)
    {
        if (sock <= 0)
        {
            std::cerr << "Connection terminated" << std::endl;
            return -1;
        }

        char* buffer = (char*)&val;

        int size = send(sock, buffer, siz, 0);
        if (size < 0)
        {
            std::cerr << "Send Err: " << strerror(errno) << std::endl;
            return false;
        }
        return size;
    }
    bool httpGet();
    bool httpPost();
    std::string parseCommand(std::string s);
    bool isConnected();
    void disconnect();
};
