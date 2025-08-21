#include "mainwindow.h"
#include <QApplication>
#include "logger.h"

int main(int argc, char *argv[]) {
    Logger& logger = Logger::getInstance();
    logger.setLogFile("messengerClient.log");
    logger.createLogFile();
    logger.open();
    logger.log(QtInfoMsg, "Приложение запущено.");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.exec();
    logger.close();
    return 0;
}
