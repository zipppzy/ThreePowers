#ifndef COLLAPSIBLEACTIONSECTION_H
#define COLLAPSIBLEACTIONSECTION_H

#include <QWidget>
#include <QToolButton>
#include <QGridLayout>

class CollapsibleActionSection : public QWidget {
    Q_OBJECT
public:
    explicit CollapsibleActionSection(const QString& title, QWidget* parent = nullptr);
    void addButton(QWidget* btn);
    void clearButtons();

private slots:
    void toggleCollapsed();

private:
    QToolButton* toggleButton;
    QWidget* contentWidget;
    QGridLayout* contentLayout;
    int nextCol = 0;
    int nextRow = 0;
    bool collapsed = false;
    static constexpr int MAX_COLS = 5;
};

#endif // COLLAPSIBLEACTIONSECTION_H
