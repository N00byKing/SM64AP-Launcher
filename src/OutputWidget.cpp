#include "OutputWidget.h"

#include <QPlainTextEdit>
#include <qplaintextedit.h>

OutputWidget::OutputWidget(QWidget* parent) : QPlainTextEdit(parent) {}

void OutputWidget::printToUser(QString str) {
    this->moveCursor(QTextCursor::End);
    this->insertPlainText (str);
    this->moveCursor(QTextCursor::End);
}
