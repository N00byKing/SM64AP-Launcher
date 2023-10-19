#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>

class BuildConfigurator : public QWidget {
    public:
        static constexpr int window_w = 1100;
        static constexpr int window_h = 500;
        BuildConfigurator(QWidget*,bool);
        struct SM64_Build {
            QString name;
            QString repo;
            QString branch;
            QString directory;
        };
        void printToUser(QString str);
    private:
        using QWidget::QWidget;

        QLineEdit repo_select{"https://github.com/N00byKing/sm64ex", this};
        QLabel repo_select_label{"Repository", this};
        QLineEdit branch_select{"archipelago", this};
        QLabel branch_select_label{"Branch", this};
        QPushButton target_directory_button{"Browse...", this};
        QLabel target_directory_button_label{"Select Target Directory", this};
        QLabel target_directory_selected_label{"⟶ Currently none selected", this};
        QLineEdit name_select{"", this};
        QLabel name_select_label{"Name for this build", this};
        QPushButton download_files{"Download Files", this};
        QLabel download_files_label{"Confirm Repo and Branch\nand start downloading the files", this};
        QPlainTextEdit subprocess_output{this};

        void setLocations();
        void setAdvanced(bool);
        void closeEvent(QCloseEvent*);
        void selectTargetDirectory();
        void confirmAndDownloadRepo();
        void DLfinishCallback(int);
        void disableDLInput();
};
