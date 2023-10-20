#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include "ConfigManager.h"
#include "BuildConfigurator.h"

class RequirementHandler : public QWidget {
    public:
        static constexpr int window_w = 480;
        static constexpr int window_h = 500;
        RequirementHandler(QWidget* parent, bool advanced);
        RequirementHandler() = delete;
    private:
        using QWidget::QWidget;
        #ifdef WIN32
        QLineEdit msys_select{Config::default_msys_path,this};
        QLabel msys_select_label{"Path to MSYS installation",this};
        QPushButton reinstall_msys{"Re-install MSYS",this};
        QPushButton reinstall_dependencies{"Re-install Dependencies",this};
        #endif
        QPushButton check_requirements{"Check requirements",this};
        QLabel troubleshooting_label{"Troubleshooting",this};
        QPushButton rewrite_config{"Re-write config",this};
        QPushButton select_rom{"Register SM64 Rom",this};
        void closeEvent(QCloseEvent *event);
        void checkRequirements();
        void registerROM();
        void setLocations();
        void setAdvanced(bool);
        BuildConfigurator::SM64_Region identifyROM(QString);
};
