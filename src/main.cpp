#include <QApplication>

#include "LogManager.h"
#include "ConfigManager.h"
#include "MainWindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    initLogs();
    if (!Config::initConfig()) {
        app.exit(1);
        return 1;
    }

    MainWindow main_win;

    main_win.show();

    return app.exec();
}
