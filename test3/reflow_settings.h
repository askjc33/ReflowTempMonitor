#pragma once

#include <QObject>
#include <QVector>

#include "board_record.h"

// ===== 新增：温区长度与阈值配置对象 =====

class ReflowSettings : public QObject
{
    Q_OBJECT
public:
    explicit ReflowSettings(QObject *parent = nullptr);

    const QVector<double> &zoneLengths() const;
    const QVector<double> &zoneThresholds() const;
    double totalDuration() const;

public slots:
    void setZoneLength(int index, double value);
    void setZoneThreshold(int index, double value);
    void setZoneLengths(const QVector<double> &values);
    void setZoneThresholds(const QVector<double> &values);
    void resetDefaults();

signals:
    void settingsChanged();

private:
    QVector<double> zoneLengths_;
    QVector<double> zoneThresholds_;
};
