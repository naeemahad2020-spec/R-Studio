#include "Database.h"
#include "Logger.h"
#include "MainWindow.h"
#include "SceneModel.h"
#include <QApplication>
#include <QDir>
#include <QStandardPaths>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("R STUDIO");
    QApplication::setOrganizationName("RStudio");

    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    Logger::init(dir + "/rstudio.log");
    Database::open(dir + "/rstudio.db");
    SceneModel::ensureSchema();

    MainWindow w;
    w.show();
    return app.exec();
}
