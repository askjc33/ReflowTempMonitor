#include "serialworker.h"
#include <QDebug>
#include <cstring>

SerialWorker::SerialWorker(QObject *parent)
    : QObject(parent)
    , m_serial(new QSerialPort(this))
{
    connect(m_serial, &QSerialPort::readyRead,
            this,     &SerialWorker::onReadyRead);
    connect(m_serial, &QSerialPort::errorOccurred,
            this,     &SerialWorker::onErrorOccurred);
}

SerialWorker::~SerialWorker()
{
    if (m_serial->isOpen())
        m_serial->close();
}

void SerialWorker::openPort(const QString &portName, int baudRate)
{
    if (m_serial->isOpen())
        m_serial->close();

    m_serial->setPortName(portName);
    m_serial->setBaudRate(baudRate);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serial->open(QIODevice::ReadWrite)) {
        emit portOpened(true, QStringLiteral("串口 %1 已打开，波特率 %2").arg(portName).arg(baudRate));
    } else {
        emit portOpened(false, QStringLiteral("串口打开失败: ") + m_serial->errorString());
    }
}

void SerialWorker::closePort()
{
    if (m_serial->isOpen()) {
        m_serial->close();
        emit portClosed();
    }
}

void SerialWorker::onReadyRead()
{
    m_buffer.append(m_serial->readAll());
    parseBuffer();
}

void SerialWorker::onErrorOccurred(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError) {
        emit errorOccurred(m_serial->errorString());
    }
}

void SerialWorker::parseBuffer()
{
    // 持续在缓冲区搜索完整帧
    while (true) {
        // 找帧头
        int headerPos = -1;
        for (int i = 0; i + 1 < m_buffer.size(); ++i) {
            if ((quint8)m_buffer[i]   == FRAME_HEADER_1 &&
                (quint8)m_buffer[i+1] == FRAME_HEADER_2) {
                headerPos = i;
                break;
            }
        }

        if (headerPos < 0) {
            // 没有帧头，清空缓冲区（保留最后1字节防止帧头被截断）
            if (m_buffer.size() > 1)
                m_buffer = m_buffer.right(1);
            break;
        }

        // 丢弃帧头前的无效数据
        if (headerPos > 0)
            m_buffer.remove(0, headerPos);

        // 最小帧长：帧头(2) + 命令(1) + 长度(1) + 校验(1) + 帧尾(2) = 7字节
        if (m_buffer.size() < 7)
            break;

        quint8 cmd     = (quint8)m_buffer[2];
        quint8 dataLen = (quint8)m_buffer[3];

        int totalLen = 2 + 1 + 1 + dataLen + 1 + 2; // header+cmd+len+data+xor+tail

        if (m_buffer.size() < totalLen)
            break;  // 数据不够，等待更多

        // 检查帧尾
        if ((quint8)m_buffer[totalLen - 2] != FRAME_TAIL_1 ||
            (quint8)m_buffer[totalLen - 1] != FRAME_TAIL_2) {
            // 帧尾不对，丢弃帧头，继续寻找
            m_buffer.remove(0, 2);
            continue;
        }

        // 校验 XOR
        quint8 xorCalc = 0;
        for (int i = 4; i < 4 + dataLen; ++i)
            xorCalc ^= (quint8)m_buffer[i];

        quint8 xorReceived = (quint8)m_buffer[4 + dataLen];

        if (xorCalc != xorReceived) {
            // 校验失败，丢弃
            m_buffer.remove(0, 2);
            continue;
        }

        // 提取数据域
        QByteArray data = m_buffer.mid(4, dataLen);
        tryParseFrame(QByteArray(1, (char)cmd) + data);

        // 移除已处理的帧
        m_buffer.remove(0, totalLen);
    }
}

bool SerialWorker::tryParseFrame(const QByteArray &frame)
{
    if (frame.isEmpty()) return false;

    quint8 cmd = (quint8)frame[0];
    QByteArray data = frame.mid(1);

    if (cmd == CMD_BARCODE) {
        // 扫码帧
        if (data.size() < 1) return false;
        quint8 idLen = (quint8)data[0];
        if (data.size() < 1 + idLen) return false;
        QString pcbId = QString::fromUtf8(data.mid(1, idLen));
        emit barcodeReceived(pcbId);
        return true;
    }
    else if (cmd == CMD_TEMP) {
        // 温度帧
        if (data.size() < 1) return false;
        quint8 idLen = (quint8)data[0];
        if (data.size() < 1 + idLen + 1 + 5 * 4) return false;

        QString pcbId     = QString::fromUtf8(data.mid(1, idLen));
        int offset        = 1 + idLen;
        quint8 chamber    = (quint8)data[offset];
        offset++;

        float temps[5];
        for (int i = 0; i < 5; ++i) {
            quint8 b[4];
            b[0] = (quint8)data[offset + i*4 + 0];
            b[1] = (quint8)data[offset + i*4 + 1];
            b[2] = (quint8)data[offset + i*4 + 2];
            b[3] = (quint8)data[offset + i*4 + 3];
            memcpy(&temps[i], b, 4);  // 小端float
        }

        emit temperatureReceived(pcbId,
                                 temps[0], temps[1], temps[2], temps[3], temps[4],
                                 (int)chamber);
        return true;
    }
    return false;
}
