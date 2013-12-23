#include <iostream>
#include <string>
#include "HuffmanCoder.h"

int main(int argc, char **argv)
{
    HuffmanCoder coder;

    std::string encoded = coder.encode("testlongtesting");
    std::string decoded = coder.decode(encoded);

    std::cout << decoded << std::endl;

    system("pause");
}