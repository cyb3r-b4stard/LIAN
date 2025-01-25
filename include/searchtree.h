#ifndef SEARCHTREE_H
#define SEARCHTREE_H

#include "node.h"
#include "tinyxml.h"
#include "tinystr.h"

#include <unordered_map>
#include <vector>
#include <list>

class SearchTree 
{
public:
    SearchTree();
    SearchTree(int openSize_);

    ~SearchTree();

    void resizeOpen(int openSize_);

    size_t getOpenSize() const;
    size_t getClosedSize() const;
    
    bool isOpenEmpty() const;
    bool isClosedEmpty() const;

    void addToOpen(Node node);
    void addToClosed(Node node, int convNumber);

    Node getMin();

    TiXmlElement *writeToXml(TiXmlElement* xmlElement, TiXmlNode* child) const;

    std::list<Node>* open;
    std::unordered_multimap<int, Node> closed;

private:
    size_t openSize;
    size_t closedSize;
    size_t openLength;
};

#endif // SEARCHTREE_H