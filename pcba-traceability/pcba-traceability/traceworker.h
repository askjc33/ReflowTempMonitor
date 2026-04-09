#pragma once

#include "types.h"
#include "boardtracemodel.h"
#include <QObject>
#include <QTimer>
#include <QSet>

class TraceWorker : public QObject
{
    Q_OBJECT
public:
    explicit TraceWorker(QObject *parent = nullptr);

public slots:
    void onSynchronizedFrame(const SyncFrame &frame);
    void clearFinished();
    void requestSnapshot();

signals:
    void recordsSnapshotReady(const QList<BoardRecord> &records);

private:
    QString normalizeBarcode(const QString &raw) const;
    void scheduleSnapshot(bool immediate = false);

private:
    BoardTraceModel model_;
    QTimer snapshotTimer_;
};
