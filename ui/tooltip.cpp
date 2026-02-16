#include "tooltip.h"
#include <qboxlayout.h>

Tooltip::Tooltip(const QString &text, QWidget *parent)
    : QWidget{parent, Qt::ToolTip | Qt::FramelessWindowHint}{
    this->setAttribute(Qt::WA_ShowWithoutActivating);
    this->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    this->setAttribute(Qt::WA_TranslucentBackground);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto style = "QLabel { background: #222; color: white; padding: 6px; }";

    nameLabel = new QLabel(this);
    nameLabel->setStyleSheet(style);
    nameLabel->setAlignment(Qt::AlignCenter);
    QFont nameFont = nameLabel->font();
    nameFont.setBold(true);
    nameFont.setPointSize(nameFont.pointSize() + 2);
    nameLabel->setFont(nameFont);

    progressLabel = new QLabel(this);
    progressLabel->setStyleSheet(style);
    progressLabel->setAlignment(Qt::AlignCenter);

    descLabel = new QLabel(this);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet(style);

    requirementsLabel = new QLabel(this);
    requirementsLabel->setWordWrap(true);
    requirementsLabel->setStyleSheet(style);
    requirementsLabel->setTextFormat(Qt::PlainText);
    requirementsLabel->hide();  // Hidden by default

    auto *frameStyle = "background: #444; max-height: 1px;";

    QFrame *line1 = new QFrame(this);
    line1->setStyleSheet(frameStyle);
    QFrame *line2 = new QFrame(this);
    line2->setStyleSheet(frameStyle);
    QFrame *line3 = new QFrame(this);
    line3->setStyleSheet(frameStyle);

    layout->addWidget(nameLabel);
    layout->addWidget(line1);
    layout->addWidget(progressLabel);
    layout->addWidget(line2);
    layout->addWidget(descLabel);
    layout->addWidget(line3);
    layout->addWidget(requirementsLabel);

    setLayout(layout);
}

void Tooltip::setActionData(const QString &name, int current, int total, const QString &desc){
    nameLabel->setText(name);
    progressLabel->setText(QString("Progress: %1 / %2").arg(current).arg(total));
    descLabel->setText(desc);
}

void Tooltip::setRequirements(const QString& reqText) {
    if (reqText.isEmpty()) {
        requirementsLabel->hide();
    } else {
        requirementsLabel->setText(reqText);
        requirementsLabel->show();
    }
}
