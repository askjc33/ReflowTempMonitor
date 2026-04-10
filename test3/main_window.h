#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QStackedWidget;
class QLabel;
class QTimer;
QT_END_NAMESPACE

#include "serial_manager.h"
#include "board_trace_manager.h"
#include "traceability_page.h"
#include "connection_page.h"

// ===== 新增：温区设置相关 =====
#include "reflow_settings.h"
#include "zone_settings_page.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void switchToConnectionPage();
    void switchToTracePage();
    void switchToZoneSettingsPage();
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

    // ===== 新增：回流炉温区长度/阈值配置 =====
    ReflowSettings reflowSettings_;

    QStackedWidget *stacked_ = nullptr;
    ConnectionPage *connPage_ = nullptr;
    TraceabilityPage *tracePage_ = nullptr;

    // ===== 新增：阈值和温度区设置页面 =====
    ZoneSettingsPage *zoneSettingsPage_ = nullptr;

    QLabel *msgLabel_ = nullptr;
    QLabel *connLabel_ = nullptr;
    QLabel *batteryLabel_ = nullptr;
    QLabel *timeLabel_ = nullptr;
    QTimer *clockTimer_ = nullptr;
};
