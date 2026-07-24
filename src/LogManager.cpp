#include "LogManager.h"

#include <QtWidgets/QMessageBox>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QCoreApplication>

#include "BuildConfigurator.h"
#include "OutputWidget.h"
#include "version.h"

namespace LogManager {

bool is_init = false;
QFile logs;
OutputWidget* forklog = nullptr;

void initLogs() {
    logs.setFileName("logs.txt");
    if (!logs.open(QIODeviceBase::Truncate | QIODeviceBase::ReadWrite)) {
            QMessageBox::critical(nullptr, "Could not open Log", "The log file is inaccessible. This may indicate further issues.");
    }
    logs.write("SM64APLauncher, commit " GIT_REV "\n");
    flush();
}

void forkLogTo(OutputWidget* textedit) { forklog = textedit; }
void unlinkFork() { forklog = nullptr; }

void writeToLog(QString line) {
    logs.write(line.toStdString().c_str());
    if (forklog) {
        forklog->printToUser(line);
    }
}

void flush() {
    logs.flush();
}

}
