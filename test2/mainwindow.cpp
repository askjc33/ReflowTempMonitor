#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    setWindowTitle("PCBA 多板过炉追溯系统");
    resize(1400, 900);

    stacked_ = new QStackedWidget(this);

    connPage_ = new ConnectionPage;
    tracePage_ = new TraceabilityPage;

    stacked_->addWidget(connPage_);
    stacked_->addWidget(tracePage_);
    setCentralWidget(stacked_);

    setupMenuBar();
    setupStatusBar();
}

void MainWindow::setupMenuBar()
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
    connect(aboutAct, &QAction::triggered, this,  {
        QMessageBox::about(
            this,
            "关于",
            QString("<b>PCBA 多板过炉追溯系统</b><br>"
                    "版本：%1<br><br>"
                    "协议：TEMP/POS 同步帧<br>"
                    "支持多板同时在炉、逐板温度曲线追溯。")
                .arg(APP_VERSION)
        );
    });
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

void MainWindow::updateClock()
{
    timeLabel_->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
}

void MainWindow::switchToConnectionPage()
{
    stacked_->setCurrentWidget(connPage_);
}

void MainWindow::switchToTracePage()
{
    stacked_->setCurrentWidget(tracePage_);
}

void MainWindow::updateStatusMessage(const QString &text)
{
    msgLabel_->setText(text);
}

void MainWindow::updateConnectionStatus(const QString &text, bool connected)
{
    connLabel_->setText(QString("● %1").arg(text));
    connLabel_->setStyleSheet(connected ? "color:green;font-weight:bold;" : "color:black;");
}

void MainWindow::updateBatteryLevel(int v)
{
    if (v < 0) {
        batteryLabel_->setText("电量：--");
        batteryLabel_->setStyleSheet("");
    } else {
        batteryLabel_->setText(QString("电量：%1%").arg(v));
        batteryLabel_->setStyleSheet(v <= 20 ? "color:red;" : "");
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 关闭窗口时自动断开串口（由controller处理）
    QMainWindow::closeEvent(event);
}
