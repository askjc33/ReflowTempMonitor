// 引入当前类的头文件，包含类声明和所需的宏/结构体定义
#include "connection_page.h"

// 引入Qt布局相关头文件：垂直布局、水平布局
#include <QVBoxLayout>
#include <QHBoxLayout>
// 引入Qt标签控件头文件：用于显示文本
#include <QLabel>
// 引入Qt字体类头文件：用于设置文本样式
#include <QFont>
// 引入Qt框架控件头文件：用于绘制分隔线
#include <QFrame>
// 引入Qt分组框控件头文件：用于分组UI元素
#include <QGroupBox>
// 引入Qt列表控件头文件：用于显示串口设备列表
#include <QListWidget>
#include <QListWidgetItem>
// 引入Qt按钮控件头文件：用于扫描/连接按钮
#include <QPushButton>
// 引入Qt消息框头文件：用于弹出连接失败提示
#include <QMessageBox>
// 引入Qt串口类头文件：用于串口波特率等常量定义
#include <QSerialPort>

// 构造函数：初始化串口连接页面
// 参数1：serial - 串口管理类指针（核心业务逻辑封装）
// 参数2：parent - 父窗口指针（Qt控件父子关系，用于内存管理）
ConnectionPage::ConnectionPage(SerialManager *serial, QWidget *parent)
    : QWidget(parent),          // 初始化父类QWidget，指定父窗口
      serial_(serial),         // 初始化串口管理器成员变量
      list_(0),                // 初始化串口列表控件指针为NULL
      infoLabel_(0),           // 初始化设备信息标签指针为NULL
      scanBtn_(0),             // 初始化扫描按钮指针为NULL
      connectBtn_(0)           // 初始化连接按钮指针为NULL
{
    // 信号槽连接1：串口管理器扫描完成信号 -> 本页面扫描完成槽函数
    // 传递参数：扫描到的串口设备列表（SerialManager::PortItem类型）
    connect(serial_, SIGNAL(scanFinished(QList<SerialManager::PortItem>)),
            this, SLOT(onScanFinished(QList<SerialManager::PortItem>)));

    // 信号槽连接2：串口管理器连接成功信号 -> 本页面连接成功槽函数
    // 传递参数：成功连接的串口名称
    connect(serial_, SIGNAL(connectSuccess(QString)),
            this, SLOT(onConnectSuccess(QString)));

    // 信号槽连接3：串口管理器连接失败信号 -> 本页面连接失败槽函数
    // 传递参数：失败原因描述
    connect(serial_, SIGNAL(connectFailed(QString)),
            this, SLOT(onConnectFailed(QString)));

    // 初始化UI界面（布局、控件、样式）
    initUi();
}

// 槽函数/成员函数：执行串口设备扫描逻辑
void ConnectionPage::scan()
{
    list_->clear();               // 清空串口列表控件的所有项
    ports_.clear();               // 清空存储串口信息的成员变量列表
    selectedPort_.clear();        // 清空已选中的串口名称
    connectBtn_->setEnabled(false); // 禁用连接按钮（未选中串口时不可点击）
    infoLabel_->setText(QString::fromUtf8("请先扫描串口设备")); // 更新设备信息标签文本

    // 发送信号：请求主窗口更新状态栏消息（提示正在扫描）
    emit requestSetStatusMessage(QString::fromUtf8("正在扫描串口设备..."));
    serial_->scanPorts();         // 调用串口管理器的扫描串口方法
}

// 槽函数：接收串口管理器扫描完成的信号，处理扫描结果
// 参数：ports - 扫描到的串口设备列表（包含端口名、描述、厂商等信息）
void ConnectionPage::onScanFinished(const QList<SerialManager::PortItem> &ports)
{
    ports_ = ports; // 将扫描结果保存到成员变量，供后续使用

    // 扫描结果为空：更新状态栏提示，直接返回
    if (ports.isEmpty()) {
        emit requestSetStatusMessage(QString::fromUtf8("未发现串口设备"));
        return;
    }

    // 扫描到设备：更新状态栏提示（显示设备数量）
    emit requestSetStatusMessage(QString::fromUtf8("扫描完成，共发现 %1 个串口设备").arg(ports.size()));

    // 遍历所有扫描到的串口设备，逐个添加到列表控件
    for (int i = 0; i < ports.size(); ++i) {
        const SerialManager::PortItem &p = ports.at(i); // 获取当前串口项（只读引用）
        // 处理设备描述：为空则显示“未知设备”，否则显示原描述
        QString label = p.description.isEmpty() ? QString::fromUtf8("未知设备") : p.description;
        // 创建列表项：显示格式为“设备描述 (端口名)”，例如“USB-SERIAL CH340 (COM3)”
        QListWidgetItem *item = new QListWidgetItem(QString("%1 (%2)").arg(label, p.portName));
        // 为列表项绑定自定义数据（Qt::UserRole）：存储原始端口名（用于后续连接）
        item->setData(Qt::UserRole, p.portName);
        list_->addItem(item); // 将列表项添加到串口列表控件
    }
}

