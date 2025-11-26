#ifndef RESERVEDELEGATE_H
#define RESERVEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class ReserveDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override;
};

#endif // RESERVEDELEGATE_H
