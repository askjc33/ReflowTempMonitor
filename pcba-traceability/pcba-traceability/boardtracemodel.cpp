#include "boardtracemodel.h"
#include <algorithm>

BoardTraceModel::BoardTraceModel(QObject *parent) : QObject(parent)
{
}

QList<BoardRecord> BoardTraceModel::allRecords() const
{
    QList<BoardRecord> list;
    list.reserve(activeBoards_.size() + historyBoards_.size());

    for (auto it = activeBoards_.cbegin(); it != activeBoards_.cend(); ++it) {
        list.append(it.value());
    }
    for (const BoardRecord &r : historyBoards_) {
        list.append(r);
    }

    std::sort(list.begin(), list.end(), [](const BoardRecord &a, const BoardRecord &b) {
        return a.enterTime > b.enterTime;
    });
    return list;
}

bool BoardTraceModel::getRecordByBarcode(const QString &barcode, BoardRecord &out) const
{
    if (activeBoards_.contains(barcode)) {
        out = activeBoards_.value(barcode);
        return true;
    }

    for (const BoardRecord &r : historyBoards_) {
        if (r.barcode == barcode) {
            out = r;
            return true;
        }
    }
    return false;
}

bool BoardTraceModel::containsActive(const QString &barcode) const
{
    return activeBoards_.contains(barcode);
}

BoardRecord BoardTraceModel::activeRecord(const QString &barcode) const
{
    return activeBoards_.value(barcode);
}

void BoardTraceModel::upsertActiveRecord(const QString &barcode, const BoardRecord &record)
{
    activeBoards_[barcode] = record;
}

void BoardTraceModel::removeActive(const QString &barcode)
{
    activeBoards_.remove(barcode);
}

QList<QString> BoardTraceModel::activeKeys() const
{
    return activeBoards_.keys();
}

void BoardTraceModel::prependHistory(const BoardRecord &record)
{
    historyBoards_.prepend(record);
    while (historyBoards_.size() > MAX_HISTORY_BOARDS) {
        historyBoards_.removeLast();
    }
}

void BoardTraceModel::clearFinished()
{
    historyBoards_.clear();
    emit modelChanged();
}
