#include <algorithm>
#include <cstdint>
#include "HuffmanCoder.h"


void HuffmanCoder::encode(std::ifstream &in, std::ofstream &out)
{
    FreqArray freqArray = buildFreqArray(in);
    Node *tree = buildTree(freqArray);

    std::vector<char> curPath;
    std::vector<char> visitHistory;
    std::vector<char> leafs;
    CharMap charMap;
    buildCharMapAndVisitHistory(charMap, visitHistory, leafs, tree, curPath);

    
    //write leafs
    byte leafsLen = leafs.size(); //1 byte is enough for 256 leafs
    out << leafsLen;
    for (auto leaf: leafs)
        out << leaf;

    //write visit history
    auto bitwiseVisitHistory = packBitVector(visitHistory);
    uint16_t historyLen = visitHistory.size(); //2 bytes (up to ~1024 hops)
    out << historyLen;
    for (auto octet: bitwiseVisitHistory)
        out << octet;

    //write encoded data
    uint64_t dataLen = 0;
    for (int i = 0; i < charMap.size(); ++i)
        dataLen += charMap[i].size() * freqArray[i];

    out << dataLen;

    //pack charMap
    auto packedCharMap = packCharMap(charMap);

    
    byte pos = 0; //write position [0; 7]
    byte outByte = 0;

    for (;;)
    {
        char inByte = 0;
        in >> inByte;

        if (in.eof())
        {
            if (pos != 0)
                out << outByte; //write last byte

            break;
        }

        ShortByte sb = packedCharMap[inByte]; //encode 1 byte

        if (pos + sb.size == 8) //exactly 1 byte
        {
            //write byte
            outByte |= (sb.value >> pos);
            pos = 0;

            out << outByte;
            outByte = 0;
        }
        else if (pos + sb.size < 8) //less then 1 byte
        {
            outByte |= (sb.value >> pos);
            pos += sb.size;
        }
        else // more then 1 byte
        {
            //write byte
            outByte |= (sb.value >> pos);
            out << outByte;
            outByte = 0;

            pos = pos + sb.size - 8; //rest bits
            outByte = sb.value << (sb.size - pos);
        }
    }
}

void HuffmanCoder::decode(std::ifstream &in, std::ofstream &out)
{
    //read leafs
    byte leafsLen = 0;
    in >> leafsLen;

    std::vector<char> leafs;

    for (uint i = 0; i < leafsLen; ++i)
    {
        byte curByte = 0;
        in >> curByte;
        //TODO: test for eof() and throw exception

        leafs.push_back(curByte);
    }

    //read visitHistory
    uint16_t historyLen = 0;
    in >> historyLen;

    uint historyLenBytes = historyLen / 8;
    uint historyRest = historyLen % 8;

    std::vector<char> history;

    for (uint i = 0; i < historyLenBytes; ++i)
    {
        byte curByte = 0;
        in >> curByte;

        byte mask = 1 << 7; //0b10000000

        for (uint j = 0; j < 8; ++j)
        {
            if ((mask & curByte) != 0)
                history.push_back('1');
            else
                history.push_back('0');

            mask >>= 1;
        }
    }

    if (historyRest != 0)
    {
        byte curByte = 0;
        in >> curByte;

        byte mask = 1 << 7; //0b10000000
        for (uint j = 0; j < historyRest; ++j)
        {
            if ((mask & curByte) != 0)
                history.push_back('1');
            else
                history.push_back('0');

            mask >>= 1;
        }
    }

    //form tree and CharMap
    Node *tree = new Node();
    Node *currentNode = tree;
    int leafId = 0;
    bool nextToRight = false;

    CharMap charMap;
    std::vector<char> path;

    for (uint i = 0; i < history.size(); ++i)
    {
        if (history[i] == '0')
        {
            Node *newNode = new Node();
            newNode->parent = currentNode;

            if (!nextToRight)
            {
                currentNode->left = newNode;
                currentNode = newNode;
                path.push_back('0');
            }
            else
            {
                currentNode->right = newNode;
                currentNode = newNode;
                path.push_back('1');
                nextToRight = false;
            }            
        }
        else if (history[i] == '1' && (i == 0 || history[i-1] != '1')) //is terminal
        {
            currentNode->value = leafs[leafId++];

            charMap[currentNode->value] = path;
            path.pop_back();

            currentNode = currentNode->parent;

            nextToRight = true;
        }
        else if (history[i] == '1') //hop up
        {
            path.pop_back();

            currentNode = currentNode->parent;

            nextToRight = true;
        }
    }

    //read data
    uint64_t dataLen = 0;
    in >> dataLen;

    byte curBit = 0;
    byte prevBit = 0;

    byte curByte = 0;

    currentNode = tree;

    for (uint64_t absPos = 0; absPos < dataLen; ++absPos)
    {
        if (absPos % 8 == 0)
        {
            //time to read one more byte
            in >> curByte;
        }

        curBit = bool((curByte << (absPos % 8)) & 0x80);

        if (curBit == 0)
            currentNode = currentNode->left;
        else // if (curBit == 1)
            currentNode = currentNode->right;

        if (currentNode->left == nullptr && currentNode->right == nullptr) //we are in leaf
        {
            out << currentNode->value;
            currentNode = tree;
        }
    }
}

