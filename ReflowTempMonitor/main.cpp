#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif

    a.setApplicationName("ReflowTempMonitor");
    a.setApplicationVersion("1.0.0");
    a.setOrganizationName("OpenAI");
    a.setOrganizationDomain("local");

    MainWindow w;
    w.show();

    return a.exec();
}
