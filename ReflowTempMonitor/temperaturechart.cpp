#include "temperaturechart.h"
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <cmath>

TemperatureChart::TemperatureChart(QWidget *parent)
    : QWidget(parent)
    , m_record(nullptr)
    , m_minTemp(0.0f)
    , m_maxTemp(300.0f)
    , m_timeWindowMs(0)
    , m_tMin(0)
    , m_tMax(0)
{
    setMinimumSize(400, 280);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

QSize TemperatureChart::sizeHint() const
{
    return QSize(860, 380);
}

void TemperatureChart::setPcbRecord(const PcbRecord *record)
{
    m_record = record;
    update();
}

void TemperatureChart::setTempRange(float minT, float maxT)
{
    m_minTemp = minT;
    m_maxTemp = maxT;
    update();
}

void TemperatureChart::setTimeWindow(qint64 windowMs)
{
    m_timeWindowMs = windowMs;
    update();
}

void TemperatureChart::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    update();
}

// ── 辅助：计算绘图区矩形 ──────────────────────────────────────────────────────
QRect TemperatureChart::chartRect() const
{
    return QRect(
        MARGIN_LEFT,
        MARGIN_TOP,
        width()  - MARGIN_LEFT - MARGIN_RIGHT,
        height() - MARGIN_TOP  - MARGIN_BOTTOM
    );
}

float TemperatureChart::tempToY(float temp) const
{
    QRect cr = chartRect();
    float ratio = (temp - m_minTemp) / (m_maxTemp - m_minTemp);
    // 温度高 → Y小（靠上）
    return cr.bottom() - ratio * cr.height();
}

float TemperatureChart::timeToX(qint64 t) const
{
    QRect cr = chartRect();
    if (m_tMax == m_tMin) return cr.left();
    float ratio = (float)(t - m_tMin) / (float)(m_tMax - m_tMin);
    return cr.left() + ratio * cr.width();
}

// ── 背景 ─────────────────────────────────────────────────────────────────────
void TemperatureChart::drawBackground(QPainter &p)
{
    // 整体深色背景
    p.fillRect(rect(), QColor(22, 22, 32));

    // 绘图区轻微渐变
    QRect cr = chartRect();
    QLinearGradient bg(cr.topLeft(), cr.bottomLeft());
    bg.setColorAt(0, QColor(30, 32, 48));
    bg.setColorAt(1, QColor(18, 20, 30));
    p.fillRect(cr, bg);
}

// ── 网格 ─────────────────────────────────────────────────────────────────────
void TemperatureChart::drawGrid(QPainter &p)
{
    QRect cr = chartRect();

    // 温度网格线：每隔50度一条
    float tempStep = 50.0f;
    float tStart   = std::ceil(m_minTemp / tempStep) * tempStep;

    QPen gridPen(QColor(60, 65, 90), 1, Qt::DashLine);
    p.setPen(gridPen);

    for (float t = tStart; t <= m_maxTemp; t += tempStep) {
        int y = (int)tempToY(t);
        if (y < cr.top() || y > cr.bottom()) continue;
        p.drawLine(cr.left(), y, cr.right(), y);
    }

    // 时间网格线：均分10条
    if (m_tMax > m_tMin) {
        int steps = 10;
        for (int i = 1; i < steps; ++i) {
            qint64 t = m_tMin + (qint64)i * (m_tMax - m_tMin) / steps;
            int x = (int)timeToX(t);
            p.drawLine(x, cr.top(), x, cr.bottom());
        }
    }
}

