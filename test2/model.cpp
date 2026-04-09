#include "model.h"
#include <algorithm>

Model::Model(QObject *parent)
    : QObject{parent}
{
    updateTimer_.setInterval(200);
    updateTimer_.setSingleShot(true);
    connect(&updateTimer_, &QTimer::timeout, this, &Model::recordsUpdated);
}

QList<BoardRecord> Model::getAllRecords() const
{
    // 拆分两组：已完成（旧板）、过炉中（新板）
    QList<BoardRecord> finishedList;
    QList<BoardRecord> activeList;

    for (const auto &r : activeBoards_)
        activeList.append(r);
    for (const auto &r : historyBoards_)
        finishedList.append(r);

    // 已完成按进板时间从旧到新排序（顶置）
    std::sort(finishedList.begin(), finishedList.end(), [](sslocal://flow/file_open?url=const+BoardRecord+%26a%2C+const+BoardRecord+%26b&flow_extra=eyJsaW5rX3R5cGUiOiJjb2RlX2ludGVycHJldGVyIn0=) {
        return a.enterTime < b.enterTime;
    });

    // 过炉中按进板时间从旧到新排序（排在后面）
    std::sort(activeList.begin(), activeList.end(), [](sslocal://flow/file_open?url=const+BoardRecord+%26a%2C+const+BoardRecord+%26b&flow_extra=eyJsaW5rX3R5cGUiOiJjb2RlX2ludGVycHJldGVyIn0=) {
        return a.enterTime < b.enterTime;
    });

    // 合并：旧板(完成) → 新板(过炉中)
    QList<BoardRecord> list;
    list.reserve(finishedList.size() + activeList.size());
    list.append(finishedList);
    list.append(activeList);

    return list;
}

bool Model::getRecordByBarcode(const QString &barcode, BoardRecord &out) const
{
    if (activeBoards_.contains(barcode)) {
        out = activeBoards_.value(barcode);
        return true;
    }

    for (const auto &r : historyBoards_) {
        if (r.barcode == barcode) {
            out = r;
            return true;
        }
    }
    return false;
}

void Model::clearFinishedRecords()
{
    historyBoards_.clear();
    requestUiRefresh(true);
}

void Model::onSynchronizedFrame(const QVector<double> &temps, const QStringList &positions)
{
    if (temps.size() != ZONE_COUNT || positions.size() != ZONE_COUNT) return;

    const QDateTime now = QDateTime::currentDateTime();
    const qint64 nowMs = now.toMSecsSinceEpoch();

    QSet<QString> presentBoards;
    presentBoards.reserve(ZONE_COUNT);

    // 遍历12个气室，更新对应板的记录
    for (int zone = 0; zone < ZONE_COUNT; ++zone) {
        QString barcode = normalizeBarcode(positions[zone]);
        if (barcode.isEmpty()) continue;

        presentBoards.insert(barcode);

        // 新板：创建记录
        if (!activeBoards_.contains(barcode)) {
            BoardRecord r;
            r.barcode = barcode;
            r.enterTime = now;
            r.status = "过炉中";
            r.startMs = nowMs;
            r.zoneTimes.resize(ZONE_COUNT);
            r.zoneTemps.resize(ZONE_COUNT);
            activeBoards_[barcode] = r;
        }

        // 更新板的温度、时间等数据
        BoardRecord &record = activeBoards_[barcode];
        double elapsed = (nowMs - record.startMs) / 1000.0;
        double temp = temps[zone];

        record.lastZone = zone + 1;
        record.missingFrames = 0;

        record.timeAxis.append(elapsed);
        record.fullTemps.append(temp);
        record.zoneAxis.append(zone + 1);

        record.zoneTimes[zone].append(elapsed);
        record.zoneTemps[zone].append(temp);
    }

    // 处理超时未检测到的板（标记为已完成）
    QList<QString> toFinish;
    for (auto it = activeBoards_.begin(); it != activeBoards_.end(); ++it) {
        if (!presentBoards.contains(it.key())) {
            it.value().missingFrames++;

            if (it.value().missingFrames >= finishMissingFrameThreshold_) {
                BoardRecord finished = it.value();
                finished.exitTime = now;
                finished.status = "已完成";
                finished.finished = true;
                historyBoards_.prepend(finished);
                toFinish.append(it.key());
            }
        }
    }

    // 从活跃列表移除已完成的板
    for (const QString &key : toFinish) {
        activeBoards_.remove(key);
    }

    // 限制历史记录数量
    while (historyBoards_.size() > maxHistoryBoards_) {
        historyBoards_.removeLast();
    }

    requestUiRefresh(false);
}

QString Model::normalizeBarcode(const QString &raw)
{
    QString s = raw.trimmed();
    if (s.isEmpty()) return QString();
    if (s.compare("EMPTY", Qt::CaseInsensitive) == 0) return QString();
    if (s.compare("NULL", Qt::CaseInsensitive) == 0) return QString();
    if (s == "-") return QString();
    return s;
}

void Model::requestUiRefresh(bool immediate)
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
