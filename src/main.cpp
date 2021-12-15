#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#include "settings.h"
#include "message.h"
#include "twitch.h"

static Message* queue;
static std::vector<std::thread*> threadPool;

static TwitchInfo twitchSettings;
static Emit controller;

void twitch()
{
    std::thread th(&Twitch::create, queue, &twitchSettings);
    th.join();
}

void manualControl()
{
    std::thread th(&Emit::poll, Emit(), queue, controller, true);
    th.join();
}

void startBot()
{
    threadPool.push_back(new std::thread(&Twitch::create, queue, &twitchSettings));
    threadPool.push_back(new std::thread(&Emit::poll, Emit(), queue, controller, false));
    threadPool[0]->join();
}

int main()
{
    queue = new Message();
    Settings* settings = new Settings(controller, twitchSettings);
    bool isActive = true;
    char command;

    std::cout << "Avalible Commands: " << std::endl
        << "T: Test Twitch" << std::endl
        << "C: Manually control bot" << std::endl
        << "S: Start Bot in chat play mode" << std::endl
        << "E: Edit Settings" << std::endl
        << "Q: Quit" << std::endl;

    while(isActive)
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
                settings->edit();
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