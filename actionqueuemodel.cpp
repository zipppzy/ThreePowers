#include "actionqueuemodel.h"

ActionQueueModel::ActionQueueModel(const std::deque<std::pair<std::shared_ptr<Action>, int>>* actionQueue, QObject* parent)
    :QAbstractListModel(parent), actionQueue(actionQueue){}

int ActionQueueModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return static_cast<int>(actionQueue->size());
}

QVariant ActionQueueModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return {};
    const auto& [action, repeats] = actionQueue->at(index.row());

    switch (role) {
    case NameRole:
        return QString::fromStdString(action->name);
    case RepeatRole:
        return repeats;
    case ProgressRole:
        return action->getCurrentProgress()/action->getDuration();
    case Qt::DisplayRole:
        return QString("%1 × %2")
            .arg(QString::fromStdString(action->name))
            .arg(repeats);
    default:
        return {};
    }
}

QHash<int, QByteArray> ActionQueueModel::roleNames() const {
    return {
        {NameRole, "name"},
        {RepeatRole, "repeats"},
        {ProgressRole, "progress"}
    };
}

void ActionQueueModel::refresh() {
    beginResetModel();
    endResetModel();
}