// ── 坐标轴及刻度标签 ─────────────────────────────────────────────────────────
void TemperatureChart::drawAxes(QPainter &p)
{
    QRect cr = chartRect();

    // 坐标轴外框
    p.setPen(QPen(QColor(120, 130, 160), 2));
    p.drawRect(cr);

    QFont font = p.font();
    font.setPixelSize(11);
    p.setFont(font);
    p.setPen(QColor(180, 185, 210));

    // Y轴刻度（温度）
    float tempStep = 50.0f;
    float tStart   = std::ceil(m_minTemp / tempStep) * tempStep;
    for (float t = tStart; t <= m_maxTemp; t += tempStep) {
        int y = (int)tempToY(t);
        if (y < cr.top() || y > cr.bottom()) continue;
        QString label = QString::number((int)t) + "°C";
        QRect lr(0, y - 10, MARGIN_LEFT - 5, 20);
        p.drawText(lr, Qt::AlignRight | Qt::AlignVCenter, label);
        // 刻度短线
        p.drawLine(cr.left() - 4, y, cr.left(), y);
    }

    // X轴刻度（时间，相对秒）
    if (m_tMax > m_tMin) {
        int steps = 10;
        for (int i = 0; i <= steps; ++i) {
            qint64 t = m_tMin + (qint64)i * (m_tMax - m_tMin) / steps;
            int    x = (int)timeToX(t);
            float  s = (t - m_tMin) / 1000.0f;
            QString label = QString::number((int)s) + "s";
            QRect lr(x - 25, cr.bottom() + 5, 50, 18);
            p.drawText(lr, Qt::AlignHCenter | Qt::AlignTop, label);
            p.drawLine(x, cr.bottom(), x, cr.bottom() + 4);
        }
    }

    // Y轴标题
    p.save();
    p.translate(12, cr.center().y());
    p.rotate(-90);
    font.setPixelSize(12);
    font.setBold(true);
    p.setFont(font);
    p.setPen(QColor(200, 205, 230));
    p.drawText(QRect(-60, -10, 120, 20), Qt::AlignCenter, "温度 (°C)");
    p.restore();

    // X轴标题
    font.setPixelSize(12);
    font.setBold(true);
    p.setFont(font);
    p.drawText(QRect(cr.left(), cr.bottom() + 22, cr.width(), 16),
               Qt::AlignCenter, "时间 (秒)");
}

// ── 温度曲线 ─────────────────────────────────────────────────────────────────
void TemperatureChart::drawCurves(QPainter &p)
{
    if (!m_record || m_record->samples.isEmpty()) return;

    QRect cr = chartRect();
    p.setClipRect(cr);

    for (int ch = 0; ch < CHAMBER_COUNT; ++ch) {
        QPainterPath path;
        bool first = true;

        for (const TempSample &s : m_record->samples) {
            if (s.timestamp < m_tMin || s.timestamp > m_tMax) continue;

            float x = timeToX(s.timestamp);
            float y = tempToY(s.temps[ch]);

            // 限制在绘图区内
            y = qBound((float)cr.top(), y, (float)cr.bottom());

            if (first) {
                path.moveTo(x, y);
                first = false;
            } else {
                path.lineTo(x, y);
            }
        }

        if (first) continue; // 没有数据点

        // 描边（带发光感）
        QColor col = CHAMBER_COLORS[ch];

        // 外层光晕
        QPen glowPen(QColor(col.red(), col.green(), col.blue(), 60), 5);
        glowPen.setCapStyle(Qt::RoundCap);
        glowPen.setJoinStyle(Qt::RoundJoin);
        p.setPen(glowPen);
        p.setBrush(Qt::NoBrush);
        p.drawPath(path);

        // 主曲线
        QPen mainPen(col, 2);
        mainPen.setCapStyle(Qt::RoundCap);
        mainPen.setJoinStyle(Qt::RoundJoin);
        p.setPen(mainPen);
        p.drawPath(path);

        // 最新数据点：小圆圈
        const TempSample &last = m_record->samples.last();
        if (last.timestamp >= m_tMin) {
            float lx = timeToX(last.timestamp);
            float ly = qBound((float)cr.top(),
                              tempToY(last.temps[ch]),
                              (float)cr.bottom());
            p.setPen(QPen(Qt::white, 1));
            p.setBrush(col);
            p.drawEllipse(QPointF(lx, ly), 4, 4);
        }
    }

    p.setClipping(false);
}

