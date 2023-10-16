#pragma once
#include <filesystem>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/http_listener.h>
#include <cpprest/oauth2.h>
#include <string.h>
#include "Buffer.h"
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <vector>

namespace fs = std::filesystem;

// Rest namespaces
using namespace utility;              // Common utilities like string conversions
using namespace web;                  // Common features like URIs.
using namespace web::http;            // Common HTTP functionality
using namespace web::http::client;    // HTTP client features
using namespace concurrency::streams; // Asynchronous streams
using namespace web::http::experimental::listener;

class Connect
{
private:
    int sock;
    bool isConnected;

public:
    virtual bool Open(const char *hostName, const char *port) = 0;
    virtual void Disconnect() = 0;
    virtual char *Recieve() = 0;
    virtual int Send(std::string buf, int size) = 0;
    virtual std::string ParseCommand(std::string s) = 0;
    bool IsConnected() { return isConnected; }
};