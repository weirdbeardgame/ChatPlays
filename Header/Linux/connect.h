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
    bool openSockFile(fs::path socket, uint slot);
    char* recieve();
    template<typename T>
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
            //std::cerr << "Send Err: " << strerror(errno) << std::endl;
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