#include "serialmanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QFrame>
#include <QFont>

DataMonitorPage::DataMonitorPage(SerialManager *manager, QWidget *parent)
    : QWidget(parent),
      m_manager(manager),
      m_presetPage(nullptr),
      m_chart(new QChart),
      m_chartView(new QChartView(m_chart)),
      m_startMs(0),
      m_prevTime(-1.0),
      m_peakValues(NUM_CHANNELS, -1e9),
      m_prevValues(NUM_CHANNELS, qQNaN())
{
    for (int i = 0; i < NUM_CHANNELS; ++i) {
        m_channelData.append(QVector<double>());
    }

    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *header = new QHBoxLayout;
    QLabel *title = new QLabel("实时温度监控");
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    title->setFont(font);
    header->addWidget(title);
    header->addStretch();

    m_statusLabel = new QLabel("未连接");
        m_batteryLabel = new QLabel("电量：--");
        m_backBtn = new QPushButton("断开连接");
        header->addWidget(m_statusLabel);
        header->addWidget(m_batteryLabel);
        header->addWidget(m_backBtn);
        layout->addLayout(header);

        QFrame *sep = new QFrame;
        sep->setFrameShape(QFrame::HLine);
        layout->addWidget(sep);

        QGroupBox *chartGroup = new QGroupBox("温度趋势图");
        QVBoxLayout *chartLayout = new QVBoxLayout(chartGroup);
        m_chart->legend()->setVisible(true);
        m_chart->createDefaultAxes();
        m_chart->axisX()->setTitleText("时间 (s)");
        m_chart->axisY()->setTitleText("温度 (°C)");
        m_chartView->setRenderHint(QPainter::Antialiasing);
        chartLayout->addWidget(m_chartView);
        layout->addWidget(chartGroup);

        QGroupBox *statusGroup = new QGroupBox("各通道状态");
        QHBoxLayout *statusLayout = new QHBoxLayout(statusGroup);
        for (int i = 0; i < NUM_CHANNELS; ++i) {
            QVBoxLayout *box = new QVBoxLayout;
            QCheckBox *cb = new QCheckBox(QString("通道%1").arg(i + 1));
            cb->setChecked(true);
            QLabel *val = new QLabel("--");
            QLabel *peak = new QLabel("峰值 --");
            QLabel *rate = new QLabel("变化 --");
            val->setAlignment(Qt::AlignCenter);
            peak->setAlignment(Qt::AlignCenter);
            rate->setAlignment(Qt::AlignCenter);
            box->addWidget(cb);
            box->addWidget(val);
            box->addWidget(peak);
            box->addWidget(rate);
            statusLayout->addLayout(box);
            m_checkboxes.append(cb);
            m_valueLabels.append(val);
            m_peakLabels.append(peak);
            m_rateLabels.append(rate);
        }
        layout->addWidget(statusGroup);

        QGroupBox *thrGroup = new QGroupBox("温区阈值设置");
            QHBoxLayout *thrLayout = new QHBoxLayout(thrGroup);
            QPushButton *loadThrBtn = new QPushButton("加载温区阈值");
            QPushButton *presetBtn = new QPushButton("阈值预设管理");
            m_thrInfoLabel = new QLabel("未加载温区阈值");
            thrLayout->addWidget(loadThrBtn);
            thrLayout->addWidget(presetBtn);
            thrLayout->addWidget(m_thrInfoLabel);
            thrLayout->addStretch();
            layout->addWidget(thrGroup);

            QHBoxLayout *ctrl = new QHBoxLayout;
            ctrl->addStretch();
            m_startBtn = new QPushButton("开始监控");
            m_stopBtn = new QPushButton("停止监控");
            QPushButton *clearBtn = new QPushButton("清除数据");
            QPushButton *loadPresetBtn = new QPushButton("加载预设曲线");
            m_stopBtn->setEnabled(false);
            m_startBtn->setEnabled(false);
            ctrl->addWidget(m_startBtn);
            ctrl->addWidget(m_stopBtn);
            ctrl->addWidget(clearBtn);
            ctrl->addWidget(loadPresetBtn);
            layout->addLayout(ctrl);

            rebuildChart();

            connect(m_startBtn, &QPushButton::clicked, this, &DataMonitorPage::startMonitoring);
            connect(m_stopBtn, &QPushButton::clicked, this, &DataMonitorPage::stopMonitoring);
            connect(clearBtn, &QPushButton::clicked, this, &DataMonitorPage::clearData);
            connect(loadPresetBtn, &QPushButton::clicked, this, &DataMonitorPage::loadPresetCurves);
            connect(loadThrBtn, &QPushButton::clicked, this, &DataMonitorPage::syncThresholdsFromPreset);
            connect(presetBtn, &QPushButton::clicked, this, &DataMonitorPage::requestGoPreset);
            connect(m_backBtn, &QPushButton::clicked, this, &DataMonitorPage::disconnectDevice);

            connect(m_manager, &SerialManager::temperaturesReceived, this, &DataMonitorPage::onTemperaturesReceived);
            connect(m_manager, &SerialManager::batteryReceived, this, &DataMonitorPage::onBatteryReceived);
            connect(m_manager, &SerialManager::disconnected, this, &DataMonitorPage::onDisconnected);

            for (int i = 0; i < NUM_CHANNELS; ++i) {
                connect(m_checkboxes[i], &QCheckBox::toggled, this, [this, i](bool checked){
                    if (i >= 0 && i < m_series.size()) m_series[i]->setVisible(checked);
                });
            }
        }

