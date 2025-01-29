#ifndef MISSION_H
#define MISSION_H

#include "config.h"
#include "liansearch.h"
#include "map.h"
#include "search.h"
#include "searchresult.h"
#include "xmllogger.h"

#include <string>

class Mission
{
public:
    Mission(const char* fileName);
    ~Mission();

    bool getMap();
    bool getConfig();
    bool createLog();
    void createSearch();
    void startSearch();
    void printSearchResultsToConsole();
    void saveSearchResultsToLog();

private:
    Map map;

    const char* fileName;
    
    Config config;

    Search* search;
    Logger* logger;


    SearchResult sResult;
};

#endif

