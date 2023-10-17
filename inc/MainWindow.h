#pragma once

#include <QWidget>
#include <QCheckBox>
#include <QPushButton>
#include <memory>

#include "BuildConfigurator.h"
#include "RequirementHandler.h"

class MainWindow : public QWidget {
    public:
        static constexpr int window_w = 1000;
        static constexpr int window_h = 300;
        MainWindow();
    private:
        using QWidget::QWidget;
        QPushButton create_default_build{"Compile default SM64AP build", this};
        QPushButton create_custom_build{"Compile custom build", this};
        QPushButton recheck_requirements{"Re-check requirements", this};
        QCheckBox use_advanced{"Show advanced options", this};
        std::unique_ptr<BuildConfigurator> configurator;
        std::unique_ptr<RequirementHandler> requirement_handler;

        void setLocations();
        void setAdvanced(bool);
        void closeEvent(QCloseEvent *event);
        void spawnDefaultConfigurator();
        void spawnAdvancedConfigurator();
        void spawnRequirementHandler();
};
