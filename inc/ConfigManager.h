#pragma once

#include <string>
#include <QString>

#include "BuildConfigurator.h"

namespace Config {
    #ifdef WIN32
    const char* const default_msys_path = "C:/msys64";
    #else
    const char* const default_msys_path = "";
    #endif
    bool initConfig();
    void writeConfig();
    void resetToDefault();
    void registerBuild();
    void setAdvanced(bool);
    bool isAdvanced();
    void setMSYSPath(QString);
    QString getMSYSPath();
    void setROMPath(QString,BuildConfigurator::SM64_Region);
    QString getROMPath(BuildConfigurator::SM64_Region);
    void setBuildHome(QString);
    QString getBuildHome();
    bool isFirstRun();
}
