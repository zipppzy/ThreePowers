#ifndef RESEARCHTABWIDGET_H
#define RESEARCHTABWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QFrame>
#include "researchtopic.h"
#include "actionbutton.h"
#include "collapsibleactionsection.h"
#include "tags.h"

// Custom widget that draws the knowledge progress bar with threshold markers
class KnowledgeBar : public QWidget {
    Q_OBJECT
public:
    explicit KnowledgeBar(QWidget* parent = nullptr);

    void setValues(double current, double attemptThreshold,
                   double guaranteedThreshold, double successChance);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    double current = 0.0;
    double attemptThreshold = 100.0;
    double guaranteedThreshold = 500.0;
    double successChance = 0.0;
};

// Displays one tier row: tier name label + scrollable note pills
class TierRowWidget : public QWidget {
    Q_OBJECT
public:
    explicit TierRowWidget(int tier, QWidget* parent = nullptr);
    void setNotes(const std::vector<ResearchNote>& sortedNotes);

private:
    int tier;
    QHBoxLayout* pillLayout = nullptr;
    QWidget* pillContainer = nullptr;
    QScrollArea* scrollArea = nullptr;
};

// The full research tab
class ResearchTabWidget : public QWidget {
    Q_OBJECT
public:
    explicit ResearchTabWidget(QWidget* parent = nullptr);

    // Called by GameLoop when research state changes or a topic is selected
    void setTopics(const std::unordered_map<std::string, ResearchTopic>* topics);
    void refresh();

    // Action button section - mirrors ActionTab pattern
    void addActionButton(ActionButton* btn);
    void clearActionButtons();

signals:
    void activeTopicChanged(const QString& topicName);

private slots:
    void onTopicSelected(int index);

private:
    const std::unordered_map<std::string, ResearchTopic>* topics = nullptr;
    QString activeTopic;

    // Top section
    QComboBox* topicCombo = nullptr;
    KnowledgeBar* knowledgeBar = nullptr;
    QLabel* knowledgeLabel = nullptr;
    QLabel* successLabel = nullptr;

    // Notes section
    QWidget* tiersContainer = nullptr;
    QVBoxLayout* tiersLayout = nullptr;
    std::vector<TierRowWidget*> tierRows;

    // Action buttons section - reuses CollapsibleActionSection
    QWidget* actionsContainer = nullptr;
    QMap<QString, CollapsibleActionSection*> actionSections;
    QVBoxLayout* actionsLayout = nullptr;

    void rebuildTierRows();
    void refreshNotes(const ResearchTopic& topic);
    void refreshBar(const ResearchTopic& topic);
};

#endif // RESEARCHTABWIDGET_H
