#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <QWidget>
#include <qlabel.h>

class Tooltip : public QWidget
{
    Q_OBJECT
public:
    explicit Tooltip(const QString &text, QWidget *parent = nullptr);
    void setActionData(const QString &name, int current, int total, const QString &desc);
    void setRequirements(const QString& reqText);

protected:
    void leaveEvent(QEvent *) override { hide(); }

private:
    QLabel *nameLabel;
    QLabel *progressLabel;
    QLabel *descLabel;
    QLabel *requirementsLabel;
signals:
};

#endif // TOOLTIP_H
