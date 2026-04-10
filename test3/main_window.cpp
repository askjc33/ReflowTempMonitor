// 引入主窗口头文件，包含MainWindow类的声明
#include "main_window.h"

// Qt核心控件头文件引入
#include <QStackedWidget>   // 栈式窗口容器（用于切换不同页面）
#include <QMenuBar>         // 菜单栏
#include <QMenu>            // 菜单
#include <QAction>          // 菜单项
#include <QStatusBar>       // 状态栏
#include <QLabel>           // 文本标签
#include <QTimer>           // 定时器（用于时钟更新）
#include <QDateTime>        // 日期时间处理
#include <QMessageBox>      // 消息弹窗
#include <QCloseEvent>      // 关闭事件
#include <QKeySequence>     // 快捷键序列

// 主窗口构造函数：初始化父类，初始化成员变量
// parent：父窗口指针（Qt父子机制用于内存管理）
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),  // 调用父类QMainWindow的构造函数
      // 初始化所有成员变量为NULL/0（Qt控件指针初始化为0，避免野指针）
      stacked_(0),          // 栈式窗口容器指针
      connPage_(0),         // 连接页面指针
      tracePage_(0),        // 追溯页面指针
      msgLabel_(0),         // 状态栏消息标签指针
      connLabel_(0),        // 连接状态标签指针
      batteryLabel_(0),     // 电量标签指针
      timeLabel_(0),        // 时间标签指针
      clockTimer_(0)        // 时钟定时器指针
{
    // 1. 初始化UI布局和控件
    initUi();
    // 2. 构建菜单栏
    setupMenu();
    // 3. 构建状态栏
    setupStatusBar();
    // 4. 信号与槽连接（核心：实现控件间的交互逻辑）
    wireSignals();
}

// 重写关闭事件：程序关闭前执行清理操作
// event：关闭事件对象（包含事件相关信息）
void MainWindow::closeEvent(QCloseEvent *event)
{
    // 关闭串口连接（释放串口资源，避免占用）
    serial_.disconnectPort();
    // 调用父类的关闭事件处理（保证默认关闭逻辑执行）
    QMainWindow::closeEvent(event);
}

// 切换到「连接页面」的槽函数
void MainWindow::switchToConnectionPage()
{
    // 设置栈式窗口当前显示的控件为连接页面
    stacked_->setCurrentWidget(connPage_);
}

// 切换到「追溯页面」的槽函数
void MainWindow::switchToTracePage()
{
    // 设置栈式窗口当前显示的控件为追溯页面
    stacked_->setCurrentWidget(tracePage_);
}

// 更新状态栏时钟的槽函数（每秒触发一次）
void MainWindow::updateClock()
{
    // 获取当前系统时间，格式化为「年-月-日 时:分:秒」并设置到时间标签
    timeLabel_->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
}

// 设置状态栏提示消息的槽函数
// text：要显示的消息文本
void MainWindow::setStatusMessage(const QString &text)
{
    // 更新消息标签的文本内容
    msgLabel_->setText(text);
}

// 设置连接状态的槽函数（显示状态文本+颜色标识）
// text：状态文本；connected：是否已连接（true=已连接，false=未连接）
void MainWindow::setConnectionStatus(const QString &text, bool connected)
{
    // 状态文本前缀加「●」标识，拼接成最终显示文本（如：● 已连接）
    connLabel_->setText(QString("● %1").arg(text));
    // 设置样式表：已连接=绿色粗体，未连接=黑色默认样式
    connLabel_->setStyleSheet(connected ? "color:green;font-weight:bold;" : "color:black;");
}

// 设置电量显示的槽函数
// v：电量百分比（-1表示未知/无法获取）
void MainWindow::setBatteryLevel(int v)
{
    if (v < 0) {
        // 电量值无效时，显示「电量：--」，清空样式
        batteryLabel_->setText(QString::fromUtf8("电量：--"));
        batteryLabel_->setStyleSheet("");
    } else {
        // 电量有效时，显示「电量：XX%」
        batteryLabel_->setText(QString::fromUtf8("电量：%1%").arg(v));
        // 电量≤20%时标红，否则使用默认样式
        batteryLabel_->setStyleSheet(v <= 20 ? "color:red;" : "");
    }
}

