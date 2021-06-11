#include "settings.h"

Settings::Settings()
{
    j = json();
    filePath = std::string();
}

Settings::Settings(std::string fileName)
{
    filePath = fileName;
    j = json(filePath.make_preferred());
}

void Settings::init()
{
    char command;
    std::cout << "Avalible Commands are: " << std::endl;
    std::cout << "E: Edit settings" << std::endl
    << "Q: Quit" << std::endl; 
    std::cin >> command;
    switch (command)
    {
        case 'E':

        break;

        case 'Q':
            exit(0);
            break;
        default:
            std::cerr << "Unrecognized command" << std::endl;
            break;
    }
}

void Settings::edit()
{
    filePath = "settings/settings.json";
    try
    {
        fileStream.open(filePath, std::ios::out);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        //fileStream.write(filePath); // try to write new file
     }
}

bool Settings::load(std::string fileName, std::string delimit)
{
    filePath = fileName;
    j = json(filePath).at(delimit);
    
    for(auto it = j.begin(); it != j.end(); it++)
    {
        // Parser logic here

    }

    return true;
}

bool Settings::save(std::string fileName)
{
    filePath = fileName;
}
