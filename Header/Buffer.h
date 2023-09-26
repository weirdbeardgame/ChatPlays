#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <queue>

class Buffer
{
private:
    std::vector<char *> buf;
    std::queue<char *> commands;

public:
    void Enqueue(char *line);
    std::string Dequeue();
    std::string GetCommand();
};