#pragma once

#include <QString>
#include <QStringList>
#include <QVector>
#include <QDateTime>
#include <QMetaType>

struct SyncFrame
{
    int seq = 0;
    QVector<double> temps;
    QStringList positions;
    QDateTime timestamp;
};

struct PortItem
{
    QString portName;
    QString description;
    QString manufacturer;
    QString systemLocation;
};

Q_DECLARE_METATYPE(SyncFrame)
Q_DECLARE_METATYPE(PortItem)
Q_DECLARE_METATYPE(QList<PortItem>)
