#ifndef MENUBUILDER_H
#define MENUBUILDER_H

#include <QMenu>
#include <QWidgetAction>
#include <QWidget>
#include <functional>
#include <numberspinnerwidget.h>

class MenuBuilder
{
public:
    static QAction* addAction(QMenu* menu, const QString& text, std::function<void()> callback);
    static QWidgetAction* addNumberSpinner(QMenu* menu, const QString& label, const QString& buttonText, int initialValue, int minValue, int maxValue, std::function<void(int)> onConfirm);
};
#endif // MENUBUILDER_H
