#include "PlatformRunner.h"

#include <QProcess>

#include "ConfigManager.h"

namespace PlatformRunner {

QProcess subprocess;

#ifdef WIN32
void runProcess() {
    QString msys_path = Config::getMSYSPath();
    //subprocess.setProgram();
}
#else
#error TODO linux
#endif

}
