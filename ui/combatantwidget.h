#ifndef COMBATANTWIDGET_H
#define COMBATANTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include "healthbarwidget.h"
#include "combatant.h"

class CombatantWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CombatantWidget(QWidget* parent = nullptr);

    void refresh(const Combatant& combatant);

    void setTargetable(bool targetable);
    void setSelected(bool selected);

signals:
    void targetClicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QLabel* nameLabel;
    HealthBarWidget* healthBar;
    QLabel* currentActionLabel;
    QProgressBar* actionProgressBar;
    bool targetable = false;
    bool selected = false;
};

#endif // COMBATANTWIDGET_H