#ifndef HEATMAP_H
#define HEATMAP_H

#include <algorithm>
#include <cmath>
#include <cfloat>
#include <QBasicTimer>
#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QTimerEvent>
#include <QWidget>


class HeatMap : public QWidget {

    Q_OBJECT

public:

    explicit    HeatMap(QWidget *parent = nullptr);
    QSize       minimumSizeHint() const override;
    QSize       sizeHint() const override;

protected:

    void        paintEvent(QPaintEvent *event) override;
    void        timerEvent(QTimerEvent *event) override;
    void        dataEvent(double data[125]);

private:

    QBasicTimer timer;
    double      data[400][125];

    bool        isDoubleEqual(double a, double b);
};

#endif // HEATMAP_H
