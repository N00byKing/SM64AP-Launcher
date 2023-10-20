#pragma once

#include <QString>

#include "BuildConfigurator.h"

namespace LogManager {
    void initLogs();
    void writeToLog(QString line);
    void forkLogTo(BuildConfigurator* bconfig);
    void unlinkFork();
    void flush();
}
