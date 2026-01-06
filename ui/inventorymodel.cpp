#include "inventorymodel.h"
#include <algorithm>

InventoryModel::InventoryModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int InventoryModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid() || !inventory) return 0;
    return static_cast<int>(inventory->size());
}

int InventoryModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return ColumnCount;
}

QVariant InventoryModel::data(const QModelIndex &index, int role) const {
    if (!inventory || !index.isValid() || index.row() >= static_cast<int>(inventory->size()))
        return {};

    int actualRow = sortedIndices.empty() ? index.row() : sortedIndices[index.row()];
    const Item &item = inventory->at(actualRow);

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case NameColumn:
            return QString::fromStdString(item.name);
        case CountColumn:
            return item.count;
        case WeightColumn:
            return QString::number(item.getWeight(), 'f', 3);
        case VolumeColumn:
            return QString::number(item.getSize(), 'f', 3);
        }
    }

    // Custom roles for delegate
    switch (role) {
    case NameRole:
        return QString::fromStdString(item.name);
    case CountRole:
        return item.count;
    case WeightRole:
        return item.getWeight();
    case VolumeRole:
        return item.getSize();
    }

    return {};
}

QVariant InventoryModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    switch (section) {
    case NameColumn:
        return "Name";
    case CountColumn:
        return "Count";
    case WeightColumn:
        return "Weight (kg)";
    case VolumeColumn:
        return "Volume (m³)";
    default:
        return {};
    }
}

void InventoryModel::sort(int column, Qt::SortOrder order) {
    if (!inventory || inventory->empty()) return;

    sortColumn = column;
    sortOrder = order;

    emit layoutAboutToBeChanged();
    updateSortedIndices();
    emit layoutChanged();
}

void InventoryModel::updateSortedIndices() {
    if (!inventory) return;

    sortedIndices.clear();
    sortedIndices.resize(inventory->size());
    std::iota(sortedIndices.begin(), sortedIndices.end(), 0);

    std::sort(sortedIndices.begin(), sortedIndices.end(), [this](int a, int b) {
        const Item& itemA = inventory->at(a);
        const Item& itemB = inventory->at(b);

        bool less = false;
        switch (sortColumn) {
        case NameColumn: {
            // Case-insensitive string comparison
            std::string nameA = itemA.name;
            std::string nameB = itemB.name;
            std::transform(nameA.begin(), nameA.end(), nameA.begin(),
                           [](unsigned char c){ return std::tolower(c); });
            std::transform(nameB.begin(), nameB.end(), nameB.begin(),
                           [](unsigned char c){ return std::tolower(c); });
            less = nameA < nameB;
            break;
        }
        case CountColumn:
            less = itemA.count < itemB.count;
            break;
        case WeightColumn:
            less = itemA.getWeight() < itemB.getWeight();
            break;
        case VolumeColumn:
            less = itemA.getSize() < itemB.getSize();
            break;
        }

        return sortOrder == Qt::AscendingOrder ? less : !less;
    });
}

void InventoryModel::setInventorySource(const std::vector<Item> *inventoryList) {
    beginResetModel();
    inventory = inventoryList;
    updateSortedIndices();
    endResetModel();
}

void InventoryModel::onItemAdded(int index) {
    Q_UNUSED(index);
    beginResetModel();
    updateSortedIndices();
    endResetModel();
}

void InventoryModel::refreshAll() {
    if (!inventory || inventory->empty()) return;
    updateSortedIndices();
    emit dataChanged(createIndex(0, 0),
                     createIndex(inventory->size() - 1, ColumnCount - 1));
}

const Item* InventoryModel::getItem(int index) const {
    if (!inventory || index < 0 || index >= static_cast<int>(inventory->size()))
        return nullptr;

    int actualRow = sortedIndices.empty() ? index : sortedIndices[index];
    return &inventory->at(actualRow);
}
