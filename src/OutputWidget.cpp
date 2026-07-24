#include "OutputWidget.h"

#include <QtWidgets/QPlainTextEdit>
#include <qplaintextedit.h>

OutputWidget::OutputWidget(QWidget* parent) : QPlainTextEdit(parent) {
    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    setFont(fixedFont);
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setReadOnly(true);
}

void OutputWidget::printToUser(QString str) {
    this->moveCursor(QTextCursor::End);
    this->insertPlainText (str);
    this->moveCursor(QTextCursor::End);
}
