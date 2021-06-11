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
    Control controller;
    Connect connection;
    
    std::string keyCode;
    std::string delimited;
    bool input = controller.CreateController();

    std::cout << "Start " << input << std::endl;

    while(input)
    {
        std::cout << "Enter a keycode: ";
        std::cin >> keyCode;
        delimited = connection.parseCommand(keyCode);
        input += controller.emit(controller.GetCommands(delimited));
    }
}


int main()
{
    Settings settings;
    std::string command;
    std::cout << "Avalible Commands: " << std::endl;

    if (command == "Twitch" || "twitch")
    {
        twitch();
    }
    else if (command == "Control" || "control")
    {
        manualControl();
    }
    if (command == "Settings" || "settings")
    {
        settings.init();
    }
    else
    {
        std::cerr << "Err: Unrecognized Command" << std::endl;
    }

    return 0;
}