// ── 图例 ─────────────────────────────────────────────────────────────────────
void TemperatureChart::drawLegend(QPainter &p)
{
    QRect cr = chartRect();
    int   lx = cr.right() - 140;
    int   ly = cr.top() + 10;

    // 图例背景
    QRect legendBg(lx - 8, ly - 6, 140, CHAMBER_COUNT * 22 + 10);
    p.setPen(QPen(QColor(100, 110, 140), 1));
    p.setBrush(QColor(30, 32, 50, 200));
    p.drawRoundedRect(legendBg, 5, 5);

    QFont font = p.font();
    font.setPixelSize(11);
    p.setFont(font);

    // 如果有最新数据，显示最新温度
    const TempSample *lastSample = nullptr;
    if (m_record && !m_record->samples.isEmpty())
        lastSample = &m_record->samples.last();

    for (int ch = 0; ch < CHAMBER_COUNT; ++ch) {
        int  iy  = ly + ch * 22;
        QColor col = CHAMBER_COLORS[ch];

        // 色块
        p.setPen(Qt::NoPen);
        p.setBrush(col);
        p.drawRoundedRect(lx, iy + 3, 18, 10, 2, 2);

        // 标签
        p.setPen(QColor(210, 215, 240));
        QString label = QString("气室%1").arg(ch + 1);
        if (lastSample)
            label += QString("  %1°C").arg(lastSample->temps[ch], 0, 'f', 1);
        p.drawText(lx + 24, iy, 110, 18, Qt::AlignLeft | Qt::AlignVCenter, label);
    }
}

// ── 当前时刻竖线标记 ─────────────────────────────────────────────────────────
void TemperatureChart::drawCurrentMarker(QPainter &p)
{
    if (!m_record || m_record->samples.isEmpty()) return;

    QRect cr = chartRect();
    qint64 lastTs = m_record->samples.last().timestamp;
    if (lastTs < m_tMin || lastTs > m_tMax) return;

    int x = (int)timeToX(lastTs);

    // 虚线
    QPen pen(QColor(255, 220, 100, 180), 1, Qt::DashLine);
    p.setPen(pen);
    p.drawLine(x, cr.top(), x, cr.bottom());

    // "NOW" 标签
    QFont font = p.font();
    font.setPixelSize(10);
    p.setFont(font);
    p.setPen(QColor(255, 220, 100));
    p.drawText(x + 3, cr.top() + 13, "NOW");
}

// ── 无数据提示 ────────────────────────────────────────────────────────────────
void TemperatureChart::drawNoDataHint(QPainter &p)
{
    QFont font = p.font();
    font.setPixelSize(16);
    font.setBold(true);
    p.setFont(font);
    p.setPen(QColor(100, 110, 140));
    p.drawText(chartRect(), Qt::AlignCenter,
               m_record ? "暂无温度数据" : "请选择一块 PCB");
}

// ── 主绘制入口 ────────────────────────────────────────────────────────────────
void TemperatureChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    drawBackground(p);

    // 计算时间范围
    if (m_record && !m_record->samples.isEmpty()) {
        m_tMin = m_record->samples.first().timestamp;
        m_tMax = m_record->samples.last().timestamp;

        if (m_timeWindowMs > 0 && (m_tMax - m_tMin) > m_timeWindowMs) {
            m_tMin = m_tMax - m_timeWindowMs;
        }

        // 保证最小时间跨度（至少显示10秒）
        if (m_tMax - m_tMin < 10000)
            m_tMax = m_tMin + 10000;
    }

    drawGrid(p);
    drawAxes(p);

    if (!m_record || m_record->samples.isEmpty()) {
        drawNoDataHint(p);
        return;
    }

    drawCurves(p);
    drawCurrentMarker(p);
    drawLegend(p);
}
