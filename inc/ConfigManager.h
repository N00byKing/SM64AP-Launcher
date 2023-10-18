#pragma once

namespace Config {
    bool initConfig();
    void writeConfig();
    void registerBuild();
    void setAdvanced(bool);
    bool isAdvanced();
    bool isFirstRun();
}