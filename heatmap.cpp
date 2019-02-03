#include "heatmap.h"

HeatMap::HeatMap(QWidget *parent) : QWidget(parent) {

    gradient = HeatGradient();
    memset(data, 0, sizeof(data));
    timer.start(33, this);
}


QSize HeatMap::minimumSizeHint() const {

    return QSize(100, 125);
}


QSize HeatMap::sizeHint() const {

    return QSize(400, 125);
}


void HeatMap::paintEvent(QPaintEvent *) {

    QPainter painter(this);
    QPen     pen;
    QPoint   point[1];

    double   bp[10] = {3, 4, 5, 7, 9, 10, 13, 20, 50, 100};

    for (int t=0; t < 400; t++) {

        for (int f=0; f < 60; f++) {

            point[0] = QPoint(t*2,f*2);

            double nval = 0.0;

            if ( Helpers::isDoubleEqual( data[f][t], 0.0 ) ) {
                nval = 0.0;
            } else if ( data[f][t] < bp[0] ) {
                nval = 0.1;
            } else if ( data[f][t] < bp[1] ) {
                nval = 0.2;
            } else if ( data[f][t] < bp[2] ) {
                nval = 0.3;
            } else if ( data[f][t] < bp[3] ) {
                nval = 0.4;
            } else if ( data[f][t] < bp[4] ) {
                nval = 0.5;
            } else if ( data[f][t] < bp[5] ) {
                nval = 0.6;
            } else if ( data[f][t] < bp[6] ) {
                nval = 0.7;
            } else if ( data[f][t] < bp[7] ) {
                nval = 0.8;
            } else if ( data[f][t] < bp[8] ) {
                nval = 0.9;
            } else if ( data[f][t] >= bp[9] ) {
                nval = 1.0;
            }

            float _r,_g,_b;
            gradient.getColorAtValue( static_cast<float>(nval), _r, _g, _b);
            int r,g,b;
            r = static_cast<int>(255 * _r);
            g = static_cast<int>(255 * _g);
            b = static_cast<int>(255 * _b);

            pen = QPen(QColor(r,g,b,96));
            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawPoints(point, 1);
        }
    }
}


void HeatMap::timerEvent(QTimerEvent *event) {

    if (event->timerId() == timer.timerId()) {
        update();
    } else {
        QWidget::timerEvent(event);
    }
}


void HeatMap::fftEvent(double packet[125]){

    for (int f=0; f < 125; f++) {

        std::rotate(data[f], data[f]+399, data[f]+400);
        data[f][0] = packet[f];
    }
}
