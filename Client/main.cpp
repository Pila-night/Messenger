#include "mainwindow.h"

#include <QApplication>
#include "protocol.h"


int main(int argc, char *argv[])
{

    QApplication ui(argc, argv);
    MainWindow w;
    w.show();
    return  ui.exec();
}
