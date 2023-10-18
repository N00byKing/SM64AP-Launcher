#pragma once

#include <string>
#include <QTextEdit>

void initLogs();
void writeToLog(QString line);
void forkLogTo(QTextEdit* textedit);
void unlinkFork();
