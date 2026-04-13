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
    connect(centerOffsetsBtn_, SIGNAL(clicked()), this, SLOT(applyCenterOffsets()));
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

    QVector<double> offsets;
    for (int i = 0; i < timeOffsetEditors_.size(); ++i) {
        offsets.append(timeOffsetEditors_.at(i)->value());
    }

    settings_->setZoneLengths(lengths);
    settings_->setZoneThresholds(thresholds);
    settings_->setZoneTimeOffsets(offsets);
    settings_->setDisplayTimeExtra(displayExtraSpin_->value());
    emit requestSetStatusMessage(QString::fromUtf8("阈值和温度区设置已更新"));
}

void ZoneSettingsPage::applyCenterOffsets()
{
    QVector<double> lengths;
    lengths.reserve(lengthEditors_.size());
    for (int i = 0; i < lengthEditors_.size(); ++i) {
        lengths.append(lengthEditors_.at(i)->value());
    }
    if (lengths.size() != 12) {
        return;
    }

    double cum = 0.0;
    QVector<double> centers(12);
    for (int i = 0; i < 12; ++i) {
        const double len = lengths.at(i);
        centers[i] = cum + len / 2.0;
        cum += len;
    }
    const double c0 = centers[0];
    QVector<double> offs(12);
    for (int i = 0; i < 12; ++i) {
        offs[i] = centers[i] - c0;
    }

    settings_->setZoneTimeOffsets(offs);
    emit requestSetStatusMessage(
        QString::fromUtf8("已按上方「区间长度」计算各温区曲线时间偏移（相对第 1 区中心），请点「应用设置」保存全部参数"));
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
    const QVector<double> offsets = settings_->zoneTimeOffsets();

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

    for (int i = 0; i < timeOffsetEditors_.size() && i < offsets.size(); ++i) {
        timeOffsetEditors_.at(i)->blockSignals(true);
        timeOffsetEditors_.at(i)->setValue(offsets.at(i));
        timeOffsetEditors_.at(i)->blockSignals(false);
    }

    if (displayExtraSpin_) {
        displayExtraSpin_->blockSignals(true);
        displayExtraSpin_->setValue(settings_->displayTimeExtra());
        displayExtraSpin_->blockSignals(false);
    }

    summaryLabel_->setText(QString::fromUtf8("总横坐标长度：%1 s（温区分段）；曲线显示：横坐标 = 采样时间 + 对应温区偏移 + 全局附加")
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
    grid->addWidget(new QLabel(QString::fromUtf8("曲线时间偏移(s)")), 0, 3);

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

        QDoubleSpinBox *offsetSpin = new QDoubleSpinBox;
        offsetSpin->setRange(-500.0, 500.0);
        offsetSpin->setDecimals(1);
        offsetSpin->setSingleStep(1.0);
        offsetSpin->setSuffix(QString::fromUtf8(" s"));
        offsetSpin->setToolTip(QString::fromUtf8(
            "追溯页曲线横坐标 = 采样时间 + 本温区偏移 + 全局附加；用于把各温区采样点对齐到分区中心"));

        lengthEditors_.append(lengthSpin);
        thresholdEditors_.append(thresholdSpin);
        timeOffsetEditors_.append(offsetSpin);

        grid->addWidget(zoneLabel, i + 1, 0);
        grid->addWidget(lengthSpin, i + 1, 1);
        grid->addWidget(thresholdSpin, i + 1, 2);
        grid->addWidget(offsetSpin, i + 1, 3);
    }

    layout->addWidget(group);

    QHBoxLayout *extraRow = new QHBoxLayout;
    extraRow->addWidget(new QLabel(QString::fromUtf8("全局附加时间偏移(s)：")));
    displayExtraSpin_ = new QDoubleSpinBox;
    displayExtraSpin_->setRange(-500.0, 500.0);
    displayExtraSpin_->setDecimals(1);
    displayExtraSpin_->setSingleStep(1.0);
    displayExtraSpin_->setSuffix(QString::fromUtf8(" s"));
    displayExtraSpin_->setToolTip(QString::fromUtf8(
        "在各区偏移之上整体平移曲线，例如 +5 使整条曲线向右移 5 秒"));
    extraRow->addWidget(displayExtraSpin_);
    extraRow->addStretch();
    layout->addLayout(extraRow);

    summaryLabel_ = new QLabel;
    layout->addWidget(summaryLabel_);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();

    centerOffsetsBtn_ = new QPushButton(QString::fromUtf8("按温区中心计算偏移"));
    centerOffsetsBtn_->setToolTip(QString::fromUtf8(
        "按当前「区间长度」计算各温区中心相对第 1 区中心的秒数，写入「曲线时间偏移」列（需再点应用设置）"));
    btnLayout->addWidget(centerOffsetsBtn_);

    resetBtn_ = new QPushButton(QString::fromUtf8("恢复默认"));
    btnLayout->addWidget(resetBtn_);

    applyBtn_ = new QPushButton(QString::fromUtf8("应用设置"));
    btnLayout->addWidget(applyBtn_);

    layout->addLayout(btnLayout);
    layout->addStretch();
}
