#include "view.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QSignalBlocker>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

// ====================== ConnectionPage 实现 ======================
ConnectionPage::ConnectionPage(QWidget *parent)
    : QWidget{parent}
{
    initUi();
}

void ConnectionPage::initUi()
{
    auto *layout = new QVBoxLayout(this);

    auto *title = new QLabel("串口连接");
    QFont f;
    f.setPointSize(12);
    f.setBold(true);
    title->setFont(f);
    layout->addWidget(title);

    auto *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    layout->addWidget(sep);

    auto *group = new QGroupBox("可用设备");
    auto *gLayout = new QVBoxLayout(group);

    portList_ = new QListWidget;
    portList_->setMinimumHeight(300);
    connect(portList_, &QListWidget::itemClicked, this, &ConnectionPage::onItemClicked);
    gLayout->addWidget(portList_);

    layout->addWidget(group);

    auto *infoGroup = new QGroupBox("设备信息");
    auto *infoLayout = new QVBoxLayout(infoGroup);
    infoLabel_ = new QLabel("请先扫描串口设备");
    infoLabel_->setWordWrap(true);
    infoLayout->addWidget(infoLabel_);
    layout->addWidget(infoGroup);

    auto *btnLayout = new QHBoxLayout;
    scanBtn_ = new QPushButton("扫描设备");
    connect(scanBtn_, &QPushButton::clicked, this, &ConnectionPage::onScanClicked);
    btnLayout->addWidget(scanBtn_);

    connectBtn_ = new QPushButton("连接");
    connectBtn_->setEnabled(false);
    connect(connectBtn_, &QPushButton::clicked, this, &ConnectionPage::onConnectClicked);
    btnLayout->addWidget(connectBtn_);

    layout->addLayout(btnLayout);
}

void ConnectionPage::onScanClicked()
{
    portList_->clear();
    ports_.clear();
    selectedPort_.clear();
    connectBtn_->setEnabled(false);
    infoLabel_->setText("请先扫描串口设备");
    emit scanRequested();
}

void ConnectionPage::onScanFinished(const QList<PortItem> &ports)
{
    ports_ = ports;
    if (ports.isEmpty()) {
        return;
    }

    for (const auto &p : ports) {
        QString label = p.description.isEmpty() ? "未知设备" : p.description;
        auto *item = new QListWidgetItem(QString("%1 (%2)").arg(label, p.portName));
        item->setData(Qt::UserRole, p.portName);
        portList_->addItem(item);
    }
}

void ConnectionPage::onItemClicked(QListWidgetItem *item)
{
    selectedPort_ = item->data(Qt::UserRole).toString();
    connectBtn_->setEnabled(true);

    for (const auto &p : ports_) {
        if (p.portName == selectedPort_) {
            infoLabel_->setText(
                QString("端口：%1\n描述：%2\n厂商：%3\n位置：%4")
                    .arg(p.portName)
                    .arg(p.description.isEmpty() ? "未知" : p.description)
                    .arg(p.manufacturer.isEmpty() ? "未知" : p.manufacturer)
                    .arg(p.systemLocation.isEmpty() ? "未知" : p.systemLocation)
            );
            break;
        }
    }
}

void ConnectionPage::onConnectClicked()
{
    if (selectedPort_.isEmpty()) return;
    emit connectRequested(selectedPort_);
}

// ====================== TraceabilityPage 实现 ======================
TraceabilityPage::TraceabilityPage(QWidget *parent)
    : QWidget{parent}
{
    initUi();
    initChart();
}

