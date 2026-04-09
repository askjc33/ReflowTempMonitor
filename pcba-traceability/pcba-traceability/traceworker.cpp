#include "traceworker.h"
#include "constants.h"
#include <QDebug>

TraceWorker::TraceWorker(QObject *parent) : QObject(parent)
{
    snapshotTimer_.setInterval(UI_REFRESH_INTERVAL_MS);
    snapshotTimer_.setSingleShot(true);
    connect(&snapshotTimer_, &QTimer::timeout, this, [this]() {
        auto list = model_.allRecords();
        qDebug() << "snapshot emit, count =" << list.size();
        emit recordsSnapshotReady(list);
    });
}

QString TraceWorker::normalizeBarcode(const QString &raw) const
{
    QString s = raw.trimmed();
    if (s.isEmpty()) return QString();
    if (s.compare("EMPTY", Qt::CaseInsensitive) == 0) return QString();
    if (s.compare("NULL", Qt::CaseInsensitive) == 0) return QString();
    if (s == "-") return QString();
    return s;
}

void TraceWorker::onSynchronizedFrame(const SyncFrame &frame)
{
    qDebug() << "TraceWorker got SyncFrame seq =" << frame.seq
                << "temps =" << frame.temps.size()
                << "positions =" << frame.positions.size();
       qDebug() << "positions content =" << frame.positions;
    if (frame.temps.size() != ZONE_COUNT || frame.positions.size() != ZONE_COUNT) return;

    const QDateTime now = frame.timestamp;
    const qint64 nowMs = now.toMSecsSinceEpoch();
    QSet<QString> presentBoards;

    for (int zone = 0; zone < ZONE_COUNT; ++zone) {
        QString barcode = normalizeBarcode(frame.positions[zone]);
        qDebug() << "zone =" << zone + 1

                         << "normalized =" << barcode;
        if (barcode.isEmpty()) continue;

        presentBoards.insert(barcode);

        if (!model_.containsActive(barcode)) {
            BoardRecord r;
            r.barcode = barcode;
            r.enterTime = now;
            r.status = "过炉中";
            r.startMs = nowMs;
            r.zoneTimes.resize(ZONE_COUNT);
            r.zoneTemps.resize(ZONE_COUNT);
            model_.upsertActiveRecord(barcode, r);
        }

        BoardRecord record = model_.activeRecord(barcode);
        const double elapsed = (nowMs - record.startMs) / 1000.0;
        const double temp = frame.temps[zone];

        record.lastZone = zone + 1;
        record.missingFrames = 0;

        record.timeAxis.append(elapsed);
        record.fullTemps.append(temp);
        record.zoneAxis.append(zone + 1);
        record.zoneTimes[zone].append(elapsed);
        record.zoneTemps[zone].append(temp);

        model_.upsertActiveRecord(barcode, record);
    }

    QList<QString> toFinish;
    const QList<QString> activeKeys = model_.activeKeys();
    for (const QString &key : activeKeys) {
        BoardRecord record = model_.activeRecord(key);
        if (!presentBoards.contains(key)) {
            record.missingFrames++;
            if (record.missingFrames >= FINISH_MISSING_FRAME_THRESHOLD) {
                record.exitTime = now;
                record.status = "已完成";
                record.finished = true;
                model_.prependHistory(record);
                toFinish.append(key);
            } else {
                model_.upsertActiveRecord(key, record);
            }
        }
    }

    for (const QString &key : toFinish) {
        model_.removeActive(key);
    }

    scheduleSnapshot(false);
}

void TraceWorker::clearFinished()
{
    model_.clearFinished();
    scheduleSnapshot(true);
}

void TraceWorker::requestSnapshot()
{
    emit recordsSnapshotReady(model_.allRecords());
    qDebug() << "🔥 EMIT DONE";
}

void TraceWorker::scheduleSnapshot(bool immediate)
{
    if (immediate) {
        snapshotTimer_.stop();
        emit recordsSnapshotReady(model_.allRecords());
        qDebug() << "🔥 EMIT DONE";
        return;
    }

    if (!snapshotTimer_.isActive()) {
        snapshotTimer_.start();
    }
    emit recordsSnapshotReady(model_.allRecords());
    qDebug() << "🔥 EMIT DONE";
}
