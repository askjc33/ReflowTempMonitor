#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QHash>
#include <QVector>
#include <QStringList>
#include <QByteArray>

// 串口设备信息结构
struct PortItem {
    QString portName;
    QString description;
    QString manufacturer;
    QString systemLocation;
};

// 串口数据解析类：负责串口通信、数据帧同步、解析
class DataParser : public QObject
{
    Q_OBJECT
public:
    explicit DataParser(QObject *parent = nullptr);

    // 扫描可用串口
    void scanPorts();
    // 连接串口
    bool connectPort(const QString &portName, qint32 baudRate = QSerialPort::Baud115200);
    // 断开串口
    void disconnectPort();
    // 获取连接状态
    bool isConnected() const;

signals:
    void scanFinished(const QList<PortItem> &ports);
    void connectSuccess(const QString &portName);
    void connectFailed(const QString &errorMsg);
    void disconnected();
    void notifyError(const QString &errorMsg);
    void batteryReceived(int value);
    // 同步帧数据信号（温度+位置）
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

    QSerialPort serial_;
    QByteArray rxBuffer_;
    QHash<int, QVector<double>> tempFrames_;
    QHash<int, QStringList> posFrames_;
    static const int ZONE_COUNT = 12;
};

#endif // DATAPARSER_H
