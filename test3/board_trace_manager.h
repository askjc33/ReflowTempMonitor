#pragma once

#include <QObject>
#include <QMap>
#include <QList>
#include <QSet>
#include <QTimer>
#include <QStringList>
#include <QVector>

#include "board_record.h"

class BoardTraceManager : public QObject
{
    Q_OBJECT
public:
    explicit BoardTraceManager(QObject *parent = 0);

    QList<BoardRecord> allRecords() const;
    bool getRecordByBarcode(const QString &barcode, BoardRecord &out) const;
    void clearFinished();

signals:
    void recordsUpdated();

public slots:
    void onSynchronizedFrame(const QVector<double> &temps, const QStringList &positions);

private:
    static QString normalizeBarcode(const QString &raw);
    void requestUiRefresh(bool immediate);

private:
    QMap<QString, BoardRecord> activeBoards_;
    QList<BoardRecord> historyBoards_;
    const int finishMissingFrameThreshold_ = 3;   //已完成阈值
    const int maxHistoryBoards_ = 5000;
    QTimer updateTimer_;
};
