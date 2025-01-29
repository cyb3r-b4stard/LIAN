#ifndef MAP_H
#define MAP_H

#include "gl_const.h"
#include "tinyxml.h"
#include "tinystr.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

class Map
{
private:
    int** grid;

    int height;
    int width;

    double cellSize;

public:
    Map();
    ~Map();

    bool getMap(const char* fileName);

    bool cellIsTraversable(int curr_i, int curr_j) const;
    bool cellOnGrid(int curr_i, int curr_j) const;
    bool cellIsObstacle(int curr_i, int curr_j) const;

    int* operator [] (int i);
    const int* operator [] (int i) const;

    int getWidth() const;
    int getHeight() const;
    double getCellSize() const;


    int start_i, start_j;
    int goal_i, goal_j;
};

#endif
