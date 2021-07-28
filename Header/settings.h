#pragma once
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#ifdef __linux__ 
#include "Linux/control.h"
#include "Linux/connect.h"
#elif _WIN32
#include "Windows/winConnect.h"
#include "Windows/control.h"
#endif

#include "twitch.h"
#include "json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

class Settings
{
    private:
    std::fstream fileStream;
    fs::path filePath;
    Emit controllerSettings;
    TwitchInfo twitchSettings;
    json j;

    public:
    Settings();
    Settings(fs::path fileName);
    void init();
    void edit();

    bool load(fs::path fileName);
    bool save(fs::path fileName);

};