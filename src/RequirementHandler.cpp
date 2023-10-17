#include "RequirementHandler.h"

#include <QWidget>

RequirementHandler::RequirementHandler(QWidget* parent, bool advanced) : QWidget(parent) {
    // Init default
    setLocations();
    setFixedSize(window_w,window_h);
    setAdvanced(advanced);
}

void RequirementHandler::setLocations() {
    msys_select.setGeometry(30,30,250,30);
    msys_select_label.setGeometry(300,30,180,30);
}

void RequirementHandler::setAdvanced(bool enabled) {
    msys_select.setEnabled(enabled);
}
