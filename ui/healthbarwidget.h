#ifndef HEALTHBARWIDGET_H
#define HEALTHBARWIDGET_H

#include <QWidget>
#include <QColor>
#include <vector>

// Generic segmented bar. Base segment is current/max health; overlaySegments stack on top,
// left to right, each just a value+color pair. Serves temp hp for now; armor's visual
// representation is still undecided, but this is the same mechanism either way once that's settled.
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
};

#endif // HEALTHBARWIDGET_H