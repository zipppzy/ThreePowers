#include "inventorydelegate.h"
#include "inventorymodel.h"
#include <QApplication>
#include <QStyle>

void InventoryDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                              const QModelIndex& index) const {
    painter->save();

    QRect rect = option.rect;

    // Background
    QColor bg = option.state & QStyle::State_Selected ? QColor(68, 68, 68) : QColor(34, 34, 34);
    painter->fillRect(rect, bg);
    painter->setPen(QColor(88, 88, 88));
    painter->drawRect(rect.adjusted(0, 0, -1, -1));

    // Text color
    painter->setPen(Qt::white);
    painter->setFont(option.font);

    // Get data
    QString text = index.data(Qt::DisplayRole).toString();

    // Draw text with padding
    QRect textRect = rect.adjusted(8, 0, -8, 0);

    // Right-align numeric columns
    Qt::Alignment alignment = Qt::AlignVCenter | Qt::AlignLeft;
    if (index.column() == InventoryModel::CountColumn ||
        index.column() == InventoryModel::WeightColumn ||
        index.column() == InventoryModel::VolumeColumn) {
        alignment = Qt::AlignVCenter | Qt::AlignRight;
    }

    painter->drawText(textRect, alignment, text);

    painter->restore();
}

QSize InventoryDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const {
    return QSize(100, 32);
}
