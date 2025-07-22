#ifndef ACTIONBUTTON_H
#define ACTIONBUTTON_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

class ActionButton : public QWidget
{
    Q_OBJECT
public:
    explicit ActionButton(const QString &text, QWidget *parent = nullptr);

    void setProgress(double value);      // 0.0 to 1.0
    void setText(const QString &text);
    QString getText() const;

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QString text;
    double progress = 0.0;
    bool hovered = false;
};

#endif // ACTIONBUTTON_H
