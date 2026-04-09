#include "mainwindow.h"
#include "serialmanager.h"
#include "connectionpage.h"
#include "datamonitorpage.h"
#include "presetpage.h"

#include <QStackedWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QAction>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QTimer>
#include <QMessageBox>

static const QString VERSION = "v1.0.0-cpp-serial";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_serialManager(new SerialManager(this))
{
    resize(1200, 820);
    setWindowTitle("回流焊温度监控软件 - 串口版");

    m_stackedWidget = new QStackedWidget(this);
    m_connectionPage = new ConnectionPage(m_serialManager, this);
    m_monitorPage = new DataMonitorPage(m_serialManager, this);
    m_presetPage = new PresetPage(this);
    m_monitorPage->setPresetPage(m_presetPage);

    m_stackedWidget->addWidget(m_connectionPage);
    m_stackedWidget->addWidget(m_monitorPage);
    m_stackedWidget->addWidget(m_presetPage);
    setCentralWidget(m_stackedWidget);

    setupMenuBar();
    setupStatusBar();

    connect(m_connectionPage, &ConnectionPage::requestEnterMonitor, this, &MainWindow::switchToMonitorPage);
        connect(m_monitorPage, &DataMonitorPage::requestBackToConnection, this, &MainWindow::switchToConnectionPage);
        connect(m_monitorPage, &DataMonitorPage::requestGoPreset, this, &MainWindow::switchToPresetPage);
        connect(m_serialManager, &SerialManager::connected, this, [this]() {
            m_connLabel->setText(QString("● 已连接 %1").arg(m_serialManager->currentPortName()));
            m_connLabel->setStyleSheet("color: green; font-weight: bold;");
        });
        connect(m_serialManager, &SerialManager::disconnected, this, [this]() {
            m_connLabel->setText("● 未连接");
            m_connLabel->setStyleSheet("");
            m_batteryLabel->setText("电量：--");
        });
        connect(m_serialManager, &SerialManager::batteryReceived, this, [this](int v) {
            m_batteryLabel->setText(QString("电量：%1%").arg(v));
        });
    }

    void MainWindow::switchToConnectionPage()
    {
        m_stackedWidget->setCurrentWidget(m_connectionPage);
        m_msgLabel->setText("已切换到连接页面");
    }

    void MainWindow::switchToMonitorPage()
    {
        m_monitorPage->setReady();
        m_stackedWidget->setCurrentWidget(m_monitorPage);
        m_msgLabel->setText("已切换到监控页面");
    }

    void MainWindow::switchToPresetPage()
    {
        m_stackedWidget->setCurrentWidget(m_presetPage);
        m_msgLabel->setText("已切换到预设页面");
    }

    void MainWindow::setupMenuBar()
    {
        QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
        QAction *exportAct = new QAction("导出数据(&E)...", this);
        exportAct->setShortcut(QKeySequence("Ctrl+S"));
        connect(exportAct, &QAction::triggered, this, &MainWindow::exportData);
        fileMenu->addAction(exportAct);

        fileMenu->addSeparator();

        QAction *quitAct = new QAction("退出(&Q)", this);
        quitAct->setShortcut(QKeySequence("Ctrl+Q"));
        connect(quitAct, &QAction::triggered, this, &MainWindow::close);
        fileMenu->addAction(quitAct);

        QMenu *viewMenu = menuBar()->addMenu("视图(&V)");
        QAction *connAct = new QAction("连接页面(&C)", this);
        QAction *monitorAct = new QAction("监控页面(&M)", this);
        QAction *presetAct = new QAction("预设页面(&P)", this);
        connect(connAct, &QAction::triggered, this, &MainWindow::switchToConnectionPage);
        connect(monitorAct, &QAction::triggered, this, &MainWindow::switchToMonitorPage);
        connect(presetAct, &QAction::triggered, this, &MainWindow::switchToPresetPage);
        viewMenu->addAction(connAct);
        viewMenu->addAction(monitorAct);
        viewMenu->addAction(presetAct);

        QMenu *helpMenu = menuBar()->addMenu("关于(&H)");
        QAction *aboutAct = new QAction("关于本软件(&A)", this);
        connect(aboutAct, &QAction::triggered, this, &MainWindow::showAbout);
        helpMenu->addAction(aboutAct);
    }

    void MainWindow::setupStatusBar()
    {
        m_msgLabel = new QLabel("就绪");
        m_connLabel = new QLabel("● 未连接");
        m_batteryLabel = new QLabel("电量：--");
        m_timeLabel = new QLabel;

        statusBar()->addWidget(m_msgLabel, 1);
        statusBar()->addPermanentWidget(new QLabel("│"));
        statusBar()->addPermanentWidget(m_connLabel);
        statusBar()->addPermanentWidget(new QLabel("│"));
        statusBar()->addPermanentWidget(m_batteryLabel);
        statusBar()->addPermanentWidget(new QLabel("│"));
        statusBar()->addPermanentWidget(new QLabel(VERSION));
        statusBar()->addPermanentWidget(new QLabel("│"));
        statusBar()->addPermanentWidget(m_timeLabel);

        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWindow::updateClock);
        timer->start(1000);
        updateClock();
    }

    void MainWindow::exportData()
    {
        const auto t = m_monitorPage->timeData();
        const auto ch = m_monitorPage->channelData();
        if (t.isEmpty()) {
            QMessageBox::information(this, "导出数据", "当前没有可导出的数据。");
            return;
        }

        QString path = QFileDialog::getSaveFileName(this, "导出数据", QString(), "CSV 文件 (*.csv)");
        if (path.isEmpty()) return;

        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "导出失败", "无法写入文件。");
            return;
        }

        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << "时间(s),通道1(°C),通道2(°C),通道3(°C),通道4(°C),通道5(°C)\n";
        for (int i = 0; i < t.size(); ++i) {
            out << QString::number(t[i], 'f', 3);
            for (int c = 0; c < ch.size(); ++c) {
                out << ",";
                if (i < ch[c].size()) out << QString::number(ch[c][i], 'f', 2);
            }
            out << "\n";
        }

        file.close();
        QMessageBox::information(this, "导出成功", QString("数据已导出到：\n%1").arg(path));
    }

    void MainWindow::showAbout()
    {
        QMessageBox::about(this, "关于本软件",
                           QString("<b>回流焊温度监控软件 - 串口版</b><br>版本：%1<br><br>"
                                   "支持串口连接、5通道温度监控、预设曲线加载、全炉12温区阈值显示。")
                           .arg(VERSION));
    }

    void MainWindow::updateClock()
    {
        m_timeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd  HH:mm:ss"));
    }
