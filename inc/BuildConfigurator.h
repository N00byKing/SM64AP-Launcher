#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

class BuildConfigurator : public QWidget {
    public:
        BuildConfigurator(QWidget*,bool);
    private:
        using QWidget::QWidget;

        QLineEdit repo_select{"https://github.com/N00byKing/sm64ex", this};
        QLabel repo_select_label{"Repository", this};
        QLineEdit branch_select{"archipelago", this};
        QLabel branch_select_label{"Branch", this};
        QPushButton target_directory_button{"Browse...", this};
        QLabel target_directory_button_label{"Select Target Directory", this};
        QLabel target_directory_selected_label{"⟶ Currently none selected", this};

        void setLocations();
        void setAdvanced(bool);
        void closeEvent(QCloseEvent*);
        void selectTargetDirectory();
};
