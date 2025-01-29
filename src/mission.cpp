#include "mission.h"

Mission::Mission(const char* fileName)
    : fileName(fileName), search(nullptr), logger(nullptr)
{}

Mission::~Mission() {
    delete search;
    delete logger;
}

bool Mission::getMap() {
    return map.getMap(fileName);
}

bool Mission::getConfig() {
    return config.getConfig(fileName);
}

void Mission::createSearch() {
    search = new LianSearch((float)config.getParamValue(CN_PT_AL),
        (int)config.getParamValue(CN_PT_D),
        (float)config.getParamValue(CN_PT_W),
        (unsigned int)config.getParamValue(CN_PT_SL),
        (float)config.getParamValue(CN_PT_CHW),
        (bool)config.getParamValue(CN_PT_PS),
        (float)config.getParamValue(CN_PT_DDF),
        (int)config.getParamValue(CN_PT_DM),
        (double)config.getParamValue(CN_PT_PC),
        (int)config.getParamValue(CN_PT_NOP));
}

bool Mission::createLog() {
    if (config.getParamValue(CN_PT_LOGLVL) == CN_LOGLVL_LOW || config.getParamValue(CN_PT_LOGLVL) == CN_LOGLVL_HIGH ||
        config.getParamValue(CN_PT_LOGLVL) == CN_LOGLVL_MED || config.getParamValue(CN_PT_LOGLVL) == CN_LOGLVL_TINY ||
        config.getParamValue(CN_PT_LOGLVL) - CN_LOGLVL_ITER < 0.001) {
        logger = new XmlLogger(config.getParamValue(CN_PT_LOGLVL));
    } else if (config.getParamValue(CN_PT_LOGLVL) == CN_LOGLVL_NO) {
        logger = new XmlLogger(config.getParamValue(CN_PT_LOGLVL));

        return true;
    } else {
        std::cout << "'loglevel' is not correctly specified in input XML-file.\n";

        return false;
    }

    return logger->getLog(fileName);
}

void Mission::startSearch() {
    sResult = search->startSearch(logger, map);
}

void Mission::printSearchResultsToConsole() {
    std::cout << "Path ";

    if (!sResult.pathFound) {
        std::cout << "NOT ";
    }

    std::cout << "found!\n";
    std::cout << "nodescreated" << sResult.nodesCreated << "\n";
    std::cout << "numberofsteps=" << sResult.numberOfSteps << "\n";
    
    if (sResult.pathFound) {
        std::cout << "pathlength=" << sResult.pathLength << "\n";
        std::cout << "length_scaled=" << sResult.pathLength * map.getCellSize() << "\n";
    }
    
    std::cout << "time=" << sResult.time << "\n";
}

void Mission::saveSearchResultsToLog() {
    logger->writeToLogSummary(sResult.hpPath, sResult.numberOfSteps, sResult.nodesCreated, sResult.pathLength, sResult.pathLength * map.getCellSize(),
        sResult.time, sResult.maxAngle, sResult.accumAngle, sResult.sections);

    if (sResult.pathFound) {
        logger->writeToLogPath(sResult.lpPath, sResult.angles);
        logger->writeToLogMap(map, sResult.lpPath);
        logger->writeToLogHpLevel(sResult.hpPath);
    }

    logger->saveLog();
}

