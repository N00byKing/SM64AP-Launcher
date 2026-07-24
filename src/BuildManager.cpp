#include "BuildManager.h"

#include <QtWidgets/QWidget>
#include <QtCore/QObject>
#include <QtWidgets/QPushButton>
#include <QtCore/QDir>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "BuildConfigurator.h"
#include "ConfigManager.h"

BuildManager::BuildManager(QWidget* parent, BuildConfigurator::SM64_Build& build, bool padvanced) : QWidget(parent, Qt::Window), sel_build(build) {
    // Init default
    setLocations();
    setFixedSize(window_w,window_h);
    advanced = padvanced;
    setAdvanced(advanced);

    setWindowTitle("SM64APLauncher - Build Manager");

    // Connect things
    QObject::connect(&rename_build, &QPushButton::released, this, &BuildManager::renameBuild);
    QObject::connect(&move_build, &QPushButton::released, this, &BuildManager::moveBuild);
    QObject::connect(&delete_build, &QPushButton::released, this, &BuildManager::deleteBuild);

    build_info.setText("Managing build: " + sel_build.name);
}

void BuildManager::setLocations() {
    build_info.setGeometry(30,30,280,30);
    rename_build.setGeometry(30,70,250,30);
    move_build.setGeometry(30,110,250,30);
    delete_build.setGeometry(30,150,250,30);
}

void BuildManager::renameBuild() {
    Config::renameBuild(this, sel_build);
    close();
}

void BuildManager::moveBuild() {
    Config::moveBuild(this, sel_build);
    close();
}

void BuildManager::deleteBuild() {
    QMessageBox::StandardButton answer = QMessageBox::question(this, "Confirm Removal", "Are you sure you want to remove build \"" + sel_build.name + "\"?"); \
    if (answer != QMessageBox::StandardButton::Yes) return;
    Config::removeBuild(this, sel_build.name);
    Config::writeConfig();
    // Build no longer exists. Managing it makes no sense, return to main window
    close();
}

void BuildManager::closeEvent(QCloseEvent *event) {
    parentWidget()->setEnabled(true);
    // No need to delete now. MainWindow will, either on close or on regen
}

void BuildManager::setAdvanced(bool enable) {}

void BuildManager::enableInput(bool enable) {
    setAdvanced(advanced && enable);
    rename_build.setEnabled(enable);
    move_build.setEnabled(enable);
    delete_build.setEnabled(enable);
}
