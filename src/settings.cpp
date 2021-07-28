#include "settings.h"

Settings::Settings()
{
    j = json();
    filePath = std::string();
}

Settings::Settings(fs::path fileName)
{
    filePath = fileName;
    j = json(filePath.make_preferred());
}

void Settings::init()
{
    char command;
    std::cout << "Avalible Commands are: " << std::endl;
    std::cout << "e: Edit settings" << std::endl
    << "q: Quit" << std::endl; 
    std::cin >> command;
    switch (command)
    {
        case 'e':
            edit();
            break;

        case 'q':
            exit(0);
            break;

        default:
            std::cerr << "Unrecognized command" << std::endl;
            break;
    }
}

void Settings::edit()
{
    json j;
    filePath = "settings/settings.json";
    if (!fs::exists(filePath))
    {
        save(filePath);
    }
}

bool Settings::load(fs::path fileName)
{
    filePath = fileName;
    if (fs::exists(filePath))
    {
        fileStream.open(filePath, std::ios::in);
        j = j.parse(fileStream);
        twitchSettings = TwitchInfo(j["TwitchInfo"]); //Psudo load function?
        controllerSettings = Emit(j["Emit"]);
    }
    else
    {
        save(filePath);
    }
    return true;
}

bool Settings::save(fs::path fileName)
{
    std::cout << "Save" << std::endl;
    if (!fs::exists(filePath.parent_path()))
    {
        fs::create_directory(filePath.parent_path());
    }
    else if (!fs::exists(filePath))
    {
        twitchSettings.save(j, true);
        //controllerSettings.initalConfig();
        //controllerSettings.save(j, true);
        fileStream.open(filePath, std::ios::out);
        fileStream << std::setw(4) << j << std::endl;
        return fileStream.fail();
    }
    else
    {
        twitchSettings.save(j);
        controllerSettings.save(j);
        fileStream.open(fileName, std::ios::out);
        fileStream << std::setw(4) << j << std::endl;
        return fileStream.fail();
    }
}
