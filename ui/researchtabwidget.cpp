#include "researchtabwidget.h"
#include <QScrollBar>
#include <cmath>

// ─────────────────────────────────────────
// KnowledgeBar
// ─────────────────────────────────────────

KnowledgeBar::KnowledgeBar(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(48);
    setMaximumHeight(48);
}

void KnowledgeBar::setValues(double current, double attemptThreshold,
                             double guaranteedThreshold, double successChance) {
    this->current            = current;
    this->attemptThreshold   = attemptThreshold;
    this->guaranteedThreshold = guaranteedThreshold;
    this->successChance      = successChance;
    update();
}

void KnowledgeBar::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRect r = rect().adjusted(2, 8, -2, -8);
    const double scale = guaranteedThreshold > 0
                             ? std::min(current / guaranteedThreshold, 1.0)
                             : 0.0;

    // Background
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(45, 45, 45));
    p.drawRoundedRect(r, 4, 4);

    // Filled portion - colour shifts from blue toward green as you approach guaranteed
    int g = static_cast<int>(80  + 175 * scale);
    int b = static_cast<int>(243 - 200 * scale);
    QColor fillColor(33, g, b);
    QRect filled(r.left(), r.top(),
                 static_cast<int>(r.width() * scale), r.height());
    p.setBrush(fillColor);
    p.drawRoundedRect(filled, 4, 4);

    // Attempt threshold marker (white dashed line)
    if (attemptThreshold > 0 && guaranteedThreshold > 0) {
        double attemptX = r.left() + r.width() * (attemptThreshold / guaranteedThreshold);
        p.setPen(QPen(QColor(220, 220, 220), 2, Qt::DashLine));
        p.drawLine(static_cast<int>(attemptX), r.top() - 4,
                   static_cast<int>(attemptX), r.bottom() + 4);

        // Label above marker
        p.setFont(QFont("Segoe UI", 7));
        p.setPen(QColor(200, 200, 200));
        p.drawText(static_cast<int>(attemptX) - 20, r.top() - 6, 40, 12,
                   Qt::AlignCenter, "Attempt");
    }

    // Guaranteed threshold marker (gold line at far right)
    {
        int guaranteedX = r.right();
        p.setPen(QPen(QColor(255, 200, 50), 2));
        p.drawLine(guaranteedX, r.top() - 4, guaranteedX, r.bottom() + 4);
    }

    // Knowledge value text inside bar
    p.setPen(Qt::white);
    p.setFont(QFont("Segoe UI", 9, QFont::Bold));
    QString barText = QString("%1 / %2")
                          .arg(static_cast<int>(current))
                          .arg(static_cast<int>(guaranteedThreshold));
    p.drawText(r, Qt::AlignCenter, barText);
}

// ─────────────────────────────────────────
// TierRowWidget
// ─────────────────────────────────────────

TierRowWidget::TierRowWidget(int tier, QWidget* parent)
    : QWidget(parent), tier(tier)
{
    auto* outerLayout = new QHBoxLayout(this);
    outerLayout->setContentsMargins(4, 2, 4, 2);
    outerLayout->setSpacing(8);

    // Tier name label (fixed width)
    auto* tierLabel = new QLabel(QString::fromStdString(ResearchNote::tierName(tier)));
    tierLabel->setFixedWidth(72);
    tierLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    QFont f = tierLabel->font();
    f.setBold(true);
    tierLabel->setFont(f);
    tierLabel->setStyleSheet("color: #aaa;");
    outerLayout->addWidget(tierLabel);

    // Scroll area for note pills
    scrollArea = new QScrollArea();
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFixedHeight(52);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    pillContainer = new QWidget();
    pillLayout = new QHBoxLayout(pillContainer);
    pillLayout->setContentsMargins(2, 2, 2, 2);
    pillLayout->setSpacing(4);
    pillLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    scrollArea->setWidget(pillContainer);
    outerLayout->addWidget(scrollArea, 1);
}

