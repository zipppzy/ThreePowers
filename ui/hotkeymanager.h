#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include <QObject>
#include <QShortcut>
#include <QWidget>
#include <QMap>
#include <QString>
#include <functional>

class HotkeyManager : public QObject
{
    Q_OBJECT

public:
    explicit HotkeyManager(QWidget* parent);

    void registerHotkey(const QString& name, const QKeySequence& key,
                        std::function<void()> callback);

    bool rebindHotkey(const QString& name, const QKeySequence& newKey);

private:
    struct HotkeyEntry {
        QShortcut* shortcut;
        std::function<void()> callback;
    };
    QMap<QString, HotkeyEntry> hotkeys;
};

#endif // HOTKEYMANAGER_H
