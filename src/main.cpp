#include <iostream>
#include "settings.h"

void twitch()
{
    Twitch twitch;

    bool isActive = twitch.login();
    while(isActive)
    {
        isActive = twitch.update();
    }
}

void manualControl()
{
    Emit controller;
    Connect connection;
    bool input = controller.CreateController();
    std::cout << "Start " << input << std::endl;
    // This is expecting a different device
    std::thread th(&Emit::CreateController, Emit());
    th.join();
}

int main()
{
    Settings settings;
    bool isActive = true;
    char command;

    std::cout << "Avalible Commands: " << std::endl
    << "t: Twitch" << std::endl
    << "c: Manually control bot" << std::endl
    << "s: Edit Settings" << std::endl;

    settings.load("settings/settings.json");

    while(isActive)
    {
        std::cout << "Enter Command: ";
        std::cin >> command;

        switch (command)
        {
            case 't':
                twitch();
                break;

            case 'c':
                manualControl();
                break;

            case 's':
                settings.init();
                break;

            default:
                std::cerr << "Err: Unrecognized Command" << std::endl;
                break;
        }
    }
    return 0;
}