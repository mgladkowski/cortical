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

private:

    QBasicTimer timer;
    double      data[125][400];

    bool        isDoubleEqual(double a, double b);

public slots:

    void        fftEvent( double[125] );
};

#endif // HEATMAP_H
