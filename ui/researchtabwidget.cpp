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
    this->current             = current;
    this->attemptThreshold    = attemptThreshold;
    this->guaranteedThreshold = guaranteedThreshold;
    this->successChance       = successChance;
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

    // Filled portion - colour shifts blue->green approaching guaranteed
    int g = static_cast<int>(80  + 175 * scale);
    int b = static_cast<int>(243 - 200 * scale);
    QRect filled(r.left(), r.top(),static_cast<int>(r.width() * scale), r.height());
    p.setBrush(QColor(33, g, b));
    p.drawRoundedRect(filled, 4, 4);

    // Attempt threshold marker
    if (attemptThreshold > 0 && guaranteedThreshold > 0) {
        double attemptX = r.left() + r.width() * (attemptThreshold / guaranteedThreshold);
        p.setPen(QPen(QColor(220, 220, 220), 2, Qt::DashLine));
        p.drawLine(static_cast<int>(attemptX), r.top() - 4,
                   static_cast<int>(attemptX), r.bottom() + 4);
        p.setFont(QFont("Segoe UI", 7));
        p.setPen(QColor(200, 200, 200));
        p.drawText(static_cast<int>(attemptX) - 20, r.top() - 6, 40, 12,
                   Qt::AlignCenter, "Attempt");
    }

    // Guaranteed threshold marker (gold, far right)
    p.setPen(QPen(QColor(255, 200, 50), 2));
    p.drawLine(r.right(), r.top() - 4, r.right(), r.bottom() + 4);

    // Text
    p.setPen(Qt::white);
    p.setFont(QFont("Segoe UI", 9, QFont::Bold));
    p.drawText(r, Qt::AlignCenter,
               QString("%1 / %2")
                   .arg(static_cast<int>(current))
                   .arg(static_cast<int>(guaranteedThreshold)));
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

    // Tier name label
    auto* tierLabel = new QLabel(QString::fromStdString(ResearchNote::tierName(tier)));
    tierLabel->setFixedWidth(72);
    tierLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    QFont f = tierLabel->font();
    f.setBold(true);
    tierLabel->setFont(f);
    tierLabel->setStyleSheet("color: #aaa;");
    outerLayout->addWidget(tierLabel);

    // Scroll area for pills
    scrollArea = new QScrollArea();
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFixedHeight(68);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    pillContainer = new QWidget();
    pillLayout = new QHBoxLayout(pillContainer);
    pillLayout->setContentsMargins(2, 2, 2, 2);
    pillLayout->setSpacing(4);
    pillLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    scrollArea->setWidget(pillContainer);
    outerLayout->addWidget(scrollArea, 1);

    // Merge button
    mergeButton = new QPushButton(
        QString("Merge\n%1s").arg(QString::fromStdString(ResearchNote::tierName(tier))));
    mergeButton->setFixedSize(70, 52);
    mergeButton->setEnabled(false);
    mergeButton->setStyleSheet(
        "QPushButton { background: #2a2a2a; color: #666; border: 1px solid #444;"
        "              border-radius: 4px; font-size: 9px; }"
        "QPushButton:enabled { background: #1a3a2a; color: #4caf50; border: 1px solid #4caf50; }"
        "QPushButton:enabled:hover { background: #2a4a3a; }"
        "QPushButton:disabled { background: #2a2a2a; color: #444; border: 1px solid #333; }");

    connect(mergeButton, &QPushButton::clicked, this, [this]() {
        emit mergeRequested(this->tier);
    });

    // Track hover over merge button to highlight pills
    mergeButton->installEventFilter(this);

    outerLayout->addWidget(mergeButton);
}

bool TierRowWidget::eventFilter(QObject* obj, QEvent* event) {
    if (obj == mergeButton) {
        if (event->type() == QEvent::Enter) {
            mergeHovered = true;
            if (noteCount > 0)
                rebuildPills(lastNotes);
        } else if (event->type() == QEvent::Leave) {
            mergeHovered = false;
            if (noteCount > 0)
                rebuildPills(lastNotes);
        }
    }
    return QWidget::eventFilter(obj, event);
}

void TierRowWidget::setNotes(const std::vector<ResearchNote>& sortedNotes) {
    noteCount = static_cast<int>(sortedNotes.size());
    lastNotes = sortedNotes;
    updateMergeButton();
    rebuildPills(sortedNotes);
}

void TierRowWidget::updateMergeButton() {
    mergeButton->setEnabled(noteCount >= ResearchTopic::MERGE_COUNT);
}

