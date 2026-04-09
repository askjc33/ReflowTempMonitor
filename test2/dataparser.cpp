#include "dataparser.h"
#include <QMessageBox>

DataParser::DataParser(QObject *parent)
    : QObject{parent}
{
    connect(&serial_, &QSerialPort::readyRead, this, &DataParser::onReadyRead);
    connect(&serial_, &QSerialPort::errorOccurred, this, &DataParser::onErrorOccurred);
}

void DataParser::scanPorts()
{
    QList<PortItem> ports;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const auto &info : infos) {
        PortItem p;
        p.portName = info.portName();
        p.description = info.description();
        p.manufacturer = info.manufacturer();
        p.systemLocation = info.systemLocation();
        ports.append(p);
    }
    emit scanFinished(ports);
}

bool DataParser::connectPort(const QString &portName, qint32 baudRate)
{
    if (serial_.isOpen()) {
        serial_.close();
    }

    serial_.setPortName(portName);
    serial_.setBaudRate(baudRate);
    serial_.setDataBits(QSerialPort::Data8);
    serial_.setParity(QSerialPort::NoParity);
    serial_.setStopBits(QSerialPort::OneStop);
    serial_.setFlowControl(QSerialPort::NoFlowControl);

    if (!serial_.open(QIODevice::ReadWrite)) {
        emit connectFailed(QString("打开串口失败：%1").arg(serial_.errorString()));
        return false;
    }

    rxBuffer_.clear();
    tempFrames_.clear();
    posFrames_.clear();
    emit connectSuccess(portName);
    return true;
}

void DataParser::disconnectPort()
{
    if (serial_.isOpen()) {
        serial_.close();
    }
    rxBuffer_.clear();
    tempFrames_.clear();
    posFrames_.clear();
    emit disconnected();
}

bool DataParser::isConnected() const
{
    return serial_.isOpen();
}

void DataParser::onReadyRead()
{
    rxBuffer_.append(serial_.readAll());

    // 防止缓冲区无限增长
    if (rxBuffer_.size() > 1024 * 1024) {
        rxBuffer_.clear();
        emit notifyError("串口接收缓冲区异常过大，已自动清空。");
        return;
    }

    while (true) {
        int idx = rxBuffer_.indexOf('\n');
        if (idx < 0) break;

        QByteArray line = rxBuffer_.left(idx);
        rxBuffer_.remove(0, idx + 1);

        QString text = QString::fromUtf8(line).trimmed();
        if (!text.isEmpty()) {
            parseLine(text);
        }
    }
}

void DataParser::onErrorOccurred(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) return;

    if (error == QSerialPort::ResourceError || error == QSerialPort::DeviceNotFoundError) {
        QString msg = serial_.errorString();
        if (serial_.isOpen()) {
            serial_.close();
        }
        emit notifyError(msg);
        emit disconnected();
    }
}

void DataParser::parseLine(const QString &line)
{
    if (line.startsWith("BATT,", Qt::CaseInsensitive) ||
        line.startsWith("BATTERY,", Qt::CaseInsensitive)) {
        parseBattery(line);
        return;
    }

    if (line.startsWith("TEMP,", Qt::CaseInsensitive)) {
        parseTempFrame(line);
        return;
    }

    if (line.startsWith("POS,", Qt::CaseInsensitive)) {
        parsePosFrame(line);
        return;
    }
}

void DataParser::parseBattery(const QString &line)
{
    QStringList parts = line.split(',', Qt::KeepEmptyParts);
    if (parts.size() < 2) return;

    bool ok = false;
    int v = parts[1].trimmed().toInt(&ok);
    if (ok) emit batteryReceived(v);
}

void DataParser::parseTempFrame(const QString &line)
{
    QStringList parts = line.split(',', Qt::KeepEmptyParts);
    if (parts.size() != 2 + ZONE_COUNT) return;

    bool seqOk = false;
    int seq = parts[1].trimmed().toInt(&seqOk);
    if (!seqOk) return;

    QVector<double> temps;
    temps.reserve(ZONE_COUNT);

    for (int i = 0; i < ZONE_COUNT; ++i) {
        bool ok = false;
        double v = parts[2 + i].trimmed().toDouble(&ok);
        if (!ok) return;
        temps.append(v);
    }

    tempFrames_[seq] = temps;
    trySyncFrame(seq);
    cleanupOldFrames(seq);
}

void DataParser::parsePosFrame(const QString &line)
{
    QStringList parts = line.split(',', Qt::KeepEmptyParts);
    if (parts.size() != 2 + ZONE_COUNT) return;

    bool seqOk = false;
    int seq = parts[1].trimmed().toInt(&seqOk);
    if (!seqOk) return;

    QStringList pos;
    pos.reserve(ZONE_COUNT);
    for (int i = 0; i < ZONE_COUNT; ++i) {
        pos.append(parts[2 + i].trimmed());
    }

    posFrames_[seq] = pos;
    trySyncFrame(seq);
    cleanupOldFrames(seq);
}

void DataParser::trySyncFrame(int seq)
{
    if (!tempFrames_.contains(seq) || !posFrames_.contains(seq)) return;
    emit synchronizedFrameReceived(tempFrames_.take(seq), posFrames_.take(seq));
}

void DataParser::cleanupOldFrames(int latestSeq)
{
    // 清理过旧的缓存帧
    QList<int> tempKeys = tempFrames_.keys();
    for (int k : tempKeys) {
        if (k < latestSeq - 50) {
            tempFrames_.remove(k);
        }
    }

    QList<int> posKeys = posFrames_.keys();
    for (int k : posKeys) {
        if (k < latestSeq - 50) {
            posFrames_.remove(k);
        }
    }

    // 双保险：极端情况下强制收缩
    if (tempFrames_.size() > 200) {
        tempFrames_.clear();
    }
    if (posFrames_.size() > 200) {
        posFrames_.clear();
    }
}
