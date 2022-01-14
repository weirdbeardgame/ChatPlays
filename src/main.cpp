#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#include "Controller.h"
#include "settings.h"
#include "message.h"
#include "twitch.h"

static Message *queue;
static std::vector<std::thread *> threadPool;

static TwitchInfo twitchSettings;
Controller *controller;

void twitch()
{
    std::thread th(&Twitch::StartTwitchThread, Twitch(), queue, &twitchSettings);
    th.join();
}

void manualControl()
{
    controller->AttachController();
    std::string com;
    while (true)
    {
        std::cout << "Enter command: ";
        std::cin >> com;

        if (!com.empty())
        {
            if (controller->SendInput(buttonName[com]))
            {
                std::cout << "Pressed: " << com << std::endl;
            }
        }
    }
}

void startBot()
{
    threadPool.push_back(new std::thread(&Twitch::StartTwitchThread, Twitch(), queue, &twitchSettings));
    threadPool[0]->join();
}

int main()
{
    queue = new Message();
    // Settings *settings = new Settings(twitchSettings);
    bool isActive = true;
    char command;

    controller = new Emit();

    std::cout << "Avalible Commands: " << std::endl
              << "T: Test Twitch" << std::endl
              << "C: Manually control bot" << std::endl
              << "S: Start Bot in chat play mode" << std::endl
              << "E: Edit Settings" << std::endl
              << "Q: Quit" << std::endl;

    while (isActive)
    {
        std::cout << "Enter Command: ";
        std::cin >> command;

        switch (std::tolower(command))
        {
        case 't':
            twitch();
            break;

        case 'c':
            manualControl();
            break;

        case 's':
            startBot();
            break;

        case 'e':
            // settings->edit();
            break;

        case 'q':
            isActive = false;
            break;

        default:
            std::cerr << "Err: Unrecognized Command" << std::endl;
            break;
        }
    }
    return 0;
}