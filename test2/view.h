#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include <QStackedWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QGroupBox>
#include <QFrame>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include "model.h"
#include "dataparser.h"

// 连接页面（串口选择、连接）
class ConnectionPage : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionPage(QWidget *parent = nullptr);

signals:
    void scanRequested();
    void connectRequested(const QString &portName);

public slots:
    void onScanFinished(const QList<PortItem> &ports);

private slots:
    void onItemClicked(QListWidgetItem *item);
    void onScanClicked();
    void onConnectClicked();

private:
    void initUi();

    QListWidget *portList_ = nullptr;
    QLabel *infoLabel_ = nullptr;
    QPushButton *scanBtn_ = nullptr;
    QPushButton *connectBtn_ = nullptr;
    QList<PortItem> ports_;
    QString selectedPort_;
};

// 追溯页面（过炉记录、温度曲线）
class TraceabilityPage : public QWidget
{
    Q_OBJECT
public:
    explicit TraceabilityPage(QWidget *parent = nullptr);

    QString selectedBarcode() const { return selectedBarcode_; }

signals:
    void disconnectRequested();
    void clearFinishedRequested();
    void exportRequested();
    void boardSelected(const QString &barcode);

public slots:
    void refreshRecordList(const QList<BoardRecord> &records);
    void showBoardRecord(const BoardRecord &r);
    void clearBoardDisplay();
    void updateBattery(int value);

private slots:
    void onTableCellClicked(int row, int);
    void onExportClicked();
    void onClearClicked();
    void onDisconnectClicked();

private:
    void initUi();
    void initChart();
    void reselectRowByBarcode(const QString &barcode);

    QLabel *batteryLabel_ = nullptr;
    QLabel *barcodeValue_ = nullptr;
    QLabel *statusValue_ = nullptr;
    QLabel *enterValue_ = nullptr;
    QLabel *exitValue_ = nullptr;
    QLabel *durationValue_ = nullptr;
    QLabel *zoneValue_ = nullptr;

    QChartView *chartView_ = nullptr;
    QChart *chart_ = nullptr;
    QLineSeries *series_ = nullptr;
    QValueAxis *axisX_ = nullptr;
    QValueAxis *axisY_ = nullptr;
    QVector<QLineSeries*> zoneRefLines_;

    QTableWidget *table_ = nullptr;
    QString selectedBarcode_;
    const double totalProcessTimeSec_ = 120.0;
};

#endif // VIEW_H
