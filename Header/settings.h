#pragma once
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "control.h"
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
    Settings(std::string fileName);
    void init();
    void edit();

    bool load(std::string fileName, std::string delimit = " ");
    bool save(std::string fileName);

};