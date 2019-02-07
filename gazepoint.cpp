#include "gazepoint.h"

GazePoint::GazePoint(QWidget *parent) : QWidget(parent) {

    pen = QPen(QColor(Qt::transparent));
    pen.setWidth(0);

    gradient = QRadialGradient(7,7,8);
    gradient.setColorAt(0.0, QColor(128,128,255,8));
    gradient.setColorAt(1.0, QColor(128,128,255,0));
    rect = QRect(0, 0, 15, 15);
}


QSize GazePoint::minimumSizeHint() const {

    return QSize(2, 2);
}


QSize GazePoint::sizeHint() const {

    return QSize(15, 15);
}


void GazePoint::paintEvent(QPaintEvent *) {

    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(gradient);
    painter.drawEllipse(rect);
}
