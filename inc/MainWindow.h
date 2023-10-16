#pragma once

#include <QWidget>
#include <QCheckBox>
#include <QPushButton>
#include <memory>

#include "BuildConfigurator.h"

class MainWindow : public QWidget {
    public:
        static constexpr int window_w = 1000;
        static constexpr int window_h = 300;
        MainWindow();
        void show();
    private:
        using QWidget::QWidget;
        using QWidget::show;
        QPushButton create_default_build{"Compile default SM64AP build", this};
        QPushButton create_custom_build{"Compile custom build", this};
        QCheckBox use_advanced{"Show advanced options", this};
        std::unique_ptr<BuildConfigurator> configurator;

        void setLocations();
        void setAdvanced(bool);
        void spawnDefaultConfigurator();
        void spawnAdvancedConfigurator();
};
