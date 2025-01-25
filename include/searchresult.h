#ifndef SEARCHRESULT_H
#define SEARCHRESULT_H

#include "node.h"
#include <vector>

struct SearchResult 
{
    bool pathFound;
    float pathLength;
    
    std::list<Node> hpPath, lpPath;
    
    unsigned int nodesCreated;
    unsigned int numberOfSteps;
    
    std::vector<float> angles;
    
    float accumAngle;
    float maxAngle;
    int   sections;
    
    double time;

    SearchResult() 
        : pathFound(false), pathLength(0), nodesCreated(0),
            numberOfSteps(0), time(0), maxAngle(0), sections(0) 
    {
        hpPath.clear();
        lpPath.clear();
        angles.clear();
    }
};

#endif // SEARCHRESULT_H
