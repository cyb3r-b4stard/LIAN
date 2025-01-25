#include "searchtree.h"

SearchTree::SearchTree()
    : openSize(0), closedSize(0)
{}

SearchTree::SearchTree(int openSize_) {
    open = new std::list<Node>[openSize_];
    openSize = 0;
    openLength = openSize_;
}

void SearchTree::resizeOpen(int openSize_) {
    open = new std::list<Node>[openSize_];
    openSize = 0;
    openLength = openSize_;
}

SearchTree::~SearchTree() {
    delete [] open;
}

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
    if (open[newNode.i].empty()) {
        open[newNode.i].emplace_back(newNode);
        ++openSize;
        return;
    }
    auto pos = open[newNode.i].end();
    bool pos_found = false;

    for(auto it = open[newNode.i].begin(); it != open[newNode.i].end(); ++it) {
        if ((it->f >= newNode.f) && (!pos_found)) {
            pos = it;
            pos_found = true;
        }
        if (*it == newNode) {
            if (newNode.f >= it->f) return;
            else {
                if(pos == it) {
                    it->g = newNode.g;
                    it->f = newNode.f;
                    it->radius = newNode.radius;
                    return;
                }
                open[newNode.i].erase(it);
                --openSize;
                break;
            }
        }
    }
    ++openSize;
    open[newNode.i].insert(pos, newNode);
}

void SearchTree::addToClosed(Node node, int convNumber) {
    closed.insert({ node.convolution(convNumber), node });
    ++closedSize;
}

Node SearchTree::getMin() {
    Node minNode;
    minNode.f = std::numeric_limits<float>::infinity();
    for(size_t i = 0; i < openLength; i++) {
        if(!open[i].empty() && open[i].begin()->f <= minNode.f) {
            if (open[i].begin()->f == minNode.f) {
                if (open[i].begin()->g >= minNode.g) {
                    minNode = open[i].front();
                }
            } else {
                minNode = open[i].front();
            }
        }
    }
    open[minNode.i].pop_front();
    --openSize;
    return minNode;
}

TiXmlElement* SearchTree::writeToXml(TiXmlElement* xmlElement, TiXmlNode* child) const {
    Node minNode;
    minNode.f = std::numeric_limits<float>::infinity();

    for(size_t i = 0; i < openLength; i++) {
        if(!open[i].empty() && open[i].begin()->f <= minNode.f) {
            if (open[i].begin()->f == minNode.f) {
                if (open[i].begin()->g >= minNode.g) {
                    minNode = open[i].front();
                }
            } else {
                minNode = open[i].front();
            }
        }
    }
    if(minNode.f != std::numeric_limits<float>::infinity()) {
        xmlElement = new TiXmlElement(CNS_TAG_NODE);
        xmlElement -> SetAttribute(CNS_TAG_ATTR_X, minNode.j);
        xmlElement -> SetAttribute(CNS_TAG_ATTR_Y, minNode.i);
        xmlElement -> SetDoubleAttribute(CNS_TAG_ATTR_F, minNode.f);
        xmlElement -> SetDoubleAttribute(CNS_TAG_ATTR_G, minNode.g);
        xmlElement -> SetAttribute(CNS_TAG_ATTR_PARX, minNode.parent->j);
        xmlElement -> SetAttribute(CNS_TAG_ATTR_PARY, minNode.parent->i);
        child -> InsertEndChild(*xmlElement);
    }
    for(size_t i = 0; i < openLength; ++i) {
        if(!open[i].empty()) {
            for (auto it = open[i].begin(); it != open[i].end(); ++it) {
                if (*it != minNode) {
                    xmlElement -> Clear();
                    xmlElement -> SetAttribute(CNS_TAG_ATTR_X, it->j);
                    xmlElement -> SetAttribute(CNS_TAG_ATTR_Y, it->i);
                    xmlElement -> SetDoubleAttribute(CNS_TAG_ATTR_F, it->f);
                    xmlElement -> SetDoubleAttribute(CNS_TAG_ATTR_G, it->g);
                    if (it->g > 0){
                        xmlElement -> SetAttribute(CNS_TAG_ATTR_PARX, it->parent->j);
                        xmlElement -> SetAttribute(CNS_TAG_ATTR_PARY, it->parent->i);
                    }
                    child->InsertEndChild(*xmlElement);
                }
            }
        }
    }
    return xmlElement;
}
