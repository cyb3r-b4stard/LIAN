#ifndef LOGGER_H
#define LOGGER_H

#include "map.h"
#include "node.h"
#include "searchtree.h"

#include <list>
#include <vector>
#include <unordered_map>


class Logger 
{
public:
    float logLevel;

    Logger() 
        : logLevel(-1) 
    {}
    virtual ~Logger() {}
    virtual bool getLog(const char* fileName) = 0;
    virtual void saveLog() = 0;
    virtual void writeToLogIter(int closedSize, const Node& curNode) = 0;
    virtual void writeToLogMap(const Map& map, const std::list<Node>& path) = 0;
    virtual void writeToLogOpenClose(const SearchTree& searchTree) = 0;
    virtual void writeToLogPath(const std::list<Node>& path, const std::vector<float>& angles) = 0;
    virtual void writeToLogHpLevel(const std::list<Node>& path) = 0;
    virtual void writeToLogSummary(const std::list<Node>& path, int numberOfSteps, int nodesCreated, float length, float lengthScaled,
        long double time, float maxAngle, float accumAngle, int sections) = 0;
};

#endif
