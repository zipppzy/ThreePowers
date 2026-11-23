#ifndef HEALTHBARWIDGET_H
#define HEALTHBARWIDGET_H

#include <QWidget>

class HealthBarWidget : public QWidget
{
    Q_OBJECT
public:
    struct Segment{
        double value;
        QColor color;
    };

    explicit HealthBarWidget(QWidget* parent = nullptr);

    void setValues(double current, double max, std::vector<Segment> overlaySegments = {});

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    double current = 0.0;
    double max = 1.0;
    std::vector<Segment> overlaySegments;
signals:
};

#endif // HEALTHBARWIDGET_H
