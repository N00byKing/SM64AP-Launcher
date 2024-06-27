#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include "BuildConfigurator.h"

class BuildManager : public QWidget {
    public:
        static constexpr int window_w = 300;
        static constexpr int window_h = 200;
        BuildManager(QWidget* parent, BuildConfigurator::SM64_Build& build, bool advanced);
        BuildManager() = delete;
    private:
        using QWidget::QWidget;
        QPushButton rename_build{"Rename Build",this};
        QPushButton move_build{"Move build",this};
        QPushButton delete_build{"Delete Build",this};
        QLabel build_info{"", this};
        bool advanced = false;
        BuildConfigurator::SM64_Build& sel_build;
        void renameBuild();
        void moveBuild();
        void deleteBuild();
        void closeEvent(QCloseEvent *event);
        void setLocations();
        void setAdvanced(bool);
        void enableInput(bool);
};
