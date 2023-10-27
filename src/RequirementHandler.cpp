#include "RequirementHandler.h"

#include <QWidget>
#include <QObject>
#include <QPushButton>
#include <QDir>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QFileDialog>
#include <functional>

#include "ConfigManager.h"
#include "LogManager.h"
#include "BuildConfigurator.h"
#include "PlatformRunner.h"

RequirementHandler::RequirementHandler(QWidget* parent, bool padvanced) : QWidget(parent, Qt::Window) {
    // Load config
    #ifdef WIN32
    msys_select.setText(Config::getMSYSPath());
    #endif

    // Init default
    setLocations();
    setFixedSize(window_w,window_h);
    advanced = padvanced;
    setAdvanced(advanced);

    setWindowTitle("SM64APLauncher - Requirements and Debugging");

    // Connect things
    QObject::connect(&select_rom, &QPushButton::released, this, &RequirementHandler::registerROM);
    QObject::connect(&rewrite_config, &QPushButton::released, this, &Config::writeConfig);
    #ifdef WIN32
    QObject::connect(&check_requirements, &QPushButton::released, this, &RequirementHandler::checkRequirementsMSYS);
    QObject::connect(&reinstall_msys, &QPushButton::released, this, &RequirementHandler::reinstallMSYS);
    QObject::connect(&reinstall_dependencies, &QPushButton::released, this, &RequirementHandler::reinstallDependencies);
    #else
    QObject::connect(&check_requirements, &QPushButton::released, this, &RequirementHandler::checkRequirementsROM);
    #endif
    LogManager::forkLogTo(&this->log_output);
}

void RequirementHandler::setLocations() {
    #ifdef WIN32
    msys_select.setGeometry(30,30,250,30);
    msys_select_label.setGeometry(300,30,180,30);
    reinstall_msys.setGeometry(30,230,250,30);
    reinstall_dependencies.setGeometry(30,270,250,30);
    #endif
    select_rom.setGeometry(30,70,250,30);
    check_requirements.setGeometry(30,110,250,30);
    troubleshooting_label.setGeometry(30,150,180,30);
    rewrite_config.setGeometry(30,190,250,30);
    log_output.setGeometry(30,310,410,200);
}

void RequirementHandler::setAdvanced(bool enabled) {
    #ifdef WIN32
    msys_select.setEnabled(enabled);
    reinstall_msys.setEnabled(enabled);
    reinstall_dependencies.setEnabled(enabled);
    #endif
}

#ifdef WIN32
void RequirementHandler::checkRequirementsMSYS() {
    QString msys_dir = QDir{QDir::cleanPath(msys_select.text())}.absolutePath();
    if (msys_dir.contains(" ")) {
        LogManager::writeToLog("Invalid MSYS Path: '" + msys_dir + "'\n");
        QMessageBox::critical(this,"Invalid MSYS Path", "The given MSYS path contains spaces. This is not allowed.");
        return;
    }
    if (!QFile::exists(msys_dir + "/usr/bin/bash.exe")) {
        QMessageBox::StandardButton answer = QMessageBox::question(this, "MSYS not installed or corrupted", "MSYS was not found at the specified location. Attempt reinstall? This will install MSYS at the default location.");
        if (answer == QMessageBox::StandardButton::Yes) {
            reinstallMSYS();
        }
        return;
    }
    Config::setMSYSPath(msys_dir);

    // Confirmed MSYS install dir, now check for dependencies. If successfull, it will continue checking ROMs after callback.
    QString dependency_check_cmd = "pacman -Q";
    for (QString dependency : dependencies) {
        dependency_check_cmd += " " + dependency;
    }
    std::function<void(int)> callback = std::bind(&RequirementHandler::checkMSYSDependencyCallback, this, std::placeholders::_1);
    PlatformRunner::runProcess(dependency_check_cmd, callback);
}
#endif

