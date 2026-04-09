#ifndef CONNECTIONPAGE_H
#define CONNECTIONPAGE_H

#include <QWidget>

class QListWidget;
class QLabel;
class QPushButton;
class SerialManager;

class ConnectionPage : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionPage(SerialManager *manager, QWidget *parent = nullptr);

signals:
    void requestEnterMonitor();

private slots:
    void refreshPorts();
    void connectSelectedPort();
    void onConnected();
    void onDisconnected();
    void onConnectionFailed(const QString &msg);

private:
    QString selectedPortName() const;

private:
    SerialManager *m_manager;
    QListWidget *m_portList;
    QLabel *m_statusLabel;
    QLabel *m_infoLabel;
    QPushButton *m_refreshBtn;
    QPushButton *m_connectBtn;
};

#endif // CONNECTIONPAGE_H
