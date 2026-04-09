#pragma once

#include "boardrecord.h"
#include <QObject>
#include <QList>

class MainWindow;
class QThread;
class SerialWorker;
class TraceWorker;

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(MainWindow *window, QObject *parent = nullptr);
    ~AppController();

    void initialize();

private:
    void setupThreads();
    void setupConnections();
    void shutdownThreads();
    void exportBoardByBarcode(const QString &barcode);

private:
    MainWindow *window_ = nullptr;

    QThread *serialThread_ = nullptr;
    QThread *traceThread_ = nullptr;

    SerialWorker *serialWorker_ = nullptr;
    TraceWorker *traceWorker_ = nullptr;

    QList<BoardRecord> latestRecords_;
};
