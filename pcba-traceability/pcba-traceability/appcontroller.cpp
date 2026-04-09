#include "appcontroller.h"
#include "mainwindow.h"
#include "connectionpage.h"
#include "traceabilitypage.h"
#include "serialworker.h"
#include "traceworker.h"
#include <QThread>
#include <QMetaObject>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>

AppController::AppController(MainWindow *window, QObject *parent)
    : QObject(parent), window_(window)
{
}

AppController::~AppController()
{
    shutdownThreads();
}

void AppController::initialize()
{
    setupThreads();
    setupConnections();
    QTimer::singleShot(2000, [this]() {

        qDebug() << "🔥 manual inject test";

        BoardRecord r;
        r.barcode = "TEST123";
        r.enterTime = QDateTime::currentDateTime();
        r.status = "过炉中";
        r.timeAxis = {0, 1, 2, 3};
        r.fullTemps = {30, 50, 80, 100};

        QList<BoardRecord> list;
        list.append(r);

        window_->tracePage()->setRecords(list);});
}

void AppController::setupThreads()
{
    serialThread_ = new QThread(this);
    traceThread_ = new QThread(this);

    serialWorker_ = new SerialWorker;
    traceWorker_ = new TraceWorker;

    serialWorker_->moveToThread(serialThread_);
    traceWorker_->moveToThread(traceThread_);

    connect(serialThread_, &QThread::finished, serialWorker_, &QObject::deleteLater);
    connect(traceThread_, &QThread::finished, traceWorker_, &QObject::deleteLater);

    serialThread_->start();
    traceThread_->start();
}


void AppController::setupConnections()
{
    ConnectionPage *connPage = window_->connectionPage();
    TraceabilityPage *tracePage = window_->tracePage();

    connect(connPage, &ConnectionPage::requestScanPorts, this, [this]() {
        QMetaObject::invokeMethod(serialWorker_, "scanPorts", Qt::QueuedConnection);
        window_->setStatusMessage("正在扫描串口设备...");
    });

    connect(connPage, &ConnectionPage::requestConnectPort, this, [this](const QString &portName) {
        QMetaObject::invokeMethod(serialWorker_, "connectPort", Qt::QueuedConnection,
                                  Q_ARG(QString, portName),
                                  Q_ARG(qint32, 115200));
        window_->setStatusMessage("正在连接串口...");
    });

    connect(tracePage, &TraceabilityPage::requestDisconnect, this, [this]() {
        QMetaObject::invokeMethod(serialWorker_, "disconnectPort", Qt::QueuedConnection);
    });

    connect(tracePage, &TraceabilityPage::requestClearFinished, this, [this]() {
        QMetaObject::invokeMethod(traceWorker_, "clearFinished", Qt::QueuedConnection);
        window_->setStatusMessage("已清空完成记录");
    });

    connect(tracePage, &TraceabilityPage::requestExportCurrent, this, [this](const QString &barcode) {
        exportBoardByBarcode(barcode);
    });

    connect(serialWorker_, &SerialWorker::scanFinished, window_, [this](const QList<PortItem> &ports) {
        window_->connectionPage()->setPortList(ports);
        window_->setStatusMessage(ports.isEmpty()
                                      ? QString("未发现串口设备")
                                      : QString("扫描完成，共发现 %1 个串口设备").arg(ports.size()));
    });

    connect(serialWorker_, &SerialWorker::connectSuccess, window_, [this](const QString &portName) {
        window_->setStatusMessage("串口已连接");
        window_->setConnectionStatus(QString("已连接 %1").arg(portName), true);
        window_->switchToTracePage();
        QMetaObject::invokeMethod(traceWorker_, "requestSnapshot", Qt::QueuedConnection);
    });

    connect(serialWorker_, &SerialWorker::connectFailed, window_, [this](const QString &msg) {
        window_->connectionPage()->showConnectError(msg);
        window_->setStatusMessage("连接失败");
    });


    connect(serialWorker_, &SerialWorker::batteryReceived, window_, [this](int value) {
        window_->setBatteryLevel(value);
        window_->tracePage()->setBatteryText(QString("电量：%1%").arg(value));
    });

    connect(serialWorker_, &SerialWorker::notifyError, window_, [this](const QString &msg) {
        QMessageBox::warning(window_, "串口错误", msg);
    });

    connect(serialWorker_, &SerialWorker::disconnected, window_, [this]() {
        window_->setStatusMessage("串口已断开");
        window_->setConnectionStatus("未连接", false);
        window_->setBatteryLevel(-1);
        window_->tracePage()->setBatteryText("电量：--");
        window_->tracePage()->clearUi();
        window_->connectionPage()->clearSelection();
        latestRecords_.clear();
        window_->switchToConnectionPage();
    });

    connect(serialWorker_, &SerialWorker::synchronizedFrameReceived,
            traceWorker_, &TraceWorker::onSynchronizedFrame,
            Qt::QueuedConnection);

    connect(traceWorker_,
            &TraceWorker::recordsSnapshotReady,
            window_->tracePage(),
            &TraceabilityPage::updateRecords,
            Qt::QueuedConnection);
       qDebug() << "connect result =ok" ;
    connect(window_, &MainWindow::windowClosing, this, [this]() {
        shutdownThreads();
    });
}

void AppController::shutdownThreads()
{
    if (serialThread_) {
        QMetaObject::invokeMethod(serialWorker_, "disconnectPort", Qt::QueuedConnection);
        serialThread_->quit();
        serialThread_->wait();
        serialThread_ = nullptr;
        serialWorker_ = nullptr;
    }

    if (traceThread_) {
        traceThread_->quit();
        traceThread_->wait();
        traceThread_ = nullptr;
        traceWorker_ = nullptr;
    }
}

void AppController::exportBoardByBarcode(const QString &barcode)
{
    if (barcode.isEmpty()) {
        QMessageBox::information(window_, "导出", "请先选择一块 PCBA 记录。");
        return;
    }

    BoardRecord record;
    bool found = false;
    for (const BoardRecord &r : latestRecords_) {
        if (r.barcode == barcode) {
            record = r;
            found = true;
            break;
        }
    }

    if (!found) {
        QMessageBox::warning(window_, "导出", "未找到当前板记录。");
        return;
    }

    const QString path = QFileDialog::getSaveFileName(
        window_,
        "导出当前板CSV",
        QString("%1.csv").arg(record.barcode),
        "CSV 文件 (*.csv)"
    );
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(window_, "导出失败", "无法写入文件。");
        return;
    }

    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#endif

    out << "Barcode," << record.barcode << "\n";
    out << "EnterTime," << (record.enterTime.isValid() ? record.enterTime.toString(Qt::ISODate) : "") << "\n";
    out << "ExitTime," << (record.exitTime.isValid() ? record.exitTime.toString(Qt::ISODate) : "") << "\n";
    out << "Status," << record.status << "\n\n";

    out << "ElapsedSec,Temp,Zone\n";
    const int n = qMin(record.timeAxis.size(), qMin(record.fullTemps.size(), record.zoneAxis.size()));
    for (int i = 0; i < n; ++i) {
        out << QString::number(record.timeAxis[i], 'f', 3) << ","
            << QString::number(record.fullTemps[i], 'f', 2) << ","
            << record.zoneAxis[i] << "\n";
    }

    file.close();
    QMessageBox::information(window_, "导出成功", QString("已导出到：\n%1").arg(path));
}
