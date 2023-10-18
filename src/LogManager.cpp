#include "LogManager.h"

#include <QMessageBox>
#include <QTextEdit>
#include <QFile>

#include "version.h"

bool is_init = false;
QFile logs{"logs.txt", nullptr};
QTextEdit* forklog = nullptr;

void initLogs() {
    if (!logs.open(QIODeviceBase::Truncate | QIODeviceBase::ReadWrite)) {
            QMessageBox::critical(nullptr, "Could not open Log", "The log file is inaccessible. This may indicate further issues.");
    }
    logs.write("SM64APLauncher, commit " GIT_REV "\n");
    logs.flush();
}

void forkLogTo(QTextEdit* textedit) { forklog = textedit; }
void unlinkFork() { forklog = nullptr; }

void writeToLog(std::string line) {
    logs.write((line + "\n").c_str());
    if (forklog) {
        forklog->append(line.c_str());
    }
}
