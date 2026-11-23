#include "combatantwidget.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>

CombatantWidget::CombatantWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(2);

    nameLabel = new QLabel(this);
    QFont boldFont = nameLabel->font();
    boldFont.setBold(true);
    nameLabel->setFont(boldFont);
    layout->addWidget(nameLabel);

    healthBar = new HealthBarWidget(this);
    layout->addWidget(healthBar);

    currentActionLabel = new QLabel(this);
    layout->addWidget(currentActionLabel);

    actionProgressBar = new QProgressBar(this);
    actionProgressBar->setTextVisible(false);
    actionProgressBar->setMaximumHeight(8);
    layout->addWidget(actionProgressBar);

    setMinimumSize(160, 90);
}

void CombatantWidget::refresh(const Combatant& combatant){
    nameLabel->setText(QString::fromStdString(combatant.name));
    healthBar->setValues(combatant.health, combatant.maxHealth);

    if(combatant.currentAttack){
        currentActionLabel->setText(QString::fromStdString(combatant.currentAttack->name));
        double duration = combatant.currentAttack->getDuration();
        double progress = duration > 0 ? combatant.currentAttack->getCurrentProgress() / duration * 100 : 0;
        actionProgressBar->setValue(int(progress));
    }else{
        currentActionLabel->setText("");
        actionProgressBar->setValue(0);
    }
}

void CombatantWidget::setTargetable(bool targetable){
    this->targetable = targetable;
    setCursor(targetable ? Qt::PointingHandCursor : Qt::ArrowCursor);
    update();
}

void CombatantWidget::setSelected(bool selected){
    this->selected = selected;
    update();
}

void CombatantWidget::mousePressEvent(QMouseEvent* event){
    if(targetable && event->button() == Qt::LeftButton){
        emit targetClicked();
    }
    QWidget::mousePressEvent(event);
}

void CombatantWidget::paintEvent(QPaintEvent* event){
    QWidget::paintEvent(event);

    if(targetable || selected){
        QPainter painter(this);
        QPen pen(selected ? QColor(255, 215, 0) : QColor(100, 200, 255));
        pen.setWidth(2);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(rect().adjusted(1, 1, -1, -1));
    }
}