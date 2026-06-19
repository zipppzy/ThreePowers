#ifndef RESEARCHTABWIDGET_H
#define RESEARCHTABWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QFrame>
#include <QEnterEvent>
#include "researchtopic.h"
#include "actionbutton.h"
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

// Displays one tier row: tier name label + scrollable note pills + merge button
class TierRowWidget : public QWidget {
    Q_OBJECT
public:
    explicit TierRowWidget(int tier, QWidget* parent = nullptr);

    // Update note pills and refresh merge button state
    void setNotes(const std::vector<ResearchNote>& sortedNotes);

signals:
    void mergeRequestedForTier(int tier);
    // Emitted when the merge button is clicked
    void mergeRequested(int tier);

private:
    int tier;
    int noteCount = 0;              // cached so merge button can check availability
    bool mergeHovered = false;      // true while mouse is over merge button

    QHBoxLayout* pillLayout = nullptr;
    QWidget* pillContainer = nullptr;
    QScrollArea* scrollArea = nullptr;
    QPushButton* mergeButton = nullptr;

    void rebuildPills(const std::vector<ResearchNote>& sortedNotes);
    bool eventFilter(QObject* obj, QEvent* event) override;
    void updateMergeButton();
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
    void mergeRequestedForTier(int tier);
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

    QWidget* actionsContainer = nullptr;
    QGridLayout* actionsLayout = nullptr;
    int nextActionCol = 0;
    int nextActionRow = 0;
    static constexpr int MAX_ACTION_COLS = 5;

    void rebuildTierRows();
    void refreshNotes(const ResearchTopic& topic);
    void refreshBar(const ResearchTopic& topic);
};

#endif // RESEARCHTABWIDGET_H