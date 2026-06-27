#include "hotkeymanager.h"

HotkeyManager::HotkeyManager(QWidget* parent)
    : QObject(parent)
{
}

void HotkeyManager::registerHotkey(const QString& name, const QKeySequence& key, std::function<void()> callback)
{
    auto* shortcut = new QShortcut(key, qobject_cast<QWidget*>(parent()));
    connect(shortcut, &QShortcut::activated, this, callback);
    hotkeys[name] = {shortcut, std::move(callback)};
}

bool HotkeyManager::rebindHotkey(const QString& name, const QKeySequence& newKey)
{
    auto it = hotkeys.find(name);
    if (it == hotkeys.end()) {
        return false;
    }

    auto* widget = qobject_cast<QWidget*>(parent());
    if (!widget) {
        return false;
    }

    delete it->shortcut;

    auto* newShortcut = new QShortcut(newKey, widget);
    connect(newShortcut, &QShortcut::activated, this, it->callback);

    it->shortcut = newShortcut;
    return true;
}
