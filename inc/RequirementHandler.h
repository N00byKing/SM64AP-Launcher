#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include "ConfigManager.h"

class RequirementHandler : public QWidget {
    public:
        static constexpr int window_w = 480;
        static constexpr int window_h = 500;
        RequirementHandler(QWidget* parent, bool advanced);
        RequirementHandler() = delete;
    private:
        using QWidget::QWidget;
        QLineEdit msys_select{Config::default_msys_path,this};
        QLabel msys_select_label{"Path to MSYS installation",this};
        QPushButton check_requirements{"Check requirements",this};
        QLabel troubleshooting_label{"Troubleshooting",this};
        QPushButton rewrite_config{"Re-write config",this};
        QPushButton reinstall_msys{"Re-install MSYS",this};
        QPushButton reinstall_dependencies{"Re-install Dependencies",this};
        void closeEvent(QCloseEvent *event);
        void checkRequirements();
        void setLocations();
        void setAdvanced(bool);
};
