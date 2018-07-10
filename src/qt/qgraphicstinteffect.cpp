#include "qgraphicstinteffect.h"

#include <QDebug>

#include <QColor>

QGraphicsTintEffect::QGraphicsTintEffect(QWidget *parent)
{
    qDebug() << "init";
}

void QGraphicsTintEffect::draw(QPainter *painter)
{
    //qDebug() << sourceIsPixmap();
    //qDebug() << sourceBoundingRect();

    QPoint offset;

    // Draw pixmap in device coordinates to avoid pixmap scaling;
    const QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates, &offset);
    painter->setWorldTransform(QTransform());
    //qDebug() << pixmap.hasAlpha() << pixmap.hasAlphaChannel();

    QImage img = pixmap.toImage().convertToFormat(QImage::Format_ARGB32,Qt::AutoColor);
    //img.convertToFormat(QImage::Format_ARGB32,Qt::AutoColor);

    //qDebug() << img.rect();
    //img.invertPixels(QImage::InvertRgb);

    uint ARGB;
    uint ARGB_tint;
    for(int x=0; x<img.width();x++){
        for(int y=0; y<img.height();y++){
            //img.setPixel(x,0,0x7F336699);
            //QColor color(img.pixel(x,0));
            ARGB = (uint)img.pixel(x,y);
            //qDebug() << x << 0 << ARGB << (ARGB >> 24 & 255) << (ARGB >> 16 & 255) << (ARGB >> 8 & 255) << (ARGB >> 0 & 255);
            ARGB_tint = (ARGB & 0xFF000000) + tint;
            img.setPixel(x,y, ARGB_tint);
            //qDebug() << x << 0 << color.red() << color.green() << color.blue() << "      " << << (uint)img.pixel(x,0) << img.pixelFormat().alphaPosition() << img.format() << QImage::Format_ARGB32_Premultiplied;
            //img.setPixel(qrand()%128,qrand()%128,0xFF000000);
        }
    }
    painter->drawPixmap(offset, QPixmap::fromImage(img));

}

void QGraphicsTintEffect::setTint(uint _tint_)
{
    tint = _tint_;
    update();
}
