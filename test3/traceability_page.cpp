#include "traceability_page.h"

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

TraceabilityPage::TraceabilityPage(SerialManager *serial, BoardTraceManager *manager, QWidget *parent)
    : QWidget(parent),
      serial_(serial),
      manager_(manager),
      batteryLabel_(0),
      barcodeValue_(0),
      statusValue_(0),
      enterValue_(0),
      exitValue_(0),
      durationValue_(0),
      zoneValue_(0),
      chartView_(0),
      chart_(0),
      series_(0),
      axisX_(0),
      axisY_(0),
      table_(0)
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

    table_->setUpdatesEnabled(false);           //减少刷新闪烁和重绘次数
    {
        QSignalBlocker blocker(table_);         //重新填表时，可能触发表格自己的信号。刷新时暂时屏蔽信号
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
        }
    }
    table_->setUpdatesEnabled(true);
    table_->viewport()->update();

    if (!selectedBarcode_.isEmpty()) {
        showBoard(selectedBarcode_);
        reselectRowByBarcode(selectedBarcode_);
    } else if (!records.isEmpty()) {
        selectedBarcode_ = records.first().barcode;
        showBoard(selectedBarcode_);
        reselectRowByBarcode(selectedBarcode_);
    } else {
        clearBoardDisplay();
    }
}

void TraceabilityPage::onTableCellClicked(int row, int)
{
    QTableWidgetItem *item = table_->item(row, 0);
    if (!item) {
        return;
    }
    selectedBarcode_ = item->data(Qt::UserRole).toString();
    showBoard(selectedBarcode_);
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

    selectedBarcode_.clear();
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

void TraceabilityPage::exportCurrentBoard()     //负责把当前选中板导出成 CSV
{
    if (selectedBarcode_.isEmpty()) {
        QMessageBox::information(this, QString::fromUtf8("导出"), QString::fromUtf8("请先选择一块PCBA记录。"));
        return;
    }

    BoardRecord r;
    if (!manager_->getRecordByBarcode(selectedBarcode_, r)) {
        QMessageBox::warning(this, QString::fromUtf8("导出"), QString::fromUtf8("未找到当前板记录。"));
        return;
    }

    QString path = QFileDialog::getSaveFileName(
        this,
        QString::fromUtf8("导出当前板CSV"),
        QString("%1.csv").arg(r.barcode),
        QString::fromUtf8("CSV 文件 (*.csv)")
    );
    if (path.isEmpty()) {
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, QString::fromUtf8("导出失败"), QString::fromUtf8("无法写入文件。"));
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
    QMessageBox::information(this, QString::fromUtf8("导出成功"),
                             QString::fromUtf8("已导出到：\n%1").arg(path));
}

void TraceabilityPage::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *top = new QHBoxLayout;

    QLabel *title = new QLabel(QString::fromUtf8("PCBA 过炉追溯"));
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

    QGroupBox *chartGroup = new QGroupBox(QString::fromUtf8("温度曲线"));
    QVBoxLayout *chartLayout = new QVBoxLayout(chartGroup);

    chartView_ = new QChartView;
    chartView_->setRenderHint(QPainter::Antialiasing);
    chartView_->setMinimumHeight(420);
    chartLayout->addWidget(chartView_);

    layout->addWidget(chartGroup, 1);

    QGroupBox *tableGroup = new QGroupBox(QString::fromUtf8("过炉记录列表"));
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);

    table_ = new QTableWidget;
    table_->setColumnCount(6);
    table_->setHorizontalHeaderLabels(
        QStringList()
        << QString::fromUtf8("条码")
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
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(table_, SIGNAL(cellClicked(int,int)), this, SLOT(onTableCellClicked(int,int)));

    tableLayout->addWidget(table_);
    layout->addWidget(tableGroup, 1);
}

void TraceabilityPage::initChart()
{
    chart_ = new QChart;    //图表容器
    chart_->legend()->setVisible(false);
    chart_->setTitle(QString::fromUtf8("选中 PCBA 的完整过炉温度曲线"));

    series_ = new QLineSeries;      //一条折线
    QPen pen(QColor("#d32f2f"));
    pen.setWidth(2);
    series_->setPen(pen);

    chart_->addSeries(series_);

    //坐标轴
    axisX_ = new QValueAxis;
    axisY_ = new QValueAxis;

    axisX_->setTitleText(QString::fromUtf8("过炉时间 (s)"));
    axisY_->setTitleText(QString::fromUtf8("温度 (°C)"));
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
    double step = totalProcessTimeSec_ / double(ZONE_COUNT);

    for (int i = 0; i <= ZONE_COUNT; ++i) {
        QLineSeries *line = new QLineSeries;
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
}

void TraceabilityPage::showBoard(const QString &barcode)    //从 BoardTraceManager 拿到这块板完整记录，然后更新
{
    BoardRecord r;
    if (!manager_->getRecordByBarcode(barcode, r)) {
        clearBoardDisplay();
        return;
    }

    barcodeValue_->setText(r.barcode);
    statusValue_->setText(r.status);
    enterValue_->setText(r.enterTime.isValid() ? r.enterTime.toString("yyyy-MM-dd HH:mm:ss") : "--");
    exitValue_->setText(r.exitTime.isValid() ? r.exitTime.toString("yyyy-MM-dd HH:mm:ss") : "--");
    durationValue_->setText(r.timeAxis.isEmpty() ? "--" : QString("%1 s").arg(r.timeAxis.last(), 0, 'f', 1));
    zoneValue_->setText(r.lastZone > 0 ? QString::number(r.lastZone) : "--");

    QVector<QPointF> points;
    int n = qMin(r.timeAxis.size(), r.fullTemps.size());
    points.reserve(n);
    for (int i = 0; i < n; ++i) {
        points.append(QPointF(r.timeAxis.at(i), r.fullTemps.at(i)));
    }

    series_->replace(points);

    double xMax = totalProcessTimeSec_;
    if (!r.timeAxis.isEmpty()) {
        xMax = qMax(totalProcessTimeSec_, r.timeAxis.last() + 5.0);
    }
    axisX_->setRange(0, xMax);

    emit requestSetStatusMessage(QString::fromUtf8("已显示板 %1 的温度曲线").arg(r.barcode));
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

    if (axisX_) {
        axisX_->setRange(0, totalProcessTimeSec_);
    }
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
