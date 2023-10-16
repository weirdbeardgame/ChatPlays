#pragma once
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#ifdef __linux__
#include "Linux/LinuxSock.h"
// The below to get the current working executable's directory
#include <libgen.h>
#include <unistd.h>
#include <linux/limits.h>
#elif _WIN32
#include "Windows/winConnect.h"
#endif

#include "Oauth.h"
#include "json.hpp"

namespace fs = std::filesystem;

struct SettingsData
{
    std::string userName;
    std::string oauthToken;
    std::string channelName;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SettingsData, userName, oauthToken, channelName);

class Settings
{
private:
    // Below are basic properties for generating a settings file and folder
    fs::path filePath = "settings/settings.json";
    nlohmann::json j;
    SettingsData settings;
    fs::path CurrentExecuteablePath();

public:
    Settings();
    bool FirstTime();
    void Edit();
    bool Load();
    bool Save();

    SettingsData *GetSettings() { return &settings; }
};