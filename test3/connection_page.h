#pragma once

#include <QWidget>
#include <QList>
#include <QString>

class QListWidget;
class QListWidgetItem;
class QLabel;
class QPushButton;

#include "serial_manager.h"

class ConnectionPage : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionPage(SerialManager *serial, QWidget *parent = 0);

signals:
    void requestSwitchToTracePage();
    void requestSetStatusMessage(const QString &text);
    void requestSetConnectionStatus(const QString &text, bool connected);

private slots:
    void scan();
    void onScanFinished(const QList<SerialManager::PortItem> &ports);
    void onItemClicked(QListWidgetItem *item);
    void doConnect();
    void onConnectSuccess(const QString &portName);
    void onConnectFailed(const QString &msg);

private:
    void initUi();

private:
    SerialManager *serial_;
    QList<SerialManager::PortItem> ports_;
    QString selectedPort_;

    QListWidget *list_;
    QLabel *infoLabel_;
    QPushButton *scanBtn_;
    QPushButton *connectBtn_;
};
