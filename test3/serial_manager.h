#pragma once

#include <QObject>
#include <QHash>
#include <QByteArray>
#include <QStringList>
#include <QVector>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "board_record.h"

class SerialManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialManager(QObject *parent = 0);

    struct PortItem {
        QString portName;
        QString description;
        QString manufacturer;
        QString systemLocation;
    };

    void scanPorts();
    bool connectPort(const QString &portName, qint32 baudRate = QSerialPort::Baud115200);
    void disconnectPort();
    bool isConnected() const;

signals:
    void scanFinished(const QList<SerialManager::PortItem> &ports);
    void connectSuccess(const QString &portName);
    void connectFailed(const QString &errorMsg);
    void disconnected();
    void notifyError(const QString &errorMsg);
    void batteryReceived(int value);
    void synchronizedFrameReceived(const QVector<double> &temps, const QStringList &positions);

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
    QSerialPort serial_;
    QByteArray rxBuffer_;
    QHash<int, QVector<double> > tempFrames_;
    QHash<int, QStringList> posFrames_;
};
