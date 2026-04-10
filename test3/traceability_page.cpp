#include "traceability_page.h"
#include "serial_manager.h"
#include "board_trace_manager.h"

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

TraceabilityPage::TraceabilityPage(SerialManager *serial, BoardTraceManager *manager, QWidget *parent)
    : QWidget(parent),
      serial_(serial),
      manager_(manager),
      batteryLabel_(nullptr),
      barcodeValue_(nullptr),
      statusValue_(nullptr),
      enterValue_(nullptr),
      exitValue_(nullptr),
      durationValue_(nullptr),
      zoneValue_(nullptr),
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
            chart_->removeSeries(multiSeries_.take(barcode));
        }
        emit requestSetStatusMessage(QString::fromUtf8("已隐藏：%1").arg(barcode));
    } else {
        selectedBarcodes_.insert(barcode);
        showBoard(barcode);
        emit requestSetStatusMessage(QString::fromUtf8("已显示：%1").arg(barcode));
    }

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
        QMessageBox::information(this, QString::fromUtf8("导出"), QString::fromUtf8("请先选择一块PCBA记录。"));
        return;
    }

    QString barcode = *selectedBarcodes_.begin();
    BoardRecord r;
    if (!manager_->getRecordByBarcode(barcode, r)) {
        QMessageBox::warning(this, QString::fromUtf8("导出"), QString::fromUtf8("未找到记录。"));
        return;
    }

    QString path = QFileDialog::getSaveFileName(
        this,
        QString::fromUtf8("导出CSV"),
        QString("%1.csv").arg(r.barcode),
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
    out << "Barcode," << r.barcode << "\n";
    out << "EnterTime," << (r.enterTime.isValid() ? r.enterTime.toString(Qt::ISODate) : "") << "\n";
    out << "ExitTime," << (r.exitTime.isValid() ? r.exitTime.toString(Qt::ISODate) : "") << "\n";
    out << "Status," << r.status << "\n\n";
    out << "ElapsedSec,Temp,Zone\n";

    int n = qMin(r.timeAxis.size(), qMin(r.fullTemps.size(), r.zoneAxis.size()));
    for (int i = 0; i < n; ++i) {
        out << QString::number(r.timeAxis.at(i), 'f', 3) << ","
            << QString::number(r.fullTemps.at(i), 'f', 2) << ","
            << r.zoneAxis.at(i) << "\n";
    }

    file.close();
    QMessageBox::information(this, QString::fromUtf8("导出成功"), path);
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
    table_->setColumnCount(6);
    table_->setHorizontalHeaderLabels(
        QStringList() << QString::fromUtf8("条码")
        << QString::fromUtf8("进板时间")
        << QString::fromUtf8("出板时间")
        << QString::fromUtf8("状态")
        << QString::fromUtf8("时长(s)")
        << QString::fromUtf8("最后气室"));

    table_->horizontalHeader()->setStretchLastSection(true);
    table_->setColumnWidth(0, 140);
    table_->setColumnWidth(1, 180);
    table_->setColumnWidth(2, 180);
    table_->setColumnWidth(3, 90);
    table_->setColumnWidth(4, 90);
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
    chart_->legend()->setVisible(true);
    chart_->legend()->setAlignment(Qt::AlignTop);
    chart_->setTitle(QString::fromUtf8("多板温度曲线对比"));

    axisX_ = new QtCharts::QValueAxis;
    axisY_ = new QtCharts::QValueAxis;
    axisX_->setTitleText(QString::fromUtf8("过炉时间 (s)"));
    axisY_->setTitleText(QString::fromUtf8("温度 (°C)"));

    // 永久固定坐标轴
    axisX_->setRange(0, 180.0);
    axisY_->setRange(0, 300.0);

    chart_->addAxis(axisX_, Qt::AlignBottom);
    chart_->addAxis(axisY_, Qt::AlignLeft);

    addZoneReferenceLines();
    chartView_->setChart(chart_);
}

void TraceabilityPage::addZoneReferenceLines()
{
    double step = 180.0 / (double)ZONE_COUNT;
    for (int i = 0; i <= ZONE_COUNT; ++i) {
        QtCharts::QLineSeries *line = new QtCharts::QLineSeries;
        QPen pen(QColor("#888888"));
        pen.setStyle(Qt::DashLine);
        pen.setWidth(1);
        line->setPen(pen);
        double x = i * step;
        line->append(x, 0);
        line->append(x, 300);
        chart_->addSeries(line);
        line->attachAxis(axisX_);
        line->attachAxis(axisY_);
        zoneRefLines_.append(line);
    }
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

    QVector<QPointF> pts;
    int n = qMin(r.timeAxis.size(), r.fullTemps.size());
    for (int i = 0; i < n; ++i) {
        pts.append({r.timeAxis[i], r.fullTemps[i]});
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
}

void TraceabilityPage::clearBoardDisplay()
{
    for (auto s : multiSeries_) chart_->removeSeries(s);
    qDeleteAll(multiSeries_);
    multiSeries_.clear();

    barcodeValue_->setText("--");
    statusValue_->setText("--");
    enterValue_->setText("--");
    exitValue_->setText("--");
    durationValue_->setText("--");
    zoneValue_->setText("--");
}
