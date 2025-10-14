#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <QWidget>

class Tooltip : public QWidget
{
    Q_OBJECT
public:
    explicit Tooltip(const QString &text, QWidget *parent = nullptr);

protected:
    void leaveEvent(QEvent *) override { hide(); }

signals:
};

#endif // TOOLTIP_H
