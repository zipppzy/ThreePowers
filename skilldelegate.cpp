#include "skilldelegate.h"

#include <QApplication>
#include <QStyle>

void SkillDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const {
    painter->save();

    QRect rect = option.rect;
    QString name = index.data(Qt::UserRole + 1).toString();  // NameRole
    int level = index.data(Qt::UserRole + 2).toInt();        // LevelRole
    double xp = index.data(Qt::UserRole + 3).toDouble();     // XpRole
    double xpToNextLevel = index.data(Qt::UserRole + 4).toDouble();  // XpToNextLevelRole
    int maxLevel = index.data(Qt::UserRole + 5).toInt();

    // Background
    QColor bg = option.state & QStyle::State_Selected ? QColor("#444") : QColor("#222");
    painter->fillRect(rect, bg);
    painter->setPen(QColor(888));
    painter->drawRect(rect.adjusted(0, 0, -1, -1));

    // Name
    QFont boldFont = option.font;
    boldFont.setBold(true);
    painter->setFont(boldFont);
    painter->setPen(Qt::white);
    painter->drawText(rect.adjusted(8, 4, -8, -4), Qt::AlignTop | Qt::AlignLeft, name);

    // Level
    painter->setFont(option.font);
    painter->setPen(Qt::lightGray);
    painter->drawText(rect.adjusted(8, 24, -8, -4), Qt::AlignLeft, QString("Lv %1/%2").arg(level).arg(maxLevel));

    // Progress bar
    int barY = rect.bottom() - 20;
    int barWidth = rect.width() - 16;
    QRect barRect(rect.left() + 8, barY, barWidth, 10);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(555));
    painter->drawRect(barRect);
    double progress = xpToNextLevel > 0 ? xp / xpToNextLevel : 0;
    QRect filled(barRect.left(), barRect.top(), int(barRect.width() * progress), barRect.height());
    painter->setBrush(QColor(76,175,80));
    painter->drawRect(filled);

    painter->restore();
}

QSize SkillDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const {
    return QSize(200, 80);
}
