#ifndef INVENTORYMODEL_H
#define INVENTORYMODEL_H

#include <QAbstractTableModel>
#include "item.h"

class InventoryModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column {
        NameColumn = 0,
        CountColumn,
        WeightColumn,
        VolumeColumn,
        ColumnCount
    };

    enum InventoryRoles {
        NameRole = Qt::UserRole + 1,
        CountRole,
        WeightRole,
        VolumeRole
    };

    explicit InventoryModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void sort(int column, Qt::SortOrder order) override;

    void setInventorySource(const std::vector<Item>* inventoryList);
    void onItemAdded();
    void refreshAll();

    const Item* getItem(int index) const;

private:
    const std::vector<Item>* inventory = nullptr;
    std::vector<int> sortedIndices;
    int sortColumn = NameColumn;
    Qt::SortOrder sortOrder = Qt::AscendingOrder;

    void updateSortedIndices();
};

#endif // INVENTORYMODEL_H
