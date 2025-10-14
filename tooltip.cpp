#include "tooltip.h"
#include <qboxlayout.h>
#include <qlabel.h>

Tooltip::Tooltip(const QString &text, QWidget *parent)
    : QWidget{parent, Qt::ToolTip | Qt::FramelessWindowHint}{
    this->setAttribute(Qt::WA_ShowWithoutActivating);
    this->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    this->setAttribute(Qt::WA_TranslucentBackground);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto* label = new QLabel(text, this);
    label->setStyleSheet("QLabel { background: #222; color: white; border: 1px solid #444; padding: 4px; }");
    layout->addWidget(label);
    this->setLayout(layout);
}
