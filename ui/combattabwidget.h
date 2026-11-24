#ifndef COMBATTABWIDGET_H
#define COMBATTABWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QShortcut>
#include <vector>
#include "combatantwidget.h"
#include "combatattack.h"
#include "combatencounter.h"
#include "actionbutton.h"

class CombatTabWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CombatTabWidget(QWidget* parent = nullptr);

    void refresh(const CombatEncounter& encounter);
    void setAttackButtons(const std::vector<std::shared_ptr<CombatAttack>>& attacks);

signals:
    void attackTargetChosen(std::shared_ptr<CombatAttack> attack, int repeats, int combatantIndex);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private:
    QWidget* enemyContainer;
    QGridLayout* enemyLayout;
    std::vector<CombatantWidget*> enemyWidgets;

    QLabel* targetingHintLabel;
    QShortcut* cancelShortcut;

    QWidget* attackButtonContainer;
    QGridLayout* attackButtonLayout;
    std::vector<ActionButton*> attackButtons;

    std::shared_ptr<CombatAttack> pendingAttack = nullptr;
    int pendingRepeats = 1;

    void enterTargetingMode(std::shared_ptr<CombatAttack> attack);
    void cancelTargeting();
};

#endif // COMBATTABWIDGET_H
