#pragma once
#include <string>
#include <vector>
#include <array>

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
    std::string encode(const std::string &str);
    std::string decode(const std::string &data);
    FreqArray buildFreqArray(const std::string &str);
    Node *HuffmanCoder::buildTree(const FreqArray &freqArray);
    void buildCharMapAndVisitHistory(CharMap &charMap, std::vector<char> &visitHistory, std::string &leafStr, Node *node, std::vector<char> &path);

private:
};

