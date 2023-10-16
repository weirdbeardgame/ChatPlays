#include "Settings.h"
#include <stdio.h>

Settings::Settings()
{
    filePath = (CurrentExecuteablePath() / filePath);

    // Assume this is the first time configuration.
    if (!fs::exists(filePath))
    {
        if (FirstTime())
        {
            Load();
        }
        else
        {
            std::cerr << "Error writing config file, make sure you have space" << std::endl;
        }
    }
    else
    {
        Load();
    }
}

fs::path Settings::CurrentExecuteablePath()
{
    std::string path;
#ifdef __linux__
    path.reserve(PATH_MAX);
    int count = readlink("/proc/self/exe", path.data(), PATH_MAX);
    if (count != -1)
    {
        return fs::path(dirname(path.data()));
    }
#elif _WIN32
    path.reserve(MAX_PATH);
    GetModuleFileName(NULL, path.data(), MAX_PATH);
    return fs::canonical(path);
#endif
    return fs::path();
}

bool Settings::FirstTime()
{
    std::cout << "Twitch or Discord? T for twitch, D for discord" << std::endl;
    char command;
    std::cout << "> ";
    std::cin >> command;

    switch (tolower(command))
    {
    case 't':
        break;

    case 'd':
        break;
    }

    return false;
}

void Settings::Edit()
{
    char command;

    std::cout << "Avalible Commands: " << std::endl
              << "T: Twitch settings" << std::endl
              << "D: Discord settings"
              << "B: Back" << std::endl;
    std::cout << "> ";
    std::cin >> command;

    switch (std::tolower(command))
    {
    case 't':
        break;
    case 'd':
        break;
    case 'b':
        return;
        break;
    }

    Save();
}

bool Settings::Load()
{
    if (fs::exists(filePath))
    {
        std::fstream fileStream(filePath, std::ios::in);
        j = j.parse(fileStream);
        settings = j;
    }
    else
    {
        Save();
    }
    return true;
}

bool Settings::Save()
{
    bool isSaved = false;

    while (!isSaved)
    {
        if (!fs::exists(filePath.parent_path()))
        {
            fs::create_directory(filePath.parent_path());
        }
        else if (!fs::exists(filePath))
        {
            std::fstream fileStream(filePath, std::ios::out);
            fileStream << std::setw(4) << j << std::endl;
            fileStream.close();
            isSaved = fileStream.good();
        }
        else
        {
            std::fstream fileStream(filePath, std::ios::out);
            fileStream << std::setw(4) << j << std::endl;
            fileStream.close();
            isSaved = fileStream.good();
        }
    }

    return isSaved;
}