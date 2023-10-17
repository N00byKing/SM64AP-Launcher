#include "MainWindow.h"
#include "BuildConfigurator.h"

#include <QApplication>
#include <QPushButton>
#include <QCheckBox>
#include <QWidget>
#include <memory>

static auto main_win = nullptr;

MainWindow::MainWindow() {
    // Init default
    setLocations();
    setAdvanced(false);
    setFixedSize(window_w,window_h);

    setWindowTitle("SM64APLauncher - Main Window");

    // Connect things
    QObject::connect(&use_advanced, &QCheckBox::toggled, this, &MainWindow::setAdvanced);
    QObject::connect(&create_default_build, &QPushButton::released, this, &MainWindow::spawnDefaultConfigurator);
    QObject::connect(&create_custom_build, &QPushButton::released, this, &MainWindow::spawnAdvancedConfigurator);
}

void MainWindow::setLocations() {
    create_default_build.setGeometry(550,100,200,30);
    create_custom_build.setGeometry(550,140,200,30);
    use_advanced.setGeometry(555, 270, 200, 30);
}

void MainWindow::setAdvanced(bool enable) {
    create_custom_build.setVisible(enable);
}

void MainWindow::spawnDefaultConfigurator() {
    configurator = std::make_unique<BuildConfigurator>(this,false);
    configurator->show();
    this->setVisible(false);
}

void MainWindow::spawnAdvancedConfigurator() {
    configurator = std::make_unique<BuildConfigurator>(this,true);
    configurator->show();
    this->setVisible(false);
}