void DataMonitorPage::setPresetPage(PresetPage *page)
void DataMonitorPage::setReady()
{
    m_statusLabel->setText("串口已连接，就绪");
    m_startBtn->setEnabled(true);
}

QVector<double> DataMonitorPage::timeData() const
{
    return m_timeData;
}

QVector<QVector<double>> DataMonitorPage::channelData() const
{
    return m_channelData;
}

void DataMonitorPage::rebuildChart()
{
    m_chart->removeAllSeries();
    m_series.clear();

    QStringList names = {"通道1", "通道2", "通道3", "通道4", "通道5"};
    QList<QColor> colors = {Qt::red, Qt::green, Qt::blue, QColor("#f39c12"), QColor("#9b59b6")};

    for (int i = 0; i < NUM_CHANNELS; ++i) {
        QLineSeries *series = new QLineSeries;
        series->setName(names[i]);
        QPen pen(colors[i], 2);
        series->setPen(pen);
        m_chart->addSeries(series);
        m_series.append(series);
    }

    m_chart->createDefaultAxes();
    m_chart->axisX()->setRange(0, 10);
    m_chart->axisY()->setRange(0, 300);
    m_chart->axisX()->setTitleText("时间 (s)");
    m_chart->axisY()->setTitleText("温度 (°C)");
}

void DataMonitorPage::startMonitoring()
{
    clearData();
    m_startMs = QDateTime::currentMSecsSinceEpoch();
    m_prevTime = -1.0;
    m_statusLabel->setText("监控中...");
    m_startBtn->setEnabled(false);
    m_stopBtn->setEnabled(true);
}

void DataMonitorPage::stopMonitoring()
{
    m_statusLabel->setText("监控已停止");
    m_startBtn->setEnabled(true);
    m_stopBtn->setEnabled(false);
}

void DataMonitorPage::clearData()
                m_rateLabels[i]->setText(QString("%1°C/s").arg(rate, 0, 'f', 2));
            }
        }
        m_prevValues[i] = v;
    }

    m_prevTime = t;
    m_chart->axisX()->setRange(qMax(0.0, t - 10.0), qMax(10.0, t));
}

void DataMonitorPage::onBatteryReceived(int value)
{
    m_batteryLabel->setText(QString("电量：%1%").arg(value));
}

void DataMonitorPage::onDisconnected()
{
    m_statusLabel->setText("已断开连接");
    m_batteryLabel->setText("电量：--");
    m_startBtn->setEnabled(false);
    m_stopBtn->setEnabled(false);
    emit requestBackToConnection();
}

void DataMonitorPage::loadPresetCurves()
{
    if (!m_presetPage) {
        QMessageBox::warning(this, "错误", "未绑定预设页面");
        return;
    }

    clearData();
    auto preset = m_presetPage->presetData();
    const double zoneStep = 10.0;

    for (int j = 0; j < preset[0].size(); ++j) {
        double t = j * zoneStep;
        m_timeData.append(t);
        for (int i = 0; i < NUM_CHANNELS; ++i) {
            double v = preset[i][j];
            m_channelData[i].append(v);
            m_series[i]->append(t, v);
            if (v > m_peakValues[i]) m_peakValues[i] = v;
        }
    }

    for (int i = 0; i < NUM_CHANNELS; ++i) {
        if (!preset[i].isEmpty()) {
            m_valueLabels[i]->setText(QString::number(preset[i].last(), 'f', 2));
            m_peakLabels[i]->setText(QString("峰值 %1°C").arg(m_peakValues[i], 0, 'f', 2));
        }
    }

    m_chart->axisX()->setRange(0, preset[0].size() * zoneStep);
       m_statusLabel->setText("预设曲线已加载");
   }

   void DataMonitorPage::syncThresholdsFromPreset()
   {
       if (!m_presetPage) {
           QMessageBox::warning(this, "错误", "未绑定预设页面");
           return;
       }

       m_thresholdValues = m_presetPage->thresholdData();
       updateThresholdLines();
       m_thrInfoLabel->setText("已加载全炉12温区阈值");
   }

   void DataMonitorPage::updateThresholdLines()
   {
       for (auto *s : m_thresholdSeries) {
           m_chart->removeSeries(s);
           delete s;
       }
       m_thresholdSeries.clear();

       const double zoneStep = 10.0;
       for (int i = 0; i < m_thresholdValues.size(); ++i) {
           QLineSeries *line = new QLineSeries;
           line->append(i * zoneStep, m_thresholdValues[i]);
           line->append((i + 1) * zoneStep, m_thresholdValues[i]);
           QPen pen(Qt::darkRed, 2, Qt::DashLine);
           line->setPen(pen);
           m_chart->addSeries(line);
           m_thresholdSeries.append(line);
       }
       m_chart->createDefaultAxes();
       m_chart->axisX()->setTitleText("时间 (s)");
       m_chart->axisY()->setTitleText("温度 (°C)");
   }

   void DataMonitorPage::disconnectDevice()
   {
       m_manager->disconnectPort();
   }
