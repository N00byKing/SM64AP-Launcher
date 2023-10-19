#include "BuildConfigurator.h"

#include <QWidget>
#include <Qt>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QCoreApplication>
#include <QFontDatabase>
#include <functional>
#include <string>

#include "MainWindow.h"
#include "LogManager.h"
#include "PlatformRunner.h"

std::string window_title_base = "Build Configurator - ";

BuildConfigurator::BuildConfigurator(QWidget* parent, bool advanced) : QWidget(parent, Qt::Window) {
    // Init default
    setLocations();
    setFixedSize(window_w,window_h);
    setAdvanced(advanced);

    setWindowTitle((window_title_base + (advanced ? "Advanced" : "Default")).c_str());

    // Connect things
    QObject::connect(&target_directory_button, &QPushButton::released, this, &BuildConfigurator::selectTargetDirectory);
    QObject::connect(&download_files, &QPushButton::released, this, &BuildConfigurator::confirmAndDownloadRepo);

    // Link logs to output
    LogManager::forkLogTo(this);
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
    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    subprocess_output.setFont(fixedFont);
    subprocess_output.setLineWrapMode(QPlainTextEdit::NoWrap);
    subprocess_output.setReadOnly(true);
}

void BuildConfigurator::setAdvanced(bool enabled) {
    repo_select.setEnabled(enabled);
    branch_select.setEnabled(enabled);
}

void BuildConfigurator::closeEvent(QCloseEvent *event) {
    LogManager::unlinkFork();
    parentWidget()->show();
    // No need to delete now. MainWindow will, either on close or on regen
}

void BuildConfigurator::selectTargetDirectory() {
    QString directory = QFileDialog::getExistingDirectory(this, "Select Target Directory");
    target_directory_selected_label.setText(directory);
}

void BuildConfigurator::confirmAndDownloadRepo() {
    // Target directory
    QDir target_dir = target_directory_selected_label.text();
    // Sanity Checks
    if (!target_dir.exists()) { QMessageBox::critical(this, "Invalid Target Directory", "The directory you selected does not exist."); return;}
    if (!target_dir.isEmpty()) { QMessageBox::critical(this, "Invalid Target Directory", "The directory you selected is not empty."); return;}
    if (name_select.text().isEmpty()) { QMessageBox::critical(this, "Invalid Build Name", "You have not entered a build name"); return; }
    // Disable Window functions
    disableDLInput();
    // Create build struct
    SM64_Build build = {
        name_select.text(),
        repo_select.text(),
        branch_select.text(),
        target_directory_selected_label.text()
    };
    // Log
    LogManager::writeToLog("### New build requested, data start ###\n");
    LogManager::writeToLog("Repository: " + build.repo + "\n");
    LogManager::writeToLog("Branch: " + build.branch  + "\n");
    LogManager::writeToLog("Target Directory: " + build.directory  + "\n");
    LogManager::writeToLog("Build Name: " + build.name  + "\n");
    LogManager::writeToLog("### Data end, starting download ###\n");
    // START DOWNLOAD
    std::function<void(int)> callback = std::bind(&BuildConfigurator::DLfinishCallback, this, std::placeholders::_1);
    PlatformRunner::runProcess(QCoreApplication::applicationDirPath() + "/presets/repo_dl.sh", build, callback);
}

void BuildConfigurator::printToUser(QString str) {
    subprocess_output.moveCursor (QTextCursor::End);
    subprocess_output.insertPlainText (str);
    subprocess_output.moveCursor (QTextCursor::End);
}

void BuildConfigurator::DLfinishCallback(int exitcode) {
    this->setEnabled(true);
}

void BuildConfigurator::disableDLInput() {
    repo_select.setEnabled(false);
    branch_select.setEnabled(false);
    target_directory_button.setEnabled(false);
    name_select.setEnabled(false);
    download_files.setEnabled(false);
}