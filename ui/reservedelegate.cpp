#include "reservedelegate.h"
#include <QApplication>
#include <QStyle>

void ReserveDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                            const QModelIndex& index) const {
    painter->save();

    QRect rect = option.rect;
    QString name = index.data(Qt::UserRole + 1).toString();           // NameRole
    double currentValue = index.data(Qt::UserRole + 2).toDouble();    // CurrentValueRole
    double maxValue = index.data(Qt::UserRole + 3).toDouble();        // MaxValueRole
    double regen = index.data(Qt::UserRole + 4).toDouble();           // RegenRole

    // Background
    QColor bg = option.state & QStyle::State_Selected ? QColor(68, 68, 68) : QColor(34, 34, 34);
    painter->fillRect(rect, bg);
    painter->setPen(QColor(88, 88, 88));
    painter->drawRect(rect.adjusted(0, 0, -1, -1));

    // Name
    QFont boldFont = option.font;
    boldFont.setBold(true);
    painter->setFont(boldFont);
    painter->setPen(Qt::white);
    painter->drawText(rect.adjusted(8, 4, -8, -4), Qt::AlignTop | Qt::AlignLeft, name);

    // Progress bar
    int barY = rect.bottom() - 20;
    int barWidth = rect.width() - 16;
    QRect barRect(rect.left() + 8, barY, barWidth, 14);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(55, 55, 55));
    painter->drawRect(barRect);

    double progress = maxValue > 0 ? currentValue / maxValue : 0;
    QRect filled(barRect.left(), barRect.top(), int(barRect.width() * progress), barRect.height());
    painter->setBrush(QColor(33, 150, 243)); // Blue color for reserves
    painter->drawRect(filled);

    // Value text in progress bar (left aligned)
    painter->setFont(option.font);
    QString valueText = QString("%1 / %2").arg(currentValue, 0, 'f', 1).arg(maxValue, 0, 'f', 1);
    painter->setPen(Qt::white);
    painter->drawText(barRect.adjusted(4, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, valueText);

    // Regen text in progress bar (right aligned)
    QString regenText = QString("+%1/s").arg(regen, 0, 'f', 2);
    painter->drawText(barRect.adjusted(0, 0, -4, 0), Qt::AlignRight | Qt::AlignVCenter, regenText);

    painter->restore();
}

QSize ReserveDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const {
    return QSize(250, 50);
}
