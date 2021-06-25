#pragma once
#include <Windows.h>
#include <iostream>
#include <json.hpp>
#include <map>

enum Buttons { UP, DOWN, LEFT, RIGHT, A, B, X, Y, START, SELECT, L1, R1, L2, R2, L3, R3, EXIT };

using json = nlohmann::json;

namespace fs = std::filesystem;

static std::map<Buttons, std::string> controlNames =
{
    {UP, "UP"},
    {DOWN, "DOWN"},
    {LEFT, "LEFT"},
    {RIGHT, "RIGHT"},
    {A, "A"},
    {B, "B"},
    {X, "X"},
    {Y, "Y"},
    {START, "START"},
    {SELECT, "SELECT"},
    {L1, "L1"},
    {R1, "R1"},
    {L2, "L2"},
    {R2, "R2"},
    {L3, "L3"},
    {R3, "R3"}
};

class Emit
{
private:
    int fd;
public:
    Emit();
    Emit(json j);

    void initalConfig();
    Buttons GetCommands(std::string key);

    json control;
    void save(json& j, bool isDefault = false);
    friend void to_json(nlohmann::json& j, const Emit& p);
    friend void from_json(const nlohmann::json& j, Emit& p);

    bool CreateController();
    bool emit(Buttons cmd);
    int moveABS(int ABS, int moveAxis, int flat);
    int resetABS(int ABS, int flatAxis);
    int pressBtn(int btn);
    int releaseBtn(int btn);
};