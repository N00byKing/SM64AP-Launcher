#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include <vector>

#include "ConfigManager.h"
#include "BuildConfigurator.h"
#include "OutputWidget.h"

class RequirementHandler : public QWidget {
    public:
        static constexpr int window_w = 480;
        static constexpr int window_h = 550;
        static constexpr std::string_view US_ROM_HASH = "9bef1128717f958171a4afac3ed78ee2bb4e86ce";
        static constexpr std::string_view JP_ROM_HASH = "8a20a5c83d6ceb0f0506cfc9fa20d8f438cafe51";
        RequirementHandler(QWidget* parent, bool advanced);
        RequirementHandler() = delete;
    private:
        using QWidget::QWidget;
        #ifdef WIN32
        QLineEdit msys_select{Config::default_msys_path,this};
        QLabel msys_select_label{"Path to MSYS installation",this};
        QPushButton reinstall_dependencies{"Re-install Dependencies",this};
        #endif
        QPushButton check_requirements{"Check requirements",this};
        QLabel troubleshooting_label{"Troubleshooting",this};
        QPushButton rewrite_config{"Re-write config",this};
        QPushButton select_rom{"Register SM64 Rom",this};
        OutputWidget log_output{this};
        bool advanced = false;
        std::vector<QString> dependencies = {
            "unzip",
            "mingw-w64-x86_64-gcc",
            "mingw-w64-x86_64-glew",
            "mingw-w64-x86_64-SDL2",
            "git",
            "make",
            "python3",
            "mingw-w64-x86_64-cmake"
        };
        void closeEvent(QCloseEvent *event);
        #ifdef WIN32
        void checkRequirementsMSYS();
        #endif
        void checkMSYSDependencyCallback(int);
        void checkRequirementsROM();
        void registerROM();
        void setLocations();
        void setAdvanced(bool);
        BuildConfigurator::SM64_Region identifyROM(QString);
        void reinstallDependencies();
        void updateMSYSCallback(int);
        void installDependencyCallback(int);
        void enableInput(bool);
        void printToUser(QString);
};
