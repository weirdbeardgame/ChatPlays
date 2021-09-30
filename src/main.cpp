#include <iostream>
#include "settings.h"
#include "message.h"

static Twitch* twitchConnect;
static Emit* controller;

void twitch()
{
    bool isActive = twitchConnect->login();
    while(isActive)
    {
        isActive = twitchConnect->update();
    }
}

void manualControl()
{
    Connect connection;
    bool input = controller->CreateController();
    std::cout << "Start " << input << std::endl;
    std::thread th(&Emit::CreateController, Emit());
    th.join();
}

int main()
{
    twitchConnect = new Twitch();
    controller = new Emit();
    Settings* settings = new Settings(controller, &twitchConnect->setting);
    bool isActive = true;
    char command;

    std::cout << "Avalible Commands: " << std::endl
    << "t: Twitch" << std::endl
    << "c: Manually control bot" << std::endl
    << "s: Edit Settings" << std::endl;

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
                settings->init();
                break;

            default:
                std::cerr << "Err: Unrecognized Command" << std::endl;
                break;
        }
    }
    return 0;
}