void TierRowWidget::rebuildPills(const std::vector<ResearchNote>& sortedNotes) {
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
        pillContainer->adjustSize();
        return;
    }

    // Top MERGE_COUNT notes will be consumed by a merge — highlight these on hover
    int highlightCount = std::min(static_cast<int>(sortedNotes.size()), ResearchTopic::MERGE_COUNT);

    for (int i = 0; i < static_cast<int>(sortedNotes.size()); ++i) {
        const auto& note = sortedNotes[i];

        double decayFactor = std::pow(ResearchTopic::DECAY_RATE, i);
        double effectiveValue = note.knowledgeValue * decayFactor;
        double reductionPct = (1.0 - decayFactor) * 100.0;

        bool highlighted = mergeHovered && (i < highlightCount);

        auto* pill = new QWidget();
        pill->setFixedHeight(42);
        pill->setMinimumWidth(100);

        auto* pillInnerLayout = new QVBoxLayout(pill);
        pillInnerLayout->setContentsMargins(6, 2, 6, 2);
        pillInnerLayout->setSpacing(1);

        // Top: effective value after decay
        auto* valueLabel = new QLabel(QString("%1").arg(effectiveValue, 0, 'f', 1));
        valueLabel->setAlignment(Qt::AlignCenter);
        valueLabel->setStyleSheet("color: white; font-weight: bold; font-size: 11px;");

        // Bottom: base value and reduction percent
        QString bottomText = (i==0) ? QString("%1 base").arg(note.knowledgeValue, 0, 'f', 1) : QString("%1  -%2%").arg(note.knowledgeValue, 0, 'f', 1).arg(reductionPct, 0, 'f', 0);

        auto* bottomLabel = new QLabel(bottomText);
        bottomLabel->setAlignment(Qt::AlignCenter);

        QString reductionColor;
        if (i == 0) reductionColor = "#4caf50";
        else if (reductionPct < 20) reductionColor = "#4caf50";
        else if (reductionPct < 50) reductionColor = "#ffeb3b";
        else reductionColor = "#f44336";

        bottomLabel->setStyleSheet(QString("color: %1; font-size: 9px;").arg(reductionColor));

        pillInnerLayout->addWidget(valueLabel);
        pillInnerLayout->addWidget(bottomLabel);

        // Pill background: highlight gold if hovered and in merge set, else darken with decay
        if (highlighted) {
            pill->setStyleSheet(
                "background-color: rgb(80,65,20); border-radius: 4px;"
                "border: 1px solid #ffeb3b;");
        } else {
            int brightness = std::max(static_cast<int>(70 - reductionPct * 0.3), 30);
            pill->setStyleSheet(
                QString("background-color: rgb(%1,%1,%2); border-radius: 4px;" "border: 1px solid #555;")
                    .arg(brightness)
                    .arg(brightness + 20));
        }

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

    // ── Topic selector ──
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

    // ── Stats row ──
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
    QFont boldFont = notesLabel->font();
    boldFont.setBold(true);
    notesLabel->setFont(boldFont);
    mainLayout->addWidget(notesLabel);

    tiersContainer = new QWidget();
    tiersLayout = new QVBoxLayout(tiersContainer);
    tiersLayout->setContentsMargins(0, 0, 0, 0);
    tiersLayout->setSpacing(4);
    mainLayout->addWidget(tiersContainer);

    rebuildTierRows();

    // ── Separator ──
    auto* sep2 = new QFrame();
    sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet("color: #444;");
    mainLayout->addWidget(sep2);

    // ── Actions section ──
    auto* actionsLabel = new QLabel("Actions");
    actionsLabel->setFont(boldFont);
    mainLayout->addWidget(actionsLabel);

    auto* actionsScrollArea = new QScrollArea();
    actionsScrollArea->setWidgetResizable(true);
    actionsScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    actionsScrollArea->setMaximumHeight(160);

    actionsContainer = new QWidget();
    actionsLayout = new QGridLayout(actionsContainer);
    actionsLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    actionsContainer->setLayout(actionsLayout);
    actionsScrollArea->setWidget(actionsContainer);
    mainLayout->addWidget(actionsScrollArea);

    connect(topicCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ResearchTabWidget::onTopicSelected);
}

void ResearchTabWidget::rebuildTierRows() {
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

        // Wire merge button signal up through ResearchTabWidget as activeTopicChanged
        // is already used for topic changes — we use a direct connection to GameLoop
        // via the mergeRequested signal on TierRowWidget, forwarded here.
        // GameLoop connects to this after construction.
        connect(row, &TierRowWidget::mergeRequested, this, [this](int tier) {
            // Find the matching merge action and add it to queue via a signal
            // For now emit a dedicated signal — GameLoop will handle it
            emit mergeRequestedForTier(tier);
        });

        tiersLayout->addWidget(row);
        tierRows.push_back(row);
    }
}

void ResearchTabWidget::setTopics(
    const std::unordered_map<std::string, ResearchTopic>* newTopics)
{
    topics = newTopics;

    QString previousTopic = activeTopic;
    topicCombo->blockSignals(true);
    topicCombo->clear();
    topicCombo->addItem("— Select a topic —", QString(""));

    if (topics) {
        for (const auto& [name, topic] : *topics) {
            if (!topic.completed) {
                topicCombo->addItem(QString::fromStdString(name), QString::fromStdString(name));
            }
        }
    }

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
        for (auto* row : tierRows) row->setNotes({});
        return;
    }

    auto it = topics->find(activeTopic.toStdString());
    if (it == topics->end()) return;

    refreshBar(it->second);
    refreshNotes(it->second);
}

void ResearchTabWidget::refreshBar(const ResearchTopic& topic) {
    double knowledge  = topic.accumulatedKnowledge();
    double guaranteed = topic.currentGuaranteedThreshold();
    double attempt    = topic.attemptThreshold;
    double chance     = topic.successChance();

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
    actionsLayout->addWidget(btn, nextActionRow, nextActionCol);
    if (++nextActionCol >= MAX_ACTION_COLS) {
        nextActionCol = 0;
        nextActionRow++;
    }
}

void ResearchTabWidget::clearActionButtons() {
    while (QLayoutItem* item = actionsLayout->takeAt(0)) {
        if (QWidget* w = item->widget()) {
            w->setParent(nullptr);
        }
        delete item;
    }
    nextActionCol = 0;
    nextActionRow = 0;
}