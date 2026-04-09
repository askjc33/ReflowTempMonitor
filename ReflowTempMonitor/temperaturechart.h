#pragma once
#include <QWidget>
#include <QVector>
#include <QString>
#include "pcbdatamanager.h"

// 颜色对应5个气室的曲线
static const QColor CHAMBER_COLORS[CHAMBER_COUNT] = {
    QColor(255,  80,  80),   // 气室0 红
    QColor( 80, 200,  80),   // 气室1 绿
    QColor( 80, 160, 255),   // 气室2 蓝
    QColor(255, 210,  60),   // 气室3 黄
    QColor(200,  80, 255),   // 气室4 紫
};

class TemperatureChart : public QWidget
{
    Q_OBJECT
public:
    explicit TemperatureChart(QWidget *parent = nullptr);

    // 设置要显示的PCB记录，nullptr 则清空
    void setPcbRecord(const PcbRecord *record);

    // 设置温度显示范围
    void setTempRange(float minT, float maxT);

    // 设置时间窗口（毫秒，0=全显示）
    void setTimeWindow(qint64 windowMs);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;

private:
    // 坐标映射
    QRect  chartRect() const;           // 绘图区（去掉边距）
    float  tempToY(float temp) const;   // 温度 -> 像素Y
    float  timeToX(qint64 t) const;     // 时间戳 -> 像素X

    void drawBackground(QPainter &p);
    void drawGrid(QPainter &p);
    void drawAxes(QPainter &p);
    void drawCurves(QPainter &p);
    void drawLegend(QPainter &p);
    void drawCurrentMarker(QPainter &p);    // 标记当前时刻竖线
    void drawNoDataHint(QPainter &p);

    const PcbRecord *m_record;  // 不拥有，只读指针

    float  m_minTemp;
    float  m_maxTemp;
    qint64 m_timeWindowMs;  // 0=全部显示

    // 缓存，避免每次paintEvent重算
    qint64 m_tMin;
    qint64 m_tMax;

    // 边距
    static const int MARGIN_LEFT   = 60;
    static const int MARGIN_RIGHT  = 20;
    static const int MARGIN_TOP    = 20;
    static const int MARGIN_BOTTOM = 40;
};
