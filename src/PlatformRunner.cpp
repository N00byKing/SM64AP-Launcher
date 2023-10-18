#include "PlatformRunner.h"

#include <QProcess>
#include <QDir>
#include <QStringList>
#include <QTextEdit>
#include <QMessageBox>
#include <QObject>
#include <string>

#include "BuildConfigurator.h"
#include "ConfigManager.h"

namespace PlatformRunner {

QProcess subprocess;
QTextEdit* output = nullptr;

void writeToOutput() {
    if (output) {
        output->append(subprocess.readLine());
    }
}

void handleProcessError(QProcess::ProcessError err) {
    QMessageBox::critical(nullptr, "Subprocess error", QString("Subprocess error occurred: ") + QChar(err));
}

#ifdef WIN32
void runProcess(QString cmd, QTextEdit* output, BuildConfigurator::SM64_Build build) {
    QString msys_path = Config::getMSYSPath();
    subprocess.setProgram(msys_path + "/usr/bin/bash.exe");
    subprocess.setArguments(QStringList() << "--login" << "-c" << "--" << cmd);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("MSYSTEM", "MINGW64");
    env.insert("CHERE_INVOKING", "yes");
    env.insert("BUILD_TARGET_DIR", build.directory);
    env.insert("BUILD_REPO_LINK", build.repo);
    env.insert("BUILD_BRANCH_NAME", build.branch);
    subprocess.setProcessEnvironment(env);
    QObject::connect(&subprocess, &QProcess::canReadLine, &writeToOutput);
    QObject::connect(&subprocess, &QProcess::errorOccurred, &handleProcessError);
    subprocess.start();
}
#else
#error TODO linux
#endif

}
