#include "traceability_page.h"
#include "serial_manager.h"
#include "board_trace_manager.h"
#include "reflow_settings.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFont>
#include <QFrame>
#include <QGroupBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QSignalBlocker>
#include <QPainter>
#include <algorithm>
#include <QSet>
#include <QColor>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QPen>
#include <QBrush>

namespace {

static const double kAxisXMaxSec = 150.0;

static void writeBoardCsv(QTextStream &out, const BoardRecord &r, const QString &alarmText)
{
    out << "Barcode," << r.barcode << "\n";
    out << "EnterTime," << (r.enterTime.isValid() ? r.enterTime.toString(Qt::ISODate) : "") << "\n";
    out << "ExitTime," << (r.exitTime.isValid() ? r.exitTime.toString(Qt::ISODate) : "") << "\n";
    out << "Status," << r.status << "\n";
    out << "Alarm," << alarmText << "\n\n";
    out << "ElapsedSec,Temp,Zone\n";

    int n = qMin(r.timeAxis.size(), qMin(r.fullTemps.size(), r.zoneAxis.size()));
    for (int i = 0; i < n; ++i) {
        out << QString::number(r.timeAxis.at(i), 'f', 3) << ","
            << QString::number(r.fullTemps.at(i), 'f', 2) << ","
            << r.zoneAxis.at(i) << "\n";
    }
}

} // namespace

