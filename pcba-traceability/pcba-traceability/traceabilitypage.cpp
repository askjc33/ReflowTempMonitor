#include "traceabilitypage.h"
#include "constants.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QFrame>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QSignalBlocker>
#include <QVector>
#include <QPointF>
#include <QPainter>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QDebug>

QT_CHARTS_USE_NAMESPACE

TraceabilityPage::TraceabilityPage(QWidget *parent) : QWidget(parent)
{
    initUi();
    initChart();
}

void TraceabilityPage::updateRecords(const QList<BoardRecord> &records)
{
    qDebug() << "🔥 updateRecords CALLED, size =" << records.size();

    setRecords(records);
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
        table_->setColumnCount(7);
        table_->setHorizontalHeaderLabels(QStringList() << "条码" << "进板时间" << "出板时间" << "状态" << "时长(s)" << "最后气室" << "点数");
        table_->horizontalHeader()->setStretchLastSection(true);
        table_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
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

    addZoneReferenceLines();
    chartView_->setChart(chart_);
}

void TraceabilityPage::addZoneReferenceLines()
{
    const double step = totalProcessTimeSec_ / double(ZONE_COUNT);
    for (int i = 0; i <= ZONE_COUNT; ++i) {
        QLineSeries *line = new QLineSeries;
        QPen pen(QColor("#888888"));
        pen.setStyle(Qt::DashLine);
        pen.setWidth(1);
        line->setPen(pen);

        const double x = i * step;
        line->append(x, 0);
        line->append(x, 260);

        chart_->addSeries(line);
        line->attachAxis(axisX_);
        line->attachAxis(axisY_);
        zoneRefLines_.append(line);
    }
}


void TraceabilityPage::setRecords(const QList<BoardRecord> &records)
{
    records_ = records;

    table_->setUpdatesEnabled(false);
    {
        QSignalBlocker blocker(table_);
        table_->clearContents();
        table_->setRowCount(records.size());

        for (int row = 0; row < records.size(); ++row) {
            const BoardRecord &r = records[row];

            QTableWidgetItem *barcodeItem = new QTableWidgetItem(r.barcode);
            barcodeItem->setData(Qt::UserRole, r.barcode);
            table_->setItem(row, 0, barcodeItem);
            table_->setItem(row, 1, new QTableWidgetItem(r.enterTime.isValid() ? r.enterTime.toString("yyyy-MM-dd HH:mm:ss") : "-"));
            table_->setItem(row, 2, new QTableWidgetItem(r.exitTime.isValid() ? r.exitTime.toString("yyyy-MM-dd HH:mm:ss") : "-"));
            table_->setItem(row, 3, new QTableWidgetItem(r.status));
            table_->setItem(row, 4, new QTableWidgetItem(r.timeAxis.isEmpty() ? "0.0" : QString::number(r.timeAxis.last(), 'f', 1)));
            table_->setItem(row, 5, new QTableWidgetItem(r.lastZone > 0 ? QString::number(r.lastZone) : "-"));
            table_->setItem(row, 6, new QTableWidgetItem(QString::number(r.timeAxis.size())));
        }
    }
    table_->setUpdatesEnabled(true);
    table_->viewport()->update();

    if (!selectedBarcode_.isEmpty()) {
        for (const BoardRecord &r : records_) {
            if (r.barcode == selectedBarcode_) {
                showBoard(r);
                reselectRowByBarcode(selectedBarcode_);
                return;
            }
        }
    }

    if (!records_.isEmpty()) {
        selectedBarcode_ = records_.first().barcode;
        showBoard(records_.first());
        reselectRowByBarcode(selectedBarcode_);
    } else {
        clearBoardDisplay();
    }
}


QString TraceabilityPage::selectedBarcode() const
{
    return selectedBarcode_;
}

void TraceabilityPage::clearUi()
{
    records_.clear();
    selectedBarcode_.clear();
    table_->clearContents();
    table_->setRowCount(0);
    clearBoardDisplay();
}

void TraceabilityPage::setBatteryText(const QString &text)
{
    batteryLabel_->setText(text);
}

void TraceabilityPage::showBoard(const BoardRecord &r)
{
    barcodeValue_->setText(r.barcode);
    statusValue_->setText(r.status);
    enterValue_->setText(r.enterTime.isValid() ? r.enterTime.toString("yyyy-MM-dd HH:mm:ss") : "--");
    exitValue_->setText(r.exitTime.isValid() ? r.exitTime.toString("yyyy-MM-dd HH:mm:ss") : "--");
    durationValue_->setText(r.timeAxis.isEmpty() ? "--" : QString("%1 s").arg(r.timeAxis.last(), 0, 'f', 1));
    zoneValue_->setText(r.lastZone > 0 ? QString::number(r.lastZone) : "--");

    QVector<QPointF> points;
    const int n = qMin(r.timeAxis.size(), r.fullTemps.size());
    points.reserve(n);
    for (int i = 0; i < n; ++i) {
        points.append(QPointF(r.timeAxis[i], r.fullTemps[i]));
    }
    series_->replace(points);

    double xMax = totalProcessTimeSec_;
    if (!r.timeAxis.isEmpty()) {
        xMax = qMax(totalProcessTimeSec_, r.timeAxis.last() + 5.0);
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
    series_->clear();
    axisX_->setRange(0, totalProcessTimeSec_);
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

void TraceabilityPage::onTableCellClicked(int row, int)
{
    QTableWidgetItem *item = table_->item(row, 0);
    if (!item) return;

    selectedBarcode_ = item->data(Qt::UserRole).toString();
    for (const BoardRecord &r : records_) {
        if (r.barcode == selectedBarcode_) {
            showBoard(r);
            emit selectedBarcodeChanged(selectedBarcode_);
            break;
        }
    }
}

void TraceabilityPage::onDisconnectClicked()
{
    emit requestDisconnect();
}

void TraceabilityPage::onClearClicked()
{
    emit requestClearFinished();
}

void TraceabilityPage::onExportClicked()
{
    if (!selectedBarcode_.isEmpty()) {
        emit requestExportCurrent(selectedBarcode_);
    }
}
