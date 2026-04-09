#pragma once

#include <QWidget>
#include <QVector>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

class QLabel;
class QTableWidget;

#include "serial_manager.h"
#include "board_trace_manager.h"

class TraceabilityPage : public QWidget
{
    Q_OBJECT
public:
    explicit TraceabilityPage(SerialManager *serial, BoardTraceManager *manager, QWidget *parent = 0);

signals:
    void requestDisconnectAndBack();
    void requestSetStatusMessage(const QString &text);
    void requestSetConnectionStatus(const QString &text, bool connected);
    void requestSetBatteryLevel(int value);

private slots:
    void refreshView();    //表格刷新
    void onTableCellClicked(int row, int column);
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
    void showBoard(const QString &barcode);
    //void showBoards(const QString &barcodes);
    void clearBoardDisplay();
    void reselectRowByBarcode(const QString &barcode);

private:
    SerialManager *serial_;
    BoardTraceManager *manager_;

    QLabel *batteryLabel_;
    QLabel *barcodeValue_;
    QLabel *statusValue_;
    QLabel *enterValue_;
    QLabel *exitValue_;
    QLabel *durationValue_;
    QLabel *zoneValue_;

    QChartView *chartView_;
    QChart *chart_;
    QLineSeries *series_;

//    // 新增多板对比
//    QMap<QString, QLineSeries*> boardSeriesMap_; // 板码 -> 折线
//    QVector<QColor> seriesColors_ = {
//        Qt::red, Qt::blue, Qt::green, Qt::magenta,
//        Qt::cyan, Qt::yellow, Qt::gray, Qt::darkRed,
//        Qt::darkBlue, Qt::darkGreen, Qt::darkMagenta
//    };
//    int colorIndex_ = 0; // 用于循环选择颜色

    QValueAxis *axisX_;
    QValueAxis *axisY_;
    QVector<QLineSeries*> zoneRefLines_;

    QTableWidget *table_;
    QString selectedBarcode_;

    const double totalProcessTimeSec_ = 120.0;
};
