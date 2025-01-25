#ifndef XMLLOGGER_H
#define XMLLOGGER_H

#include "gl_const.h"
#include "logger.h"
#include "tinyxml.h"
#include "tinystr.h"

#include <iostream>
#include <sstream>
#include <string>

class XmlLogger : public Logger 
{
private:
    std::string logFileName;
    
    TiXmlDocument* doc;

public:
    XmlLogger(float logLevel);
    ~XmlLogger();

    bool getLog(const char* fileName);
    void saveLog();
    void writeToLogIter(int closedSize, const Node& curNode);
    void writeToLogMap(const Map& map, const std::list<Node>& path);
    void writeToLogOpenClose(const SearchTree& searchTree);
    void writeToLogPath(const std::list<Node>& path, const std::vector<float>& angles);
    void writeToLogHpLevel(const std::list<Node>& path);
    void writeToLogSummary(const std::list<Node>& path, int numberOfSteps, int nodesCreated, float length, float lengthScaled,
        long double time, float maxAngle, float accumAngle, int sections);
};

#endif
