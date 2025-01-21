#include "ConfigManager.h"

#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFileDialog>
#include <QInputDialog>

#include "LogManager.h"
#include "version.h"
#include "BuildConfigurator.h"

namespace Config {

QFile config_file;
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
    #ifdef WIN32
    config_file.setFileName(QCoreApplication::applicationDirPath() + "/SM64APLauncher_config.json");
    #else
    config_file.setFileName(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/SM64APLauncher_config.json");
    #endif
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
    config["builds"] = QJsonObject();
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

bool hasRomRegistered() {
    return getROMPath(BuildConfigurator::SM64_Region::JP) != "_None" ||
           getROMPath(BuildConfigurator::SM64_Region::US) != "_None";
}

void setBuildHome(QString path) {
    config["default_build_root"] = path;
}

QString getBuildHome() {
    return config["default_build_root"].toString();
}

void registerBuild(BuildConfigurator::SM64_Build const& build) {
    QJsonObject builds = config["builds"].toObject();
    QJsonArray patches;
    for (QString patch : build.patches)
        patches.append(patch);
    QJsonObject build_o =
    {
        {"repo", build.repo},
        {"branch", build.branch},
        {"directory", build.directory},
        {"region", (build.region == BuildConfigurator::SM64_Region::US ? "US" : (build.region == BuildConfigurator::SM64_Region::JP ? "JP" : "!UNKNOWN!"))},
        {"make_flags", build.make_flags},
        {"patches", patches},
        {"launch_opts", build.default_launch_opts}
    };
    builds[build.name] = build_o;
    config["builds"] = builds;
    LogManager::writeToLog("Added build \"" + build.name + "\"\n");
}

void removeBuildRef(QString const& name) {
    QJsonObject builds = config["builds"].toObject();
    builds.remove(name);
    config["builds"] = builds;
    LogManager::writeToLog("Removed build \"" + name + "\"\n");
}

void removeBuild(QWidget* parent, QString name) {
    QDir build_folder = config["builds"][name]["directory"].toString() + "/" + name;
    QMessageBox::StandardButton answer = QMessageBox::question(parent, "Delete Build", "Do you want to DELETE (not just remove the config entry!) this build?\nThe following directory will be deleted:\n" + build_folder.absolutePath());
    if (answer == QMessageBox::StandardButton::Yes) {
        if (!build_folder.removeRecursively()) {
            QMessageBox::critical(parent,"Deletion failed", "Could not delete build folder! Is the game still running?");
            return;
        }
    }
    removeBuildRef(name);
}

void renameBuild(QWidget* parent, BuildConfigurator::SM64_Build& build) {
    QString new_name = QInputDialog::getText(parent, "Select new name", "Enter the new name for this build");
    if (new_name.isEmpty()) {
        QMessageBox::critical(parent, "Invalid Name", "Empty name given!");
        return;
    }
    QString new_build_dir = config["builds"][build.name]["directory"].toString() + "/" + new_name;
    QString old_build_dir = config["builds"][build.name]["directory"].toString() + "/" + build.name;
    bool success = QDir(old_build_dir).rename(old_build_dir, new_build_dir);
    if (!success) {
        QMessageBox::critical(parent,"Rename failed", "Could not move build folder! Is the game still running?");
        return;
    }
    QString old_name = build.name;
    build.name = new_name;
    removeBuildRef(old_name);
    registerBuild(build);
    LogManager::writeToLog("Renamed build \"" + old_name + "\" to \"" + build.name + "\"\n");
}

void moveBuild(QWidget* parent, BuildConfigurator::SM64_Build& build) {
    QString new_parent_dir = QFileDialog::getExistingDirectory(parent, "Select new parent Directory");
    QString old_build_dir = config["builds"][build.name]["directory"].toString() + "/" + build.name;
    bool success = QDir(old_build_dir).rename(old_build_dir, new_parent_dir + "/" + build.name);
    if (!success) {
        QMessageBox::critical(parent,"Move failed", "Could not move build folder! Is the game still running?");
        return;
    }
    build.directory = new_parent_dir;
    registerBuild(build);
    LogManager::writeToLog("Moved build \"" + build.name + "\" from \"" + old_build_dir + "\" to \"" + build.directory + "\"\n");
}

std::map<QString,BuildConfigurator::SM64_Build> getBuilds() {
    std::map<QString,BuildConfigurator::SM64_Build> builds;
    for (QString name : config["builds"].toObject().keys()) {
        BuildConfigurator::SM64_Build build;
        build.name = name;
        QJsonValue build_o = config["builds"][build.name];

        build.repo = build_o["repo"].toString();
        build.branch = build_o["branch"].toString();
        build.directory = build_o["directory"].toString();
        build.region = build_o["region"].toString() == "US" ? BuildConfigurator::SM64_Region::US : (build_o["region"].toString() == "JP" ? BuildConfigurator::SM64_Region::JP : BuildConfigurator::SM64_Region::Undef);
        build.make_flags = build_o["make_flags"].toString();
        for (QJsonValue patch_o : build_o["patches"].toArray()) {
            build.patches.append(patch_o.toString());
        }
        build.default_launch_opts = build_o["launch_opts"].toString();
        builds[build.name] = build;
    }
    return builds;
}

}
