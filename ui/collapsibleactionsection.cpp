#include "collapsibleactionsection.h"
#include <QVBoxLayout>

CollapsibleActionSection::CollapsibleActionSection(const QString& title, QWidget* parent)
    : QWidget(parent)
{
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 4);
    outerLayout->setSpacing(0);

    toggleButton = new QToolButton();
    toggleButton->setText("▼  " + title);
    toggleButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toggleButton->setStyleSheet("QToolButton { text-align: left; font-weight: bold; padding: 4px; }");
    connect(toggleButton, &QToolButton::clicked, this, &CollapsibleActionSection::toggleCollapsed);

    contentWidget = new QWidget();
    contentLayout = new QGridLayout(contentWidget);
    contentLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    contentLayout->setContentsMargins(4, 4, 4, 4);

    outerLayout->addWidget(toggleButton);
    outerLayout->addWidget(contentWidget);
}

void CollapsibleActionSection::addButton(QWidget* btn) {
    contentLayout->addWidget(btn, nextRow, nextCol);
    if (++nextCol >= MAX_COLS) {
        nextCol = 0;
        nextRow++;
    }
}

void CollapsibleActionSection::clearButtons() {
    while (QLayoutItem* item = contentLayout->takeAt(0)) {
        if (QWidget* w = item->widget()) {
            w->setParent(nullptr);
            delete w;
        }
        delete item;
    }
    nextCol = 0;
    nextRow = 0;
}

void CollapsibleActionSection::toggleCollapsed() {
    collapsed = !collapsed;
    contentWidget->setVisible(!collapsed);
    // Update arrow, preserving the title text after "▼  " / "▶  "
    QString title = toggleButton->text().mid(3);
    toggleButton->setText((collapsed ? "▶  " : "▼  ") + title);
}
