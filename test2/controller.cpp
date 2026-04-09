#include "controller.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

Controller::Controller(QObject *parent)
    : QObject{parent}
{
    mainWindow_ = new MainWindow;
    mainWindow_->show();

    // ====================== 串口 → 界面/模型 信号连接 ======================
    // 串口扫描结果 → 连接页面
    connect(&serialParser_, &DataParser::scanFinished,
            mainWindow_->connectionPage(), &ConnectionPage::onScanFinished);
    // 串口连接成功 → 切换页面、更新状态
    connect(&serialParser_, &DataParser::connectSuccess,
            this, &Controller::onSerialConnectSuccess);
    // 串口连接失败 → 提示
    connect(&serialParser_, &DataParser::connectFailed,
            this, &Controller::onSerialConnectFailed);
    // 串口断开 → 重置界面
    connect(&serialParser_, &DataParser::disconnected,
            this, &Controller::onSerialDisconnected);
    // 串口错误 → 提示
    connect(&serialParser_, &DataParser::notifyError,
            this, &Controller::onSerialError);
    // 电量数据 → 界面
    connect(&serialParser_, &DataParser::batteryReceived,
            this, &Controller::onBatteryReceived);
    // 同步帧数据 → 模型
    connect(&serialParser_, &DataParser::synchronizedFrameReceived,
            &dataModel_, &Model::onSynchronizedFrame);

    // ====================== 界面 → 串口/模型 信号连接 ======================
    // 连接页面扫描请求 → 串口
    connect(mainWindow_->connectionPage(), &ConnectionPage::scanRequested,
            &serialParser_, &DataParser::scanPorts);
    // 连接页面连接请求 → 串口
    connect(mainWindow_->connectionPage(), &ConnectionPage::connectRequested,
            &serialParser_, &DataParser::connectPort);
    // 追溯页面断开请求 → 串口
    connect(mainWindow_->tracePage(), &TraceabilityPage::disconnectRequested,
            &serialParser_, &DataParser::disconnectPort);
    // 追溯页面清空请求 → 模型
    connect(mainWindow_->tracePage(), &TraceabilityPage::clearFinishedRequested,
            &dataModel_, &Model::clearFinishedRecords);
    // 追溯页面导出请求 → 控制器
    connect(mainWindow_->tracePage(), &TraceabilityPage::exportRequested,
            this, &Controller::onExportRequested);
    // 追溯页面板选中 → 控制器
    connect(mainWindow_->tracePage(), &TraceabilityPage::boardSelected,
            this, &Controller::onBoardSelected);

    // ====================== 模型 → 界面 信号连接 ======================
    connect(&dataModel_, &Model::recordsUpdated,
            this, &Controller::onRecordsUpdated);
}

void Controller::onSerialConnectSuccess(const QString &portName)
{
    mainWindow_->updateStatusMessage("串口已连接");
    mainWindow_->updateConnectionStatus(QString("已连接 %1").arg(portName), true);
    mainWindow_->switchToTracePage();
}

void Controller::onSerialConnectFailed(const QString &msg)
{
    QMessageBox::warning(mainWindow_, "连接失败", msg);
    mainWindow_->updateStatusMessage("连接失败");
}

void Controller::onSerialDisconnected()
{
    mainWindow_->updateStatusMessage("串口已断开");
    mainWindow_->updateConnectionStatus("未连接", false);
    mainWindow_->updateBatteryLevel(-1);
    mainWindow_->tracePage()->clearBoardDisplay();
    mainWindow_->tracePage()->refreshRecordList({});
    mainWindow_->switchToConnectionPage();
}

void Controller::onSerialError(const QString &msg)
{
    QMessageBox::warning(mainWindow_, "串口错误", msg);
}

void Controller::onBatteryReceived(int value)
{
    mainWindow_->tracePage()->updateBattery(value);
    mainWindow_->updateBatteryLevel(value);
}

void Controller::onRecordsUpdated()
{
    mainWindow_->tracePage()->refreshRecordList(dataModel_.getAllRecords());
}

void Controller::onBoardSelected(const QString &barcode)
{
    BoardRecord r;
    if (dataModel_.getRecordByBarcode(barcode, r)) {
        mainWindow_->tracePage()->showBoardRecord(r);
        mainWindow_->updateStatusMessage(QString("已显示板 %1 的温度曲线").arg(barcode));
    }
}

void Controller::onExportRequested()
{
    QString selectedBarcode = mainWindow_->tracePage()->selectedBarcode();
    if (selectedBarcode.isEmpty()) {
        QMessageBox::information(mainWindow_, "导出", "请先选择一块PCBA记录。");
        return;
    }

    BoardRecord r;
    if (!dataModel_.getRecordByBarcode(selectedBarcode, r)) {
        QMessageBox::warning(mainWindow_, "导出", "未找到当前板记录。");
        return;
    }

    QString path = QFileDialog::getSaveFileName(
        mainWindow_,
        "导出当前板CSV",
        QString("%1.csv").arg(r.barcode),
        "CSV 文件 (*.csv)"
    );
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(mainWindow_, "导出失败", "无法写入文件。");
        return;
    }

    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#endif
    out << "Barcode," << r.barcode << "\n";
    out << "EnterTime," << (r.enterTime.isValid() ? r.enterTime.toString(Qt::ISODate) : "") << "\n";
    out << "ExitTime," << (r.exitTime.isValid() ? r.exitTime.toString(Qt::ISODate) : "") << "\n";
    out << "Status," << r.status << "\n\n";

    out << "ElapsedSec,Temp,Zone\n";
    const int n = std::min({r.timeAxis.size(), r.fullTemps.size(), r.zoneAxis.size()});
    for (int i = 0; i < n; ++i) {
        out << QString::number(r.timeAxis[i], 'f', 3) << ","
            << QString::number(r.fullTemps[i], 'f', 2) << ","
            << r.zoneAxis[i] << "\n";
    }

    file.close();
    QMessageBox::information(mainWindow_, "导出成功", QString("已导出到：\n%1").arg(path));
}

void Controller::onClearFinishedRequested()
{
    dataModel_.clearFinishedRecords();
    mainWindow_->updateStatusMessage("已清空完成记录");
}

void Controller::onDisconnectRequested()
{
    serialParser_.disconnectPort();
}
