#include <iostream>
#include "control.h"

int main()
{
    Control controller;
    std::string keyCode;
    bool input = controller.init();
    std::cout << "Start " << input << std::endl;
    while(input)
    {
        std::cout << "Enter a keycode: ";
        std::cin >> keyCode;
        input += controller.emit(keyCode);
    }

    /*
    Twitch connection;

    bool isActive = connection.open();

    while (isActive)
    {
        // process input in here
        // connection.input(); 
    }
    */
    return 0;
}