TraceabilityPage::TraceabilityPage(SerialManager *serial, BoardTraceManager *manager, ReflowSettings *settings, QWidget *parent)
    : QWidget(parent),
      serial_(serial),
      manager_(manager),
      settings_(settings),
      batteryLabel_(nullptr),
      barcodeValue_(nullptr),
      statusValue_(nullptr),
      enterValue_(nullptr),
      exitValue_(nullptr),
      durationValue_(nullptr),
      zoneValue_(nullptr),
      alarmValue_(nullptr),
      chartView_(nullptr),
      chart_(nullptr),
      axisX_(nullptr),
      axisY_(nullptr),
      table_(nullptr)
{
    connect(manager_, SIGNAL(recordsUpdated()), this, SLOT(refreshView()));
    connect(serial_, SIGNAL(batteryReceived(int)), this, SLOT(onBatteryReceived(int)));
    connect(serial_, SIGNAL(notifyError(QString)), this, SLOT(onSerialError(QString)));
    connect(serial_, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

    // ===== 新增：温区设置变化时刷新曲线参考线 =====
    connect(settings_, SIGNAL(settingsChanged()), this, SLOT(refreshChartReferences()));

    initUi();
    initChart();
}

void TraceabilityPage::refreshView()
{
    QList<BoardRecord> records = manager_->allRecords();

    table_->setUpdatesEnabled(false);
    {
        QSignalBlocker blocker(table_);
        table_->clearContents();
        table_->setRowCount(records.size());

        static const QString kNormalAlarm = QString::fromUtf8("正常");

        for (int row = 0; row < records.size(); ++row) {
            const BoardRecord &r = records.at(row);

            QTableWidgetItem *itemBarcode = new QTableWidgetItem(r.barcode);
            itemBarcode->setData(Qt::UserRole, r.barcode);
            table_->setItem(row, 0, itemBarcode);

            table_->setItem(row, 1, new QTableWidgetItem(
                r.enterTime.isValid() ? r.enterTime.toString("yyyy-MM-dd HH:mm:ss") : "-"));
            table_->setItem(row, 2, new QTableWidgetItem(
                r.exitTime.isValid() ? r.exitTime.toString("yyyy-MM-dd HH:mm:ss") : "-"));
            table_->setItem(row, 3, new QTableWidgetItem(r.status));

            double duration = r.timeAxis.isEmpty() ? 0.0 : r.timeAxis.last();
            table_->setItem(row, 4, new QTableWidgetItem(QString::number(duration, 'f', 1)));

            table_->setItem(row, 5, new QTableWidgetItem(
                r.lastZone > 0 ? QString::number(r.lastZone) : "-"));

            const QString alarmText = buildAlarmText(r);
            QTableWidgetItem *alarmItem = new QTableWidgetItem(alarmText);
            if (alarmText != kNormalAlarm) {
                alarmItem->setForeground(QBrush(QColor("#c62828")));
            }
            table_->setItem(row, 6, alarmItem);

            if (selectedBarcodes_.contains(r.barcode)) {
                table_->selectRow(row);
            }
        }
    }
    table_->setUpdatesEnabled(true);
    table_->viewport()->update();

    for (const QString &code : selectedBarcodes_) {
        showBoard(code);
    }

    updateAxisRange();
    refreshSelectedBoardInfo();
}

void TraceabilityPage::onTableCellClicked(int row, int)
{
    QTableWidgetItem *item = table_->item(row, 0);
    if (!item) return;

    QString barcode = item->data(Qt::UserRole).toString();

    if (selectedBarcodes_.contains(barcode)) {
        selectedBarcodes_.remove(barcode);
        if (multiSeries_.contains(barcode)) {
            QtCharts::QLineSeries *series = multiSeries_.take(barcode);
            chart_->removeSeries(series);
            delete series;
        }
        emit requestSetStatusMessage(QString::fromUtf8("已隐藏：%1").arg(barcode));
    } else {
        selectedBarcodes_.insert(barcode);
        showBoard(barcode);
        emit requestSetStatusMessage(QString::fromUtf8("已显示：%1").arg(barcode));
    }

    updateAxisRange();
    refreshSelectedBoardInfo();
}

void TraceabilityPage::onBatteryReceived(int value)
{
    batteryLabel_->setText(QString::fromUtf8("电量：%1%").arg(value));
    emit requestSetBatteryLevel(value);
}

void TraceabilityPage::disconnectDevice()
{
    serial_->disconnectPort();
}

void TraceabilityPage::onSerialError(const QString &msg)
{
    QMessageBox::warning(this, QString::fromUtf8("串口错误"), msg);
}

void TraceabilityPage::onDisconnected()
{
    emit requestSetStatusMessage(QString::fromUtf8("串口已断开"));
    emit requestSetConnectionStatus(QString::fromUtf8("未连接"), false);
    emit requestSetBatteryLevel(-1);

    selectedBarcodes_.clear();
    clearBoardDisplay();
    table_->clearContents();
    table_->setRowCount(0);

    emit requestDisconnectAndBack();
}

void TraceabilityPage::clearFinishedRecords()
{
    manager_->clearFinished();
    emit requestSetStatusMessage(QString::fromUtf8("已清空完成记录"));
}

void TraceabilityPage::exportCurrentBoard()
{
    if (selectedBarcodes_.isEmpty()) {
        QMessageBox::information(this, QString::fromUtf8("导出"), QString::fromUtf8("请先选择至少一块PCBA记录。"));
        return;
    }

    QString path = QFileDialog::getSaveFileName(
        this,
        QString::fromUtf8("导出CSV"),
        QString::fromUtf8("selected_boards.csv"),
        QString::fromUtf8("CSV 文件 (*.csv)")
    );
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, QString::fromUtf8("导出失败"), QString::fromUtf8("无法写入文件"));
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    bool exportedAny = false;
    for (const QString &barcode : selectedBarcodes_) {
        BoardRecord r;
        if (!manager_->getRecordByBarcode(barcode, r)) {
            continue;
        }

        if (exportedAny) {
            out << "\n";
        }

        writeBoardCsv(out, r, buildAlarmText(r));

        exportedAny = true;
    }

    file.close();

    if (!exportedAny) {
        QMessageBox::warning(this, QString::fromUtf8("导出失败"), QString::fromUtf8("未找到可导出的记录。"));
        return;
    }

    QMessageBox::information(this, QString::fromUtf8("导出成功"), path);
}

void TraceabilityPage::exportFinishedBoardsCsv()
{
    QList<BoardRecord> records = manager_->allRecords();
    QList<BoardRecord> finished;
    static const QString kDone = QString::fromUtf8("已完成");
    for (int i = 0; i < records.size(); ++i) {
        const BoardRecord &r = records.at(i);
        if (r.finished || r.status == kDone) {
            finished.append(r);
        }
    }

    if (finished.isEmpty()) {
        QMessageBox::information(
            this,
            QString::fromUtf8("导出"),
            QString::fromUtf8("当前没有状态为「已完成」的 PCBA 记录。"));
        return;
    }

    QString path = QFileDialog::getSaveFileName(
        this,
        QString::fromUtf8("导出已完成板CSV"),
        QString::fromUtf8("finished_boards.csv"),
        QString::fromUtf8("CSV 文件 (*.csv)")
    );
    if (path.isEmpty()) {
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, QString::fromUtf8("导出失败"), QString::fromUtf8("无法写入文件"));
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    for (int i = 0; i < finished.size(); ++i) {
        if (i > 0) {
            out << "\n";
        }
        const BoardRecord &r = finished.at(i);
        writeBoardCsv(out, r, buildAlarmText(r));
    }

    file.close();
    QMessageBox::information(
        this,
        QString::fromUtf8("导出成功"),
        QString::fromUtf8("已导出 %1 条已完成记录。\n%2").arg(finished.size()).arg(path));
}