// 显示「关于」弹窗的槽函数
void MainWindow::showAbout()
{
    // 弹出关于对话框，包含软件名称、版本、协议、功能说明
    QMessageBox::about(
        this,                                    // 父窗口（弹窗居中显示在主窗口）
        QString::fromUtf8("关于"),               // 弹窗标题
        // 弹窗内容：HTML格式，支持加粗、换行
        QString::fromUtf8("<b>PCBA 多板过炉追溯系统</b><br>"
                          "版本：%1<br><br>"
                          "协议：TEMP/POS 同步帧<br>"
                          "支持多板同时在炉、逐板温度曲线追溯。<br>"
                          "本版本已修复长时间运行后界面未响应问题。").arg(APP_VERSION)
    );
}

// 初始化UI的核心函数：创建窗口布局、核心控件
void MainWindow::initUi()
{
    // 设置主窗口标题
    setWindowTitle(QString::fromUtf8("PCBA 多板过炉追溯系统"));
    // 设置主窗口初始大小（宽1400px，高900px）
    resize(1400, 900);

    // 创建栈式窗口容器（用于切换「连接页」和「追溯页」），父对象设为主窗口（自动管理内存）
    stacked_ = new QStackedWidget(this);

    // 创建连接页面：传入串口对象指针（连接页需操作串口）
    connPage_ = new ConnectionPage(&serial_);
    // 创建追溯页面：传入串口对象和追溯管理器对象（追溯页需串口数据+数据管理）
    tracePage_ = new TraceabilityPage(&serial_, &traceManager_);

    // 将连接页、追溯页添加到栈式容器中（添加顺序决定默认显示顺序）
    stacked_->addWidget(tracePage_);
    stacked_->addWidget(connPage_);
    // 将栈式容器设为主窗口的中心控件（占据主窗口核心显示区域）
    setCentralWidget(stacked_);
}

// 信号与槽连接函数：实现各模块间的交互（Qt核心通信机制）
void MainWindow::wireSignals()
{
    // ========== 连接页 -> 主窗口 信号连接 ==========
    // 连接页发送「请求切换到追溯页」信号 → 主窗口执行切换函数
    connect(connPage_, SIGNAL(requestSwitchToTracePage()), this, SLOT(switchToTracePage()));
    // 连接页发送「设置状态栏消息」信号 → 主窗口执行消息设置函数
    connect(connPage_, SIGNAL(requestSetStatusMessage(QString)), this, SLOT(setStatusMessage(QString)));
    // 连接页发送「设置连接状态」信号 → 主窗口执行连接状态设置函数
    connect(connPage_, SIGNAL(requestSetConnectionStatus(QString,bool)), this, SLOT(setConnectionStatus(QString,bool)));

    // ========== 追溯页 -> 主窗口 信号连接 ==========
    // 追溯页发送「断开连接并返回连接页」信号 → 主窗口切换到连接页
    connect(tracePage_, SIGNAL(requestDisconnectAndBack()), this, SLOT(switchToConnectionPage()));
    // 追溯页发送「设置状态栏消息」信号 → 主窗口执行消息设置函数
    connect(tracePage_, SIGNAL(requestSetStatusMessage(QString)), this, SLOT(setStatusMessage(QString)));
    // 追溯页发送「设置连接状态」信号 → 主窗口执行连接状态设置函数
    connect(tracePage_, SIGNAL(requestSetConnectionStatus(QString,bool)), this, SLOT(setConnectionStatus(QString,bool)));
    // 追溯页发送「设置电量」信号 → 主窗口执行电量设置函数
    connect(tracePage_, SIGNAL(requestSetBatteryLevel(int)), this, SLOT(setBatteryLevel(int)));

    // ========== 串口 -> 追溯管理器 核心数据通道 ==========
    // 串口接收到「同步帧数据」信号 → 追溯管理器处理该帧数据（温度/位置同步）
    connect(&serial_, SIGNAL(synchronizedFrameReceived(QVector<double>,QStringList)),
            &traceManager_, SLOT(onSynchronizedFrame(QVector<double>,QStringList)));
}

