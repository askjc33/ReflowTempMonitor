#pragma once

#include "types.h"
#include "constants.h"
#include <QObject>
#include <QSerialPort>
#include <QHash>

class SerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit SerialWorker(QObject *parent = nullptr);
    ~SerialWorker();

public slots:
    void scanPorts();
    void connectPort(const QString &portName, qint32 baudRate = QSerialPort::Baud115200);
    void disconnectPort();

signals:
    void scanFinished(const QList<PortItem> &ports);
    void connectSuccess(const QString &portName);
    void connectFailed(const QString &errorMsg);
    void disconnected();
    void notifyError(const QString &errorMsg);
    void batteryReceived(int value);
    void synchronizedFrameReceived(const SyncFrame &frame);

private slots:
    void onReadyRead();
    void onErrorOccurred(QSerialPort::SerialPortError error);

private:
    void parseLine(const QString &line);
    void parseBattery(const QString &line);
    void parseTempFrame(const QString &line);
    void parsePosFrame(const QString &line);
    void trySyncFrame(int seq);
    void cleanupOldFrames(int latestSeq);

private:
    QSerialPort *serial_ = nullptr;
    QByteArray rxBuffer_;
    QHash<int, QVector<double>> tempFrames_;
    QHash<int, QStringList> posFrames_;
};
