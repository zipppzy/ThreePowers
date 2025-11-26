#ifndef RESERVEMODEL_H
#define RESERVEMODEL_H

#include <QAbstractListModel>
#include "reserve.h"

class ReserveModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ReserveRoles {
        NameRole = Qt::UserRole + 1,
        CurrentValueRole,
        MaxValueRole,
        RegenRole
    };
    Q_ENUM(ReserveRoles)

    explicit ReserveModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setReserveSource(const std::vector<Reserve>* reserveList);
    void onReserveAdded(int index);
    void refreshAll();

    const Reserve* getReserve(int index) const;

private:
    const std::vector<Reserve>* reserves = nullptr;
};

#endif // RESERVEMODEL_H
