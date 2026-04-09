#include "serialmanager.h"
#include <QRegularExpression>

SerialManager::SerialManager(QObject *parent)
    : QObject(parent),
      m_serial(new QSerialPort(this)),
      m_packet(5, qQNaN())
{
    connect(m_serial, &QSerialPort::readyRead, this, &SerialManager::onReadyRead);
    connect(m_serial, &QSerialPort::errorOccurred, this, &SerialManager::onSerialError);
}

SerialManager::~SerialManager()
{
    disconnectPort();
}

QStringList SerialManager::availablePorts() const
{
    QStringList list;
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        QString desc = info.description().isEmpty() ? "Unknown" : info.description();
        list << QString("%1 (%2)").arg(info.portName(), desc);
    }
    return list;
}

bool SerialManager::connectPort(const QString &portName, qint32 baudRate)
{
    if (m_serial->isOpen()) {
        m_serial->close();
    }

    m_serial->setPortName(portName);
    m_serial->setBaudRate(baudRate);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!m_serial->open(QIODevice::ReadWrite)) {
        emit connectionFailed(m_serial->errorString());
        return false;
    }

    m_buffer.clear();
    m_packet.fill(qQNaN());
    emit connected();
    return true;
}

void SerialManager::disconnectPort()
{
    if (m_serial->isOpen()) {
        m_serial->close();
        emit disconnected();
    }
}

void SerialManager::onReadyRead()
{
    const QByteArray data = m_serial->readAll();
    if (data.isEmpty()) return;

    emit rawDataReceived(data);
    m_buffer.append(data);
    parseBuffer();
}

void SerialManager::parseBuffer()
{
    static QRegularExpression tempRe("T([1-5]):(-?\\d+\\.\\d+),?");
    static QRegularExpression battRe("batt(?:e)?ry[:\\s]*(\\d+)", QRegularExpression::CaseInsensitiveOption);

    while (true) {
        auto battMatch = battRe.match(QString::fromUtf8(m_buffer));
        if (battMatch.hasMatch()) {
            emit batteryReceived(battMatch.captured(1).toInt());
        }

        QString text = QString::fromUtf8(m_buffer);
        auto match = tempRe.match(text);
        if (!match.hasMatch()) {
            break;
        }

        int ch = match.captured(1).toInt() - 1;
        double value = match.captured(2).toDouble();

        if (ch == 0) {
            m_packet.fill(qQNaN());
        }

        if (ch >= 0 && ch < 5) {
            m_packet[ch] = value;
        }

        m_buffer.remove(0, match.capturedEnd());
        tryEmitPacket();
    }

    if (m_buffer.size() > 4096) {
        m_buffer = m_buffer.right(1024);
    }
}

void SerialManager::tryEmitPacket()
{
    for (int i = 0; i < 5; ++i) {
        if (qIsNaN(m_packet[i])) {
            return;
        }
    }
    emit temperaturesReceived(m_packet);
}

void SerialManager::onSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        emit errorOccurred(m_serial->errorString());
        disconnectPort();
    }
}
