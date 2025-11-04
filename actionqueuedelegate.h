#ifndef ACTIONQUEUEDELEGATE_H
#define ACTIONQUEUEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QMouseEvent>

class ActionQueueDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ActionQueueDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

signals:
    void removeRequested(int row);
    void moveUpRequested(int row);
    void hoverChanged();

private:
    mutable int hoverRow = -1;

    static void drawButton(QPainter* p, const QRect& r, const QString& label, int offsetX);
    static QRect buttonRect(const QRect& r, int offsetX);
};

#endif // ACTIONQUEUEDELEGATE_H
