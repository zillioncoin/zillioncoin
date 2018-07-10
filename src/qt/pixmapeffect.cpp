#include "pixmapeffect.h"

QPixmap PixmapEffect::tint(const QPixmap ressource, uint tint)
{
    QImage img = ressource.toImage().convertToFormat(QImage::Format_ARGB32,Qt::AutoColor);

    uint ARGB;
    uint ARGB_tint;
    for(int x=0; x<img.width();x++){
        for(int y=0; y<img.height();y++){
            ARGB = (uint)img.pixel(x,y);
            ARGB_tint = (ARGB & 0xFF000000) + tint;
            img.setPixel(x,y, ARGB_tint);
        }
    }
    return QPixmap::fromImage(img);
}
