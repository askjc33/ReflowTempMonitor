#include "mainwindow.h"

#include <QStackedWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QCloseEvent>
#include <QKeySequence>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initUi();
    setupMenu();
    setupStatusBar();
    wireSignals();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    serial_.disconnectPort();
    QMainWindow::closeEvent(event);
}

void MainWindow::switchToConnectionPage()
{
    stacked_->setCurrentWidget(connPage_);
}

void MainWindow::switchToTracePage()
{
    stacked_->setCurrentWidget(tracePage_);
}

void MainWindow::updateClock()
{
    timeLabel_->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
}

void MainWindow::setStatusMessage(const QString &text)
{
    msgLabel_->setText(text);
}

void MainWindow::setConnectionStatus(const QString &text, bool connected)
{
    connLabel_->setText(QString("● %1").arg(text));
    connLabel_->setStyleSheet(connected ? "color:green;font-weight:bold;" : "color:black;");
}

void MainWindow::setBatteryLevel(int v)
{
    if (v < 0) {
        batteryLabel_->setText("电量：--");
        batteryLabel_->setStyleSheet("");
    } else {
        batteryLabel_->setText(QString("电量：%1%").arg(v));
        batteryLabel_->setStyleSheet(v <= 20 ? "color:red;" : "");
    }
}

void MainWindow::showAbout()
{
    QMessageBox::about(
        this,
        "关于",
        QString("<b>PCBA 多板过炉追溯系统</b><br>"
                "版本：%1<br><br>"
                "协议：TEMP/POS 同步帧<br>"
                "支持多板同时在炉、逐板温度曲线追溯。<br>"
                "本版本已修复长时间运行后界面未响应问题。")
            .arg(APP_VERSION)
    );
}

void MainWindow::initUi()
{
    setWindowTitle("PCBA 多板过炉追溯系统");
    resize(1400, 900);

    stacked_ = new QStackedWidget(this);

    connPage_ = new ConnectionPage(&serial_);
    tracePage_ = new TraceabilityPage(&serial_, &traceManager_);

    stacked_->addWidget(connPage_);
    stacked_->addWidget(tracePage_);
    setCentralWidget(stacked_);
}

void MainWindow::wireSignals()
{
    connect(connPage_, &ConnectionPage::requestSwitchToTracePage,
            this, &MainWindow::switchToTracePage);
    connect(connPage_, &ConnectionPage::requestSetStatusMessage,
            this, &MainWindow::setStatusMessage);
    connect(connPage_, &ConnectionPage::requestSetConnectionStatus,
            this, &MainWindow::setConnectionStatus);

    connect(tracePage_, &TraceabilityPage::requestDisconnectAndBack,
            this, &MainWindow::switchToConnectionPage);
    connect(tracePage_, &TraceabilityPage::requestSetStatusMessage,
            this, &MainWindow::setStatusMessage);
    connect(tracePage_, &TraceabilityPage::requestSetConnectionStatus,
            this, &MainWindow::setConnectionStatus);
    connect(tracePage_, &TraceabilityPage::requestSetBatteryLevel,
            this, &MainWindow::setBatteryLevel);

    connect(&serial_, &SerialManager::synchronizedFrameReceived,
            &traceManager_, &BoardTraceManager::onSynchronizedFrame);
}

void MainWindow::setupMenu()
{
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");

    QAction *quitAct = new QAction("退出(&Q)", this);
    quitAct->setShortcut(QKeySequence("Ctrl+Q"));
    connect(quitAct, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(quitAct);

    QMenu *viewMenu = menuBar()->addMenu("视图(&V)");

    QAction *connAct = new QAction("连接页(&C)", this);
    connect(connAct, &QAction::triggered, this, &MainWindow::switchToConnectionPage);
    viewMenu->addAction(connAct);

    QAction *traceAct = new QAction("追溯页(&T)", this);
    connect(traceAct, &QAction::triggered, this, &MainWindow::switchToTracePage);
    viewMenu->addAction(traceAct);

    QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");

    QAction *aboutAct = new QAction("关于(&A)", this);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::showAbout);
    helpMenu->addAction(aboutAct);
}

void MainWindow::setupStatusBar()
{
    statusBar()->setSizeGripEnabled(true);

    msgLabel_ = new QLabel("就绪");
    statusBar()->addWidget(msgLabel_, 1);

    statusBar()->addPermanentWidget(new QLabel("│"));

    connLabel_ = new QLabel("● 未连接");
    statusBar()->addPermanentWidget(connLabel_);

    statusBar()->addPermanentWidget(new QLabel("│"));

    batteryLabel_ = new QLabel("电量：--");
    statusBar()->addPermanentWidget(batteryLabel_);

    statusBar()->addPermanentWidget(new QLabel("│"));

    QLabel *verLabel = new QLabel(APP_VERSION);
    statusBar()->addPermanentWidget(verLabel);

    statusBar()->addPermanentWidget(new QLabel("│"));

    timeLabel_ = new QLabel;
    statusBar()->addPermanentWidget(timeLabel_);

    clockTimer_ = new QTimer(this);
    connect(clockTimer_, &QTimer::timeout, this, &MainWindow::updateClock);
    clockTimer_->start(1000);
    updateClock();
}
