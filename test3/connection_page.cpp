#include "connection_page.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QFrame>
#include <QGroupBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QMessageBox>
#include <QSerialPort>

ConnectionPage::ConnectionPage(SerialManager *serial, QWidget *parent)
    : QWidget(parent),
      serial_(serial),
      list_(0),
      infoLabel_(0),
      scanBtn_(0),
      connectBtn_(0)
{
    connect(serial_, SIGNAL(scanFinished(QList<SerialManager::PortItem>)),
            this, SLOT(onScanFinished(QList<SerialManager::PortItem>)));
    connect(serial_, SIGNAL(connectSuccess(QString)),
            this, SLOT(onConnectSuccess(QString)));
    connect(serial_, SIGNAL(connectFailed(QString)),
            this, SLOT(onConnectFailed(QString)));

    initUi();
}

void ConnectionPage::scan()
{
    list_->clear();
    ports_.clear();
    selectedPort_.clear();
    connectBtn_->setEnabled(false);
    infoLabel_->setText(QString::fromUtf8("请先扫描串口设备"));
    emit requestSetStatusMessage(QString::fromUtf8("正在扫描串口设备..."));
    serial_->scanPorts();
}

void ConnectionPage::onScanFinished(const QList<SerialManager::PortItem> &ports)
{
    ports_ = ports;

    if (ports.isEmpty()) {
        emit requestSetStatusMessage(QString::fromUtf8("未发现串口设备"));
        return;
    }

    emit requestSetStatusMessage(QString::fromUtf8("扫描完成，共发现 %1 个串口设备").arg(ports.size()));

    for (int i = 0; i < ports.size(); ++i) {
        const SerialManager::PortItem &p = ports.at(i);
        QString label = p.description.isEmpty() ? QString::fromUtf8("未知设备") : p.description;
        QListWidgetItem *item = new QListWidgetItem(QString("%1 (%2)").arg(label, p.portName));
        item->setData(Qt::UserRole, p.portName);
        list_->addItem(item);
    }
}

void ConnectionPage::onItemClicked(QListWidgetItem *item)
{
    selectedPort_ = item->data(Qt::UserRole).toString();
    connectBtn_->setEnabled(true);

    for (int i = 0; i < ports_.size(); ++i) {
        const SerialManager::PortItem &p = ports_.at(i);
        if (p.portName == selectedPort_) {
            infoLabel_->setText(
                QString::fromUtf8("端口：%1\n描述：%2\n厂商：%3\n位置：%4")
                    .arg(p.portName)
                    .arg(p.description.isEmpty() ? QString::fromUtf8("未知") : p.description)
                    .arg(p.manufacturer.isEmpty() ? QString::fromUtf8("未知") : p.manufacturer)
                    .arg(p.systemLocation.isEmpty() ? QString::fromUtf8("未知") : p.systemLocation)
            );
            break;
        }
    }
}

void ConnectionPage::doConnect()
{
    if (selectedPort_.isEmpty()) {
        return;
    }
    emit requestSetStatusMessage(QString::fromUtf8("正在连接串口..."));
    serial_->connectPort(selectedPort_, QSerialPort::Baud115200);
}

void ConnectionPage::onConnectSuccess(const QString &portName)
{
    emit requestSetStatusMessage(QString::fromUtf8("串口已连接"));
    emit requestSetConnectionStatus(QString::fromUtf8("已连接 %1").arg(portName), true);
    emit requestSwitchToTracePage();
}

void ConnectionPage::onConnectFailed(const QString &msg)
{
    QMessageBox::warning(this, QString::fromUtf8("连接失败"), msg);
    emit requestSetStatusMessage(QString::fromUtf8("连接失败"));
}

void ConnectionPage::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel(QString::fromUtf8("串口连接"));
    QFont f;
    f.setPointSize(12);
    f.setBold(true);
    title->setFont(f);
    layout->addWidget(title);

    QFrame *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    layout->addWidget(sep);

    QGroupBox *group = new QGroupBox(QString::fromUtf8("可用设备"));
    QVBoxLayout *gLayout = new QVBoxLayout(group);

    list_ = new QListWidget;
    list_->setMinimumHeight(300);
    connect(list_, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(onItemClicked(QListWidgetItem*)));
    gLayout->addWidget(list_);

    layout->addWidget(group);

    QGroupBox *infoGroup = new QGroupBox(QString::fromUtf8("设备信息"));
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);

    infoLabel_ = new QLabel(QString::fromUtf8("请先扫描串口设备"));
    infoLabel_->setWordWrap(true);
    infoLayout->addWidget(infoLabel_);

    layout->addWidget(infoGroup);

    QHBoxLayout *btnLayout = new QHBoxLayout;

    scanBtn_ = new QPushButton(QString::fromUtf8("扫描设备"));
    connect(scanBtn_, SIGNAL(clicked()), this, SLOT(scan()));
    btnLayout->addWidget(scanBtn_);

    connectBtn_ = new QPushButton(QString::fromUtf8("连接"));
    connectBtn_->setEnabled(false);
    connect(connectBtn_, SIGNAL(clicked()), this, SLOT(doConnect()));
    btnLayout->addWidget(connectBtn_);

    layout->addLayout(btnLayout);
}
