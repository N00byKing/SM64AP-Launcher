#include "MainWindow.h"

#include <QApplication>
#include <QPushButton>
#include <QCheckBox>
#include <QShowEvent>
#include <QWidget>
#include <QListWidget>
#include <QMessageBox>
#include <QFile>
#include <QObject>
#include <QFileInfo>
#include <memory>
#include <map>

#include "BuildManager.h"
#include "ConfigManager.h"
#include "BuildConfigurator.h"
#include "PlatformRunner.h"
#include "RequirementHandler.h"

MainWindow::MainWindow() {
    // Load config
    use_advanced.setChecked(Config::isAdvanced());
    show_require_firstrun = Config::isFirstRun();

    // Init default
    setLocations();
    setAdvanced(use_advanced.isChecked());
    setFixedSize(window_w,window_h);

    build_list.setSelectionMode(QListWidget::SingleSelection);
    selected_build_info.setReadOnly(true);

    play_build.setEnabled(false);
    manage_build.setEnabled(false);
    launch_options.setEnabled(false);

    save_launch_opts.setChecked(true);

    setWindowTitle("SM64APLauncher - Main Window");

    // Connect things
    QObject::connect(&use_advanced, &QCheckBox::toggled, this, &MainWindow::setAdvanced);
    QObject::connect(&play_build, &QPushButton::released, this, &MainWindow::startGame);
    QObject::connect(&manage_build, &QPushButton::released, this, &MainWindow::spawnBuildManager);
    QObject::connect(&create_default_build, &QPushButton::released, this, &MainWindow::spawnDefaultConfigurator);
    QObject::connect(&create_custom_build, &QPushButton::released, this, &MainWindow::spawnAdvancedConfigurator);
    QObject::connect(&recheck_requirements, &QPushButton::released, this, &MainWindow::spawnRequirementHandler);
    QObject::connect(&build_list, &QListWidget::currentItemChanged, this, &MainWindow::buildSelectionHandler);
}

void MainWindow::setLocations() {
    build_list_label.setGeometry(30,10,200,30);
    launch_options_label.setGeometry(240,10,200,30);
    launch_options.setGeometry(240,40,250,100);
    build_list.setGeometry(30,40,200,100);
    build_info_label.setGeometry(30,140,150,30);
    selected_build_info.setGeometry(30,170,460,100);
    play_build.setGeometry(550,40,200,30);
    save_launch_opts.setGeometry(550, 70, 200,30);
    manage_build.setGeometry(550,100,200,30);
    create_default_build.setGeometry(550,160,200,30);
    create_custom_build.setGeometry(550,200,200,30);
    recheck_requirements.setGeometry(550,240,200,30);
    use_advanced.setGeometry(555, 270, 220, 30);
}

void MainWindow::setAdvanced(bool enable) {
    create_custom_build.setVisible(enable);
    Config::setAdvanced(enable);
}

void MainWindow::parseBuilds() {
    build_list.clear();
    std::map<QString,BuildConfigurator::SM64_Build> builds = Config::getBuilds();
    for (std::pair<QString,BuildConfigurator::SM64_Build> build : builds) {
        build_list.addItem(build.first);
    }
    if (build_list.count() == 0)
        build_list.addItem("No builds!\nCreate a new one.");
}

void MainWindow::spawnBuildManager() {
    if (selected_build.name == "_None") {
        QMessageBox::information(this, "No build selected", "You need to select a build from the left first.\nIf there are none, create one with the buttons below.");
        return;
    }
    build_manager = std::make_unique<BuildManager>(this, selected_build, use_advanced.isChecked());
    this->setEnabled(false);
    build_manager->setEnabled(true);
    build_manager->show();
}

void MainWindow::spawnDefaultConfigurator() {
    if (!Config::hasRomRegistered()) {
        QMessageBox::information(this, "No ROM registered", "You need to register a ROM first.\nIn the main screen, click 'Requirements and Debugging', then 'Register SM64 ROM'");
        return;
    }
    configurator = std::make_unique<BuildConfigurator>(this,false);
    this->setEnabled(false);
    configurator->setEnabled(true);
    configurator->show();
}

void MainWindow::spawnAdvancedConfigurator() {
    if (!Config::hasRomRegistered()) {
        QMessageBox::information(this, "No ROM registered", "You need to register a ROM first.\nIn the main screen, click 'Requirements and Debugging', then 'Register SM64 ROM'");
        return;
    }
    configurator = std::make_unique<BuildConfigurator>(this,true);
    this->setEnabled(false);
    configurator->setEnabled(true);
    configurator->show();
}

void MainWindow::spawnRequirementHandler() {
    requirement_handler = std::make_unique<RequirementHandler>(this,use_advanced.isChecked());
    this->setEnabled(false);
    requirement_handler->setEnabled(true);
    requirement_handler->show();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // System quit. Destroy stuff on the way out
    Config::writeConfig();
    configurator.reset();
    requirement_handler.reset();
    build_manager.reset();
}

void MainWindow::showEvent(QShowEvent *event) {
    if (Config::isFirstRun() && show_require_firstrun) {
        show_require_firstrun = false;
        spawnRequirementHandler();
    } else {
        event->accept();
    }
    parseBuilds();
}

void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::EnabledChange) parseBuilds();
}

void MainWindow::buildSelectionHandler(QListWidgetItem *current, QListWidgetItem *previous) {
    if (!current) return;
    QString selected_build_name = current->text();
    if (selected_build_name.contains("No builds")) return;
    selected_build = Config::getBuilds()[selected_build_name];

    // Build Info String for textbox
    QString build_info_string =
                 "Repo: " + selected_build.repo
        + "\n" + "Branch: " + selected_build.branch
        + "\n" + "Region: " + (selected_build.region == BuildConfigurator::SM64_Region::US ? "US" : (selected_build.region == BuildConfigurator::SM64_Region::JP ? "JP" : "!UNKNOWN!"))
        + "\n" + "Directory: " + selected_build.directory
        + "\n" + "Make Flags: " + selected_build.make_flags;
    build_info_string += "\nPatches: ";
    for (QString patch : selected_build.patches) {
        build_info_string += QFileInfo(patch).fileName() + " ";
    }
    selected_build_info.setPlainText(build_info_string);

    // Set launch options
    launch_options.setPlainText(selected_build.default_launch_opts);

    play_build.setEnabled(true);
    manage_build.setEnabled(true);
    launch_options.setEnabled(true);
}

void MainWindow::startGame() {
    if (selected_build.name == "_None") {
        QMessageBox::information(this, "No build selected", "You need to select a build from the left first.\nIf there are none, create one with the buttons below.");
        return;
    }
    if (!QFile::exists(selected_build.directory + "/" + selected_build.name)) {
        QMessageBox::critical(this, "Build not found", "The build folder does not seem to exist. Did you move / rename it?\n Not in: '" + selected_build.directory + "/" + selected_build.name + "'");
        return;
    }
    if (save_launch_opts.isChecked()) {
        selected_build.default_launch_opts = launch_options.toPlainText();
        Config::registerBuild(selected_build);
        Config::writeConfig();
    }
    QString no_newlines_args = launch_options.toPlainText().replace("\n", " ");
    PlatformRunner::runProcessDetached("./presets/run_game.sh " + no_newlines_args, selected_build);
}
