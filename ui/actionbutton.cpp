#include "actionbutton.h"

ActionButton::ActionButton(std::shared_ptr<Action> action, QPushButton *parent)
    : QPushButton{parent}
    , text{QString::fromStdString(action->name)}
    ,action{action}
{
    this->tooltip = new Tooltip{this->text};
    this->tooltip->setActionData(QString::fromStdString(action->name), action->getCurrentProgress(), action->getDuration(), "Test Description");

    this->setMinimumHeight(40);
    this->setMinimumWidth(150);
    this->setCursor(Qt::PointingHandCursor);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(this, &QPushButton::clicked, this, &ActionButton::tryStartAction);
}

void ActionButton::setProgress(double value)
{
    this->progress = std::clamp(value, 0.0, 1.0);
    update();
}

void ActionButton::updateProgress(){
    if(this->action->getDuration() == 0.0) return;
    this->setProgress(this->action->getCurrentProgress()/this->action->getDuration());
}

void ActionButton::setText(const QString &text)
{
    this->text = text;
    update();
}

QString ActionButton::getText() const
{
    return this->text;
}

void ActionButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // Draw base background (empty bar)
    QColor baseColor = QColor(0x444444); // dark gray
    painter.fillRect(rect(), baseColor);

    // Draw progress background
    QColor progressColor = QColor(0x66c2ff); // light blue
    QRect progressRect = rect();
    progressRect.setWidth(int(rect().width() * progress));
    painter.fillRect(progressRect, progressColor);

    // Draw hover overlay
    if (hovered) {
        painter.fillRect(rect(), QColor(255, 255, 255, 30)); // white translucent
        this->tooltip->setActionData(QString::fromStdString(action->name), action->getCurrentProgress(), action->getDuration(), "Test Description");
    }

    // Draw centered text
    painter.setPen(Qt::white);
    painter.setFont(QFont("Segoe UI", 10, QFont::Bold));
    painter.drawText(rect(), Qt::AlignCenter, text);
}

void ActionButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}

void ActionButton::enterEvent(QEnterEvent *)
{
    hovered = true;
    QPoint pos = mapToGlobal(rect().topRight());
    tooltip->move(pos.x() + 8, pos.y());
    this->tooltip->move(pos);
    this->tooltip->show();
    update();
}

void ActionButton::leaveEvent(QEvent *)
{
    hovered = false;
    this->tooltip->hide();
    update();
}

void ActionButton::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu(this);

    MenuBuilder::addNumberSpinner(&menu, "Queue: ", "Add to Queue", 1, 1, 999, [this](int count){
        emit tryStartMultipleActions(count);
    });

    menu.addSeparator();

    MenuBuilder::addAction(&menu, "Set Rest Action", [this]() {
        emit setRestAction(this->action);
    });

    menu.exec(event->globalPos());
}
