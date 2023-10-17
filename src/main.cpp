#include <QApplication>

#include "LogManager.h"
#include "MainWindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    initLogs();

    MainWindow main_win;

    main_win.show();

    return app.exec();
}
