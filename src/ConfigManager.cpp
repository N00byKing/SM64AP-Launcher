#include "ConfigManager.h"

#include <QFile>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>

#include "version.h"
#include "BuildConfigurator.h"

namespace Config {

QFile config_file{"config.json",nullptr};
QJsonObject config;
bool first_run = false;

void writeConfig() {
    if (!config_file.open(QIODeviceBase::WriteOnly | QIODeviceBase::Truncate)) {
        QMessageBox::critical(nullptr, "Could not write to config file", "The config file is inaccessible.\nMake sure config.json is not write-protected or opened by another program.\nTo avoid data loss, trigger a manual config save from the 'Re-check requirements' window.");
        return;
    }
    config_file.write(QJsonDocument(config).toJson(QJsonDocument::Compact));
    config_file.close();
}

bool isFirstRun() { return first_run; }

bool initConfig() {
    if (!config_file.exists()) { first_run = true; }
    if (!config_file.open(QIODeviceBase::ReadWrite)) {
        QMessageBox::critical(nullptr, "Could not read/write config file", "The config file is inaccessible.\nMake sure config.json is not write-protected or opened by another program.\nThe launcher will now exit.");
        return false;
    }
    QByteArray file_data = config_file.readAll();
    QJsonParseError error;
    QJsonDocument config_doc = QJsonDocument::fromJson(file_data, &error);
    config_file.close();
    if (error.error != QJsonParseError::NoError && !first_run) {
        QMessageBox::critical(nullptr, "Could not read config file", "The config file is corrupted.\nDelete config.json and try again. You will lose the list of builds.");
        return false;
    }
    config = config_doc.object();
    config["version"] = QString(GIT_REV);
    if (first_run) resetToDefault();
    writeConfig();
    config_file.close();
    return true;
}

void resetToDefault() {
    setAdvanced(false);
    setMSYSPath(default_msys_path);
    setROMPath("_None",BuildConfigurator::SM64_Region::US);
    setROMPath("_None",BuildConfigurator::SM64_Region::JP);
    setBuildHome("_None");
}

void setAdvanced(bool enable) {
    config["advanced"] = enable;
}

bool isAdvanced() {
    return config["advanced"].toBool();
}

void setMSYSPath(QString path) {
    config["msys_path"] = path;
}

QString getMSYSPath() {
    return config["msys_path"].toString();
}

void setROMPath(QString path, BuildConfigurator::SM64_Region region) {
    QJsonObject rom_paths = config["rom_paths"].toObject();
    switch (region) {
        case BuildConfigurator::SM64_Region::US:
            rom_paths["us"] = path;
            break;
        case BuildConfigurator::SM64_Region::JP:
            rom_paths["jp"] = path;
            break;
        default:
            break;
    }
    config["rom_paths"] = rom_paths;
}

QString getROMPath(BuildConfigurator::SM64_Region region) {
    QJsonObject rom_paths = config["rom_paths"].toObject();
    switch (region) {
        case BuildConfigurator::SM64_Region::US:
            return rom_paths["us"].toString();
            break;
        case BuildConfigurator::SM64_Region::JP:
            return rom_paths["jp"].toString();
            break;
        default:
            return "_None";
            break;
    }
}

void setBuildHome(QString path) {
    config["default_build_root"] = path;
}

QString getBuildHome() {
    return config["default_build_root"].toString();
}

}
