#include <iostream>
#include <string>
#include <fstream>
#include "HuffmanCoder.h"

int main(int argc, char **argv)
{
    HuffmanCoder coder;

    std::ifstream in("file.txt", std::ios::binary);
    std::ofstream out("file.txt.encoded", std::ios::binary);

    coder.encode(in, out);

    in.close();
    out.close();

    std::ifstream coded("file.txt.encoded", std::ios::binary);
    std::ofstream decoded("file.txt.decoded", std::ios::binary);

    coder.decode(coded, decoded);

    coded.close();
    decoded.close();


    system("pause");
}