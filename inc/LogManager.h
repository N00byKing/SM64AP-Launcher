#pragma once

#include <string>
#include <QTextEdit>

void initLogs();
void writeToLog(std::string line);
void forkLogTo(QTextEdit* textedit);
void unlinkFork();
