#ifndef SEARCH_H
#define SEARCH_H

#include "gl_const.h"
#include "logger.h"
#include "map.h"
#include "searchresult.h"
#include "xmllogger.h"

class Search 
{
public:
    SearchResult sResult;
    
    Search() {}
    
    virtual ~Search() {}
    
    virtual SearchResult startSearch(Logger* log, const Map& map) = 0;
};

#endif
