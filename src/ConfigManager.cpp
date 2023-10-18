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

void writeConfig() {
    if (!config_file.open(QIODeviceBase::WriteOnly | QIODeviceBase::Truncate)) {
        QMessageBox::critical(nullptr, "Could not write to config file", "The config file is inaccessible.\nMake sure config.json is not write-protected or opened by another program.\nTo avoid data loss, trigger a manual config save from the 'Re-check requirements' window.");
        return;
    }
    config_file.write(QJsonDocument(config).toJson(QJsonDocument::Compact));
    config_file.close();
}

bool initConfig() {
    bool first_run = false;
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
    writeConfig();
    config_file.close();
    return true;
}

void setAdvanced(bool enable) {
    config["advanced"] = enable;
}

bool isAdvanced() {
    return config["advanced"].toBool();
}

}
