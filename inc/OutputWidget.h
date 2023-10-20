#pragma once

#include <QPlainTextEdit>

class OutputWidget : public QPlainTextEdit {
    public:
        OutputWidget(QWidget* parent);
        void printToUser(QString);
    private:
};
