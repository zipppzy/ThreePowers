#ifndef ACTIONQUEUEMODEL_H
#define ACTIONQUEUEMODEL_H

#include "action.h"
#include <QAbstractListModel>
#include <deque>

class ActionQueueModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles { NameRole = Qt::UserRole + 1, RepeatRole, ProgressRole };

    explicit ActionQueueModel(const std::deque<std::pair<std::shared_ptr<Action>, int>>* actionQueue, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void refresh();

private:
    const std::deque<std::pair<std::shared_ptr<Action>, int>>* actionQueue = nullptr;
};

#endif // ACTIONQUEUEMODEL_H
