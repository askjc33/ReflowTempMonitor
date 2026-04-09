#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>
#include <QDateTime>

#define CHAMBER_COUNT 5

// 单个温度采样点
struct TempSample {
    qint64 timestamp;           // Unix毫秒时间戳
    float  temps[CHAMBER_COUNT];// 5个气室当前温度(℃)
    int    currentChamber;      // PCB当前所在气室(0~4), -1=未进入或已离开
};

// 单块PCB完整数据
struct PcbRecord {
    QString             id;
    QVector<TempSample> samples;
    int                 currentChamber; // 实时所在气室
    QDateTime           enterTime;
    bool                isActive;
};

class PcbDataManager : public QObject
{
    Q_OBJECT
public:
    explicit PcbDataManager(QObject *parent = nullptr);

    void registerPcb(const QString &id);
    void updatePcbTemperature(const QString &id,
                              const float chamberTemps[CHAMBER_COUNT],
                              int currentChamber);
    void markPcbLeft(const QString &id);
    void clearAll();

    QStringList getAllIds() const;
    const PcbRecord* getPcbRecord(const QString &id) const;

signals:
    void pcbRegistered(const QString &id);
    void pcbDataUpdated(const QString &id);
    void pcbLeft(const QString &id);

private:
    QMap<QString, PcbRecord> m_records;
};
