#ifndef DATAMONITORPAGE_H
#define DATAMONITORPAGE_H

#include <QWidget>
#include <QVector>
#include <QtGlobal>

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

class QLabel;
class QPushButton;
class QCheckBox;
class SerialManager;
class PresetPage;

class DataMonitorPage : public QWidget
{
    Q_OBJECT

public:
    explicit DataMonitorPage(SerialManager *manager, QWidget *parent = nullptr);

    void setPresetPage(PresetPage *page);
    void setReady();

    QVector<double> timeData() const;
    QVector<QVector<double>> channelData() const;

signals:
    void requestBackToConnection();
    void requestGoPreset();

private slots:
    void startMonitoring();
    void stopMonitoring();
    void clearData();
    void onTemperaturesReceived(const QVector<double> &values);
    void onBatteryReceived(int value);
    void onDisconnected();
    void loadPresetCurves();
    void syncThresholdsFromPreset();
    void disconnectDevice();

private:
    void rebuildChart();
    void updateThresholdLines();

private:
    static const int NUM_CHANNELS = 5;

    SerialManager *m_manager;
    PresetPage *m_presetPage;

    QChart *m_chart;
    QChartView *m_chartView;
    QVector<QLineSeries*> m_series;
    QVector<QLineSeries*> m_thresholdSeries;

    QVector<double> m_timeData;
    QVector<QVector<double>> m_channelData;
    QVector<double> m_thresholdValues;

    qint64 m_startMs;

    QLabel *m_statusLabel;
    QLabel *m_batteryLabel;
    QLabel *m_thrInfoLabel;
    QVector<QLabel*> m_valueLabels;
    QVector<QLabel*> m_peakLabels;
    QVector<QLabel*> m_rateLabels;
    QVector<QCheckBox*> m_checkboxes;
    QVector<double> m_peakValues;
    QVector<double> m_prevValues;
    double m_prevTime;

    QPushButton *m_startBtn;
    QPushButton *m_stopBtn;
    QPushButton *m_backBtn;
};

#endif // DATAMONITORPAGE_H
