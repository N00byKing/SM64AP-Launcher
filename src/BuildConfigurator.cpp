#include "BuildConfigurator.h"

#include <QWidget>
#include <Qt>
#include <QFileDialog>
#include <QPushButton>
#include <qfiledialog.h>

#include "MainWindow.h"

constexpr int window_w = 530;
constexpr int window_h = 500;

std::string window_title_base = "Build Configurator - ";

BuildConfigurator::BuildConfigurator(QWidget* parent, bool advanced) : QWidget(parent, Qt::Window) {
    // Init default
    setLocations();
    setAdvanced(false);
    setFixedSize(window_w,window_h);
    setAdvanced(advanced);

    setWindowTitle((window_title_base + (advanced ? "Advanced" : "Default")).c_str());

    // Connect things
    QObject::connect(&target_directory_button, &QPushButton::released, this, &BuildConfigurator::selectTargetDirectory);
};

void BuildConfigurator::setLocations() {
    repo_select.setGeometry(30,30,250,30);
    repo_select_label.setGeometry(300,30,180,30);
    branch_select.setGeometry(30,70,250,30);
    branch_select_label.setGeometry(300,70,180,30);
    target_directory_button.setGeometry(30,110,180,30);
    target_directory_button_label.setGeometry(300, 110,180,30);
    target_directory_selected_label.setGeometry(30,150,500,30);
}

void BuildConfigurator::setAdvanced(bool enabled) {
    repo_select.setEnabled(enabled);
    branch_select.setEnabled(enabled);
}

void BuildConfigurator::closeEvent(QCloseEvent *event) {
    ((MainWindow*)parentWidget())->show();
    this->deleteLater();
}

void BuildConfigurator::selectTargetDirectory() {
    QString directory = QFileDialog::getExistingDirectory(this, "Select Target Directory");
    target_directory_selected_label.setText(directory);
}
