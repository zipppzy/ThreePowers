#include "skillmodel.h"

SkillModel::SkillModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int SkillModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid() || !skills) return 0;
    return static_cast<int>(skills->size());
}

QVariant SkillModel::data(const QModelIndex &index, int role) const {
    if (!skills || !index.isValid() || index.row() >= static_cast<int>(skills->size()))
        return {};

    const Skill &s = skills->at(index.row());

    switch (role) {
    case NameRole:
        return QString::fromStdString(s.name);
    case LevelRole:
        return static_cast<int>(s.getLevel());
    case XpRole:
        return s.getXp();
    case MaxLevelRole:
        return static_cast<int>(s.getMaxLevel());
    default:
        return {};
    }
}

QHash<int, QByteArray> SkillModel::roleNames() const {
    return {
        {NameRole, "name"},
        {LevelRole, "level"},
        {XpRole, "xp"},
        {MaxLevelRole, "maxLevel"}
    };
}

void SkillModel::setSkillSource(const std::vector<Skill> *skillList) {
    beginResetModel();
    skills = skillList;
    endResetModel();
}


void SkillModel::refreshAll() {
    if (!skills || skills->empty()) return;
    QModelIndex top = createIndex(0, 0);
    QModelIndex bottom = createIndex(skills->size() - 1, 0);
    emit dataChanged(top, bottom);
}