void TraceabilityPage::refreshChartReferences()
{
    // ===== 新增：重建温区竖线、阈值横线、横坐标范围 =====
    rebuildReferenceLines();
    updateAxisRange();

    for (const QString &code : selectedBarcodes_) {
        showBoard(code);
    }
}

void TraceabilityPage::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *top = new QHBoxLayout;
    QLabel *title = new QLabel(QString::fromUtf8("PCBA 过炉追溯（多板对比）"));
    QFont titleFont;
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    title->setFont(titleFont);
    top->addWidget(title);
    top->addStretch();

    batteryLabel_ = new QLabel(QString::fromUtf8("电量：--"));
    top->addWidget(batteryLabel_);

    QPushButton *exportBtn = new QPushButton(QString::fromUtf8("导出当前板CSV"));
    connect(exportBtn, SIGNAL(clicked()), this, SLOT(exportCurrentBoard()));
    top->addWidget(exportBtn);

    QPushButton *exportFinishedBtn = new QPushButton(QString::fromUtf8("导出已完成板CSV"));
    connect(exportFinishedBtn, SIGNAL(clicked()), this, SLOT(exportFinishedBoardsCsv()));
    top->addWidget(exportFinishedBtn);

    QPushButton *clearBtn = new QPushButton(QString::fromUtf8("清空完成记录"));
    connect(clearBtn, SIGNAL(clicked()), this, SLOT(clearFinishedRecords()));
    top->addWidget(clearBtn);

    QPushButton *disconnectBtn = new QPushButton(QString::fromUtf8("断开连接"));
    connect(disconnectBtn, SIGNAL(clicked()), this, SLOT(disconnectDevice()));
    top->addWidget(disconnectBtn);

    layout->addLayout(top);

    QFrame *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    layout->addWidget(sep);

    QGroupBox *infoGroup = new QGroupBox(QString::fromUtf8("当前选中板信息"));
    QGridLayout *infoLayout = new QGridLayout(infoGroup);

    barcodeValue_ = new QLabel("--");
    statusValue_ = new QLabel("--");
    enterValue_ = new QLabel("--");
    exitValue_ = new QLabel("--");
    durationValue_ = new QLabel("--");
    zoneValue_ = new QLabel("--");
    peakTempValue_ = new QLabel("--");
    maxSlopeValue_ = new QLabel("--");

    infoLayout->addWidget(new QLabel(QString::fromUtf8("条码：")), 0, 0);
    infoLayout->addWidget(barcodeValue_, 0, 1);
    infoLayout->addWidget(new QLabel(QString::fromUtf8("状态：")), 0, 2);
    infoLayout->addWidget(statusValue_, 0, 3);
    infoLayout->addWidget(new QLabel(QString::fromUtf8("当前/最后气室：")), 0, 4);
    infoLayout->addWidget(zoneValue_, 0, 5);

    infoLayout->addWidget(new QLabel(QString::fromUtf8("进板时间：")), 1, 0);
    infoLayout->addWidget(enterValue_, 1, 1);
    infoLayout->addWidget(new QLabel(QString::fromUtf8("出板时间：")), 1, 2);
    infoLayout->addWidget(exitValue_, 1, 3);
    infoLayout->addWidget(new QLabel(QString::fromUtf8("过炉时长：")), 1, 4);
    infoLayout->addWidget(durationValue_, 1, 5);

    alarmValue_ = new QLabel("--");
    infoLayout->addWidget(new QLabel(QString::fromUtf8("报警信息：")), 2, 0);
    infoLayout->addWidget(alarmValue_, 2, 1, 1, 5);

    infoLayout->addWidget(new QLabel(QString::fromUtf8("峰值温度：")), 3, 0);
    infoLayout->addWidget(peakTempValue_, 3, 1);
    infoLayout->addWidget(new QLabel(QString::fromUtf8("最大斜率：")), 3, 2);
    infoLayout->addWidget(maxSlopeValue_, 3, 3);

    layout->addWidget(infoGroup);

    QGroupBox *chartGroup = new QGroupBox(QString::fromUtf8("温度曲线对比"));
    QVBoxLayout *chartLayout = new QVBoxLayout(chartGroup);
    chartView_ = new QtCharts::QChartView;
    chartView_->setRenderHint(QPainter::Antialiasing);
    chartView_->setMinimumHeight(420);
    chartLayout->addWidget(chartView_);
    layout->addWidget(chartGroup, 1);

    QGroupBox *tableGroup = new QGroupBox(QString::fromUtf8("过炉记录列表"));
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
    table_ = new QTableWidget;
    table_->setColumnCount(7);
    table_->setHorizontalHeaderLabels(
        QStringList() << QString::fromUtf8("条码")
        << QString::fromUtf8("进板时间")
        << QString::fromUtf8("出板时间")
        << QString::fromUtf8("状态")
        << QString::fromUtf8("时长(s)")
        << QString::fromUtf8("最后气室")
        << QString::fromUtf8("报警信息"));

    table_->horizontalHeader()->setStretchLastSection(true);
    table_->setColumnWidth(0, 140);
    table_->setColumnWidth(1, 180);
    table_->setColumnWidth(2, 180);
    table_->setColumnWidth(3, 90);
    table_->setColumnWidth(4, 90);
    table_->setColumnWidth(5, 90);
    table_->setColumnWidth(6, 180);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setAlternatingRowColors(true);
    table_->setSelectionMode(QAbstractItemView::MultiSelection);
    connect(table_, SIGNAL(cellClicked(int,int)), this, SLOT(onTableCellClicked(int,int)));

    tableLayout->addWidget(table_);
    layout->addWidget(tableGroup, 1);
}

