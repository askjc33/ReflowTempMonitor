#ifndef TRACEABILITY_PAGE_H
#define TRACEABILITY_PAGE_H

#include <QWidget>
#include <QSet>
#include <QMap>
#include <QColor>
#include <QTableWidget>
#include <QLabel>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>

class SerialManager;
class BoardTraceManager;
struct BoardRecord;

class TraceabilityPage : public QWidget
{
    Q_OBJECT

public:
    explicit TraceabilityPage(SerialManager *serial, BoardTraceManager *manager, QWidget *parent = nullptr);

signals:
    void requestSetStatusMessage(const QString &text);
    void requestSetConnectionStatus(const QString &text, bool connected);
    void requestSetBatteryLevel(int value);
    void requestDisconnectAndBack();

private slots:
    void refreshView();
    void onTableCellClicked(int row, int);
    void onBatteryReceived(int value);
    void disconnectDevice();
    void onSerialError(const QString &msg);
    void onDisconnected();
    void clearFinishedRecords();
    void exportCurrentBoard();

private:
    void initUi();
    void initChart();
    void addZoneReferenceLines();
    QColor getNextLineColor();
    void showBoard(const QString &barcode);
    void refreshSelectedBoardInfo();
    void clearBoardDisplay();

    SerialManager *serial_;
    BoardTraceManager *manager_;

    QLabel *batteryLabel_;
    QLabel *barcodeValue_;
    QLabel *statusValue_;
    QLabel *enterValue_;
    QLabel *exitValue_;
    QLabel *durationValue_;
    QLabel *zoneValue_;

    QtCharts::QChartView *chartView_;
    QtCharts::QChart *chart_;
    QtCharts::QValueAxis *axisX_;
    QtCharts::QValueAxis *axisY_;
    QList<QtCharts::QLineSeries*> zoneRefLines_;

    QTableWidget *table_;

    QSet<QString> selectedBarcodes_;
    QMap<QString, QtCharts::QLineSeries*> multiSeries_;
};

#endif // TRACEABILITY_PAGE_H
