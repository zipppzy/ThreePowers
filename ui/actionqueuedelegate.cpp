#include "ActionQueueDelegate.h"
#include "ActionQueueModel.h"

ActionQueueDelegate::ActionQueueDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

void ActionQueueDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();
    QRect r = option.rect;

    // background
    painter->fillRect(r, option.state & QStyle::State_Selected
                             ? QColor("#d0d0d0")
                             : QColor("#f8f8f8"));

    // text
    QString text = index.data(Qt::DisplayRole).toString();
    painter->setPen(Qt::black);
    painter->drawText(r.adjusted(6, 2, -6, -16), Qt::AlignLeft | Qt::AlignTop, text);

    // progress bar
    double progress = index.data(ActionQueueModel::ProgressRole).toDouble();
    QRectF barRect = r.adjusted(6, r.height() - 10, -6, -4);
    painter->setBrush(QColor("#4caf50"));
    painter->setPen(Qt::NoPen);
    painter->drawRect(QRectF(barRect.left(), barRect.top(),
                             barRect.width() * progress, barRect.height()));

    // buttons on hover
    if (index.row() == hoverRow) {
        drawButton(painter, r, "✖", 30);  // remove
        drawButton(painter, r, "↑", 60);  // move up
    }

    painter->restore();
}

QSize ActionQueueDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const {
    return {200, 36};
}

bool ActionQueueDelegate::editorEvent(QEvent* event, QAbstractItemModel*,
                                      const QStyleOptionViewItem& option,
                                      const QModelIndex& index) {
    if (event->type() == QEvent::MouseMove) {
        auto* me = static_cast<QMouseEvent*>(event);
        int newHover = option.rect.contains(me->pos()) ? index.row() : -1;
        if (newHover != hoverRow) {
            hoverRow = newHover;
            emit const_cast<ActionQueueDelegate*>(this)->hoverChanged();
        }
    } else if (event->type() == QEvent::MouseButtonPress) {
        auto* me = static_cast<QMouseEvent*>(event);
        if (buttonRect(option.rect, 30).contains(me->pos()))
            emit removeRequested(index.row());
        else if (buttonRect(option.rect, 60).contains(me->pos()))
            emit moveUpRequested(index.row());
    }
    return false;
}

void ActionQueueDelegate::drawButton(QPainter* p, const QRect& r,
                                     const QString& label, int offsetX) {
    QRect br = buttonRect(r, offsetX);
    p->setBrush(QColor("#cccccc"));
    p->setPen(Qt::NoPen);
    p->drawRoundedRect(br, 4, 4);
    p->setPen(Qt::black);
    p->drawText(br, Qt::AlignCenter, label);
}

QRect ActionQueueDelegate::buttonRect(const QRect& r, int offsetX) {
    QSize sz(24, 24);
    return QRect(r.right() - offsetX, r.center().y() - 12, sz.width(), sz.height());
}