void TraceabilityPage::initChart()
{
    chart_ = new QtCharts::QChart;
    chart_->legend()->setVisible(false);
    chart_->legend()->setAlignment(Qt::AlignTop);
    chart_->setTitle(QString::fromUtf8("多板温度曲线对比"));

    axisX_ = new QtCharts::QValueAxis;
    axisY_ = new QtCharts::QValueAxis;
    axisX_->setTitleText(QString::fromUtf8("过炉时间 (s)"));
    axisY_->setTitleText(QString::fromUtf8("温度 (°C)"));

    // ===== 新增：横坐标细分显示，50s 一个主数字，5s 细刻度 =====
    axisX_->setLabelFormat("%.0f");
    axisX_->setTickType(QtCharts::QValueAxis::TicksDynamic);
    axisX_->setTickAnchor(0.0);
    axisX_->setTickInterval(50.0);
    axisX_->setMinorTickCount(9);
    axisY_->setRange(0.0, 300.0);
    axisY_->setTickCount(7);

    chart_->addAxis(axisX_, Qt::AlignBottom);
    chart_->addAxis(axisY_, Qt::AlignLeft);

    rebuildReferenceLines();
    updateAxisRange();
    chartView_->setChart(chart_);
}

void TraceabilityPage::updateAxisRange()
{
    // 横坐标固定 0~200 s，与温区长度设置无关
    axisX_->setRange(0.0, kAxisXMaxSec);
}

