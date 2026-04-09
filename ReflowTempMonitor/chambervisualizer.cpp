#include "chambervisualizer.h"
#include <QPainter>
#include <QLinearGradient>
#include <QPolygon>
#include <QtMath>

ChamberVisualizer::ChamberVisualizer(QWidget *parent)
    : QWidget(parent)
    , m_current(-1)
{
    for (int i = 0; i < 5; ++i) {
        m_temps[i] = 0.0f;
    }

    setMinimumHeight(130);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize ChamberVisualizer::sizeHint() const
{
    return QSize(900, 140);
}

void ChamberVisualizer::setCurrentChamber(int chamber)
{
    if (chamber < -1) chamber = -1;
    if (chamber > 4)  chamber = 4;

    m_current = chamber;
    update();
}

void ChamberVisualizer::setTemperatures(const float temps[5])
{
    if (!temps) return;

    for (int i = 0; i < 5; ++i) {
        m_temps[i] = temps[i];
    }
    update();
}

void ChamberVisualizer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    const int W = width();
    const int H = height();

    // 背景
    p.fillRect(rect(), QColor(24, 26, 36));

    // 标题
    QFont titleFont = p.font();
    titleFont.setPixelSize(15);
    titleFont.setBold(true);
    p.setFont(titleFont);
    p.setPen(QColor(210, 220, 240));
    p.drawText(QRect(12, 6, W - 24, 24), Qt::AlignLeft | Qt::AlignVCenter,
               QStringLiteral("回流焊气室位置示意"));

    // 有效绘图区域
    const int topMargin    = 34;
    const int leftMargin   = 18;
    const int rightMargin  = 18;
    const int bottomMargin = 26;

    const int beltY = H - bottomMargin;
    const int totalWidth = W - leftMargin - rightMargin;
    const int sectionGap = 10;
    const int chamberCount = 5;
    const int chamberWidth = (totalWidth - sectionGap * (chamberCount - 1)) / chamberCount;
    const int chamberHeight = H - topMargin - bottomMargin - 18;

    // 传送带
    QRect beltRect(leftMargin, beltY - 10, totalWidth, 12);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(70, 75, 92));
    p.drawRoundedRect(beltRect, 4, 4);

    // 传送带滚轮
    p.setBrush(QColor(110, 115, 135));
    for (int i = 0; i <= 14; ++i) {
        int x = leftMargin + i * totalWidth / 14;
        p.drawEllipse(QPointF(x, beltY - 4), 4, 4);
    }

    // 方向箭头
    p.setPen(QPen(QColor(180, 190, 215), 2));
    p.setBrush(QColor(180, 190, 215));
    {
        int ax = W - 40;
        int ay = beltY - 25;
        QPolygon arrow;
        arrow << QPoint(ax, ay)
              << QPoint(ax + 18, ay + 8)
              << QPoint(ax, ay + 16);
        p.drawPolygon(arrow);
        p.drawLine(ax - 26, ay + 8, ax + 2, ay + 8);
    }

    // 温度颜色映射函数
    auto tempColor = [](float t) -> QColor {
        if (t < 80.0f) {
            return QColor(70, 130, 255);
        } else if (t < 150.0f) {
            return QColor(80, 200, 120);
        } else if (t < 220.0f) {
            return QColor(255, 185, 60);
        } else {
            return QColor(255, 95, 95);
        }
    };

    // 绘制5个气室
    for (int i = 0; i < chamberCount; ++i) {
        const int x = leftMargin + i * (chamberWidth + sectionGap);
        QRect chamberRect(x, topMargin, chamberWidth, chamberHeight);

        bool active = (i == m_current);
        QColor baseColor = tempColor(m_temps[i]);

        // 外发光
        if (active) {
            for (int g = 4; g >= 1; --g) {
                QColor glow(255, 220, 120, 24 * g);
                p.setPen(QPen(glow, g * 2));
                p.setBrush(Qt::NoBrush);
                p.drawRoundedRect(chamberRect.adjusted(-g, -g, g, g), 8, 8);
            }
        }

        // 主体渐变
        QLinearGradient grad(chamberRect.topLeft(), chamberRect.bottomLeft());
        if (active) {
            grad.setColorAt(0.0, baseColor.lighter(150));
            grad.setColorAt(1.0, baseColor.darker(170));
        } else {
            grad.setColorAt(0.0, QColor(58, 62, 80));
            grad.setColorAt(1.0, QColor(40, 43, 58));
        }

        p.setPen(QPen(active ? QColor(255, 220, 120) : QColor(90, 95, 120), active ? 2 : 1));
        p.setBrush(grad);
        p.drawRoundedRect(chamberRect, 8, 8);

        // 顶部标题
        QFont font = p.font();
        font.setPixelSize(13);
        font.setBold(true);
        p.setFont(font);
        p.setPen(active ? QColor(255, 245, 200) : QColor(210, 215, 235));
        p.drawText(QRect(x, topMargin + 8, chamberWidth, 22),
                   Qt::AlignHCenter | Qt::AlignVCenter,
                   QString("气室 %1").arg(i + 1));

        // 温度显示
        QFont tempFont = p.font();
        tempFont.setPixelSize(18);
        tempFont.setBold(true);
        p.setFont(tempFont);
        p.setPen(active ? QColor(255, 255, 255) : QColor(180, 190, 220));
        p.drawText(QRect(x, topMargin + 35, chamberWidth, 28),
                   Qt::AlignHCenter | Qt::AlignVCenter,
                   QString::number(m_temps[i], 'f', 1) + " °C");

        // 状态说明
        QFont stateFont = p.font();
        stateFont.setPixelSize(11);
        stateFont.setBold(false);
        p.setFont(stateFont);
        p.setPen(active ? QColor(255, 235, 160) : QColor(135, 145, 175));
        p.drawText(QRect(x, topMargin + 68, chamberWidth, 18),
                   Qt::AlignHCenter | Qt::AlignVCenter,
                   active ? QStringLiteral("当前所在位置") : QStringLiteral("等待通过"));

        // 底部编号条
        QRect tagRect(x + 16, chamberRect.bottom() - 24, chamberWidth - 32, 14);
        p.setPen(Qt::NoPen);
        p.setBrush(active ? QColor(255, 220, 120) : QColor(85, 90, 110));
        p.drawRoundedRect(tagRect, 6, 6);
    }

    // PCB当前位置标记
    if (m_current >= 0 && m_current < chamberCount) {
        const int x = leftMargin + m_current * (chamberWidth + sectionGap);
        const int pcbCenterX = x + chamberWidth / 2;
        const int pcbY = beltY - 22;

        // PCB板
        QRect pcbRect(pcbCenterX - 28, pcbY - 10, 56, 16);
        p.setPen(QPen(QColor(40, 120, 80), 1));
        p.setBrush(QColor(60, 170, 110));
        p.drawRoundedRect(pcbRect, 3, 3);

        // 焊盘点
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(220, 220, 180));
        for (int k = 0; k < 4; ++k) {
            p.drawEllipse(QPointF(pcbRect.left() + 10 + k * 12, pcbRect.center().y()), 2.2, 2.2);
        }

        // 向上指示箭头
        p.setPen(QPen(QColor(255, 220, 120), 2));
        p.setBrush(QColor(255, 220, 120));
        QPolygon tri;
        tri << QPoint(pcbCenterX, pcbY - 18)
            << QPoint(pcbCenterX - 7, pcbY - 6)
            << QPoint(pcbCenterX + 7, pcbY - 6);
        p.drawPolygon(tri);
    }

    // 底部说明
    p.setPen(QColor(130, 140, 170));
    QFont footFont = p.font();
    footFont.setPixelSize(11);
    footFont.setBold(false);
    p.setFont(footFont);

    QString statusText;
    if (m_current < 0) {
        statusText = QStringLiteral("当前PCB未进入气室或已离开产线");
    } else {
        statusText = QStringLiteral("当前PCB位于第 %1 气室").arg(m_current + 1);
    }

    p.drawText(QRect(12, H - 22, W - 24, 18),
               Qt::AlignLeft | Qt::AlignVCenter,
               statusText);
}
