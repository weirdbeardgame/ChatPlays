#pragma once
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#ifdef __linux__
#include "Linux/Uinput.h"
#include "Linux/connect.h"
#elif _WIN32
#include "Windows/winConnect.h"
#include "Windows/Xinput.h"
#endif

#include "twitch.h"
#include "json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

class Settings
{
private:
    // ToDo: Fix path handling to generate settings folder in the executeable's directory. Missing a gui libary makes finding that a challenge.
    fs::path filePath = "settings/settings.json";
    fs::path executeable;
    Emit *controllerSettings;
    TwitchInfo *twitchSettings;
    json j;

public:
    Settings();
    Settings(TwitchInfo &t);
    void edit();

    bool load();
    bool save();
};