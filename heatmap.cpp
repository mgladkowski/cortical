#include "heatmap.h"

HeatMap::HeatMap(QWidget *parent) : QWidget(parent) {

    //setBackgroundRole(QPalette::Base);
    //setAutoFillBackground(true);

    memset(data, 0, sizeof(data));

    timer.start(33, this);

    //int x[5] = { 5, 4, 3, 2, 1 };
    //qDebug() << x[0] << x[1] << x[2] << x[3] << x[4];
    //std::rotate(x, x+4, x+5);
    //x[0] = 6;
    //qDebug() << x[0] << x[1] << x[2] << x[3] << x[4];
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

    for (int x=0; x < 400; x++) {

        for (int y=0; y < 125; y++) {

            point[0] = QPoint(x,y);

            if ( HeatMap::isDoubleEqual( data[x][y], 0.0 ) ) {
                pen = QPen(Qt::black);
            } else {
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


void HeatMap::fftEvent(double data[125]){

    Q_UNUSED(data);
}


bool HeatMap::isDoubleEqual(double a, double b) {

    return fabs(a - b) < DBL_EPSILON;
}
