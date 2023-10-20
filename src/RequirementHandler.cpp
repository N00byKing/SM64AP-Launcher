#include "RequirementHandler.h"

#include <QWidget>
#include <QObject>
#include <QPushButton>
#include <QDir>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QFileDialog>

#include "ConfigManager.h"
#include "LogManager.h"
#include "BuildConfigurator.h"

RequirementHandler::RequirementHandler(QWidget* parent, bool advanced) : QWidget(parent, Qt::Window) {
    // Load config
    #ifdef WIN32
    msys_select.setText(Config::getMSYSPath());
    #endif

    // Init default
    setLocations();
    setFixedSize(window_w,window_h);
    setAdvanced(advanced);

    setWindowTitle("SM64APLauncher - Requirements and Debugging");

    // Connect things
    QObject::connect(&check_requirements, &QPushButton::released, this, &RequirementHandler::checkRequirements);
    QObject::connect(&select_rom, &QPushButton::released, this, &RequirementHandler::registerROM);
    QObject::connect(&rewrite_config, &QPushButton::released, this, &Config::writeConfig);
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
}

void RequirementHandler::setAdvanced(bool enabled) {
    #ifdef WIN32
    msys_select.setEnabled(enabled);
    reinstall_msys.setEnabled(enabled);
    reinstall_dependencies.setEnabled(enabled);
    #endif
}

void RequirementHandler::checkRequirements() {
    #ifdef WIN32
    QString msys_dir = QDir{QDir::cleanPath(msys_select.text())}.absolutePath();
    if (msys_dir.contains(" ")) {
        QMessageBox::critical(this,"Invalid MSYS Path", "The given MSYS path contains spaces. This is not allowed.");
        return;
    }
    if (!QFile::exists(msys_dir + "/usr/bin/bash.exe")) {
        QMessageBox::StandardButton answer = QMessageBox::question(this, "MSYS not installed or corrupted", "MSYS was not found at the specified location. Attempt reinstall? This will install MSYS at the default location.");
        if (answer == QMessageBox::StandardButton::Yes) {
            // TODO automatic downloading of MSYS
        }
        return;
    }
    Config::setMSYSPath(msys_dir);
    // TODO install requirements
    #endif
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
            return;
        case BuildConfigurator::SM64_Region::JP:
            Config::setROMPath(rom_file_path, BuildConfigurator::SM64_Region::JP);
            QMessageBox::information(this, "Registered JP ROM", "ROM was recognized and registered as JP SM64 ROM.");
            return;
        default:
            QMessageBox::critical(this, "ROM Invalid", "ROM file is of wrong format or corrupted.");
            return;
    }
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
