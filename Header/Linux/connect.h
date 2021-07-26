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
    template<typename T>
    T recieve()
    {
        int i = 0;
        int buffSize = 512, buffRecieved = 0;
        T* buff = new T[512];
        while (buffRecieved < buffSize)
        {
            // not equal to catch neg error!!!
            i = recv(sock, buff + buffRecieved, buffSize, 0); 

            if (i == 0)
            {
                std::cerr << "Connection severed by server" << std::endl;
                return nullptr;
            }

            else if (i < 0)
            {
                std::cerr << "Recieve Err: " << gai_strerror(i) << std::endl;
                return nullptr;
            }
            if (buffSize < i)
            {
                buffSize = i;
                buff = new T[buffSize];
            }
            buffRecieved += i;
            buffSize -= i;
        }

        return *buff;
    }
    bool sendAll(std::string buf);
    bool httpGet();
    bool httpPost();
    std::string parseCommand(std::string s);
    bool isConnected();
    void disconnect();
};