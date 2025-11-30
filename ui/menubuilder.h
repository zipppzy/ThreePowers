#ifndef MENUBUILDER_H
#define MENUBUILDER_H

#include <QMenu>
#include <QWidgetAction>
#include <QWidget>
#include <functional>

class MenuBuilder
{
public:
    static QAction* addAction(QMenu* menu, const QString& text, std::function<void()> callback);
};
#endif // MENUBUILDER_H
