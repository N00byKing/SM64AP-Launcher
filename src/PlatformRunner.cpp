#include "PlatformRunner.h"

#include <QProcess>
#include <QDir>
#include <QStringList>
#include <QObject>
#include <functional>
#include <string>

#include "BuildConfigurator.h"
#include "LogManager.h"
#include "ConfigManager.h"

namespace PlatformRunner {

QProcess subprocess;

std::function<void(int)> finishCallback = nullptr;

void stdout_writeToOutput() {
    LogManager::writeToLog(subprocess.readAllStandardOutput());
}
void stderr_writeToOutput() {
    LogManager::writeToLog(subprocess.readAllStandardError());
}


void handleProcessFinished(int exitcode) {
    if (finishCallback) {
        finishCallback(exitcode);
    }
}

void runProcess(QString cmd, BuildConfigurator::SM64_Build build, std::function<void(int)> pfinishCallback) {
    #ifdef WIN32
    QString msys_path = Config::getMSYSPath();
    subprocess.setProgram(msys_path + "/usr/bin/bash.exe");
    #else
    subprocess.setProgram("/usr/bin/bash");
    #endif
    subprocess.setArguments(QStringList() << "-c" << "--" << cmd);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("MSYSTEM", "MINGW64");
    env.insert("CHERE_INVOKING", "yes");
    env.insert("BUILD_NAME", build.name);
    env.insert("BUILD_TARGET_DIR", build.directory);
    env.insert("BUILD_REPO_LINK", build.repo);
    env.insert("BUILD_BRANCH_NAME", build.branch);
    if (build.region == BuildConfigurator::SM64_Region::US)
        env.insert("BUILD_REGION", "us");
    if (build.region == BuildConfigurator::SM64_Region::JP)
        env.insert("BUILD_REGION", "jp");
    subprocess.setProcessEnvironment(env);
    QObject::connect(&subprocess, &QProcess::readyReadStandardOutput, &stdout_writeToOutput);
    QObject::connect(&subprocess, &QProcess::readyReadStandardError, &stderr_writeToOutput);
    QObject::connect(&subprocess, &QProcess::finished, &handleProcessFinished);
    finishCallback = pfinishCallback;
    subprocess.start();
}

}
