#pragma once

#include <QMainWindow>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include <memory>

#include "BuildConfigurator.h"
#include "BuildManager.h"
#include "RequirementHandler.h"

class MainWindow : public QMainWindow {
    public:
        static constexpr int window_w = 800;
        static constexpr int window_h = 300;
        MainWindow();
    private:
        using QMainWindow::QMainWindow;
        QPushButton play_build{"Play selected build",this};
        QCheckBox save_launch_opts{"Save launch options", this};
        QPushButton manage_build{"Manage selected build", this};
        QPushButton create_default_build{"Compile default SM64AP build", this};
        QPushButton create_custom_build{"Compile custom build", this};
        QPushButton recheck_requirements{"Requirements and Debugging", this};
        QCheckBox use_advanced{"Show advanced options", this};
        QLabel build_list_label{"Playable builds:",this};
        QListWidget build_list{this};
        QLabel build_info_label{"Build Info:",this};
        QPlainTextEdit selected_build_info{"No build selected", this};
        QLabel launch_options_label{"Launch Options:",this};
        QPlainTextEdit launch_options{this};
        // Other
        std::unique_ptr<BuildConfigurator> configurator;
        std::unique_ptr<RequirementHandler> requirement_handler;
        std::unique_ptr<BuildManager> build_manager;
        bool show_require_firstrun = false;
        BuildConfigurator::SM64_Build selected_build = {
            "_None"
        };

        void setLocations();
        void setAdvanced(bool);
        void parseBuilds();
        void startGame();
        void closeEvent(QCloseEvent *event);
        void showEvent(QShowEvent *event);
        void changeEvent(QEvent *event);
        void spawnDefaultConfigurator();
        void spawnAdvancedConfigurator();
        void spawnBuildManager();
        void spawnRequirementHandler();
        void buildSelectionHandler(QListWidgetItem *current, QListWidgetItem *previous);
};
