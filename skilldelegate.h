#ifndef SKILLDELEGATE_H
#define SKILLDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class SkillDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override;
};


#endif // SKILLDELEGATE_H
