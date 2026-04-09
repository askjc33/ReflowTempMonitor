#pragma once

#include <QString>
#include <QDateTime>
#include <QVector>

struct BoardRecord
{
    QString barcode;
    QDateTime enterTime;
    QDateTime exitTime;
    QString status;

    qint64 startMs = 0;
    int lastZone = 0;
    int missingFrames = 0;
    bool finished = false;

    QVector<double> timeAxis;
    QVector<double> fullTemps;
    QVector<int> zoneAxis;

    QVector<QVector<double>> zoneTimes;
    QVector<QVector<double>> zoneTemps;
};
