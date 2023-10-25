#pragma once

#include "OutputWidget.h"
#include <QWidget>
#include <QLineEdit>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QMainWindow>

class BuildConfigurator : public QMainWindow {
    public:
        static constexpr int window_w = 1100;
        static constexpr int window_h = 500;
        BuildConfigurator(QWidget*,bool);
        struct SM64_Build_Proto {
            QString name;
            QString repo;
            QString branch;
            QString directory;
        };
        enum struct SM64_Region {
            Undef, US, JP
        };
        struct SM64_Build : SM64_Build_Proto {
            SM64_Region region;
            QString make_flags;
        };
        void printToUser(QString str);
    private:
        using QMainWindow::QMainWindow;

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
        OutputWidget subprocess_output{this};
        QComboBox region_select{this};
        QLabel region_select_label{"Region",this};
        QLineEdit make_flags{"-j8",this};
        QLabel make_flags_label{"Make Flags",this};
        QPushButton start_compile{"Create Build", this};
        SM64_Build active_build;
        bool advanced = false;
        void setLocations();
        void setAdvanced(bool);
        void closeEvent(QCloseEvent*);
        void selectTargetDirectory();
        void confirmAndDownloadRepo();
        void compileBuild();
        void DLFinishCallback(int);
        void enableDLInput(bool);
        void CompileFinishCallback(int);
        void enableCompileInput(bool);
};
