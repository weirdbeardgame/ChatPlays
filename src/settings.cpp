#include "settings.h"

Settings::Settings()
{
    j = json();
}

Settings::Settings(Emit* c, TwitchInfo* t)
{
    twitchSettings = t;
    controllerSettings = c;
    load();
}

void Settings::init()
{
    char command;
    std::cout << "Avalible Commands are: " << std::endl << "e: Edit settings" << std::endl
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
    char command;


    std::cout << "T: twitch settings" << std::endl << "C: controller settings" << std::endl;
    std::cin >> command;

    switch (command)
    {
        case 'T':
            if (twitchSettings == nullptr)
            {
                twitchSettings = new TwitchInfo();
            }
            twitchSettings->Save(j, true);
            break;
        case 'C':
            if (controllerSettings == nullptr)
            {
                controllerSettings = new Emit();
            }
            controllerSettings->initalConfig();
            break;
    }

    if (!fs::exists(filePath))
    {
        save();
    }
}

bool Settings::load()
{
    if (fs::exists(filePath))
    {
        fileStream.open(filePath, std::ios::in);
        j = j.parse(fileStream);
        twitchSettings->Load(j); //Psudo load function?
        controllerSettings = new Emit(j);
    }
    else
    {
        save();
    }
    return true;
}

bool Settings::save()
{
    if (!fs::exists(filePath.parent_path()))
    {
        fs::create_directory(filePath.parent_path());
    }
    else if (!fs::exists(filePath))
    {
        twitchSettings = new TwitchInfo();
        controllerSettings = new Emit();

        twitchSettings->Save(j, true);

        controllerSettings->initalConfig();
        controllerSettings->save(j);

        fileStream.open(filePath, std::ios::out);
        fileStream << std::setw(4) << j << std::endl;
        return fileStream.fail();
    }
    else
    {
        twitchSettings->Save(j);
        controllerSettings->save(j);

        fileStream.open(filePath, std::ios::out);
        fileStream << std::setw(4) << j << std::endl;

        return fileStream.fail();
    }
}
