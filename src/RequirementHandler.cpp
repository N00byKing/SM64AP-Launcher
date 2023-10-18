#include "RequirementHandler.h"

#include <QWidget>
#include <QObject>
#include <QPushButton>
#include <QDir>
#include <QMessageBox>

#include "ConfigManager.h"

RequirementHandler::RequirementHandler(QWidget* parent, bool advanced) : QWidget(parent, Qt::Window) {
    // Load config
    msys_select.setText(Config::getMSYSPath());

    // Init default
    setLocations();
    setFixedSize(window_w,window_h);
    setAdvanced(advanced);

    setWindowTitle("SM64APLauncher - Requirements and Debugging");

    // Connect things
    QObject::connect(&check_requirements, &QPushButton::released, this, &RequirementHandler::checkRequirements);
    QObject::connect(&rewrite_config, &QPushButton::released, this, &Config::writeConfig);
}

void RequirementHandler::setLocations() {
    msys_select.setGeometry(30,30,250,30);
    msys_select_label.setGeometry(300,30,180,30);
    check_requirements.setGeometry(30,70,250,30);
    troubleshooting_label.setGeometry(30,110,180,30);
    rewrite_config.setGeometry(30,150,250,30);
    reinstall_msys.setGeometry(30,190,250,30);
    reinstall_dependencies.setGeometry(30,230,250,30);
}

void RequirementHandler::setAdvanced(bool enabled) {
    msys_select.setEnabled(enabled);
    troubleshooting_label.setEnabled(enabled);
    reinstall_msys.setEnabled(enabled);
    reinstall_dependencies.setEnabled(enabled);
}

void RequirementHandler::checkRequirements() {
    QString msys_dir = QDir{QDir::cleanPath(msys_select.text())}.absolutePath();
    if (msys_dir.contains(" ")) {
        QMessageBox::critical(this,"Invalid MSYS Path", "The given MSYS path contains spaces. This is not allowed.");
        return;
    }
    if (!QFile::exists(msys_dir + QDir::separator() + "usr" + QDir::separator() + "bin" + QDir::separator() + "bash.exe")) {
        QMessageBox::StandardButton answer = QMessageBox::question(this, "MSYS not installed or corrupted", "MSYS was not found at the specified location. Attempt reinstall? This will install MSYS at the default location.");
        if (answer == QMessageBox::StandardButton::Ok) {
            // TODO automatic downloading of MSYS
        }
        return;
    }
    Config::setMSYSPath(msys_dir);
    // TODO install requirements
}

void RequirementHandler::closeEvent(QCloseEvent *event) {
    parentWidget()->setEnabled(true);
    // No need to delete now. MainWindow will, either on close or on regen
}
