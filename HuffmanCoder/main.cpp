#include <iostream>
#include <string>
#include <fstream>
#include "HuffmanCoder.h"

int main(int argc, char **argv)
{
    HuffmanCoder coder;

    std::string command;
    std::string name;
    std::string newName;

    if (argc == 4) //program pack|unpack file.name result.name
    {
        command = argv[1];
        name = argv[2];
        newName = argv[3];
    }
    else if(argc == 2) //program file.name
    {
        name = argv[1];
        newName = name;

        std::string extension = name.substr(name.rfind("."));
        if (extension == ".hc")
        {
            command = "unpack";
            newName = name.substr(0, name.length() - 3); //slice ".hc"
        }
        else
        {
            command = "pack";
            newName += ".hc";
        }
    }
    else
    {
        std::cout << "Usage:\n   HuffmanCoder.exe <pathToFile>\n or\n   HuffmanCoder.exe pack|unpack <pathToFile> <pathToResult>" << std::endl;

        std::cout << std::endl << "Press any Enter to exit..." << std::endl;
        std::cin.peek();
        return 1;
    }


    if (command == "pack")
    {
        std::ifstream in(name, std::ios::binary);
        std::ofstream out(newName, std::ios::binary);

        coder.encode(in, out);
    }
    else if (command == "unpack")
    {
        std::ifstream in(name, std::ios::binary);
        std::ofstream out(newName, std::ios::binary);

        coder.decode(in, out);
    }
}