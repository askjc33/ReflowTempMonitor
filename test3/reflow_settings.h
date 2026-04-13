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
    const QVector<double> &zoneTimeOffsets() const;
    double displayTimeExtra() const;
    double totalDuration() const;

public slots:
    void setZoneLength(int index, double value);
    void setZoneThreshold(int index, double value);
    void setZoneLengths(const QVector<double> &values);
    void setZoneThresholds(const QVector<double> &values);
    void setZoneTimeOffset(int index, double value);
    void setZoneTimeOffsets(const QVector<double> &values);
    void setDisplayTimeExtra(double value);
    /** 按当前温区长度，把各温区偏移设为「该区中心相对第 1 区中心」的秒数，便于曲线点对齐竖线分区 */
    void applyZoneCenterTimeOffsets();
    void resetDefaults();

signals:
    void settingsChanged();

private:
    QVector<double> zoneLengths_;
    QVector<double> zoneThresholds_;
    QVector<double> zoneTimeOffsets_;
    double displayTimeExtra_ = 0.0;
};
