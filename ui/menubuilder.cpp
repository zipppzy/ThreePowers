#include "menubuilder.h"

QAction* MenuBuilder::addAction(QMenu* menu, const QString& text, std::function<void()> callback){
    QAction* action = menu->addAction(text);
    QObject::connect(action, &QAction::triggered, callback);
    return action;
}
