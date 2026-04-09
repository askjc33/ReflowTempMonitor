#include "mainwindow.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDateTime>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QSerialPortInfo>

// ================= 构造 =================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_serialThread(new QThread(this))
    , m_serialWorker(new SerialWorker)
    , m_connected(false)
    , m_dataManager(new PcbDataManager(this))
    , m_refreshTimer(new QTimer(this))
    , m_simTimer(new QTimer(this))
{
    setWindowTitle("回流焊温度监控系统");
    resize(1500, 900);

    applyLightStyle();
    setupUi();
    setupConnections();
    refreshPortList();

    m_serialWorker->moveToThread(m_serialThread);
    m_serialThread->start();

    m_refreshTimer->start(200);
}

MainWindow::~MainWindow()
{
    m_serialThread->quit();
    m_serialThread->wait();
    delete m_serialWorker;
}

// ================= UI风格 =================
void MainWindow::applyLightStyle()
{
    qApp->setStyleSheet(R"(
        QWidget { background:#f5f7fa; color:#333; }
        QPushButton {
            background:#ffffff;
            border:1px solid #ccc;
            padding:6px;
            border-radius:4px;
        }
        QPushButton:hover { background:#e6f0ff; }
        QGroupBox {
            border:1px solid #ddd;
            border-radius:6px;
            margin-top:10px;
        }
        QTableWidget { background:white; }
    )");
}

// ================= UI =================
void MainWindow::setupUi()
{
    QWidget *central = new QWidget;
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // ===== 顶部 =====
    QHBoxLayout *top = new QHBoxLayout;
    m_connectBtn = new QPushButton("连接");
    QPushButton *btnSim = new QPushButton("模拟");
    QPushButton *btnClear = new QPushButton("清空");

    top->addWidget(m_connectBtn);
    top->addWidget(btnSim);
    top->addWidget(btnClear);
    top->addStretch();

    mainLayout->addLayout(top);

    // ===== 三栏 =====
    QHBoxLayout *body = new QHBoxLayout;
    mainLayout->addLayout(body);

    // 左
    m_pcbListWidget = new QListWidget;
    m_pcbListWidget->setFixedWidth(250);
    body->addWidget(m_pcbListWidget);

    // 中
    QVBoxLayout *center = new QVBoxLayout;

    m_tempChart = new TemperatureChart;
    center->addWidget(m_tempChart, 3);

    // 表格
    QTableWidget *table = new QTableWidget(12, 2);
    table->setHorizontalHeaderLabels({"气室", "温度"});
    for(int i=0;i<12;i++)
        table->setItem(i,0,new QTableWidgetItem(QString("Zone%1").arg(i+1)));

    center->addWidget(table,1);

    body->addLayout(center,1);

    // 右
    QLabel *info = new QLabel("说明:\n每块PCB生成曲线\n支持追溯");
    info->setWordWrap(true);
    info->setFixedWidth(200);
    body->addWidget(info);

    // ===== 连接 =====
    connect(m_connectBtn, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(btnSim, &QPushButton::clicked, this, &MainWindow::onSimulateClicked);
    connect(btnClear, &QPushButton::clicked, this, &MainWindow::onClearClicked);
    connect(m_pcbListWidget, &QListWidget::itemClicked,
            this, &MainWindow::onPcbListItemClicked);
}

// ================= 串口 =================
void MainWindow::refreshPortList()
{
}

void MainWindow::onConnectClicked()
{
    m_connected = !m_connected;
    m_connectBtn->setText(m_connected ? "断开" : "连接");
}

void MainWindow::onPortOpened(bool, const QString &) {}
void MainWindow::onPortClosed() {}
void MainWindow::onPortError(const QString &) {}

// ================= PCB =================
void MainWindow::onBarcodeReceived(const QString &id)
{
    m_dataManager->registerPcb(id);
    m_pcbListWidget->addItem(id);
}

void MainWindow::onTemperatureReceived(const QString &id,
                                       float t0,float t1,float t2,float t3,float t4,
                                       int chamber)
{
    float temps[5]={t0,t1,t2,t3,t4};
    m_dataManager->updatePcbTemperature(id,temps,chamber);
}

void MainWindow::onPcbRegistered(const QString &id)
{
    m_pcbListWidget->addItem(id);
}

void MainWindow::onPcbDataUpdated(const QString &) {}
void MainWindow::onPcbLeft(const QString &) {}

void MainWindow::onPcbListItemClicked(QListWidgetItem *item)
{
    QString id=item->text();
    const PcbRecord *rec=m_dataManager->getPcbRecord(id);
    if(rec) m_tempChart->setPcbRecord(rec);
}

// ================= 模拟 =================
void MainWindow::onSimulateClicked()
{
    connect(m_simTimer,&QTimer::timeout,this,[this](){

        QString id="PCB_TEST";

        if(!m_dataManager->getPcbRecord(id))
            m_dataManager->registerPcb(id);

        float temps[5];
        for(int i=0;i<5;i++)
            temps[i]=50+i*20+QRandomGenerator::global()->bounded(10);

        int chamber=QRandomGenerator::global()->bounded(5);

        m_dataManager->updatePcbTemperature(id,temps,chamber);

    });

    m_simTimer->start(500);
}

void MainWindow::onManualScanClicked() {}

void MainWindow::onClearClicked()
{
    m_pcbListWidget->clear();
    m_dataManager->clearAll();
}

// ================= 刷新 =================
void MainWindow::onRefreshTimer()
{
}
void MainWindow::setupConnections()
{
    // 在这里写所有信号槽连接的逻辑，比如串口、按钮、数据更新的connect
}
