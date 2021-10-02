#include <iostream>
#include "settings.h"
#include "message.h"
#include "twitch.h"

void twitch()
{
    // Why can't the compiler resolve this function?
    std::thread th(&Twitch::create);
    th.join();
}

void manualControl()
{
    std::thread th(&Emit::CreateController, Emit());
    th.join();
}

int main()
{
    //twitchConnect = new Twitch();
    //controller = new Emit();
    //Settings* settings = new Settings(controller, &twitchConnect->setting);
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
                //settings->init();
                break;

            default:
                std::cerr << "Err: Unrecognized Command" << std::endl;
                break;
        }
    }
    return 0;
}