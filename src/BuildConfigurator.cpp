#include "BuildConfigurator.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>
#include <Qt>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtCore/QCoreApplication>
#include <QtGui/QFontDatabase>
#include <functional>

#include <string>
#include <vector>

#include "ConfigManager.h"
#include "MainWindow.h"
#include "LogManager.h"
#include "PlatformRunner.h"

std::string window_title_base = "Build Configurator - ";

BuildConfigurator::BuildConfigurator(QWidget* parent, bool padvanced) : QWidget(parent, Qt::Window) {
    // Load config
    QString default_home = Config::getBuildHome();
    if (default_home != "_None" && default_home != "") {
        target_directory_selected_label.setText(default_home);
    }
    if (Config::getROMPath(BuildConfigurator::SM64_Region::US) != "_None")
        region_select.addItem("US");
    if (Config::getROMPath(BuildConfigurator::SM64_Region::JP) != "_None")
        region_select.addItem("JP");
    

    // Init default
    advanced = padvanced;

    setLocations();
    setFixedSize(window_w,window_h);
    setAdvanced(advanced);

    enableCompileInput(false);

    setWindowTitle((window_title_base + (advanced ? "Advanced" : "Default")).c_str());

    // Connect things
    QObject::connect(&target_directory_button, &QPushButton::released, this, &BuildConfigurator::selectTargetDirectory);
    QObject::connect(&download_files, &QPushButton::released, this, &BuildConfigurator::confirmAndDownloadRepo);
    QObject::connect(&start_compile, &QPushButton::released, this, &BuildConfigurator::compileBuild);
    QObject::connect(&apply_patches, &QPushButton::released, this, &BuildConfigurator::selectAndApplyPatches);

    // Link logs to output
    LogManager::forkLogTo(&this->subprocess_output);
};

void BuildConfigurator::setLocations() {
    repo_select.setGeometry(30,30,250,30);
    repo_select_label.setGeometry(300,30,180,30);
    branch_select.setGeometry(30,70,250,30);
    branch_select_label.setGeometry(300,70,180,30);
    target_directory_button.setGeometry(30,110,180,30);
    target_directory_button_label.setGeometry(300, 110,180,30);
    target_directory_selected_label.setGeometry(30,150,500,30);
    name_select.setGeometry(30,190,250,30);
    name_select_label.setGeometry(300, 190, 180, 30);
    download_files.setGeometry(30,230,180,30);
    download_files_label.setGeometry(300,230,180,30);
    subprocess_output.setGeometry(500,30,570,400);
    region_select.setGeometry(30,300,180,30);
    region_select_label.setGeometry(300,300,180,30);
    make_flags.setGeometry(30,340,250,30);
    make_flags_label.setGeometry(300,340,180,30);
    apply_patches.setGeometry(30,380,180,30);
    start_compile.setGeometry(30,420,180,30);
}

void BuildConfigurator::setAdvanced(bool enabled) {
    repo_select.setEnabled(enabled);
    branch_select.setEnabled(enabled);
    make_flags.setEnabled(enabled);
    apply_patches.setEnabled(enabled);
}

void BuildConfigurator::closeEvent(QCloseEvent *event) {
    LogManager::unlinkFork();
    parentWidget()->setEnabled(true);
    // No need to delete now. MainWindow will, either on close or on regen
}

void BuildConfigurator::selectTargetDirectory() {
    QString directory = QFileDialog::getExistingDirectory(this, "Select Target Directory");
    if (directory != "") {
        target_directory_selected_label.setText(directory);
    }
}

void BuildConfigurator::confirmAndDownloadRepo() {
    // Create build struct
    active_build = {
        name_select.text(),
        repo_select.text(),
        branch_select.text(),
        target_directory_selected_label.text(),
        SM64_Region::Undef
    };
    // Target directory
    QDir target_dir = active_build.directory;
    // Sanity Checks
    if (!target_dir.exists()) { QMessageBox::critical(this, "Invalid Target Directory", "The directory you selected does not exist."); return;}
    if (name_select.text().isEmpty()) { QMessageBox::critical(this, "Invalid Build Name", "You have not entered a build name"); return; }
    if (Config::getBuilds().count(active_build.name)) {
        QMessageBox::StandardButton answer = QMessageBox::question(this, "Existing Build Name", "An existing build already has this name. Do you want to remove its entry?");
        if (answer == QMessageBox::StandardButton::Yes) {
            Config::removeBuild(this, active_build.name);
        }
        return;
    }
    QDir build_path = active_build.directory + "/" + active_build.name;
    if (build_path.exists()) {
        QMessageBox::StandardButton answer = QMessageBox::question(this, "Folder exists", "There already is a folder called '" + active_build.name + "' in the target directory.\nDo you want to remove it and continue?");
        if (answer == QMessageBox::StandardButton::Yes) {
            if (!build_path.removeRecursively()) {
                QMessageBox::critical(this, "Could not remove folder", "The folder at '" + build_path.absolutePath() + "' could not be deleted!\nCheck if it is open in another program, or write protected.");
                return;
            }
        } else {
            return;
        }
    }
    // Disable Window functions
    enableDLInput(false);
    // Log
    LogManager::writeToLog("### New build requested, data start ###\n");
    LogManager::writeToLog("Repository: " + active_build.repo + "\n");
    LogManager::writeToLog("Branch: " + active_build.branch  + "\n");
    LogManager::writeToLog("Target Directory: " + active_build.directory  + "\n");
    LogManager::writeToLog("Build Name: " + active_build.name  + "\n");
    LogManager::writeToLog("### Data end, starting download ###\n");
    // START DOWNLOAD
    std::function<void(int)> callback = std::bind(&BuildConfigurator::DLFinishCallback, this, std::placeholders::_1);
    PlatformRunner::runProcess("./presets/repo_dl.sh", active_build, callback);
}

