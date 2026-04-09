#pragma once

#include <QMainWindow>

class QStackedWidget;
class QLabel;
class QTimer;
class QCloseEvent;

#include "serial_manager.h"
#include "board_trace_manager.h"
#include "connection_page.h"
#include "traceability_page.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void switchToConnectionPage();
    void switchToTracePage();
    void updateClock();
    void setStatusMessage(const QString &text);
    void setConnectionStatus(const QString &text, bool connected);
    void setBatteryLevel(int v);
    void showAbout();

private:
    void initUi();
    void wireSignals();
    void setupMenu();
    void setupStatusBar();

private:
    SerialManager serial_;
    BoardTraceManager traceManager_;

    QStackedWidget *stacked_;
    ConnectionPage *connPage_;
    TraceabilityPage *tracePage_;

    QLabel *msgLabel_;
    QLabel *connLabel_;
    QLabel *batteryLabel_;
    QLabel *timeLabel_;
    QTimer *clockTimer_;
};
