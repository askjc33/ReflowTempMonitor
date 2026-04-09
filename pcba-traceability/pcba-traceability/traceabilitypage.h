#pragma once

#include "boardrecord.h"
#include "constants.h"
#include <QWidget>
#include <QMetaType>

namespace QtCharts {
class QChartView;
class QChart;
class QLineSeries;
class QValueAxis;
}

class QLabel;
class QTableWidget;

class TraceabilityPage : public QWidget
{
    Q_OBJECT
public:
    explicit TraceabilityPage(QWidget *parent = nullptr);

    void setRecords(const QList<BoardRecord> &records);
    QString selectedBarcode() const;
    void clearUi();
    void setBatteryText(const QString &text);
void updateRecords(const QList<BoardRecord> &records);

signals:
    void requestDisconnect();
    void requestClearFinished();
    void requestExportCurrent(const QString &barcode);
    void selectedBarcodeChanged(const QString &barcode);

private slots:
    void onTableCellClicked(int row, int column);
    void onDisconnectClicked();
    void onClearClicked();
    void onExportClicked();


private:
    void initUi();
    void initChart();
    void addZoneReferenceLines();
    void showBoard(const BoardRecord &r);
    void clearBoardDisplay();
    void reselectRowByBarcode(const QString &barcode);

private:
    QLabel *batteryLabel_ = nullptr;
    QLabel *barcodeValue_ = nullptr;
    QLabel *statusValue_ = nullptr;
    QLabel *enterValue_ = nullptr;
    QLabel *exitValue_ = nullptr;
    QLabel *durationValue_ = nullptr;
    QLabel *zoneValue_ = nullptr;

    QtCharts::QChartView *chartView_ = nullptr;
    QtCharts::QChart *chart_ = nullptr;
    QtCharts::QLineSeries *series_ = nullptr;
    QtCharts::QValueAxis *axisX_ = nullptr;
    QtCharts::QValueAxis *axisY_ = nullptr;
    QVector<QtCharts::QLineSeries*> zoneRefLines_;

    QTableWidget *table_ = nullptr;
    QList<BoardRecord> records_;
    QString selectedBarcode_;
    const double totalProcessTimeSec_ = DEFAULT_TOTAL_PROCESS_TIME_SEC;
};
