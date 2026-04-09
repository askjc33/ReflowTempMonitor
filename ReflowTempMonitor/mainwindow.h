#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QSplitter>
#include <QStatusBar>
#include <QTimer>
#include <QThread>
#include <QLineEdit>
#include <QSerialPortInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QString>
#include <QMap>

#include "serialworker.h"
#include "pcbdatamanager.h"
#include "temperaturechart.h"
#include "chambervisualizer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectClicked();
    void onPortOpened(bool ok, const QString &msg);
    void onPortClosed();
    void onPortError(const QString &msg);

    void onTemperatureReceived(const QString &pcbId,
                               float t0, float t1, float t2, float t3, float t4,
                               int chamber);
    void onBarcodeReceived(const QString &pcbId);

    void onPcbListItemClicked(QListWidgetItem *item);
    void onPcbRegistered(const QString &id);
    void onPcbDataUpdated(const QString &id);
    void onPcbLeft(const QString &id);

    void onRefreshTimer();
    void onSimulateClicked();
    void onManualScanClicked();
    void onClearClicked();

private:
    void setupUi();
    void setupConnections();
    void setupSerialPanel(QWidget *parent, QVBoxLayout *layout);
    void setupPcbListPanel(QWidget *parent, QVBoxLayout *layout);
    void setupChartPanel(QWidget *parent, QVBoxLayout *layout);
    void refreshPortList();
    void updatePcbListItem(const QString &id);
    void selectPcb(const QString &id);
    void applyDarkStyle();
    void applyLightStyle();

private:
    QThread       *m_serialThread;
    SerialWorker  *m_serialWorker;
    bool           m_connected;

    PcbDataManager *m_dataManager;
    QString         m_selectedId;

    QTimer *m_refreshTimer;
    QTimer *m_simTimer;

    int    m_simChamber;
    float  m_simTime;
    QMap<QString, int> m_simPcbChamber;

    QComboBox   *m_portCombo;
    QComboBox   *m_baudCombo;
    QPushButton *m_connectBtn;
    QPushButton *m_refreshPortBtn;
    QLabel      *m_connStatusLabel;

    QListWidget *m_pcbListWidget;
    QLabel      *m_pcbCountLabel;
    QPushButton *m_clearBtn;

    QLineEdit   *m_manualScanEdit;
    QPushButton *m_manualScanBtn;
    QPushButton *m_simulateBtn;

    QLabel      *m_pcbIdLabel;
    QLabel      *m_chamberLabel;
    QLabel      *m_enterTimeLabel;
    QLabel      *m_sampleCountLabel;

    ChamberVisualizer *m_chamberVis;
    TemperatureChart  *m_tempChart;

    QLabel *m_tempLabels[CHAMBER_COUNT];

    QLabel *m_statusLabel;
};
