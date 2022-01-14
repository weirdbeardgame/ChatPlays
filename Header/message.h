#pragma once
#include <condition_variable>
#include <iostream>
#include <string.h>
#include <queue>
#include <mutex>

/*************************************************************************
 * Thinking about why a class over just a queue in main or something.
 * First would be easier access to queue type pushing and poping from queue etc
 * Will we need a type other then string in the future?
 **************************************************************************/
class Message
{
private:
    std::mutex lock;
    std::condition_variable condition;
    std::queue<std::string> messageQueue;

public:
    Message() = default;
    Message(const std::string m);
    Message(const Message &m);
    Message(const Message &&m) noexcept;
    Message &operator=(Message &m);
    Message &operator=(Message &&m) noexcept;
    std::string dequeue();
    void enque(std::string message);
};