void TraceabilityPage::initUi()
{
    auto *layout = new QVBoxLayout(this);

    auto *top = new QHBoxLayout;

    auto *title = new QLabel("PCBA 过炉追溯");
    QFont titleFont;
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    title->setFont(titleFont);
    top->addWidget(title);

    top->addStretch();

    batteryLabel_ = new QLabel("电量：--");
    top->addWidget(batteryLabel_);

    auto *exportBtn = new QPushButton("导出当前板CSV");
    connect(exportBtn, &QPushButton::clicked, this, &TraceabilityPage::onExportClicked);
    top->addWidget(exportBtn);

    auto *clearBtn = new QPushButton("清空完成记录");
    connect(clearBtn, &QPushButton::clicked, this, &TraceabilityPage::onClearClicked);
    top->addWidget(clearBtn);

    auto *disconnectBtn = new QPushButton("断开连接");
    connect(disconnectBtn, &QPushButton::clicked, this, &TraceabilityPage::onDisconnectClicked);
    top->addWidget(disconnectBtn);

    layout->addLayout(top);

    auto *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    layout->addWidget(sep);

    auto *infoGroup = new QGroupBox("当前选中板信息");
    auto *infoLayout = new QGridLayout(infoGroup);

    barcodeValue_ = new QLabel("--");
    statusValue_ = new QLabel("--");
    enterValue_ = new QLabel("--");
    exitValue_ = new QLabel("--");
    durationValue_ = new QLabel("--");
    zoneValue_ = new QLabel("--");

    infoLayout->addWidget(new QLabel("条码："), 0, 0);
    infoLayout->addWidget(barcodeValue_, 0, 1);
    infoLayout->addWidget(new QLabel("状态："), 0, 2);
    infoLayout->addWidget(statusValue_, 0, 3);
    infoLayout->addWidget(new QLabel("当前/最后气室："), 0, 4);
    infoLayout->addWidget(zoneValue_, 0, 5);

    infoLayout->addWidget(new QLabel("进板时间："), 1, 0);
    infoLayout->addWidget(enterValue_, 1, 1);
    infoLayout->addWidget(new QLabel("出板时间："), 1, 2);
    infoLayout->addWidget(exitValue_, 1, 3);
    infoLayout->addWidget(new QLabel("过炉时长："), 1, 4);
    infoLayout->addWidget(durationValue_, 1, 5);

    layout->addWidget(infoGroup);

    auto *chartGroup = new QGroupBox("温度曲线");
    auto *chartLayout = new QVBoxLayout(chartGroup);

    chartView_ = new QChartView;
    chartView_->setRenderHint(QPainter::Antialiasing);
    chartView_->setMinimumHeight(420);
    chartLayout->addWidget(chartView_);

    layout->addWidget(chartGroup, 1);

    auto *tableGroup = new QGroupBox("过炉记录列表");
    auto *tableLayout = new QVBoxLayout(tableGroup);

    table_ = new QTableWidget;
    table_->setColumnCount(6);
    table_->setHorizontalHeaderLabels(
        QStringList() << "条码" << "进板时间" << "出板时间" << "状态" << "时长(s)" << "最后气室");
    table_->horizontalHeader()->setStretchLastSection(true);

    // 设置列宽
    table_->setColumnWidth(0, 140);
    table_->setColumnWidth(1, 180);
    table_->setColumnWidth(2, 180);
    table_->setColumnWidth(3, 90);
    table_->setColumnWidth(4, 90);

    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setAlternatingRowColors(true);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(table_, &QTableWidget::cellClicked, this, &TraceabilityPage::onTableCellClicked);

    tableLayout->addWidget(table_);
    layout->addWidget(tableGroup, 1);
}

void TraceabilityPage::initChart()
{
    chart_ = new QChart;
    chart_->legend()->setVisible(false);
    chart_->setTitle("选中 PCBA 的完整过炉温度曲线");

    series_ = new QLineSeries;
    QPen pen(QColor("#d32f2f"));
    pen.setWidth(2);
    series_->setPen(pen);

    chart_->addSeries(series_);

    axisX_ = new QValueAxis;
    axisY_ = new QValueAxis;

    axisX_->setTitleText("过炉时间 (s)");
    axisY_->setTitleText("温度 (°C)");
    axisX_->setRange(0, totalProcessTimeSec_);
    axisY_->setRange(0, 260);

    chart_->addAxis(axisX_, Qt::AlignBottom);
    chart_->addAxis(axisY_, Qt::AlignLeft);
    series_->attachAxis(axisX_);
    series_->attachAxis(axisY_);

    // 添加气室参考线
    const double step = totalProcessTimeSec_ / 12.0;
    for (int i = 0; i <= 12; ++i) {
        auto *line = new QLineSeries;
        QPen pen(QColor("#888888"));
        pen.setStyle(Qt::DashLine);
        pen.setWidth(1);
        line->setPen(pen);

        double x = i * step;
        line->append(x, 0);
        line->append(x, 260);

        chart_->addSeries(line);
        line->attachAxis(axisX_);
        line->attachAxis(axisY_);
        zoneRefLines_.append(line);
    }

    chartView_->setChart(chart_);
}

