#include <QApplication>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("QtTimer");
    QCoreApplication::setApplicationName("QtTimer-v1");
    MainWindow window;
    window.show();
    return app.exec();
}
