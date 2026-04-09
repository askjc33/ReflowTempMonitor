#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>

class SerialManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialManager(QObject *parent = nullptr);
    ~SerialManager();

    QStringList availablePorts() const;
    bool connectPort(const QString &portName, qint32 baudRate = QSerialPort::Baud115200);
    void disconnectPort();
    bool isConnected() const;
    QString currentPortName() const;

signals:
    void connected();
    void disconnected();
    void connectionFailed(const QString &msg);
    void rawDataReceived(const QByteArray &data);
    void batteryReceived(int value);
    void temperaturesReceived(const QVector<double> &values);
    void errorOccurred(const QString &msg);

private slots:
    void onReadyRead();
    void onSerialError(QSerialPort::SerialPortError error);

private:
    void parseBuffer();
    void tryEmitPacket();

private:
    QSerialPort *m_serial;
    QByteArray m_buffer;
    QVector<double> m_packet;
};

#endif // SERIALMANAGER_H
