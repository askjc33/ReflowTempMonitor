#include "serialworker.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QThread>

SerialWorker::SerialWorker(QObject *parent) : QObject(parent)
{
    serial_ = new QSerialPort(this);
    connect(serial_, &QSerialPort::readyRead, this, &SerialWorker::onReadyRead);
    connect(serial_, &QSerialPort::errorOccurred, this, &SerialWorker::onErrorOccurred);
}

SerialWorker::~SerialWorker()
{
    disconnectPort();
}

void SerialWorker::scanPorts()
{
    qDebug() << "scanPorts called in thread:" << QThread::currentThread();
    QList<PortItem> ports;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        PortItem p;
        p.portName = info.portName();
        p.description = info.description();
        p.manufacturer = info.manufacturer();
        p.systemLocation = info.systemLocation();
        ports.append(p);
    }
    emit scanFinished(ports);
}

void SerialWorker::connectPort(const QString &portName, qint32 baudRate)
{
    if (serial_->isOpen()) {
        serial_->close();
    }

    serial_->setPortName(portName);
       serial_->setBaudRate(baudRate);
       serial_->setDataBits(QSerialPort::Data8);
       serial_->setParity(QSerialPort::NoParity);
       serial_->setStopBits(QSerialPort::OneStop);
       serial_->setFlowControl(QSerialPort::NoFlowControl);

       if (!serial_->open(QIODevice::ReadWrite)) {
           emit connectFailed(QString("打开串口失败：%1").arg(serial_->errorString()));
           return;
       }

       rxBuffer_.clear();
       tempFrames_.clear();
       posFrames_.clear();
       emit connectSuccess(portName);
   }

   void SerialWorker::disconnectPort()
   {
       if (serial_ && serial_->isOpen()) {
           serial_->close();
       }
       rxBuffer_.clear();
       tempFrames_.clear();
       posFrames_.clear();
       emit disconnected();
   }

   void SerialWorker::onReadyRead()
   {
       rxBuffer_.append(serial_->readAll());

       if (rxBuffer_.size() > 1024 * 1024) {
           rxBuffer_.clear();
           emit notifyError("串口接收缓冲区异常过大，已自动清空。");
           return;
       }

       while (true) {
           const int idx = rxBuffer_.indexOf('\n');
           if (idx < 0) break;

           QByteArray line = rxBuffer_.left(idx);
           rxBuffer_.remove(0, idx + 1);

           QString text = QString::fromUtf8(line).trimmed();
           if (!text.isEmpty()) {
               parseLine(text);
           }
       }
   }

   void SerialWorker::onErrorOccurred(QSerialPort::SerialPortError error)
   {
       if (error == QSerialPort::NoError) return;

       if (error == QSerialPort::ResourceError || error == QSerialPort::DeviceNotFoundError) {
           QString msg = serial_->errorString();
           if (serial_->isOpen()) {
               serial_->close();
           }
           emit notifyError(msg);
           emit disconnected();
       }
   }

   void SerialWorker::parseLine(const QString &line)
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

   void SerialWorker::parseBattery(const QString &line)
   {
       const QStringList parts = line.split(',', Qt::KeepEmptyParts);
       if (parts.size() < 2) return;

       bool ok = false;
       int v = parts[1].trimmed().toInt(&ok);
       if (ok) emit batteryReceived(v);
   }

   void SerialWorker::parseTempFrame(const QString &line)
   {
       const QStringList parts = line.split(',', Qt::KeepEmptyParts);
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

   void SerialWorker::parsePosFrame(const QString &line)
   {
       qDebug() << "raw POS line =" << line;

       const QStringList parts = line.split(',', Qt::KeepEmptyParts);
       if (parts.size() != 2 + ZONE_COUNT) return;

       bool seqOk = false;
       int seq = parts[1].trimmed().toInt(&seqOk);
       if (!seqOk) return;

       QStringList pos;
       pos.reserve(ZONE_COUNT);
       for (int i = 0; i < ZONE_COUNT; ++i) {
           pos.append(parts[2 + i].trimmed());
       }

     qDebug() << "parsed POS seq =" << seq << "pos =" << pos;

       posFrames_[seq] = pos;
       trySyncFrame(seq);
       cleanupOldFrames(seq);
   }
   void SerialWorker::trySyncFrame(int seq)
   {
       if (!tempFrames_.contains(seq) || !posFrames_.contains(seq)) return;

       SyncFrame frame;
       frame.seq = seq;
       frame.temps = tempFrames_.take(seq);
       frame.positions = posFrames_.take(seq);
       frame.timestamp = QDateTime::currentDateTime();

       qDebug() << "emit synchronizedFrameReceived seq =" << seq
                   << "temps =" << frame.temps.size()
                   << "positions =" << frame.positions.size();

       emit synchronizedFrameReceived(frame);
   }

   void SerialWorker::cleanupOldFrames(int latestSeq)
   {
       const QList<int> tempKeys = tempFrames_.keys();
       for (int k : tempKeys) {
           if (k < latestSeq - 50) {
               tempFrames_.remove(k);
           }
       }

       const QList<int> posKeys = posFrames_.keys();
       for (int k : posKeys) {
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
