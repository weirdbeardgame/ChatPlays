#include <iostream>
#include "twitch.h"

int main()
{
/*
    Connect connection;
    Control controller;
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
    */

    Twitch connection;
    bool isActive = connection.login();

    while (isActive)
    {
        isActive = connection.update();
    }
    return 0;
}