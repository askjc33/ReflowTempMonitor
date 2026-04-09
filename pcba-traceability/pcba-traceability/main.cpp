#include <QApplication>
#include <QMetaType>
#include "types.h"
#include "mainwindow.h"
#include "appcontroller.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<SyncFrame>("SyncFrame");
    qRegisterMetaType<PortItem>("PortItem");
    qRegisterMetaType<QList<PortItem>>("QList<PortItem>");

    MainWindow w;
    AppController controller(&w);
    controller.initialize();

    w.show();
    return app.exec();
}
