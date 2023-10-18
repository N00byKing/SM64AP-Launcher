#pragma once

#include <QString>
#include <QTextEdit>

#include "BuildConfigurator.h"

namespace PlatformRunner {
    void runProcess(QString,QTextEdit*,BuildConfigurator::SM64_Build);
}
