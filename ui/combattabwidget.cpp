#include "combattabwidget.h"
#include <QVBoxLayout>

CombatTabWidget::CombatTabWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* mainLayout = new QVBoxLayout(this);

    enemyContainer = new QWidget(this);
    enemyLayout = new QGridLayout(enemyContainer);
    mainLayout->addWidget(enemyContainer);

    attackButtonContainer = new QWidget(this);
    attackButtonLayout = new QGridLayout(attackButtonContainer);
    mainLayout->addWidget(attackButtonContainer);

    mainLayout->addStretch();
}

void CombatTabWidget::refresh(const CombatEncounter& encounter){
    //1v1 scope - single enemy widget created lazily, reused on every refresh.
    //becomes a rebuild-on-roster-change loop once multi-enemy lands.
    if(enemyWidgets.empty()){
        auto* widget = new CombatantWidget(enemyContainer);
        enemyLayout->addWidget(widget, 0, 0);
        enemyWidgets.push_back(widget);

        connect(widget, &CombatantWidget::targetClicked, this, [this](){
            if(!pendingAttack) return;
            emit attackTargetChosen(pendingAttack, pendingRepeats, 0);   //index 0 - only one combatant exists right now
            cancelTargeting();
        });
    }

    enemyWidgets[0]->refresh(encounter.enemy);
}

void CombatTabWidget::setAttackButtons(const std::vector<std::shared_ptr<CombatAttack>>& attacks){
    for(ActionButton* btn : attackButtons){
        btn->setParent(nullptr);
        delete btn;
    }
    attackButtons.clear();

    int col = 0;
    for(const auto& attack : attacks){
        auto* btn = new ActionButton(attack, nullptr);
        attackButtonLayout->addWidget(btn, 0, col++);
        attackButtons.push_back(btn);

        connect(btn, &ActionButton::tryStartAction, this, [this, attack](){
            pendingRepeats = 1;
            enterTargetingMode(attack);
        });
        connect(btn, &ActionButton::tryStartMultipleActions, this, [this, attack](int count){
            pendingRepeats = count;
            enterTargetingMode(attack);
        });
    }
}

void CombatTabWidget::enterTargetingMode(std::shared_ptr<CombatAttack> attack){
    pendingAttack = attack;
    for(CombatantWidget* widget : enemyWidgets){
        widget->setTargetable(true);
    }
}

void CombatTabWidget::cancelTargeting(){
    pendingAttack = nullptr;
    pendingRepeats = 1;
    for(CombatantWidget* widget : enemyWidgets){
        widget->setTargetable(false);
    }
}