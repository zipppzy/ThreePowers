#include "eventdialog.h"
#include <QFrame>

EventDialog::EventDialog(const Event& event, QWidget* parent)
    : QDialog(parent),
    eventPtr(&event)
{
    setupUI(event);

    // Make dialog modal and cover most of the screen
    setModal(true);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    // Size to 80% of parent window
    if (parent) {
        int width = static_cast<int>(parent->width() * 0.8);
        int height = static_cast<int>(parent->height() * 0.8);
        resize(width, height);

        // Center on parent
        move(parent->geometry().center() - rect().center());
    } else {
        resize(800, 600);
    }
}

void EventDialog::setupUI(const Event& event) {
    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(20);

    // Styling
    // setStyleSheet(
    //     "QDialog { "
    //     "   background-color: #2b2b2b; "
    //     "   border: 2px solid #555; "
    //     "}"
    //     "QLabel { "
    //     "   color: white; "
    //     "}"
    //     "QPushButton { "
    //     "   background-color: #444; "
    //     "   color: white; "
    //     "   border: 2px solid #666; "
    //     "   border-radius: 5px; "
    //     "   padding: 10px 20px; "
    //     "   font-size: 14px; "
    //     "   min-width: 120px; "
    //     "}"
    //     "QPushButton:hover { "
    //     "   background-color: #555; "
    //     "   border-color: #888; "
    //     "}"
    //     "QPushButton:pressed { "
    //     "   background-color: #333; "
    //     "}"
    //     "QScrollArea { "
    //     "   border: 1px solid #555; "
    //     "   background-color: #1e1e1e; "
    //     "}"
    //     );

    // Title
    titleLabel = new QLabel(QString::fromStdString(event.title), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);
    mainLayout->addWidget(titleLabel);

    // Separator line
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #555;");
    line->setMaximumHeight(2);
    mainLayout->addWidget(line);

    // Body scroll area
    bodyScrollArea = new QScrollArea(this);
    bodyScrollArea->setWidgetResizable(true);
    bodyScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Body content
    QWidget* bodyContainer = new QWidget();
    QVBoxLayout* bodyLayout = new QVBoxLayout(bodyContainer);
    bodyLayout->setContentsMargins(20, 20, 20, 20);

    bodyLabel = new QLabel(QString::fromStdString(event.body), bodyContainer);
    QFont bodyFont = bodyLabel->font();
    bodyFont.setPointSize(12);
    bodyLabel->setFont(bodyFont);
    bodyLabel->setWordWrap(true);
    bodyLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    bodyLayout->addWidget(bodyLabel);
    bodyLayout->addStretch();

    bodyScrollArea->setWidget(bodyContainer);
    mainLayout->addWidget(bodyScrollArea, 1); // stretch factor 1 to take remaining space

    // Separator line
    QFrame* line2 = new QFrame(this);
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    line2->setStyleSheet("background-color: #555;");
    line2->setMaximumHeight(2);
    mainLayout->addWidget(line2);

    // Choices layout
    choicesLayout = new QHBoxLayout();
    choicesLayout->setSpacing(15);
    choicesLayout->setAlignment(Qt::AlignCenter);

    // Create choice buttons
    for (size_t i = 0; i < event.choices.size(); ++i) {
        QPushButton* btn = new QPushButton(
            QString::fromStdString(event.choices[i].name),
            this
            );

        // Connect button to emit signal with choice index
        connect(btn, &QPushButton::clicked, this, [this, i]() {
            emit choiceSelected(static_cast<int>(i), eventPtr->choices[i].effects);
            accept(); // Close dialog
        });

        choiceButtons.push_back(btn);
        choicesLayout->addWidget(btn);
    }

    mainLayout->addLayout(choicesLayout);
}
