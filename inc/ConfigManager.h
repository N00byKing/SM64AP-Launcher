#pragma once

#include <string>
#include <QString>

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
    bool isFirstRun();
}
