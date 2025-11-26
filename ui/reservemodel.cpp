#include "reservemodel.h"

ReserveModel::ReserveModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int ReserveModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid() || !reserves) return 0;
    return static_cast<int>(reserves->size());
}

QVariant ReserveModel::data(const QModelIndex &index, int role) const {
    if (!reserves || !index.isValid() || index.row() >= static_cast<int>(reserves->size()))
        return {};

    const Reserve &r = reserves->at(index.row());

    switch (role) {
    case NameRole:
        return QString::fromStdString(r.name);
    case CurrentValueRole:
        return r.getCurrentValue();
    case MaxValueRole:
        return r.getMaxValue();
    case RegenRole:
        return r.getRegen();
    default:
        return {};
    }
}

QHash<int, QByteArray> ReserveModel::roleNames() const {
    return {
        {NameRole, "name"},
        {CurrentValueRole, "currentValue"},
        {MaxValueRole, "maxValue"},
        {RegenRole, "regen"}
    };
}

void ReserveModel::setReserveSource(const std::vector<Reserve> *reserveList) {
    beginResetModel();
    reserves = reserveList;
    endResetModel();
}

void ReserveModel::onReserveAdded(int index) {
    beginInsertRows(QModelIndex(), index, index);
    endInsertRows();
}

void ReserveModel::refreshAll() {
    if (!reserves || reserves->empty()) return;
    QModelIndex top = createIndex(0, 0);
    QModelIndex bottom = createIndex(reserves->size() - 1, 0);
    emit dataChanged(top, bottom);
}

const Reserve* ReserveModel::getReserve(int index) const {
    if (!reserves || index < 0 || index >= static_cast<int>(reserves->size()))
        return nullptr;
    return &reserves->at(index);
}
