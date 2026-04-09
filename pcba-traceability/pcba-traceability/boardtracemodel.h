#pragma once

#include "boardrecord.h"
#include "constants.h"
#include <QObject>
#include <QMap>
#include <QList>

class BoardTraceModel : public QObject
{
    Q_OBJECT
public:
    explicit BoardTraceModel(QObject *parent = nullptr);

    QList<BoardRecord> allRecords() const;
    bool getRecordByBarcode(const QString &barcode, BoardRecord &out) const;

    bool containsActive(const QString &barcode) const;
    BoardRecord activeRecord(const QString &barcode) const;
    void upsertActiveRecord(const QString &barcode, const BoardRecord &record);
    void removeActive(const QString &barcode);
    QList<QString> activeKeys() const;

    void prependHistory(const BoardRecord &record);
    void clearFinished();

signals:
    void modelChanged();

private:
    QMap<QString, BoardRecord> activeBoards_;
    QList<BoardRecord> historyBoards_;
};
