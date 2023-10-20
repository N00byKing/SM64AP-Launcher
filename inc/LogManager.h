#pragma once

#include <QString>

#include "OutputWidget.h"

namespace LogManager {
    void initLogs();
    void writeToLog(QString line);
    void forkLogTo(OutputWidget*);
    void unlinkFork();
    void flush();
}
