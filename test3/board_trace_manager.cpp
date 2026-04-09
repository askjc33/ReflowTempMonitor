#include "board_trace_manager.h"

#include <algorithm>

BoardTraceManager::BoardTraceManager(QObject *parent)    //根据同步后的温度+位置帧，维护所有板子的过炉状态。
    : QObject(parent)
{
    updateTimer_.setInterval(200);   //200ms 内多次更新，合并成一次 UI 刷新
    updateTimer_.setSingleShot(true);
    connect(&updateTimer_, SIGNAL(timeout()), this, SIGNAL(recordsUpdated()));
}

QList<BoardRecord> BoardTraceManager::allRecords() const
{
    QList<BoardRecord> finishedList;
    QList<BoardRecord> activeList;

    QMap<QString, BoardRecord>::const_iterator it1;
    for (it1 = activeBoards_.constBegin(); it1 != activeBoards_.constEnd(); ++it1) {
        activeList.append(it1.value());
    }

    for (int i = 0; i < historyBoards_.size(); ++i) {
        finishedList.append(historyBoards_.at(i));
    }

    std::sort(finishedList.begin(), finishedList.end(),
              [](const BoardRecord &a, const BoardRecord &b) {
                  return a.enterTime < b.enterTime;
              });

    std::sort(activeList.begin(), activeList.end(),
              [](const BoardRecord &a, const BoardRecord &b) {
                  return a.enterTime < b.enterTime;
              });

    QList<BoardRecord> list;
    list.reserve(finishedList.size() + activeList.size());
    list.append(finishedList);
    list.append(activeList);

    return list;
}

bool BoardTraceManager::getRecordByBarcode(const QString &barcode, BoardRecord &out) const
{
    if (activeBoards_.contains(barcode)) {
        out = activeBoards_.value(barcode);
        return true;
    }

    for (int i = 0; i < historyBoards_.size(); ++i) {
        const BoardRecord &r = historyBoards_.at(i);
        if (r.barcode == barcode) {
            out = r;
            return true;
        }
    }

    return false;
}

void BoardTraceManager::clearFinished()
{
    historyBoards_.clear();
    requestUiRefresh(true);
}

void BoardTraceManager::onSynchronizedFrame(const QVector<double> &temps, const QStringList &positions)  //入口函数,每来一帧，就做一轮业务更新
{
    if (temps.size() != ZONE_COUNT || positions.size() != ZONE_COUNT) {
        return;
    }

    const QDateTime now = QDateTime::currentDateTime();
    const qint64 nowMs = now.toMSecsSinceEpoch();

    QSet<QString> presentBoards;

    for (int zone = 0; zone < ZONE_COUNT; ++zone) {
        QString barcode = normalizeBarcode(positions.at(zone));  //过滤无效值“empty”，“null”
        if (barcode.isEmpty()) {
            continue;
        }

        presentBoards.insert(barcode);
        //新板第一次出现，建档
        if (!activeBoards_.contains(barcode)) {
            BoardRecord r;
            r.barcode = barcode;
            r.enterTime = now;
            r.status = QString::fromUtf8("过炉中");
            r.startMs = nowMs;
            r.zoneTimes.resize(ZONE_COUNT);
            r.zoneTemps.resize(ZONE_COUNT);
            activeBoards_[barcode] = r;
        }

        BoardRecord &record = activeBoards_[barcode];
        double elapsed = (nowMs - record.startMs) / 1000.0;     //相对进板时间
        double temp = temps.at(zone);

        record.lastZone = zone + 1;
        record.missingFrames = 0;

        record.timeAxis.append(elapsed);
        record.fullTemps.append(temp);
        record.zoneAxis.append(zone + 1);

        record.zoneTimes[zone].append(elapsed);
        record.zoneTemps[zone].append(temp);
    }

    QList<QString> toFinish;

    QMap<QString, BoardRecord>::iterator it;
    for (it = activeBoards_.begin(); it != activeBoards_.end(); ++it) {
        if (!presentBoards.contains(it.key())) {
            it.value().missingFrames++;

            if (it.value().missingFrames >= finishMissingFrameThreshold_) {
                BoardRecord finished = it.value();
                finished.exitTime = now;
                finished.status = QString::fromUtf8("已完成");
                finished.finished = true;
                historyBoards_.prepend(finished);
                toFinish.append(it.key());
            }
        }
    }

    for (int i = 0; i < toFinish.size(); ++i) {
        activeBoards_.remove(toFinish.at(i));
    }

    while (historyBoards_.size() > maxHistoryBoards_) {
        historyBoards_.removeLast();
    }

    requestUiRefresh(false);
}

QString BoardTraceManager::normalizeBarcode(const QString &raw)
{
    QString s = raw.trimmed();
    if (s.isEmpty()) {
        return QString();
    }
    if (s.compare("EMPTY", Qt::CaseInsensitive) == 0) {
        return QString();
    }
    if (s.compare("NULL", Qt::CaseInsensitive) == 0) {
        return QString();
    }
    if (s == "-") {
        return QString();
    }
    return s;
}

void BoardTraceManager::requestUiRefresh(bool immediate)
{
    if (immediate) {
        updateTimer_.stop();
        emit recordsUpdated();
        return;
    }

    if (!updateTimer_.isActive()) {
        updateTimer_.start();
    }
}
