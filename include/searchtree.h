#ifndef SEARCHTREE_H
#define SEARCHTREE_H

#include "node.h"
#include "tinyxml.h"
#include "tinystr.h"

#include <unordered_map>
#include <vector>
#include <list>
#include <queue>

class SearchTree 
{
    struct Compare {
        bool operator () (const Node& lhs, const Node& rhs) {
            return lhs.f > rhs.f;
        }
    };

public:
    SearchTree();

    ~SearchTree();

    size_t getOpenSize() const;
    size_t getClosedSize() const;
    
    bool isOpenEmpty() const;
    bool isClosedEmpty() const;

    void addToOpen(Node node);
    void addToClosed(Node node);

    Node getMin();

    TiXmlElement* writeToXml(TiXmlElement* xmlElement, TiXmlNode* child) const;

    std::priority_queue<Node, std::vector<Node>, Compare> open;
    std::unordered_multimap<Node, Node> closed;

private:
    size_t openSize;
    size_t closedSize;
    size_t openLength;
};

#endif // SEARCHTREE_H