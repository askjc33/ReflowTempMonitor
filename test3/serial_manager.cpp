#include "serial_manager.h"

SerialManager::SerialManager(QObject *parent)
    : QObject(parent)
{
    connect(&serial_, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(&serial_, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
            this, SLOT(onErrorOccurred(QSerialPort::SerialPortError)));
}

void SerialManager::scanPorts()
{
    QList<PortItem> ports;
    const QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();//枚举所有可用串口
    for (int i = 0; i < infos.size(); ++i) {
        const QSerialPortInfo &info = infos.at(i);
        PortItem p;
        p.portName = info.portName();
        p.description = info.description();
        p.manufacturer = info.manufacturer();
        p.systemLocation = info.systemLocation();
        ports.append(p);
    }
    emit scanFinished(ports);
}

bool SerialManager::connectPort(const QString &portName, qint32 baudRate)
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
        emit connectFailed(QString::fromUtf8("打开串口失败：%1").arg(serial_.errorString()));
        return false;
    }

    rxBuffer_.clear();
    tempFrames_.clear();
    posFrames_.clear();
    emit connectSuccess(portName);
    return true;
}

void SerialManager::disconnectPort()
{
    if (serial_.isOpen()) {
        serial_.close();
    }
    rxBuffer_.clear();
    tempFrames_.clear();
    posFrames_.clear();
    emit disconnected();
}

bool SerialManager::isConnected() const
{
    return serial_.isOpen();
}

void SerialManager::onReadyRead()
{
    rxBuffer_.append(serial_.readAll());

    if (rxBuffer_.size() > 1024 * 1024) {
        rxBuffer_.clear();
        emit notifyError(QString::fromUtf8("串口接收缓冲区异常过大，已自动清空。"));
        return;
    }

    while (true) {
        int idx = rxBuffer_.indexOf('\n');   //保证“一次刚好一整行”数据
        if (idx < 0) {
            break;
        }

        QByteArray line = rxBuffer_.left(idx);
        rxBuffer_.remove(0, idx + 1);

        QString text = QString::fromUtf8(line).trimmed();
        if (!text.isEmpty()) {
            parseLine(text);
        }
    }
}

void SerialManager::onErrorOccurred(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) {
        return;
    }

    if (error == QSerialPort::ResourceError || error == QSerialPort::DeviceNotFoundError) {
        QString msg = serial_.errorString();
        if (serial_.isOpen()) {
            serial_.close();
        }
        emit notifyError(msg);
        emit disconnected();
    }
}

void SerialManager::parseLine(const QString &line)  //协议解析，按前缀分流
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

void SerialManager::parseBattery(const QString &line)
{
    QStringList parts = line.split(',', Qt::KeepEmptyParts);
    if (parts.size() < 2) {
        return;
    }

    bool ok = false;
    int v = parts.at(1).trimmed().toInt(&ok);
    if (ok) {
        emit batteryReceived(v);
    }
}

void SerialManager::parseTempFrame(const QString &line)
{
    QStringList parts = line.split(',', Qt::KeepEmptyParts);
    if (parts.size() != 2 + ZONE_COUNT) {
        return;
    }

    bool seqOk = false;
    int seq = parts.at(1).trimmed().toInt(&seqOk);
    if (!seqOk) {
        return;
    }

    QVector<double> temps;
    temps.reserve(ZONE_COUNT);

    for (int i = 0; i < ZONE_COUNT; ++i) {
        bool ok = false;
        double v = parts.at(2 + i).trimmed().toDouble(&ok);
        if (!ok) {
            return;
        }
        temps.append(v);
    }

    tempFrames_[seq] = temps;
    trySyncFrame(seq);
    cleanupOldFrames(seq);
}

void SerialManager::parsePosFrame(const QString &line)
{
    QStringList parts = line.split(',', Qt::KeepEmptyParts);
    if (parts.size() != 2 + ZONE_COUNT) {
        return;
    }

    bool seqOk = false;
    int seq = parts.at(1).trimmed().toInt(&seqOk);
    if (!seqOk) {
        return;
    }

    QStringList pos;
    for (int i = 0; i < ZONE_COUNT; ++i) {
        pos.append(parts.at(2 + i).trimmed());
    }

    posFrames_[seq] = pos;
    trySyncFrame(seq);
    cleanupOldFrames(seq);
}

void SerialManager::trySyncFrame(int seq)
{
    if (!tempFrames_.contains(seq) || !posFrames_.contains(seq)) {
        return;
    }

    emit synchronizedFrameReceived(tempFrames_.take(seq), posFrames_.take(seq));
}

void SerialManager::cleanupOldFrames(int latestSeq)  //清理哈希表，只保留最近50个seq，超过200条就整体清空兜底
{
    QList<int> tempKeys = tempFrames_.keys();
    for (int i = 0; i < tempKeys.size(); ++i) {
        int k = tempKeys.at(i);
        if (k < latestSeq - 50) {
            tempFrames_.remove(k);
        }
    }

    QList<int> posKeys = posFrames_.keys();
    for (int i = 0; i < posKeys.size(); ++i) {
        int k = posKeys.at(i);
        if (k < latestSeq - 50) {
            posFrames_.remove(k);
        }
    }

    if (tempFrames_.size() > 200) {
        tempFrames_.clear();
    }

    if (posFrames_.size() > 200) {
        posFrames_.clear();
    }
}