void TraceabilityPage::rebuildReferenceLines()
{
    // ===== 新增：根据 12 个温区长度与阈值重建参考线 =====
    clearReferenceLineList(zoneRefLines_);
    clearReferenceLineList(thresholdRefLines_);

    const QVector<double> lengths = settings_->zoneLengths();
    const QVector<double> thresholds = settings_->zoneThresholds();
    double x = 0.0;

    for (int i = 0; i < lengths.size(); ++i) {
        QtCharts::QLineSeries *zoneLine = new QtCharts::QLineSeries;
        // ===== 新增：温区分隔竖线 =====
        QPen zonePen(QColor("#777777"));
        zonePen.setStyle(Qt::DashLine);
        zonePen.setWidth(1);
        zoneLine->setPen(zonePen);
        zoneLine->append(x, 0.0);
        zoneLine->append(x, 300.0);
        chart_->addSeries(zoneLine);
        zoneLine->attachAxis(axisX_);
        zoneLine->attachAxis(axisY_);
        zoneRefLines_.append(zoneLine);

        double nextX = x + lengths.at(i);

        QtCharts::QLineSeries *thresholdLine = new QtCharts::QLineSeries;
        // ===== 新增：当前温区阈值横线 =====
        QPen thresholdPen(QColor("#9C27B0"));
        thresholdPen.setStyle(Qt::DotLine);
        thresholdPen.setWidth(2);
        thresholdLine->setPen(thresholdPen);
        thresholdLine->setName(QString::fromUtf8("%1区阈值 %2°C").arg(i + 1).arg(thresholds.value(i), 0, 'f', 0));
        thresholdLine->append(x, thresholds.value(i));
        thresholdLine->append(nextX, thresholds.value(i));
        chart_->addSeries(thresholdLine);
        thresholdLine->attachAxis(axisX_);
        thresholdLine->attachAxis(axisY_);
        thresholdRefLines_.append(thresholdLine);

        x = nextX;
    }

    QtCharts::QLineSeries *lastZoneLine = new QtCharts::QLineSeries;
    QPen lastZonePen(QColor("#777777"));
    lastZonePen.setStyle(Qt::DashLine);
    lastZonePen.setWidth(1);
    lastZoneLine->setPen(lastZonePen);
    lastZoneLine->append(x, 0.0);
    lastZoneLine->append(x, 300.0);
    chart_->addSeries(lastZoneLine);
    lastZoneLine->attachAxis(axisX_);
    lastZoneLine->attachAxis(axisY_);
    zoneRefLines_.append(lastZoneLine);
}

void TraceabilityPage::clearReferenceLineList(QList<QtCharts::QLineSeries*> &lines)
{
    for (int i = 0; i < lines.size(); ++i) {
        chart_->removeSeries(lines.at(i));
        delete lines.at(i);
    }
    lines.clear();
}

QString TraceabilityPage::buildAlarmText(const BoardRecord &record) const
{
    const QVector<double> thresholds = settings_->zoneThresholds();
    QStringList exceededZones;

    for (int zone = 0; zone < record.zoneTemps.size() && zone < thresholds.size(); ++zone) {
        const QVector<double> &temps = record.zoneTemps.at(zone);
        for (int i = 0; i < temps.size(); ++i) {
            if (temps.at(i) > thresholds.at(zone)) {
                exceededZones.append(QString::fromUtf8("%1区").arg(zone + 1));
                break;
            }
        }
    }

    if (exceededZones.isEmpty()) {
        return QString::fromUtf8("正常");
    }

    return QString::fromUtf8("报警（%1 超阈值）").arg(exceededZones.join(QString::fromUtf8("、")));
}

QColor TraceabilityPage::getNextLineColor()
{
    QList<QColor> colors = {
        QColor(211,47,47),
        QColor(25,118,210),
        QColor(56,142,60),
        QColor(245,124,0),
        QColor(123,31,162),
        QColor(0,121,107),
        QColor(194,24,91),
        QColor(251,192,45)
    };

    for (const QColor &c : colors) {
        bool used = false;
        for (auto s : multiSeries_) {
            if (s->pen().color() == c) {
                used = true;
                break;
            }
        }
        if (!used) return c;
    }
    return colors.first();
}

void TraceabilityPage::showBoard(const QString &barcode)
{
    BoardRecord r;
    if (!manager_->getRecordByBarcode(barcode, r)) return;

    const QVector<double> &zoneOff = settings_->zoneTimeOffsets();
    const double extra = settings_->displayTimeExtra();

    QVector<QPointF> pts;
    int n = qMin(r.timeAxis.size(), qMin(r.fullTemps.size(), r.zoneAxis.size()));
    for (int i = 0; i < n; ++i) {
        int zi = r.zoneAxis.at(i) - 1;
        double dx = extra;
        if (zi >= 0 && zi < zoneOff.size()) {
            dx += zoneOff.at(zi);
        }
        pts.append(QPointF(r.timeAxis.at(i) + dx, r.fullTemps.at(i)));
    }

    QtCharts::QLineSeries *series = nullptr;
    if (multiSeries_.contains(barcode)) {
        series = multiSeries_[barcode];
        series->replace(pts);
    } else {
        series = new QtCharts::QLineSeries;
        QColor color = getNextLineColor();
        QPen pen(color);
        pen.setWidth(2);
        series->setPen(pen);
        series->setName(barcode);
        chart_->addSeries(series);
        series->attachAxis(axisX_);
        series->attachAxis(axisY_);
        multiSeries_[barcode] = series;
        series->replace(pts);
    }
}

