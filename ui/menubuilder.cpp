#include "menubuilder.h"
#include <qboxlayout.h>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QIntValidator>
#include <QEvent>
#include <QWheelEvent>

QAction* MenuBuilder::addAction(QMenu* menu, const QString& text, std::function<void()> callback){
    QAction* action = menu->addAction(text);
    QObject::connect(action, &QAction::triggered, callback);
    return action;
}

QWidgetAction* MenuBuilder::addNumberSpinner(QMenu* menu, const QString& label, const QString& buttonText, int initialValue, int minValue, int maxValue, std::function<void(int)> onConfirm){
    NumberSpinnerWidget* spinner = new NumberSpinnerWidget(label, buttonText, initialValue, minValue, maxValue, onConfirm, menu, menu);

    QWidgetAction* action = new QWidgetAction(menu);
    action->setDefaultWidget(spinner);
    menu->addAction(action);
    return action;
}
