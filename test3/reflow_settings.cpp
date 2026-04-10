#include "reflow_settings.h"

#include <QtGlobal>

namespace {
// ===== 新增：默认 12 温区长度（用于曲线横坐标分段） =====
QVector<double> defaultZoneLengths()
{
    return QVector<double>() << 40.0 << 20.0 << 20.0 << 20.0 << 20.0 << 25.0
                             << 25.0 << 20.0 << 20.0 << 20.0 << 20.0 << 20.0;
}

QVector<double> defaultZoneThresholds()
{
    // ===== 新增：默认 12 温区阈值（用于曲线横向阈值线） =====
    return QVector<double>() << 140.0 << 150.0 << 160.0 << 170.0 << 180.0 << 185.0
                             << 190.0 << 210.0 << 240.0 << 250.0 << 200.0 << 150.0;
}
}

ReflowSettings::ReflowSettings(QObject *parent)
    : QObject(parent),
      zoneLengths_(defaultZoneLengths()),
      zoneThresholds_(defaultZoneThresholds())
{
}

const QVector<double> &ReflowSettings::zoneLengths() const
{
    return zoneLengths_;
}

const QVector<double> &ReflowSettings::zoneThresholds() const
{
    return zoneThresholds_;
}

double ReflowSettings::totalDuration() const
{
    double total = 0.0;
    for (int i = 0; i < zoneLengths_.size(); ++i) {
        total += zoneLengths_.at(i);
    }
    return total;
}

void ReflowSettings::setZoneLength(int index, double value)
{
    if (index < 0 || index >= zoneLengths_.size()) {
        return;
    }

    double nextValue = qMax(1.0, value);
    if (qFuzzyCompare(zoneLengths_.at(index) + 1.0, nextValue + 1.0)) {
        return;
    }

    zoneLengths_[index] = nextValue;
    emit settingsChanged();
}

void ReflowSettings::setZoneThreshold(int index, double value)
{
    if (index < 0 || index >= zoneThresholds_.size()) {
        return;
    }

    double nextValue = qMax(0.0, value);
    if (qFuzzyCompare(zoneThresholds_.at(index) + 1.0, nextValue + 1.0)) {
        return;
    }

    zoneThresholds_[index] = nextValue;
    emit settingsChanged();
}

void ReflowSettings::setZoneLengths(const QVector<double> &values)
{
    if (values.size() != ZONE_COUNT) {
        return;
    }

    bool changed = false;
    for (int i = 0; i < values.size(); ++i) {
        double nextValue = qMax(1.0, values.at(i));
        if (!qFuzzyCompare(zoneLengths_.at(i) + 1.0, nextValue + 1.0)) {
            zoneLengths_[i] = nextValue;
            changed = true;
        }
    }

    if (changed) {
        emit settingsChanged();
    }
}

void ReflowSettings::setZoneThresholds(const QVector<double> &values)
{
    if (values.size() != ZONE_COUNT) {
        return;
    }

    bool changed = false;
    for (int i = 0; i < values.size(); ++i) {
        double nextValue = qMax(0.0, values.at(i));
        if (!qFuzzyCompare(zoneThresholds_.at(i) + 1.0, nextValue + 1.0)) {
            zoneThresholds_[i] = nextValue;
            changed = true;
        }
    }

    if (changed) {
        emit settingsChanged();
    }
}

void ReflowSettings::resetDefaults()
{
    // ===== 新增：一键恢复默认温区长度和阈值 =====
    zoneLengths_ = defaultZoneLengths();
    zoneThresholds_ = defaultZoneThresholds();
    emit settingsChanged();
}
