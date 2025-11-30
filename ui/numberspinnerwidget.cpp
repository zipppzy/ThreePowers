#include "numberspinnerwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QIntValidator>
#include <QWheelEvent>
#include <QMenu>

NumberSpinnerWidget::NumberSpinnerWidget(const QString& label, const QString& buttonText, int initial, int min, int max, std::function<void(int)> callback, QMenu* parentMenu, QWidget* parent)
    : QWidget(parent)
    , minValue(min)
    , maxValue(max)
    , onConfirm(callback)
    , menu(parentMenu)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 4, 8, 4);
    mainLayout->setSpacing(4);

    QHBoxLayout* spinnerLayout = new QHBoxLayout();
    spinnerLayout->setSpacing(4);

    if (!label.isEmpty()) {
        QLabel* lbl = new QLabel(label, this);
        spinnerLayout->addWidget(lbl);
    }

    QPushButton* decrementBtn = new QPushButton("-", this);
    decrementBtn->setMaximumWidth(30);
    decrementBtn->setFocusPolicy(Qt::NoFocus);
    connect(decrementBtn, &QPushButton::clicked, [this]() {
        setValue(currentValue - 1);
    });
    spinnerLayout->addWidget(decrementBtn);

    lineEdit = new QLineEdit(this);
    lineEdit->setText(QString::number(initial));
    lineEdit->setMaximumWidth(60);
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setValidator(new QIntValidator(minValue, maxValue, lineEdit));

    connect(lineEdit, &QLineEdit::editingFinished, [this]() {
        bool ok;
        int value = lineEdit->text().toInt(&ok);
        if (ok) {
            setValue(value);
        } else {
            lineEdit->setText(QString::number(currentValue));
        }
    });

    connect(lineEdit, &QLineEdit::returnPressed, [this]() {
        confirm();
    });

    spinnerLayout->addWidget(lineEdit);

    QPushButton* incrementBtn = new QPushButton("+", this);
    incrementBtn->setMaximumWidth(30);
    incrementBtn->setFocusPolicy(Qt::NoFocus);
    connect(incrementBtn, &QPushButton::clicked, [this]() {
        setValue(currentValue + 1);
    });
    spinnerLayout->addWidget(incrementBtn);

    mainLayout->addLayout(spinnerLayout);

    QPushButton* confirmBtn = new QPushButton(buttonText, this);
    connect(confirmBtn, &QPushButton::clicked, [this]() {
        confirm();
    });
    mainLayout->addWidget(confirmBtn);

    currentValue = initial;

    lineEdit->installEventFilter(this);
}

void NumberSpinnerWidget::setValue(int value) {
    value = qBound(minValue, value, maxValue);
    currentValue = value;
    lineEdit->setText(QString::number(currentValue));
}

int NumberSpinnerWidget::getValue() const {
    return currentValue;
}

void NumberSpinnerWidget::confirm() {
    if (onConfirm) {
        onConfirm(currentValue);
    }
    if (menu) {
        menu->close();
    }
}

bool NumberSpinnerWidget::eventFilter(QObject* obj, QEvent* event) {
    if (obj == lineEdit && event->type() == QEvent::Wheel) {
        if (auto* wheelEvent = dynamic_cast<QWheelEvent*>(event)) {
            int delta = wheelEvent->angleDelta().y();
            if (delta > 0) {
                setValue(currentValue + 1);
            } else if (delta < 0) {
                setValue(currentValue - 1);
            }
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