void TierRowWidget::setNotes(const std::vector<ResearchNote>& sortedNotes) {
    // Clear existing pills
    while (QLayoutItem* item = pillLayout->takeAt(0)) {
        if (QWidget* w = item->widget()) {
            w->setParent(nullptr);
            delete w;
        }
        delete item;
    }

    if (sortedNotes.empty()) {
        auto* emptyLabel = new QLabel("—");
        emptyLabel->setStyleSheet("color: #555;");
        pillLayout->addWidget(emptyLabel);
        return;
    }

    double firstValue = sortedNotes.front().knowledgeValue;

    for (int i = 0; i < static_cast<int>(sortedNotes.size()); ++i) {
        const auto& note = sortedNotes[i];

        // Reduction relative to first note in tier
        double reductionPct = firstValue > 0
                                  ? (1.0 - note.knowledgeValue / firstValue) * 100.0
                                  : 0.0;

        // Pill widget
        auto* pill = new QWidget();
        pill->setFixedHeight(42);
        pill->setMinimumWidth(80);

        auto* pillLayout2 = new QVBoxLayout(pill);
        pillLayout2->setContentsMargins(6, 2, 6, 2);
        pillLayout2->setSpacing(1);

        auto* valueLabel = new QLabel(QString("%1").arg(note.knowledgeValue, 0, 'f', 1));
        valueLabel->setAlignment(Qt::AlignCenter);
        valueLabel->setStyleSheet("color: white; font-weight: bold; font-size: 11px;");

        QString reductionText = i == 0
                                    ? "base"
                                    : QString("-%1%").arg(reductionPct, 0, 'f', 0);
        auto* reductionLabel = new QLabel(reductionText);
        reductionLabel->setAlignment(Qt::AlignCenter);

        // Color the reduction label: green for base, yellow->red as reduction grows
        QString reductionColor;
        if (i == 0) {
            reductionColor = "#4caf50";
        } else if (reductionPct < 30) {
            reductionColor = "#ffeb3b";
        } else if (reductionPct < 60) {
            reductionColor = "#ff9800";
        } else {
            reductionColor = "#f44336";
        }
        reductionLabel->setStyleSheet(
            QString("color: %1; font-size: 9px;").arg(reductionColor));

        pillLayout2->addWidget(valueLabel);
        pillLayout2->addWidget(reductionLabel);

        // Pill background darkens with reduction
        int brightness = static_cast<int>(70 - reductionPct * 0.3);
        pill->setStyleSheet(
            QString("background-color: rgb(%1,%1,%2); border-radius: 4px; border: 1px solid #555;")
                .arg(brightness)
                .arg(brightness + 20));

        pillLayout->addWidget(pill);
    }

    pillLayout->addStretch();
    pillContainer->adjustSize();
}

// ─────────────────────────────────────────
// ResearchTabWidget
// ─────────────────────────────────────────

ResearchTabWidget::ResearchTabWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);

    // ── Topic selector row ──
    auto* selectorRow = new QHBoxLayout();
    auto* selectorLabel = new QLabel("Research Topic:");
    selectorLabel->setStyleSheet("font-weight: bold;");
    topicCombo = new QComboBox();
    topicCombo->setMinimumWidth(200);
    selectorRow->addWidget(selectorLabel);
    selectorRow->addWidget(topicCombo, 1);
    mainLayout->addLayout(selectorRow);

    // ── Knowledge bar ──
    knowledgeBar = new KnowledgeBar();
    mainLayout->addWidget(knowledgeBar);

    // ── Stats row (attempt threshold label + success chance) ──
    auto* statsRow = new QHBoxLayout();
    knowledgeLabel = new QLabel("Attempt threshold: —");
    knowledgeLabel->setStyleSheet("color: #aaa; font-size: 10px;");
    successLabel = new QLabel("Success chance: —");
    successLabel->setStyleSheet("color: #aaa; font-size: 10px;");
    statsRow->addWidget(knowledgeLabel);
    statsRow->addStretch();
    statsRow->addWidget(successLabel);
    mainLayout->addLayout(statsRow);

    // ── Separator ──
    auto* sep1 = new QFrame();
    sep1->setFrameShape(QFrame::HLine);
    sep1->setStyleSheet("color: #444;");
    mainLayout->addWidget(sep1);

    // ── Notes section ──
    auto* notesLabel = new QLabel("Notes");
    QFont notesFont = notesLabel->font();
    notesFont.setBold(true);
    notesLabel->setFont(notesFont);
    mainLayout->addWidget(notesLabel);

    tiersContainer = new QWidget();
    tiersLayout = new QVBoxLayout(tiersContainer);
    tiersLayout->setContentsMargins(0, 0, 0, 0);
    tiersLayout->setSpacing(4);
    mainLayout->addWidget(tiersContainer);

    // Pre-build tier rows for all tiers up to MAX_TIER
    rebuildTierRows();

    // ── Separator ──
    auto* sep2 = new QFrame();
    sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet("color: #444;");
    mainLayout->addWidget(sep2);

    // ── Actions section ──
    auto* actionsLabel = new QLabel("Actions");
    actionsLabel->setFont(notesFont);
    mainLayout->addWidget(actionsLabel);

    auto* actionsScrollArea = new QScrollArea();
    actionsScrollArea->setWidgetResizable(true);

    actionsContainer = new QWidget();
    actionsLayout = new QVBoxLayout(actionsContainer);
    actionsLayout->setAlignment(Qt::AlignTop);
    actionsContainer->setLayout(actionsLayout);
    actionsScrollArea->setWidget(actionsContainer);

    mainLayout->addWidget(actionsScrollArea, 1);

    // Wire up combo box
    connect(topicCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ResearchTabWidget::onTopicSelected);
}

