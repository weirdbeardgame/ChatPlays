#include <iostream>
#include "twitch.h"

int main()
{
    /*
    Control controller;
    std::string keyCode;
    bool input = controller.CreateController();
    std::cout << "Start " << input << std::endl;
    while(input)
    {
        std::cout << "Enter a keycode: ";
        std::cin >> keyCode;
        input += controller.emit(controller.GetCommands(keyCode));
    }*/

    Twitch connection;

    bool isActive = connection.login();

    while (isActive)
    {
        isActive = connection.update();
    }
    return 0;
}