#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("FITS Stacker");
    app.setOrganizationName("Astro");

    MainWindow w;
    w.show();
    return app.exec();
}
