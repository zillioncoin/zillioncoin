#ifndef QGRAPHICSTINTEFFECT_H
#define QGRAPHICSTINTEFFECT_H

#include <QGraphicsEffect>

#include <QPainter>

class QGraphicsTintEffect : public QGraphicsEffect
{
public:
    QGraphicsTintEffect(QWidget *parent = 0);

    virtual void draw(QPainter *painter);

    uint tint;
    void setTint(uint _tint_);
};

#endif // QGRAPHICSTINTEFFECT_H
