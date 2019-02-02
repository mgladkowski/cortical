#include "heatmap.h"

HeatMap::HeatMap(QWidget *parent) : QWidget(parent) {

    //setBackgroundRole(QPalette::Base);
    //setAutoFillBackground(true);
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

        for (int f=0; f < 100; f++) {

            point[0] = QPoint(t,f);

            if ( HeatMap::isDoubleEqual( data[f][t], 0.0 ) ) {
                pen = QPen(Qt::black);
            } else if ( data[f][t] < bp[0] ) {
                pen = QPen(Qt::black);
            } else if ( data[f][t] < bp[1] ) {
                pen = QPen(Qt::darkRed);
            } else if ( data[f][t] < bp[2] ) {
                pen = QPen(Qt::red);
            } else if ( data[f][t] < bp[3] ) {
                pen = QPen(Qt::yellow);
            } else if ( data[f][t] < bp[4] ) {
                pen = QPen(Qt::green);
            } else if ( data[f][t] < bp[5] ) {
                pen = QPen(Qt::cyan);
            } else if ( data[f][t] < bp[6] ) {
                pen = QPen(Qt::blue);
            } else if ( data[f][t] < bp[7] ) {
                pen = QPen(Qt::darkMagenta);
            } else if ( data[f][t] < bp[8] ) {
                pen = QPen(Qt::magenta);
            } else if ( data[f][t] >= bp[9] ) {
                pen = QPen(Qt::white);
            }
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


bool HeatMap::isDoubleEqual(double a, double b) {

    return fabs(a - b) < DBL_EPSILON;
}
