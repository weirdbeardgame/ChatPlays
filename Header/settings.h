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
    // ToDo: Fix path handling to generate settings folder in the executeable's directory. Missing a gui libary makes finding that a challenge.
    fs::path filePath = "settings/settings.json";
    Emit* controllerSettings;
    TwitchInfo* twitchSettings;
    json j;

    public:
    Settings();
    Settings(Emit* c, TwitchInfo* t);
    void init();
    void edit();

    bool load();
    bool save();

};