void ResearchTabWidget::rebuildTierRows() {
    // Clear existing rows
    while (QLayoutItem* item = tiersLayout->takeAt(0)) {
        if (QWidget* w = item->widget()) {
            w->setParent(nullptr);
            delete w;
        }
        delete item;
    }
    tierRows.clear();

    for (int tier = 0; tier <= ResearchNote::MAX_TIER; ++tier) {
        auto* row = new TierRowWidget(tier, tiersContainer);
        tiersLayout->addWidget(row);
        tierRows.push_back(row);
    }
}

void ResearchTabWidget::setTopics(
    const std::unordered_map<std::string, ResearchTopic>* newTopics)
{
    topics = newTopics;

    // Repopulate combo box, preserving selection if possible
    QString previousTopic = activeTopic;
    topicCombo->blockSignals(true);
    topicCombo->clear();
    topicCombo->addItem("— Select a topic —", QString(""));

    if (topics) {
        for (const auto& [name, topic] : *topics) {
            if (!topic.completed) {
                topicCombo->addItem(QString::fromStdString(name),
                                    QString::fromStdString(name));
            }
        }
    }

    // Restore previous selection
    int idx = topicCombo->findData(previousTopic);
    topicCombo->setCurrentIndex(idx >= 0 ? idx : 0);
    topicCombo->blockSignals(false);

    refresh();
}

void ResearchTabWidget::refresh() {
    if (!topics || activeTopic.isEmpty()) {
        knowledgeBar->setValues(0, 0, 1, 0);
        knowledgeLabel->setText("Attempt threshold: —");
        successLabel->setText("Success chance: —");
        for (auto* row : tierRows) {
            row->setNotes({});
        }
        return;
    }

    auto it = topics->find(activeTopic.toStdString());
    if (it == topics->end()) return;

    refreshBar(it->second);
    refreshNotes(it->second);
}

void ResearchTabWidget::refreshBar(const ResearchTopic& topic) {
    double knowledge   = topic.accumulatedKnowledge();
    double guaranteed  = topic.currentGuaranteedThreshold();
    double attempt     = topic.attemptThreshold;
    double chance      = topic.successChance();

    knowledgeBar->setValues(knowledge, attempt, guaranteed, chance);

    knowledgeLabel->setText(
        QString("Attempt threshold: %1  |  Guaranteed: %2")
            .arg(static_cast<int>(attempt))
            .arg(static_cast<int>(guaranteed)));

    if (chance <= 0.0) {
        successLabel->setText("Success chance: —");
        successLabel->setStyleSheet("color: #f44336; font-size: 10px;");
    } else {
        successLabel->setText(
            QString("Success chance: %1%").arg(chance * 100.0, 0, 'f', 1));
        QString color = chance >= 1.0 ? "#4caf50"
                        : chance >= 0.5 ? "#ffeb3b"
                                        : "#ff9800";
        successLabel->setStyleSheet(
            QString("color: %1; font-size: 10px;").arg(color));
    }
}

void ResearchTabWidget::refreshNotes(const ResearchTopic& topic) {
    for (int tier = 0; tier <= ResearchNote::MAX_TIER; ++tier) {
        auto it = topic.notes.find(tier);
        if (it != topic.notes.end()) {
            tierRows[tier]->setNotes(it->second);
        } else {
            tierRows[tier]->setNotes({});
        }
    }
}

void ResearchTabWidget::onTopicSelected(int index) {
    activeTopic = topicCombo->itemData(index).toString();
    emit activeTopicChanged(activeTopic);
    refresh();
}

void ResearchTabWidget::addActionButton(ActionButton* btn) {
    QString section = "Research";
    if (auto sectionTag = Tags::getSectionName(btn->getAction()->tags)) {
        section = Tags::toDisplayName(*sectionTag);
    }

    if (!actionSections.contains(section)) {
        auto* s = new CollapsibleActionSection(section);
        actionsLayout->addWidget(s);
        actionSections[section] = s;
    }
    actionSections[section]->addButton(btn);
}

void ResearchTabWidget::clearActionButtons() {
    for (auto* section : actionSections) {
        section->setParent(nullptr);
        delete section;
    }
    actionSections.clear();
}