void TraceabilityPage::refreshRecordList(const QList<BoardRecord> &records)
{
    table_->setUpdatesEnabled(false);
    {
        QSignalBlocker blocker(table_);
        table_->clearContents();
        table_->setRowCount(records.size());

        for (int row = 0; row < records.size(); ++row) {
            const BoardRecord &r = records[row];

            auto *itemBarcode = new QTableWidgetItem(r.barcode);
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
        }
    }
    table_->setUpdatesEnabled(true);
    table_->viewport()->update();

    if (!selectedBarcode_.isEmpty()) {
        reselectRowByBarcode(selectedBarcode_);
    } else if (!records.isEmpty()) {
        selectedBarcode_ = records.first().barcode;
        showBoardRecord(records.first());
        reselectRowByBarcode(selectedBarcode_);
    } else {
        clearBoardDisplay();
    }
}

void TraceabilityPage::onTableCellClicked(int row, int)
{
    QTableWidgetItem *item = table_->item(row, 0);
    if (!item) return;
    selectedBarcode_ = item->data(Qt::UserRole).toString();
    emit boardSelected(selectedBarcode_);
}

void TraceabilityPage::showBoardRecord(const BoardRecord &r)
{
    barcodeValue_->setText(r.barcode);
    statusValue_->setText(r.status);
    enterValue_->setText(r.enterTime.isValid() ? r.enterTime.toString("yyyy-MM-dd HH:mm:ss") : "--");
    exitValue_->setText(r.exitTime.isValid() ? r.exitTime.toString("yyyy-MM-dd HH:mm:ss") : "--");
    durationValue_->setText(r.timeAxis.isEmpty() ? "--" : QString("%1 s").arg(r.timeAxis.last(), 0, 'f', 1));
    zoneValue_->setText(r.lastZone > 0 ? QString::number(r.lastZone) : "--");

    QVector<QPointF> points;
    points.reserve(std::min(r.timeAxis.size(), r.fullTemps.size()));
    const int n = std::min(r.timeAxis.size(), r.fullTemps.size());
    for (int i = 0; i < n; ++i) {
        points.append(QPointF(r.timeAxis[i], r.fullTemps[i]));
    }

    series_->replace(points);

    double xMax = totalProcessTimeSec_;
    if (!r.timeAxis.isEmpty()) {
        xMax = std::max(totalProcessTimeSec_, r.timeAxis.last() + 5.0);
    }
    axisX_->setRange(0, xMax);
}

void TraceabilityPage::clearBoardDisplay()
{
    barcodeValue_->setText("--");
    statusValue_->setText("--");
    enterValue_->setText("--");
    exitValue_->setText("--");
    durationValue_->setText("--");
    zoneValue_->setText("--");
    if (series_) {
        QVector<QPointF> empty;
        series_->replace(empty);
    }
    axisX_->setRange(0, totalProcessTimeSec_);
    selectedBarcode_.clear();
}

void TraceabilityPage::updateBattery(int value)
{
    batteryLabel_->setText(QString("电量：%1%").arg(value));
}

void TraceabilityPage::reselectRowByBarcode(const QString &barcode)
{
    for (int row = 0; row < table_->rowCount(); ++row) {
        QTableWidgetItem *item = table_->item(row, 0);
        if (item && item->data(Qt::UserRole).toString() == barcode) {
            table_->selectRow(row);
            return;
        }
    }
}

void TraceabilityPage::onExportClicked()
{
    emit exportRequested();
}

void TraceabilityPage::onClearClicked()
{
    emit clearFinishedRequested();
}

void TraceabilityPage::onDisconnectClicked()
{
    emit disconnectRequested();
}
