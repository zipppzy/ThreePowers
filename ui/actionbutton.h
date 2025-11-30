#ifndef ACTIONBUTTON_H
#define ACTIONBUTTON_H

#include "action.h"
#include "tooltip.h"
#include "menubuilder.h"
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <qpushbutton.h>
#include <QMenu>

class ActionButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ActionButton(std::shared_ptr<Action> action, QPushButton *parent = nullptr);

    void setProgress(double value);      // 0.0 to 1.0
    void updateProgress();
    void setText(const QString &text);
    QString getText() const;

signals:
    void tryStartAction();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    QString text;
    Tooltip* tooltip;
    double progress = 0.0;
    bool hovered = false;

    std::shared_ptr<Action> action;
};

#endif // ACTIONBUTTON_H