void RequirementHandler::checkRequirementsROM() {
    QString us_rom_path = Config::getROMPath(BuildConfigurator::SM64_Region::US);
    QString jp_rom_path = Config::getROMPath(BuildConfigurator::SM64_Region::JP);
    // Check if ANY rom is registered
    if (us_rom_path == "_None") {
        if (jp_rom_path == "_None") {
            QMessageBox::critical(this, "No ROMs registered", "Set ROMs to use using the 'Register SM64 ROM' button");
            return;
        }
    }
    // Check if registered ROMs are still there
    #define CHECK_REG_ROMS(region_small,region_large) \
        if (region_small##_rom_path != "_None" && !QFile( region_small##_rom_path).exists()) { \
            QMessageBox::StandardButton answer = QMessageBox::question(this, "ROM not found", "A registered ROM for region " #region_large " was not found. Reset entry from config?"); \
            if (answer == QMessageBox::StandardButton::Ok) { \
                Config::setROMPath("_None", BuildConfigurator::SM64_Region:: region_large); \
            } \
        } \
        if (region_small##_rom_path != "_None" && identifyROM(region_small##_rom_path) != BuildConfigurator::SM64_Region:: region_large) { \
            QMessageBox::critical(this, "ROM Invalid", "ROM file for region " #region_large " is of wrong format or corrupted."); \
        }
    CHECK_REG_ROMS(us, US)
    CHECK_REG_ROMS(jp, JP)

    // All checks passed
    #ifdef WIN32
    QMessageBox::information(this, "No issues found", "There seem to be no issues with your installation.");
    #else
    QMessageBox::information(this, "No issues found", "There seem to be no issues with your installation.\nHowever, dependencies are not checked on linux.");
    #endif
}

void RequirementHandler::checkMSYSDependencyCallback(int exitcode) {
    if (exitcode == 0) {
        // Looks good. just continue with ROMs
        checkRequirementsROM();
    } else {
        QMessageBox::StandardButton answer = QMessageBox::question(this, "Error getting dependencies", "There are either dependencies missing, or there was an error trying to get the info.\nWould you like to attempt an automatic installation?");
        if (answer == QMessageBox::StandardButton::Yes) {
            reinstallDependencies();
        }
    }
}

void RequirementHandler::registerROM() {
    QString rom_file_path = QFileDialog::getOpenFileName(this, "Select ROM", "", "SM64 Rom (*.z64)");
    QFile rom_file(rom_file_path);
    if (!rom_file.exists()) {
        QMessageBox::critical(this, "File not found", "Selected a nonexisting file, or cancelled selection.");
        return;
    }
    BuildConfigurator::SM64_Region region = identifyROM(rom_file_path);
    switch(region) {
        case BuildConfigurator::SM64_Region::US:
            Config::setROMPath(rom_file_path, BuildConfigurator::SM64_Region::US);
            QMessageBox::information(this, "Registered US ROM", "ROM was recognized and registered as US SM64 ROM.");
            break;
        case BuildConfigurator::SM64_Region::JP:
            Config::setROMPath(rom_file_path, BuildConfigurator::SM64_Region::JP);
            QMessageBox::information(this, "Registered JP ROM", "ROM was recognized and registered as JP SM64 ROM.");
            break;
        default:
            QMessageBox::critical(this, "ROM Invalid", "ROM file is of wrong format or corrupted.");
            return;
    }
    Config::writeConfig();
}

BuildConfigurator::SM64_Region RequirementHandler::identifyROM(QString rom_file_path) {
    QFile rom_file(rom_file_path);
    if (!rom_file.exists()) {
        return BuildConfigurator::SM64_Region::Undef;
    }
    if (!rom_file.open(QFile::ReadOnly)) {
        QMessageBox::critical(this, "Couldn't read file", "Could not read file, try again.");
        return BuildConfigurator::SM64_Region::Undef;
    }
    QString rom_sha1 = QCryptographicHash::hash(rom_file.readAll(),QCryptographicHash::Algorithm::Sha1).toHex();
    rom_file.close();
    if (rom_sha1 == "9bef1128717f958171a4afac3ed78ee2bb4e86ce") {
        return BuildConfigurator::SM64_Region::US;
    } else if (rom_sha1 == "8a20a5c83d6ceb0f0506cfc9fa20d8f438cafe51") {
        return BuildConfigurator::SM64_Region::JP;
    } else {
        return BuildConfigurator::SM64_Region::Undef;
    }
}

void RequirementHandler::closeEvent(QCloseEvent *event) {
    parentWidget()->setEnabled(true);
    // No need to delete now. MainWindow will, either on close or on regen
}

void RequirementHandler::reinstallMSYS() {
    QMessageBox::critical(this, "TODO", "Currently not implemented, do it yourself :)");
    return;
    LogManager::writeToLog("Reinstalling MSYS...");
}

void RequirementHandler::reinstallDependencies() {
    enableInput(false);
    LogManager::writeToLog("Reinstalling Dependencies...\n");
    // First, update MSYS
    std::function<void(int)> callback = std::bind(&RequirementHandler::updateMSYSCallback, this, std::placeholders::_1);
    PlatformRunner::runProcess("pacman -Syu --noconfirm", callback);
    // Then, install dependencies in callback
}

void RequirementHandler::updateMSYSCallback(int exitcode) {
    enableInput(true);
    if (exitcode == 0) {
        std::function<void(int)> callback = std::bind(&RequirementHandler::installDependencyCallback, this, std::placeholders::_1);
        QString dependency_install_cmd = "pacman -S --noconfirm";
        for (QString dependency : dependencies) {
            dependency_install_cmd += " " + dependency;
        }
        enableInput(false);
        PlatformRunner::runProcess(dependency_install_cmd, callback);
    } else {
        QMessageBox::critical(this, "Error updating MSYS", "Could not update MSYS! Check the log output for details");
    }
}

void RequirementHandler::installDependencyCallback(int exitcode) {
    enableInput(true);
    if (exitcode == 0) {
        LogManager::writeToLog("Dependencies installed.\n");
        QMessageBox::information(this, "MSYS Dependencies installed", "MSYS was updated and all dependencies were installed successfully.");
    } else {
        QMessageBox::critical(this, "MSYS Dependency install error", "There was an error installing the MSYS dependencies, check log for details.");
    }
}

void RequirementHandler::enableInput(bool enable) {
    setAdvanced(advanced && enable);
    select_rom.setEnabled(enable);
    check_requirements.setEnabled(enable);
    rewrite_config.setEnabled(enable);
}

void RequirementHandler::printToUser(QString str) {
    log_output.moveCursor (QTextCursor::End);
    log_output.insertPlainText (str);
    log_output.moveCursor (QTextCursor::End);
}
