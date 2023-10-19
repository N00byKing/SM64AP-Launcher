#pragma once

#include <QString>
#include <QPlainTextEdit>
#include <functional>

#include "BuildConfigurator.h"

namespace PlatformRunner {
    void runProcess(QString,BuildConfigurator::SM64_Build, std::function<void(int)> callback);
}
