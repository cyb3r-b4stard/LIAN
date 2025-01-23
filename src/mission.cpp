#include "mission.h"

Mission::Mission(const char* fName)
    : fileName(fName), search(nullptr), logger(nullptr)
{
}

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
    sr = search->startSearch(logger, map);
}

void Mission::printSearchResultsToConsole() {
    std::cout << "Path ";
    if (!sr.pathfound)
        std::cout << "NOT ";
    std::cout << "found!\n";
    std::cout << "nodescreated" << sr.nodescreated << "\n";
    std::cout << "numberofsteps=" << sr.numberofsteps << "\n";
    if (sr.pathfound) {
        std::cout << "pathlength=" << sr.pathlength << "\n";
        std::cout << "length_scaled=" << sr.pathlength * map.getCellSize() << "\n";
    }
    std::cout << "time=" << sr.time << "\n";
}

void Mission::saveSearchResultsToLog() {
    logger->writeToLogSummary(sr.hppath, sr.numberofsteps, sr.nodescreated, sr.pathlength, sr.pathlength * map.getCellSize(),
        sr.time, sr.max_angle, sr.accum_angle, sr.sections);

    if (sr.pathfound) {
        logger->writeToLogPath(sr.lppath, sr.angles);
        logger->writeToLogMap(map, sr.lppath);
        logger->writeToLogHpLevel(sr.hppath);
    }

    logger->saveLog();
}

