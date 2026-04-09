#pragma once
#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QTimer>

/*
 * 串口协议帧格式（自定义，可根据单片机实际协议修改）:
 *
 * 帧头:    0xAA 0x55
 * 命令字:  0x01 (温度数据帧)
 *          0x02 (PCB扫码ID帧)
 * 数据长度: 1 byte (N)
 * 数据:    N bytes
 * 校验:    XOR校验 (数据字节异或)
 * 帧尾:    0x0D 0x0A (\r\n)
 *
 * 温度数据帧数据域 (N=22):
 *   [0]     : PCB ID 长度(K)
 *   [1..K]  : PCB ID 字符串
 *   [K+1]   : 当前气室编号(0~4)
 *   [K+2..K+21]: 5 x float (每个气室温度, 小端4字节float)
 *
 * 扫码ID帧数据域:
 *   [0]     : ID 长度(K)
 *   [1..K]  : ID 字符串
 */

class SerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit SerialWorker(QObject *parent = nullptr);
    ~SerialWorker();

public slots:
    void openPort(const QString &portName, int baudRate);
    void closePort();

signals:
    // 解析出温度数据
    void temperatureReceived(const QString &pcbId,
                             float temp0, float temp1,
                             float temp2, float temp3,
                             float temp4,
                             int currentChamber);
    // 解析出扫码ID
    void barcodeReceived(const QString &pcbId);

    void portOpened(bool success, const QString &msg);
    void portClosed();
    void errorOccurred(const QString &msg);
    void portError(const QString &msg);

private slots:
    void onReadyRead();
    void onErrorOccurred(QSerialPort::SerialPortError error);

private:
    void parseBuffer();
    bool tryParseFrame(const QByteArray &frame);

    QSerialPort *m_serial;
    QByteArray   m_buffer;

    static const quint8 FRAME_HEADER_1 = 0xAA;
    static const quint8 FRAME_HEADER_2 = 0x55;
    static const quint8 FRAME_TAIL_1   = 0x0D;
    static const quint8 FRAME_TAIL_2   = 0x0A;
    static const quint8 CMD_TEMP       = 0x01;
    static const quint8 CMD_BARCODE    = 0x02;
};
