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
void __setup_process(QProcess& subprocess, QString cmd, QProcessEnvironment const& env);
QProcessEnvironment __setup_env(BuildConfigurator::SM64_Build* build);

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

void init() {
    QObject::connect(&subprocess, &QProcess::readyReadStandardOutput, &stdout_writeToOutput);
    QObject::connect(&subprocess, &QProcess::readyReadStandardError, &stderr_writeToOutput);
    QObject::connect(&subprocess, &QProcess::finished, &handleProcessFinished);
}

void runProcess(QString cmd, BuildConfigurator::SM64_Build build, std::function<void (int)> callback) {
    finishCallback = callback;
    QProcessEnvironment env = __setup_env(&build);
    __setup_process(subprocess, cmd, env);
    subprocess.start();
}

void runProcessDetached(QString cmd, BuildConfigurator::SM64_Build build) {
    QProcess proc;
    QProcessEnvironment env = __setup_env(&build);
    __setup_process(proc, cmd, env);
    proc.startDetached();
}

void runProcess(QString cmd, std::function<void(int)> callback) {
    finishCallback = callback;
    QProcessEnvironment env = __setup_env(nullptr);
    __setup_process(subprocess, cmd, env);
    subprocess.start();
}

QProcessEnvironment __setup_env(BuildConfigurator::SM64_Build* build) {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (build == nullptr) return env;
    env.insert("MSYSTEM", "MINGW64");
    env.insert("CHERE_INVOKING", "yes");
    env.insert("BUILD_NAME", build->name);
    env.insert("BUILD_TARGET_DIR", build->directory);
    env.insert("BUILD_REPO_LINK", build->repo);
    env.insert("BUILD_BRANCH_NAME", build->branch);
    if (build->region == BuildConfigurator::SM64_Region::US) {
        env.insert("BUILD_REGION", "us");
        env.insert("BUILD_ROM", Config::getROMPath(BuildConfigurator::SM64_Region::US));
    }
    if (build->region == BuildConfigurator::SM64_Region::JP) {
        env.insert("BUILD_REGION", "jp");
        env.insert("BUILD_ROM", Config::getROMPath(BuildConfigurator::SM64_Region::JP));
    }
    env.insert("BUILD_MAKE_FLAGS", build->make_flags);
    return env;
}

void __setup_process(QProcess& subprocess, QString cmd, QProcessEnvironment const& env) {
    #ifdef WIN32
    QString msys_path = Config::getMSYSPath();
    subprocess.setProgram(msys_path + "/usr/bin/bash.exe");
    #else
    subprocess.setProgram("bash");
    #endif
    subprocess.setArguments(QStringList() << "--login" << "-c" << "--" << cmd);
    subprocess.setProcessEnvironment(env);
}

}
