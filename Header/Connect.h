#pragma once
#include <filesystem>
#include <string.h>
#include "Buffer.h"
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <vector>

namespace fs = std::filesystem;

class Connect
{
private:
    int sock;
    bool isConnected;

public:
    virtual bool Open(const char *hostName, const char *port) = 0;
    virtual void Disconnect() = 0;
    virtual char *Recieve() = 0;
    virtual bool Send(std::string buf, int size) = 0;
    virtual std::string ParseCommand(std::string s) = 0;
    bool IsConnected() { return isConnected; }
};