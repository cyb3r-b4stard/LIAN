#ifndef NODE_H
#define NODE_H

#include "gl_const.h"

#include <limits>
#include <iostream>
#include <cmath>

struct circleNode
{
    int i, j;
    double heading;
    double cost;

    circleNode() 
        : i(-1), j(-1), heading(-1), cost(0) 
    {}
    circleNode(int i, int j, double cost) 
        : i(i), j(j), heading(-1), cost(cost) 
    {}
    circleNode(int i, int j, double heading, double cost) 
        : i(i), j(j), heading(heading), cost(cost) 
    {}
};

struct Node 
{
    Node* parent;

    int   i, j;
    int   radius;
    float f;
    float g;

    double angle;

    Node() 
        : i(-1), j(-1), f(std::numeric_limits<float>::infinity()), g(std::numeric_limits<float>::infinity()),
            parent(nullptr), radius(CN_PTD_D), angle(0) 
    {}

    Node(int x, int y, float g_ = std::numeric_limits<float>::infinity(), double h_ = std::numeric_limits<float>::infinity(),
        float radius_ = CN_PTD_D, Node* parent_ = nullptr, float cWeightDist_ = 0, double ang_ = 0) 
        : i(x), j(y), g(g_), radius(radius_), parent(parent_), angle(ang_)
    {
        if (parent) {
            f = g + h_ + cWeightDist_ * fabs(ang_ - parent->angle);
        } else {
            f = g + h_;
        }
    }

    ~Node() { 
        parent = nullptr; 
    }

    inline Node& operator=(const Node& other) {
        i = other.i;
        j = other.j;
        f = other.f;
        g = other.g;
        parent = other.parent;
        angle = other.angle;
        radius = other.radius;
        return *this;
    }

    inline bool operator==(const Node& p) const {
        if (this->parent && p.parent) {
            return i == p.i && j == p.j && parent->i == p.parent->i && parent->j == p.parent->j;
        }
        return i == p.i && j == p.j;
    }

    inline bool operator!=(const Node& p) const {
        return !(*this == p);
    }

    int convolution(int width) const {
        return i * width + j;
    }
};

template<>
struct std::hash<Node> 
{
    std::size_t operator()(const Node& node) const noexcept {
        return std::hash<int>{}(node.i * 971 + node.j);
    }
};

#endif
