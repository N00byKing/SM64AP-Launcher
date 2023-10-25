#pragma once

#include <QString>
#include <functional>

#include "BuildConfigurator.h"

namespace PlatformRunner {
    void init();
    void runProcess(QString, BuildConfigurator::SM64_Build, std::function<void(int)> callback);
    void runProcess(QString, std::function<void(int)> callback);
    void runProcessDetached(QString,BuildConfigurator::SM64_Build);
}
