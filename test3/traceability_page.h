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
class ReflowSettings;
struct BoardRecord;

// ===== 新增：曲线页支持温区设置联动 =====

class TraceabilityPage : public QWidget
{
    Q_OBJECT

public:
    explicit TraceabilityPage(SerialManager *serial,
                              BoardTraceManager *manager,
                              ReflowSettings *settings,
                              QWidget *parent = nullptr);

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

    // ===== 新增：温区参数变化后重建图表参考线 =====
    void refreshChartReferences();

private:
    void initUi();
    void initChart();

    // ===== 新增：横坐标与温区参考线更新 =====
    void updateAxisRange();
    void rebuildReferenceLines();
    void clearReferenceLineList(QList<QtCharts::QLineSeries*> &lines);
    QString buildAlarmText(const BoardRecord &record) const;
    QColor getNextLineColor();
    void showBoard(const QString &barcode);
    void refreshSelectedBoardInfo();
    void clearBoardDisplay();

    SerialManager *serial_;
    BoardTraceManager *manager_;
    ReflowSettings *settings_;

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

    // ===== 新增：温区竖线 / 阈值横线 =====
    QList<QtCharts::QLineSeries*> zoneRefLines_;
    QList<QtCharts::QLineSeries*> thresholdRefLines_;

    QTableWidget *table_;

    QSet<QString> selectedBarcodes_;
    QMap<QString, QtCharts::QLineSeries*> multiSeries_;
};

#endif // TRACEABILITY_PAGE_H
