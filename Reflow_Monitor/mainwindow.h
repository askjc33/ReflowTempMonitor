#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class QLabel;
class SerialManager;
class ConnectionPage;
class DataMonitorPage;
class PresetPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void switchToConnectionPage();
    void switchToMonitorPage();
    void switchToPresetPage();
    void exportData();
    void showAbout();
    void updateClock();

private:
    void setupMenuBar();
    void setupStatusBar();

private:
    SerialManager *m_serialManager;
    QStackedWidget *m_stackedWidget;
    ConnectionPage *m_connectionPage;
    DataMonitorPage *m_monitorPage;
    PresetPage *m_presetPage;

    QLabel *m_msgLabel;
    QLabel *m_connLabel;
    QLabel *m_batteryLabel;
    QLabel *m_timeLabel;
};

#endif // MAINWINDOW_H
