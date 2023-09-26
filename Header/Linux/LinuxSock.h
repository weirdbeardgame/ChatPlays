#pragma once
#include <filesystem>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include "Connect.h"
namespace fs = std::filesystem;

class Socket : public Connect
{
private:
    int sock;
    sockaddr_in sockIn;
    hostent *host;
    addrinfo info, *infoP;
    timeval tv;
    fd_set set;

public:
    virtual bool Open(const char *hostname, const char *port) override;
    virtual char *Recieve() override;
    virtual bool Send(std::string buf, int size) override;
    virtual std::string ParseCommand(std::string s) override;
    virtual void Disconnect() override;
};