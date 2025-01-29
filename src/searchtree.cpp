#include "searchtree.h"

SearchTree::SearchTree()
    : openSize(0), closedSize(0)
{}

SearchTree::~SearchTree() {}

size_t SearchTree::getOpenSize() const {
    return openSize;
}

size_t SearchTree::getClosedSize() const {
    return closedSize;
}

bool SearchTree::isOpenEmpty() const {
    return openSize == 0;
}

bool SearchTree::isClosedEmpty() const {
    return closedSize == 0;
}

void SearchTree::addToOpen(Node newNode) {
    open.push(newNode);
    ++openSize;
}

void SearchTree::addToClosed(Node node) {
    closed.insert({ node, node });
    ++closedSize;
}

Node SearchTree::getMin() {
    Node minNode;

    if (!open.empty()) {
        minNode = open.top();
        open.pop();
    }

    while (closed.find(minNode) != closed.end()) {
        if (!open.empty()) {
            minNode = open.top();
            open.pop();
        }
    }

    return minNode;
}

TiXmlElement* SearchTree::writeToXml(TiXmlElement* xmlElement, TiXmlNode* child) const {
    Node minNode;
    minNode.f = std::numeric_limits<float>::infinity();

    for (auto x : reinterpret_cast<const std::vector<Node>&>(open)) {
        if (x.f < minNode.f && x.g >= minNode.g) {
            minNode = x;
        }
    }

    if (minNode.f != std::numeric_limits<float>::infinity()) {
        xmlElement = new TiXmlElement(CNS_TAG_NODE);
        xmlElement->SetAttribute(CNS_TAG_ATTR_X, minNode.j);
        xmlElement->SetAttribute(CNS_TAG_ATTR_Y, minNode.i);
        xmlElement->SetDoubleAttribute(CNS_TAG_ATTR_F, minNode.f);
        xmlElement->SetDoubleAttribute(CNS_TAG_ATTR_G, minNode.g);
        xmlElement->SetAttribute(CNS_TAG_ATTR_PARX, minNode.parent->j);
        xmlElement->SetAttribute(CNS_TAG_ATTR_PARY, minNode.parent->i);
        child->InsertEndChild(*xmlElement);
    }

    for (auto it : reinterpret_cast<const std::vector<Node>&>(open)) {
        if (it != minNode) {
            xmlElement->Clear();
            xmlElement->SetAttribute(CNS_TAG_ATTR_X, it.j);
            xmlElement->SetAttribute(CNS_TAG_ATTR_Y, it.i);
            xmlElement->SetDoubleAttribute(CNS_TAG_ATTR_F, it.f);
            xmlElement->SetDoubleAttribute(CNS_TAG_ATTR_G, it.g);
            if (it.g > 0) {
                xmlElement->SetAttribute(CNS_TAG_ATTR_PARX, it.parent->j);
                xmlElement->SetAttribute(CNS_TAG_ATTR_PARY, it.parent->i);
            }
            child->InsertEndChild(*xmlElement);
        }   
    }
    return xmlElement;
}
