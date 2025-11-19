#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include <QWidget>
#include <QWidget>
#include <QToolButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QTimer>
#include <deque>

#define MAX_MESSAGES 100

class MessageLog : public QWidget
{
    Q_OBJECT
public:
    explicit MessageLog(QWidget* parent = nullptr);

    QSize sizeHint() const override;

    void addMessage(const QString& text);
    void clearEntries();

private slots:
    void toggleCollapsed();

private:
    QToolButton* collapseButton;
    QScrollArea* scrollArea;
    QVBoxLayout* entriesLayout;

    std::deque<QString> messages;
    bool collapsed = false;
};

#endif // MESSAGELOG_H