void TraceabilityPage::refreshSelectedBoardInfo()
{
    if (selectedBarcodes_.isEmpty()) {
        clearBoardDisplay();
        return;
    }

    QString last = *selectedBarcodes_.begin();
    BoardRecord r;
    if (!manager_->getRecordByBarcode(last, r)) {
        clearBoardDisplay();
        return;
    }

    barcodeValue_->setText(QString("%1（已选%2块）").arg(last).arg(selectedBarcodes_.size()));
    statusValue_->setText(r.status);
    enterValue_->setText(r.enterTime.isValid() ? r.enterTime.toString("yyyy-MM-dd HH:mm:ss") : "--");
    exitValue_->setText(r.exitTime.isValid() ? r.exitTime.toString("yyyy-MM-dd HH:mm:ss") : "--");
    durationValue_->setText(r.timeAxis.isEmpty() ? "--" : QString("%1 s").arg(r.timeAxis.last(), 0, 'f', 1));
    zoneValue_->setText(r.lastZone > 0 ? QString::number(r.lastZone) : "--");

    // ===== 新增：计算并显示峰值温度和最大斜率 =====
    double peakTemp = calculatePeakTemperature(r);
    double maxSlope = calculateMaxSlope(r);
    peakTempValue_->setText(peakTemp > 0 ? QString("%1 °C").arg(peakTemp, 0, 'f', 2) : "--");
    maxSlopeValue_->setText(maxSlope > 0 ? QString("%1 °C/s").arg(maxSlope, 0, 'f', 2) : "--");

    const QString alarmText = buildAlarmText(r);
    alarmValue_->setText(alarmText);
    static const QString kNormalAlarm = QString::fromUtf8("正常");
    if (alarmText == kNormalAlarm) {
        alarmValue_->setStyleSheet("");
    } else {
        alarmValue_->setStyleSheet(QString::fromUtf8("color:#c62828;font-weight:bold;"));
    }
}

void TraceabilityPage::clearBoardDisplay()
{
    for (auto s : multiSeries_) {
        chart_->removeSeries(s);
        delete s;
    }
    multiSeries_.clear();

    barcodeValue_->setText("--");
    statusValue_->setText("--");
    enterValue_->setText("--");
    exitValue_->setText("--");
    durationValue_->setText("--");
    zoneValue_->setText("--");
    peakTempValue_->setText("--");
    maxSlopeValue_->setText("--");
    alarmValue_->setText("--");
    alarmValue_->setStyleSheet("");

    updateAxisRange();
}

// ===== 新增：计算PCB峰值温度 =====
double TraceabilityPage::calculatePeakTemperature(const BoardRecord &record) const
{
    if (record.fullTemps.isEmpty()) {
        return 0.0;
    }

    double peakTemp = record.fullTemps.first();
    for (int i = 0; i < record.fullTemps.size(); ++i) {
        if (record.fullTemps.at(i) > peakTemp) {
            peakTemp = record.fullTemps.at(i);
        }
    }
    return peakTemp;
}

// ===== 新增：计算最大斜率（温度变化速率，单位：°C/s） =====
double TraceabilityPage::calculateMaxSlope(const BoardRecord &record) const
{
    if (record.fullTemps.size() < 2 || record.timeAxis.size() < 2) {
        return 0.0;
    }

    double maxSlope = 0.0;
    int n = qMin(record.fullTemps.size(), record.timeAxis.size());

    for (int i = 1; i < n; ++i) {
        double tempDelta = record.fullTemps.at(i) - record.fullTemps.at(i - 1);
        double timeDelta = record.timeAxis.at(i) - record.timeAxis.at(i - 1);

        if (timeDelta > 0.0) {
            double slope = qAbs(tempDelta / timeDelta);
            if (slope > maxSlope) {
                maxSlope = slope;
            }
        }
    }

    return maxSlope;
}
