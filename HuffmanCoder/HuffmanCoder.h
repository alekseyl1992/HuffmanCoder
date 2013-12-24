#pragma once
#include <string>
#include <vector>
#include <array>
#include <fstream>

typedef unsigned char byte;
typedef unsigned int uint;
typedef std::array<uint, 256> FreqArray;
typedef std::array<std::vector<char>, 256> CharMap;

struct Node
{
    Node *left;
    Node *right;
    Node *parent;
    char value;
    uint priority;

    Node(char v = 0, uint p = 0, Node *l = nullptr, Node *r = nullptr)
        : value(v), priority(p), left(l), right(r)
    { }
};

class HuffmanCoder
{
public:
    void encode(std::ifstream &in, std::ofstream &out);
    void decode(std::ifstream &in, std::ofstream &out);
    FreqArray buildFreqArray(std::ifstream &in);
    Node *HuffmanCoder::buildTree(const FreqArray &freqArray);
    void buildCharMapAndVisitHistory(CharMap &charMap, std::vector<char> &visitHistory, std::vector<char> &leafStr, Node *node, std::vector<char> &path);

    std::vector<byte> packBitVector(const std::vector<char> &vec);
    byte packVectorToByte(const std::vector<char> &vec);
private:
};

