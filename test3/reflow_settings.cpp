#include "reflow_settings.h"

#include <QtGlobal>

namespace {
// ===== 新增：默认 12 温区长度（用于曲线横坐标分段） =====
QVector<double> defaultZoneLengths()
{
    return QVector<double>() << 10.0 << 10.0 << 10.0 << 10.0 << 10.0 << 10.0
                             << 10.0 << 10.0 << 10.0 << 10.0 << 10.0 << 10.0;
}

QVector<double> defaultZoneThresholds()
{
    // ===== 新增：默认 12 温区阈值（用于曲线横向阈值线） =====
    return QVector<double>() << 100.0 << 120.0 << 160.0 << 170.0 << 180.0 << 185.0
                             << 190.0 << 210.0 << 210.0 << 220.0 << 200.0 << 150.0;
}

QVector<double> defaultZoneTimeOffsets()
{
    QVector<double> v;
    v.resize(ZONE_COUNT);
    for (int i = 0; i < ZONE_COUNT; ++i) {
        v[i] = 0.0;
    }
    return v;
}
}

ReflowSettings::ReflowSettings(QObject *parent)
    : QObject(parent),
      zoneLengths_(defaultZoneLengths()),
      zoneThresholds_(defaultZoneThresholds()),
      zoneTimeOffsets_(defaultZoneTimeOffsets()),
      displayTimeExtra_(0.0)
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

const QVector<double> &ReflowSettings::zoneTimeOffsets() const
{
    return zoneTimeOffsets_;
}

double ReflowSettings::displayTimeExtra() const
{
    return displayTimeExtra_;
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

void ReflowSettings::setZoneTimeOffset(int index, double value)
{
    if (index < 0 || index >= zoneTimeOffsets_.size()) {
        return;
    }
    if (qFuzzyCompare(zoneTimeOffsets_.at(index) + 1.0, value + 1.0)) {
        return;
    }
    zoneTimeOffsets_[index] = value;
    emit settingsChanged();
}

void ReflowSettings::setZoneTimeOffsets(const QVector<double> &values)
{
    if (values.size() != ZONE_COUNT) {
        return;
    }
    bool changed = false;
    for (int i = 0; i < ZONE_COUNT; ++i) {
        if (!qFuzzyCompare(zoneTimeOffsets_.at(i) + 1.0, values.at(i) + 1.0)) {
            zoneTimeOffsets_[i] = values.at(i);
            changed = true;
        }
    }
    if (changed) {
        emit settingsChanged();
    }
}

void ReflowSettings::setDisplayTimeExtra(double value)
{
    if (qFuzzyCompare(displayTimeExtra_ + 1.0, value + 1.0)) {
        return;
    }
    displayTimeExtra_ = value;
    emit settingsChanged();
}

void ReflowSettings::applyZoneCenterTimeOffsets()
{
    double cum = 0.0;
    QVector<double> centers(ZONE_COUNT);
    for (int i = 0; i < ZONE_COUNT; ++i) {
        const double len = zoneLengths_.at(i);
        centers[i] = cum + len / 2.0;
        cum += len;
    }
    const double c0 = centers[0];
    for (int i = 0; i < ZONE_COUNT; ++i) {
        zoneTimeOffsets_[i] = centers[i] - c0;
    }
    emit settingsChanged();
}

void ReflowSettings::resetDefaults()
{
    // ===== 新增：一键恢复默认温区长度和阈值 =====
    zoneLengths_ = defaultZoneLengths();
    zoneThresholds_ = defaultZoneThresholds();
    zoneTimeOffsets_ = defaultZoneTimeOffsets();
    displayTimeExtra_ = 0.0;
    emit settingsChanged();
}
