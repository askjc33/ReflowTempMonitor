#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "dataparser.h"
#include "model.h"
#include "mainwindow.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);

private slots:
    // 串口相关
    void onSerialConnectSuccess(const QString &portName);
    void onSerialConnectFailed(const QString &msg);
    void onSerialDisconnected();
    void onSerialError(const QString &msg);
    void onBatteryReceived(int value);

    // 数据相关
    void onRecordsUpdated();
    void onBoardSelected(const QString &barcode);
    void onExportRequested();
    void onClearFinishedRequested();
    void onDisconnectRequested();

private:
    DataParser serialParser_;
    Model dataModel_;
    MainWindow *mainWindow_;
};

#endif // CONTROLLER_H
