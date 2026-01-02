#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include "event.h"

class EventDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EventDialog(const Event& event, QWidget* parent = nullptr);

signals:
    void choiceSelected(int choiceIndex);

private:
    QLabel* titleLabel = nullptr;
    QScrollArea* bodyScrollArea = nullptr;
    QLabel* bodyLabel = nullptr;
    QHBoxLayout* choicesLayout = nullptr;
    std::vector<QPushButton*> choiceButtons;

    void setupUI(const Event& event);
};

#endif // EVENTDIALOG_H
