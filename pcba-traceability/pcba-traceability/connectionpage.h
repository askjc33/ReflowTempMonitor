#pragma once

#include "types.h"
#include <QWidget>

class QListWidget;
class QLabel;
class QPushButton;
class QListWidgetItem;

class ConnectionPage : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionPage(QWidget *parent = nullptr);

    void setPortList(const QList<PortItem> &ports);
    void showConnectError(const QString &msg);
    void clearSelection();

signals:
    void requestScanPorts();
    void requestConnectPort(const QString &portName);

private slots:
    void onItemClicked(QListWidgetItem *item);
    void onScanClicked();
    void onConnectClicked();

private:
    void initUi();

private:
    QList<PortItem> ports_;
    QString selectedPort_;

    QListWidget *list_ = nullptr;
    QLabel *infoLabel_ = nullptr;
    QPushButton *scanBtn_ = nullptr;
    QPushButton *connectBtn_ = nullptr;
};
