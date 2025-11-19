#include "messagelog.h"

MessageLog::MessageLog(QWidget *parent)
    : QWidget{parent} {

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* headerWidget = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    // Collapse toggle button
    collapseButton = new QToolButton();
    collapseButton->setText("▼");
    connect(collapseButton, &QToolButton::clicked, this, &MessageLog::toggleCollapsed);

    auto* titleLabel = new QLabel("Message Log", headerWidget);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


    headerLayout->addWidget(collapseButton);
    headerLayout->addWidget(titleLabel);

    layout->addWidget(headerWidget);

    // Scroll area containing message entries
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    layout->addWidget(scrollArea);

    // Container for log entries
    auto* container = new QWidget();
    entriesLayout = new QVBoxLayout(container);
    entriesLayout->setContentsMargins(0, 0, 0, 0);
    entriesLayout->setSpacing(2);
    entriesLayout->setAlignment(Qt::AlignTop);

    scrollArea->setWidget(container);
}

void MessageLog::addMessage(const QString& text){
    messages.push_back(text);

    if (messages.size() > MAX_MESSAGES) {
        messages.pop_front();
        QLayoutItem* item = entriesLayout->takeAt(0);
        if (item) {
            delete item->widget();
            delete item;
        }
    }

    QLabel* lbl = new QLabel(text);
    lbl->setWordWrap(true);
    entriesLayout->addWidget(lbl);

    // Auto-scroll to bottom after layout updates
    QTimer::singleShot(0, [this]{
        scrollArea->widget()->adjustSize();
        scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
    });
}

QSize MessageLog::sizeHint() const{
    if (collapsed) {
        return QSize(QWidget::sizeHint().width(), collapseButton->sizeHint().height());
    }
    return QWidget::sizeHint();
}

void MessageLog::toggleCollapsed(){
    this->collapsed = !collapsed;
    this->scrollArea->setVisible(!collapsed);
    collapseButton->setText(collapsed ? "▲" : "▼");

    this->setSizePolicy(QSizePolicy::Preferred, collapsed ? QSizePolicy::Fixed : QSizePolicy::Expanding);

    this->updateGeometry();
    this->parentWidget()->updateGeometry();
}
