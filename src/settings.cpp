#include "settings.h"

Settings::Settings()
{
    j = json();
}

Settings::Settings(Emit& c, TwitchInfo& t)
{
    // First we want to get the path to the executeable
#ifdef _WIN32
    char temp[1050];
    GetModuleFileNameA(NULL, &temp[0], 1050);
    executeable = temp;

#elif __linux__
    //readlink();
#endif

    filePath = (executeable.parent_path() / filePath);

    load();
    t = *twitchSettings;
    c = *controllerSettings;
}

void Settings::edit()
{
    json j;
    char command;

    std::cout << "Avalible Commands: " << std::endl << "T: twitch settings" << std::endl << "C: controller settings" << "B: Back" << std::endl;
    std::cout << "> ";
    std::cin >> command;

    switch (std::tolower(command))
    {
        case 't':
            if (twitchSettings == nullptr)
            {
                twitchSettings = new TwitchInfo();
            }
            twitchSettings = twitchSettings->InitalConfig();
            break;
        case 'c':
            if (controllerSettings == nullptr)
            {
                controllerSettings = new Emit();
            }
            controllerSettings = controllerSettings->InitalConfig();
            break;
        case 'b':
            return;
            break;
    }

    save();

}

bool Settings::load()
{
    if (fs::exists(filePath))
    {
        std::fstream fileStream(filePath, std::ios::in);
        j = j.parse(fileStream);

        twitchSettings = new TwitchInfo();
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
    bool isSaved = false;

    while (!isSaved)
    {
        if (!fs::exists(filePath.parent_path()))
        {
            fs::create_directory(filePath.parent_path());
        }
        else if (!fs::exists(filePath))
        {
            twitchSettings = new TwitchInfo();
            controllerSettings = new Emit();

            twitchSettings = twitchSettings->InitalConfig();
            twitchSettings->Save(j, false);

            controllerSettings = controllerSettings->InitalConfig();
            controllerSettings->save(j, false);

            std::fstream fileStream(filePath, std::ios::out);
            fileStream << std::setw(4) << j << std::endl;
            fileStream.close();
            isSaved = fileStream.good();
        }
        else
        {
            twitchSettings->Save(j);
            controllerSettings->save(j);

            std::fstream fileStream(filePath, std::ios::out);
            fileStream << std::setw(4) << j << std::endl;
            fileStream.close();
            isSaved = fileStream.good();
        }
    }

    return isSaved;
}
