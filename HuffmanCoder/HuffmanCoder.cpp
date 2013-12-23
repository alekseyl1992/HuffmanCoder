#include <algorithm>
#include "HuffmanCoder.h"


std::string HuffmanCoder::encode(const std::string &str)
{
    FreqArray freqArray = buildFreqArray(str);
    Node *tree = buildTree(freqArray);

    std::vector<char> curPath;
    std::vector<char> visitHistory;
    std::string leafStr;
    CharMap charMap;
    buildCharMapAndVisitHistory(charMap, visitHistory, leafStr, tree, curPath);

    std::string result;

    result += leafStr;
    result += "\n";

    for (auto ch: visitHistory)
        result += ch;
    result += "\n";

    //actual encoding
    for (auto ch: str)
    {
        for(auto d: charMap[ch])
            result += d;
        result += " ";
    }

    return result;
}

std::string HuffmanCoder::decode(const std::string &data)
{
    //form leafStr
    std::string leafStr;

    int i = 0;
    for (i = 0; i < data.length(); ++i)
    {
        if (data[i] >= 'a' && data[i] <= 'z')
            leafStr += data[i];
        else if (data[i] == '\n')
            break;
    }

    //form tree and CharMap
    Node *tree = new Node();
    Node *currentNode = tree;
    int leafId = 0;
    bool nextToRight = false;

    CharMap charMap;
    std::vector<char> path;

    for (++i ; i < data.length(); ++i)
    {
        if (data[i] == '0')
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
        else if (data[i] == '1' && data[i-1] != '1') //is terminal
        {
            currentNode->value = leafStr[leafId++];

            charMap[currentNode->value] = path;
            path.pop_back();

            currentNode = currentNode->parent;

            nextToRight = true;
        }
        else if (data[i] == '1') //hop up
        {
            path.pop_back();

            currentNode = currentNode->parent;

            nextToRight = true;
        }
        else if (data[i] == '\n')
            break;
    }

    //actual decode
    currentNode = tree;
    std::string result;

    for (++i ; i < data.length(); ++i)
    {
        if (currentNode->left == nullptr && currentNode->right == nullptr) //we are in leaf
        {
            result += currentNode->value;
            currentNode = tree;
            continue;
        }

        if (data[i] == '0')
            currentNode = currentNode->left;
        else if (data[i] == '1')
            currentNode = currentNode->right;
    }


    return result;
}

FreqArray HuffmanCoder::buildFreqArray(const std::string &str)
{
    FreqArray freqArray;
    freqArray.fill(0);

    for (auto ch: str)
        ++freqArray[ch];

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

void HuffmanCoder::buildCharMapAndVisitHistory(CharMap &charMap, std::vector<char> &visitHistory,  std::string &leafStr, Node *node, std::vector<char> &path)
{
    if (node->left == nullptr && node->right == nullptr) //we are in leaf
    {
        charMap[node->value] = path;
        //path.pop_back();

        //visitHistory.push_back('1');
        leafStr += node->value;

        return;
    }

    if (node->left)
    {
        path.push_back('0');
        visitHistory.push_back('0');

        buildCharMapAndVisitHistory(charMap, visitHistory, leafStr, node->left, path);
        visitHistory.push_back('1');
        path.pop_back();
    }
    if (node->right)
    {
        path.push_back('1');
        visitHistory.push_back('0');

        buildCharMapAndVisitHistory(charMap, visitHistory, leafStr, node->right, path);
        visitHistory.push_back('1');
        path.pop_back();
    }
}
