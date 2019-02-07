#ifndef HEATMAP_H
#define HEATMAP_H

#include <QBasicTimer>
#include <QLinearGradient>
#include <QPainter>
#include <QTimerEvent>
#include <QWidget>
#include "helpers.h"
#include "heatgradient.h"


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

    QBasicTimer  timer;
    HeatGradient gradient;
    double       data[125][200];


public slots:

    void        fftEvent( double[125] );
};

#endif // HEATMAP_H
