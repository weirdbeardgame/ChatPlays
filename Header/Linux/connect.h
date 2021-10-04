#pragma once
#include <filesystem>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <string.h>
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
    bool openSockFile(fs::path socket, uint slot);
    char* recieve();
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