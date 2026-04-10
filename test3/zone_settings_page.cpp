#include "zone_settings_page.h"
#include "reflow_settings.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFont>
#include <QFrame>
#include <QGroupBox>
#include <QPushButton>
#include <QDoubleSpinBox>

ZoneSettingsPage::ZoneSettingsPage(ReflowSettings *settings, QWidget *parent)
    : QWidget(parent),
      settings_(settings),
      summaryLabel_(nullptr),
      applyBtn_(nullptr),
      resetBtn_(nullptr)
{
    initUi();
    syncFromSettings();

    connect(applyBtn_, SIGNAL(clicked()), this, SLOT(applySettings()));
    connect(resetBtn_, SIGNAL(clicked()), this, SLOT(resetDefaults()));
    connect(settings_, SIGNAL(settingsChanged()), this, SLOT(syncFromSettings()));
}

void ZoneSettingsPage::applySettings()
{
    // ===== 新增：应用 12 个温区长度和阈值 =====
    QVector<double> lengths;
    QVector<double> thresholds;
    for (int i = 0; i < lengthEditors_.size(); ++i) {
        lengths.append(lengthEditors_.at(i)->value());
        thresholds.append(thresholdEditors_.at(i)->value());
    }

    settings_->setZoneLengths(lengths);
    settings_->setZoneThresholds(thresholds);
    emit requestSetStatusMessage(QString::fromUtf8("阈值和温度区设置已更新"));
}

void ZoneSettingsPage::resetDefaults()
{
    settings_->resetDefaults();
    emit requestSetStatusMessage(QString::fromUtf8("已恢复默认温区设置"));
}

void ZoneSettingsPage::syncFromSettings()
{
    const QVector<double> lengths = settings_->zoneLengths();
    const QVector<double> thresholds = settings_->zoneThresholds();

    for (int i = 0; i < lengthEditors_.size() && i < lengths.size(); ++i) {
        lengthEditors_.at(i)->blockSignals(true);
        lengthEditors_.at(i)->setValue(lengths.at(i));
        lengthEditors_.at(i)->blockSignals(false);
    }

    for (int i = 0; i < thresholdEditors_.size() && i < thresholds.size(); ++i) {
        thresholdEditors_.at(i)->blockSignals(true);
        thresholdEditors_.at(i)->setValue(thresholds.at(i));
        thresholdEditors_.at(i)->blockSignals(false);
    }

    summaryLabel_->setText(QString::fromUtf8("总横坐标长度：%1 s（当前按 12 个温区累计划分）")
                           .arg(settings_->totalDuration(), 0, 'f', 1));
}

void ZoneSettingsPage::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // ===== 新增：页面标题“阈值和温度区设置” =====
    QLabel *title = new QLabel(QString::fromUtf8("阈值和温度区设置"));
    QFont titleFont;
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    title->setFont(titleFont);
    layout->addWidget(title);

    QFrame *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    layout->addWidget(sep);

    QGroupBox *group = new QGroupBox(QString::fromUtf8("12 个温区参数"));
    QGridLayout *grid = new QGridLayout(group);
    // ===== 新增：12 个温区的长度和阈值输入表 =====
    grid->addWidget(new QLabel(QString::fromUtf8("温区")), 0, 0);
    grid->addWidget(new QLabel(QString::fromUtf8("区间长度(s)")), 0, 1);
    grid->addWidget(new QLabel(QString::fromUtf8("阈值温度(°C)")), 0, 2);

    for (int i = 0; i < 12; ++i) {
        QLabel *zoneLabel = new QLabel(QString::fromUtf8("第 %1 区").arg(i + 1));
        QDoubleSpinBox *lengthSpin = new QDoubleSpinBox;
        lengthSpin->setRange(1.0, 300.0);
        lengthSpin->setDecimals(1);
        lengthSpin->setSingleStep(1.0);
        lengthSpin->setSuffix(" s");

        QDoubleSpinBox *thresholdSpin = new QDoubleSpinBox;
        thresholdSpin->setRange(0.0, 350.0);
        thresholdSpin->setDecimals(1);
        thresholdSpin->setSingleStep(5.0);
        thresholdSpin->setSuffix(QString::fromUtf8(" °C"));

        lengthEditors_.append(lengthSpin);
        thresholdEditors_.append(thresholdSpin);

        grid->addWidget(zoneLabel, i + 1, 0);
        grid->addWidget(lengthSpin, i + 1, 1);
        grid->addWidget(thresholdSpin, i + 1, 2);
    }

    layout->addWidget(group);

    summaryLabel_ = new QLabel;
    layout->addWidget(summaryLabel_);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();

    resetBtn_ = new QPushButton(QString::fromUtf8("恢复默认"));
    btnLayout->addWidget(resetBtn_);

    applyBtn_ = new QPushButton(QString::fromUtf8("应用设置"));
    btnLayout->addWidget(applyBtn_);

    layout->addLayout(btnLayout);
    layout->addStretch();
}
