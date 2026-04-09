#include "connectionpage.h"
#include "serialmanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QGroupBox>
#include <QFrame>
#include <QSerialPortInfo>
#include <QFont>

ConnectionPage::ConnectionPage(SerialManager *manager, QWidget *parent)
    : QWidget(parent), m_manager(manager)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("串口连接");
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    title->setFont(font);
    layout->addWidget(title);

    QFrame *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    layout->addWidget(sep);

    m_statusLabel = new QLabel("就绪");
    layout->addWidget(m_statusLabel);

    QGroupBox *group = new QGroupBox("可用串口列表");
        QVBoxLayout *groupLayout = new QVBoxLayout(group);
        m_portList = new QListWidget;
        groupLayout->addWidget(m_portList);
        layout->addWidget(group);

        m_infoLabel = new QLabel("请刷新并选择串口");
        layout->addWidget(m_infoLabel);

        QHBoxLayout *btnLayout = new QHBoxLayout;
        m_refreshBtn = new QPushButton("刷新串口");
        m_connectBtn = new QPushButton("连接串口");
        m_connectBtn->setEnabled(false);
        btnLayout->addWidget(m_refreshBtn);
        btnLayout->addWidget(m_connectBtn);
        layout->addLayout(btnLayout);

        connect(m_refreshBtn, &QPushButton::clicked, this, &ConnectionPage::refreshPorts);
        connect(m_connectBtn, &QPushButton::clicked, this, &ConnectionPage::connectSelectedPort);
        connect(m_portList, &QListWidget::currentTextChanged, this, [this](const QString &text){
            m_connectBtn->setEnabled(!text.isEmpty());
            m_infoLabel->setText(text.isEmpty() ? "请刷新并选择串口" : text);
        });

        connect(m_manager, &SerialManager::connected, this, &ConnectionPage::onConnected);
        connect(m_manager, &SerialManager::disconnected, this, &ConnectionPage::onDisconnected);
        connect(m_manager, &SerialManager::connectionFailed, this, &ConnectionPage::onConnectionFailed);

        refreshPorts();
    }

void ConnectionPage::refreshPorts()
{
    m_portList->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        QString text = QString("%1 | %2 | %3")
                .arg(info.portName())
                .arg(info.description().isEmpty() ? "Unknown" : info.description())
                .arg(info.manufacturer().isEmpty() ? "Unknown" : info.manufacturer());
        m_portList->addItem(text);
    }

    m_statusLabel->setText(ports.isEmpty() ? "未发现串口设备" : QString("发现 %1 个串口设备").arg(ports.size()));
}

QString ConnectionPage::selectedPortName() const
{
    auto item = m_portList->currentItem();
    if (!item) return QString();
    return item->text().section(" | ", 0, 0);
}

void ConnectionPage::connectSelectedPort()
{
    const QString portName = selectedPortName();
    if (portName.isEmpty()) return;

    m_statusLabel->setText("正在连接串口...");
    m_manager->connectPort(portName, QSerialPort::Baud115200);
}

void ConnectionPage::onConnected()
{
    m_statusLabel->setText("串口已连接");
    emit requestEnterMonitor();
}

void ConnectionPage::onDisconnected()
{
    m_statusLabel->setText("串口已断开");
}

void ConnectionPage::onConnectionFailed(const QString &msg)
{
    m_statusLabel->setText("连接失败");
    QMessageBox::warning(this, "连接失败", msg);
}
