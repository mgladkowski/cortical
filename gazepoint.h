#ifndef GAZEPOINT_H
#define GAZEPOINT_H

#include <QPainter>
#include <QWidget>

class GazePoint : public QWidget {

    Q_OBJECT

public:

    explicit    GazePoint(QWidget *parent = nullptr);
    QSize       minimumSizeHint() const override;
    QSize       sizeHint() const override;

protected:

    void        paintEvent(QPaintEvent *event) override;

private:

    QPen        pen;
    QRect       rect;
    QRadialGradient gradient;
};

#endif // GAZEPOINT_H