void BuildConfigurator::selectAndApplyPatches() {
    enableCompileInput(false);
    active_build.patches = QFileDialog::getOpenFileNames(this, "Select Patch Files", active_build.directory + "/" + active_build.name + "/enhancements", "Patch Files (*.patch)");
    if (active_build.patches.empty()) {
        QMessageBox::information(this, "No patches selected", "No patches were selected, none will be applied.");
        enableCompileInput(true);
        return;
    }
    LogManager::writeToLog("### Applying Patches ###\n");
    for (QString patch : active_build.patches) {
        QFileInfo patch_file(patch);
        LogManager::writeToLog("Patch: " + patch_file.fileName() + " [" + patch_file.absoluteFilePath() + "]" + "\n");
    }
    std::function<void(int)> callback = std::bind(&BuildConfigurator::PatchApplyCallback, this, std::placeholders::_1);
    PlatformRunner::runProcess("./presets/apply_patches.sh", active_build, callback);
}

void BuildConfigurator::PatchApplyCallback(int exitcode) {
    if (exitcode == 0) {
        QMessageBox::information(this, "Patch application successfull", "There were no errors when applying the patches.");
    } else {
        QMessageBox::critical(this, "Error applying patches", "There were errors when applying the patches. Patches will be rolled back.\nPlease wait until you see 'Repo Reset' in the log window!");
        PlatformRunner::runProcess("./presets/reset_repo.sh", active_build, nullptr);
    }
    enableCompileInput(true);
}

void BuildConfigurator::DLFinishCallback(int exitcode) {
    LogManager::flush();
    enableDLInput(false);
    if (exitcode == 0) {
        LogManager::writeToLog("Repo downloaded successfully.\n");
        Config::setBuildHome(active_build.directory);
        enableCompileInput(true);
    } else {
        QMessageBox::critical(this, "Download error", "Could not download repo!");
    }
}

void BuildConfigurator::enableDLInput(bool enable) {
    repo_select.setEnabled(enable);
    branch_select.setEnabled(enable);
    target_directory_button.setEnabled(enable);
    name_select.setEnabled(enable);
    download_files.setEnabled(enable);
}

void BuildConfigurator::compileBuild() {
    enableCompileInput(false);
    std::function<void(int)> callback = std::bind(&BuildConfigurator::CompileFinishCallback, this, std::placeholders::_1);
    active_build.region = region_select.currentText() == "US" ? BuildConfigurator::SM64_Region::US : (region_select.currentText() == "JP" ? BuildConfigurator::SM64_Region::JP : BuildConfigurator::SM64_Region::Undef);
    active_build.make_flags = make_flags.text();
    LogManager::writeToLog("### Compiling Build ###\n");
    PlatformRunner::runProcess("./presets/compile_build.sh", active_build, callback);
}

void BuildConfigurator::enableCompileInput(bool enable) {
    start_compile.setEnabled(enable);
    region_select.setEnabled(enable);
    make_flags.setEnabled(enable && advanced);
    if (active_build.patches.empty()) {
        apply_patches.setEnabled(enable && advanced);
    }
}

void BuildConfigurator::CompileFinishCallback(int exitcode) {
    LogManager::flush();
    if (exitcode == 0) {
        Config::registerBuild(active_build);
        Config::writeConfig();
        QMessageBox::information(this,"Build successful!", "The build was generated without error, and can be launched from the main launcher window.");
        this->close();
    } else {
        QMessageBox::critical(this,"Build error!", "The build was aborted due to a compile error.\nRe-check the requirements, check the logs and try again.");
        enableDLInput(true);
    }
}