FreqArray HuffmanCoder::buildFreqArray(std::ifstream &in)
{
    FreqArray freqArray;
    freqArray.fill(0);

    for (;;)
    {
        byte ch;
        in >> ch;

        if (in.eof())
            break;

        ++freqArray[ch];
    }

    in.clear();
    in.seekg(0, std::ios::beg);

    return freqArray;
}

Node *HuffmanCoder::buildTree(const FreqArray &freqArray)
{
    std::vector<Node *> heap;

    //push leafs
    for (char i = 0; i < freqArray.size(); ++i)
        if (freqArray[i] != 0)
            heap.push_back(new Node(i, freqArray[i]));

    //create heap
    auto comparator = [](const Node *l, const Node *r)
    {
        return l->priority > r->priority;
    };

    std::make_heap(heap.begin(), heap.end(), comparator);
    
    //build tree in heap
    while (heap.size() != 1)
    {
        //get 2 nodes with top priority
        Node *first = *heap.begin();
        std::pop_heap(heap.begin(), heap.end(), comparator);
        heap.pop_back();
        

        Node *second = *heap.begin();        
        std::pop_heap(heap.begin(), heap.end(), comparator);
        heap.pop_back();

        //combine them into tree node
        Node *newNode = new Node(0, first->priority + second->priority, first, second);
        heap.push_back(newNode);
        std::push_heap(heap.begin(), heap.end(), comparator);
    }

    Node *huffmanTree = *heap.begin();
    return huffmanTree;
}

void HuffmanCoder::buildCharMapAndVisitHistory(CharMap &charMap, std::vector<char> &visitHistory,  std::vector<char> &leafs, Node *node, std::vector<char> &path)
{
    if (node->left == nullptr && node->right == nullptr) //we are in leaf
    {
        charMap[node->value] = path;
        //path.pop_back();

        //visitHistory.push_back('1');
        leafs.push_back(node->value);

        return;
    }

    if (node->left)
    {
        path.push_back('0');
        visitHistory.push_back('0');

        buildCharMapAndVisitHistory(charMap, visitHistory, leafs, node->left, path);
        visitHistory.push_back('1');
        path.pop_back();
    }
    if (node->right)
    {
        path.push_back('1');
        visitHistory.push_back('0');

        buildCharMapAndVisitHistory(charMap, visitHistory, leafs, node->right, path);
        visitHistory.push_back('1');
        path.pop_back();
    }
}

std::vector<byte> HuffmanCoder::packBitVector(const std::vector<char> &vec)
{
    std::vector<byte> result;

    int size = vec.size() / 8;
    int rest = vec.size() % 8;

    for (int i = 0; i < size; ++i) //for each octet
    {
        byte current = 0;

        for (int j = 0; j < 8; ++j) //for each bit in octet
            if (vec[i*8 + j] == '1')
                current |= (1 << (7 - j));

        result.push_back(current);
    }

    if (rest != 0)
    {
        byte last = 0;
        for (int j = 0; j < rest; ++j) //for each bit in rest octet
            if (vec[size*8 + j] == '1')
                last |= (1 << (7 - j));

        result.push_back(last);
    }

    return result;
}

PackedCharMap HuffmanCoder::packCharMap(const CharMap &charMap)
{
    PackedCharMap packedCharMap;
    
    for (int i = 0; i < charMap.size(); ++i)
    {
        if (charMap[i].size() != 0)
        {
            auto &vec = charMap[i];
            ShortByte shortByte(0, vec.size());

            for (int j = 0; j < vec.size(); ++j) //for each bit in octet
                if (vec[j] == '1')
                    shortByte.value |= (1 << (7 - j));

            packedCharMap[i] = shortByte;
        }
    }

    return packedCharMap;
}
