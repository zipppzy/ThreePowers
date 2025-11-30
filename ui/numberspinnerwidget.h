#ifndef NUMBERSPINNERWIDGET_H
#define NUMBERSPINNERWIDGET_H

#include <QWidget>
#include <functional>

class QLineEdit;
class QMenu;

class NumberSpinnerWidget : public QWidget {
    Q_OBJECT
public:
    NumberSpinnerWidget(const QString& label, const QString& buttonText, int initial, int min, int max, std::function<void(int)> callback, QMenu* parentMenu, QWidget* parent = nullptr);

    void setValue(int value);
    int getValue() const;
    void confirm();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    QLineEdit* lineEdit = nullptr;
    int currentValue = 0;
    int minValue = 0;
    int maxValue = 0;
    std::function<void(int)> onConfirm;
    QMenu* menu = nullptr;
};

#endif // NUMBERSPINNERWIDGET_H
