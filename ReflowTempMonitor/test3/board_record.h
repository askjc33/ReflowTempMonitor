#pragma once

#include <QString>
#include <QDateTime>
#include <QVector>

static const QString APP_VERSION = "v3.1.0-pos-traceability-fixed";   //版本号
static const int ZONE_COUNT = 12;    // 固定 12 个气室

struct BoardRecord
{
    QString barcode;      //板码
    QDateTime enterTime;      //进板时间
    QDateTime exitTime;       //出板时间
    QString status;               // 过炉中 / 已完成

    qint64 startMs = 0;     //这块板开始计时的毫秒时间
    int lastZone = 0;       //最近一次出现在第几气室
    int missingFrames = 0;   //连续丢失了多少帧
    bool finished = false;   //是否已完成

    QVector<double> timeAxis;     // 完整过炉曲线时间轴
    QVector<double> fullTemps;    // 完整过炉曲线温度
    QVector<int> zoneAxis;        // 每个采样点来自哪个气室

    QVector<QVector<double>> zoneTimes; // 12个气室各自采样时间
    QVector<QVector<double>> zoneTemps; // 12个气室各自采样温度
};
