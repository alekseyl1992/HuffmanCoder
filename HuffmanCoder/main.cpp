#include <iostream>
#include <string>
#include <fstream>
#include "HuffmanCoder.h"

int main(int argc, char **argv)
{
    HuffmanCoder coder;

    std::ifstream in("CUU.doc", std::ios::binary);
    std::ofstream out("CUU.doc.encoded", std::ios::binary);

    coder.encode(in, out);

    in.close();
    out.close();

    std::ifstream coded("CUU.doc.encoded", std::ios::binary);
    std::ofstream decoded("CUU.decoded.doc", std::ios::binary);

    coder.decode(coded, decoded);

    coded.close();
    decoded.close();


    system("pause");
}