// 槽函数：处理串口列表项点击事件
// 参数：item - 被点击的列表项指针
void ConnectionPage::onItemClicked(QListWidgetItem *item)
{
    // 从列表项的自定义数据中获取选中的端口名，保存到成员变量
    selectedPort_ = item->data(Qt::UserRole).toString();
    connectBtn_->setEnabled(true); // 启用连接按钮（已选中串口）

    // 遍历已扫描的串口列表，匹配选中的端口名，显示详细设备信息
    for (int i = 0; i < ports_.size(); ++i) {
        const SerialManager::PortItem &p = ports_.at(i);
        if (p.portName == selectedPort_) { // 端口名匹配
            // 更新设备信息标签：显示端口名、描述、厂商、系统位置（为空则显示“未知”）
            infoLabel_->setText(
                QString::fromUtf8("端口：%1\n描述：%2\n厂商：%3\n位置：%4")
                    .arg(p.portName)
                    .arg(p.description.isEmpty() ? QString::fromUtf8("未知") : p.description)
                    .arg(p.manufacturer.isEmpty() ? QString::fromUtf8("未知") : p.manufacturer)
                    .arg(p.systemLocation.isEmpty() ? QString::fromUtf8("未知") : p.systemLocation)
            );
            break; // 匹配到后退出循环，避免无效遍历
        }
    }
}

// 槽函数：执行串口连接逻辑
void ConnectionPage::doConnect()
{
    if (selectedPort_.isEmpty()) { // 未选中串口，直接返回
        return;
    }
    // 发送信号：请求主窗口更新状态栏消息（提示正在连接）
    emit requestSetStatusMessage(QString::fromUtf8("正在连接串口..."));
    // 调用串口管理器的连接方法：指定端口名 + 波特率115200
    serial_->connectPort(selectedPort_, QSerialPort::Baud115200);
}

// 槽函数：接收串口管理器连接成功的信号，处理连接成功逻辑
// 参数：portName - 成功连接的串口名称
void ConnectionPage::onConnectSuccess(const QString &portName)
{
    // 发送信号：更新状态栏消息（提示连接成功）
    emit requestSetStatusMessage(QString::fromUtf8("串口已连接"));
    // 发送信号：更新全局连接状态（显示连接的端口名 + 连接成功标记）
    emit requestSetConnectionStatus(QString::fromUtf8("已连接 %1").arg(portName), true);
    // 发送信号：请求主窗口切换到“追踪页面”（核心业务页面）
    emit requestSwitchToTracePage();
}

// 槽函数：接收串口管理器连接失败的信号，处理连接失败逻辑
// 参数：msg - 连接失败的原因描述
void ConnectionPage::onConnectFailed(const QString &msg)
{
    // 弹出警告对话框：标题“连接失败”，内容为失败原因
    QMessageBox::warning(this, QString::fromUtf8("连接失败"), msg);
    // 发送信号：更新状态栏消息（提示连接失败）
    emit requestSetStatusMessage(QString::fromUtf8("连接失败"));
}

// 成员函数：初始化UI界面（布局、控件、样式、信号槽）
void ConnectionPage::initUi()
{
    // 创建页面主布局（垂直布局），并设置为当前窗口的布局
    QVBoxLayout *layout = new QVBoxLayout(this);

    // ========== 标题栏 ==========
    QLabel *title = new QLabel(QString::fromUtf8("串口连接")); // 创建标题标签
    QFont f;                     // 创建字体对象
    f.setPointSize(12);          // 设置字体大小为12号
    f.setBold(true);             // 设置粗体
    title->setFont(f);           // 为标题标签应用字体
    layout->addWidget(title);    // 将标题添加到主布局

    // ========== 分隔线 ==========
    QFrame *sep = new QFrame;    // 创建框架控件
    sep->setFrameShape(QFrame::HLine); // 设置为水平分隔线
    layout->addWidget(sep);      // 将分隔线添加到主布局

    // ========== 可用设备分组框 ==========
    QGroupBox *group = new QGroupBox(QString::fromUtf8("可用设备")); // 创建分组框
    QVBoxLayout *gLayout = new QVBoxLayout(group); // 创建分组框内部布局（垂直）

    list_ = new QListWidget;     // 创建串口列表控件
    list_->setMinimumHeight(300); // 设置列表最小高度为300像素（保证显示空间）
    // 信号槽连接：列表项点击事件 -> 本页面的onItemClicked槽函数
    connect(list_, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(onItemClicked(QListWidgetItem*)));
    gLayout->addWidget(list_);   // 将列表控件添加到分组框布局

    layout->addWidget(group);    // 将分组框添加到主布局

    // ========== 设备信息分组框 ==========
    QGroupBox *infoGroup = new QGroupBox(QString::fromUtf8("设备信息")); // 创建设备信息分组框
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup); // 创建分组框内部布局（垂直）

    infoLabel_ = new QLabel(QString::fromUtf8("请先扫描串口设备")); // 创建设备信息标签
    infoLabel_->setWordWrap(true); // 开启自动换行（防止文本超出控件宽度）
    infoLayout->addWidget(infoLabel_); // 将标签添加到分组框布局

    layout->addWidget(infoGroup); // 将设备信息分组框添加到主布局

    // ========== 按钮布局 ==========
    QHBoxLayout *btnLayout = new QHBoxLayout; // 创建按钮水平布局

    scanBtn_ = new QPushButton(QString::fromUtf8("扫描设备")); // 创建扫描按钮
    // 信号槽连接：扫描按钮点击事件 -> 本页面的scan槽函数
    connect(scanBtn_, SIGNAL(clicked()), this, SLOT(scan()));
    btnLayout->addWidget(scanBtn_); // 将扫描按钮添加到按钮布局

    connectBtn_ = new QPushButton(QString::fromUtf8("连接")); // 创建连接按钮
    connectBtn_->setEnabled(false); // 初始状态禁用（未选中串口）
    // 信号槽连接：连接按钮点击事件 -> 本页面的doConnect槽函数
    connect(connectBtn_, SIGNAL(clicked()), this, SLOT(doConnect()));
    btnLayout->addWidget(connectBtn_); // 将连接按钮添加到按钮布局

    layout->addLayout(btnLayout); // 将按钮布局添加到主布局
}
