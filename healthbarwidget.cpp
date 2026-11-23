#include "healthbarwidget.h"
#include <QPainter>
#include <algorithm>

HealthBarWidget::HealthBarWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(20);
    setMaximumHeight(20);
}

void HealthBarWidget::setValues(double current, double max, std::vector<Segment> overlaySegments){
    this->current = current;
    this->max = max;
    this->overlaySegments = overlaySegments;
    update();
}

void HealthBarWidget::paintEvent(QPaintEvent*){
    QPainter painter(this);
    QRect barRect = rect();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(55, 55, 55));
    painter.drawRect(barRect);

    double baseProgress = max > 0 ? std::clamp(current / max, 0.0, 1.0) : 0.0;
    QRect baseFilled(barRect.left(), barRect.top(), int(barRect.width() * baseProgress), barRect.height());
    painter.setBrush(QColor(200, 60, 60));
    painter.drawRect(baseFilled);

    double offset = baseProgress;
    for(const Segment& segment : overlaySegments){
        double segmentProgress = max > 0 ? segment.value / max : 0.0;
        QRect segmentRect(barRect.left() + int(barRect.width() * offset), barRect.top(), int(barRect.width() * segmentProgress), barRect.height());
        painter.setBrush(segment.color);
        painter.drawRect(segmentRect);
        offset += segmentProgress;
    }

    painter.setPen(Qt::white);
    QString text = QString("%1 / %2").arg(int(current)).arg(int(max));
    painter.drawText(barRect, Qt::AlignCenter, text);
}