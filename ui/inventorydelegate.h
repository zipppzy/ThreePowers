#ifndef INVENTORYDELEGATE_H
#define INVENTORYDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class InventoryDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override;
};

#endif // INVENTORYDELEGATE_H
