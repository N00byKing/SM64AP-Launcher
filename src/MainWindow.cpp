#include "MainWindow.h"

#include <QApplication>
#include <QPushButton>
#include <QCheckBox>
#include <QWidget>
#include <memory>

#include "ConfigManager.h"
#include "BuildConfigurator.h"
#include "RequirementHandler.h"

MainWindow::MainWindow() {
    // Load config
    use_advanced.setChecked(Config::isAdvanced());

    // Init default
    setLocations();
    setAdvanced(use_advanced.isChecked());
    setFixedSize(window_w,window_h);

    setWindowTitle("SM64APLauncher - Main Window");

    // Connect things
    QObject::connect(&use_advanced, &QCheckBox::toggled, this, &MainWindow::setAdvanced);
    QObject::connect(&create_default_build, &QPushButton::released, this, &MainWindow::spawnDefaultConfigurator);
    QObject::connect(&create_custom_build, &QPushButton::released, this, &MainWindow::spawnAdvancedConfigurator);
    QObject::connect(&recheck_requirements, &QPushButton::released, this, &MainWindow::spawnRequirementHandler);
}

void MainWindow::setLocations() {
    create_default_build.setGeometry(550,100,200,30);
    create_custom_build.setGeometry(550,140,200,30);
    recheck_requirements.setGeometry(550,180,200,30);
    use_advanced.setGeometry(555, 270, 200, 30);
}

void MainWindow::setAdvanced(bool enable) {
    create_custom_build.setVisible(enable);
    Config::setAdvanced(enable);
}

void MainWindow::spawnDefaultConfigurator() {
    configurator = std::make_unique<BuildConfigurator>(this,false);
    configurator->show();
    this->hide();
}

void MainWindow::spawnAdvancedConfigurator() {
    configurator = std::make_unique<BuildConfigurator>(this,true);
    configurator->show();
    this->hide();
}

void MainWindow::spawnRequirementHandler() {
    requirement_handler = std::make_unique<RequirementHandler>(this,use_advanced.isChecked());
    requirement_handler->show();
    this->hide();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // System quit. Destroy stuff on the way out
    Config::writeConfig();
    configurator.reset();
    requirement_handler.reset();
}
