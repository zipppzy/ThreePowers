#ifndef SKILLMODEL_H
#define SKILLMODEL_H

#include <QAbstractListModel>
#include "skill.h"

class SkillModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum SkillRoles {
        NameRole = Qt::UserRole + 1,
        LevelRole,
        XpRole,
        MaxLevelRole
    };
    Q_ENUM(SkillRoles)

    explicit SkillModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setSkillSource(const std::vector<Skill>* skillList);
    void refreshAll();

    const Skill *getSkill(int index) const;

private:
    const std::vector<Skill> *skills = nullptr;  // pointer to Player's skill list
};

#endif // SKILLMODEL_H