// 构建菜单栏的函数
void MainWindow::setupMenu()
{
    // 1. 创建「文件」菜单（&F表示快捷键Alt+F）
    QMenu *fileMenu = menuBar()->addMenu(QString::fromUtf8("文件(&F)"));

    // 创建「退出」菜单项（&Q表示快捷键Alt+Q）
    QAction *quitAct = new QAction(QString::fromUtf8("退出(&Q)"), this);
    // 设置退出快捷键为Ctrl+Q
    quitAct->setShortcut(QKeySequence("Ctrl+Q"));
    // 点击退出菜单项 → 触发主窗口关闭函数
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    // 将退出菜单项添加到文件菜单
    fileMenu->addAction(quitAct);

    // 2. 创建「视图」菜单（&V表示快捷键Alt+V）
    QMenu *viewMenu = menuBar()->addMenu(QString::fromUtf8("视图(&V)"));

    // 创建「连接页」菜单项（&C表示快捷键Alt+C）
    QAction *connAct = new QAction(QString::fromUtf8("连接页(&C)"), this);
    // 点击连接页菜单项 → 切换到连接页
    connect(connAct, SIGNAL(triggered()), this, SLOT(switchToConnectionPage()));
    // 添加到视图菜单
    viewMenu->addAction(connAct);

    // 创建「追溯页」菜单项（&T表示快捷键Alt+T）
    QAction *traceAct = new QAction(QString::fromUtf8("追溯页(&T)"), this);
    // 点击追溯页菜单项 → 切换到追溯页
    connect(traceAct, SIGNAL(triggered()), this, SLOT(switchToTracePage()));
    // 添加到视图菜单
    viewMenu->addAction(traceAct);

    // 3. 创建「帮助」菜单（&H表示快捷键Alt+H）
    QMenu *helpMenu = menuBar()->addMenu(QString::fromUtf8("帮助(&H)"));

    // 创建「关于」菜单项（&A表示快捷键Alt+A）
    QAction *aboutAct = new QAction(QString::fromUtf8("关于(&A)"), this);
    // 点击关于菜单项 → 显示关于弹窗
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(showAbout()));
    // 添加到帮助菜单
    helpMenu->addAction(aboutAct);
}

// 构建状态栏的函数
void MainWindow::setupStatusBar()
{
    // 启用状态栏右下角的大小调整手柄（用户可拖动调整窗口大小）
    statusBar()->setSizeGripEnabled(true);

    // 1. 创建状态栏消息标签（默认显示「就绪」）
    msgLabel_ = new QLabel(QString::fromUtf8("就绪"));
    // 添加到状态栏，拉伸因子=1（占据尽可能多的空间）
    statusBar()->addWidget(msgLabel_, 1);

    // 添加分隔符（竖线），区分不同状态项
    statusBar()->addPermanentWidget(new QLabel(QString::fromUtf8("│")));

    // 2. 创建连接状态标签（默认显示「● 未连接」）
    connLabel_ = new QLabel(QString::fromUtf8("● 未连接"));
    // 添加到状态栏永久区域（右侧固定显示）
    statusBar()->addPermanentWidget(connLabel_);

    // 添加分隔符
    statusBar()->addPermanentWidget(new QLabel(QString::fromUtf8("│")));

    // 3. 创建电量标签（默认显示「电量：--」）
    batteryLabel_ = new QLabel(QString::fromUtf8("电量：--"));
    statusBar()->addPermanentWidget(batteryLabel_);

    // 添加分隔符
    statusBar()->addPermanentWidget(new QLabel(QString::fromUtf8("│")));

    // 4. 创建版本号标签（显示APP_VERSION宏定义的版本号）
    QLabel *verLabel = new QLabel(APP_VERSION);
    statusBar()->addPermanentWidget(verLabel);

    // 添加分隔符
    statusBar()->addPermanentWidget(new QLabel(QString::fromUtf8("│")));

    // 5. 创建时间标签（动态更新）
    timeLabel_ = new QLabel;
    statusBar()->addPermanentWidget(timeLabel_);

    // 6. 创建时钟定时器（每秒触发一次更新）
    clockTimer_ = new QTimer(this);
    // 定时器超时信号 → 触发时钟更新函数
    connect(clockTimer_, SIGNAL(timeout()), this, SLOT(updateClock()));
    // 启动定时器，间隔1000ms（1秒）
    clockTimer_->start(1000);
    // 立即更新一次时钟（避免初始为空）
    updateClock();
}
