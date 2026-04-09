#include "connectionpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QFrame>
#include <QMessageBox>

ConnectionPage::ConnectionPage(QWidget *parent) : QWidget(parent)
{
    initUi();
}

void ConnectionPage::initUi()
{
    auto *layout = new QVBoxLayout(this);

    auto *title = new QLabel("串口连接");
    QFont f;
    f.setPointSize(12);
    f.setBold(true);
    title->setFont(f);
    layout->addWidget(title);

    auto *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    layout->addWidget(sep);

    auto *group = new QGroupBox("可用设备");
    auto *gLayout = new QVBoxLayout(group);

    list_ = new QListWidget;
    list_->setMinimumHeight(300);
    connect(list_, &QListWidget::itemClicked, this, &ConnectionPage::onItemClicked);
    gLayout->addWidget(list_);
    layout->addWidget(group);

    auto *infoGroup = new QGroupBox("设备信息");
    auto *infoLayout = new QVBoxLayout(infoGroup);
    infoLabel_ = new QLabel("请先扫描串口设备");
    infoLabel_->setWordWrap(true);
    infoLayout->addWidget(infoLabel_);
    layout->addWidget(infoGroup);

    auto *btnLayout = new QHBoxLayout;
    scanBtn_ = new QPushButton("扫描设备");
    connect(scanBtn_, &QPushButton::clicked, this, &ConnectionPage::onScanClicked);
    btnLayout->addWidget(scanBtn_);

    connectBtn_ = new QPushButton("连接");
    connectBtn_->setEnabled(false);
    connect(connectBtn_, &QPushButton::clicked, this, &ConnectionPage::onConnectClicked);
    btnLayout->addWidget(connectBtn_);

    layout->addLayout(btnLayout);
}

void ConnectionPage::setPortList(const QList<PortItem> &ports)
{
    ports_ = ports;
    list_->clear();
    selectedPort_.clear();
    connectBtn_->setEnabled(false);
    infoLabel_->setText(ports.isEmpty() ? "未发现串口设备" : "请选择一个串口设备");
    for (const PortItem &p : ports) {
         QString label = p.description.isEmpty() ? "未知设备" : p.description;
         QListWidgetItem *item = new QListWidgetItem(QString("%1 (%2)").arg(label, p.portName));
         item->setData(Qt::UserRole, p.portName);
         list_->addItem(item);
     }
 }

 void ConnectionPage::showConnectError(const QString &msg)
 {
     QMessageBox::warning(this, "连接失败", msg);
 }

 void ConnectionPage::clearSelection()
 {
     selectedPort_.clear();
     connectBtn_->setEnabled(false);
     list_->clearSelection();
     infoLabel_->setText("请先扫描串口设备");
 }

 void ConnectionPage::onItemClicked(QListWidgetItem *item)
 {
     selectedPort_ = item->data(Qt::UserRole).toString();
     connectBtn_->setEnabled(true);

     for (const PortItem &p : ports_) {
         if (p.portName == selectedPort_) {
             infoLabel_->setText(
                 QString("端口：%1\n描述：%2\n厂商：%3\n位置：%4")
                     .arg(p.portName)
                     .arg(p.description.isEmpty() ? "未知" : p.description)
                     .arg(p.manufacturer.isEmpty() ? "未知" : p.manufacturer)
                     .arg(p.systemLocation.isEmpty() ? "未知" : p.systemLocation)
             );
             break;
         }
     }
 }
 void ConnectionPage::onScanClicked()
 {
     emit requestScanPorts();
 }

 void ConnectionPage::onConnectClicked()
 {
     if (!selectedPort_.isEmpty()) {
         emit requestConnectPort(selectedPort_);
     }
 }
