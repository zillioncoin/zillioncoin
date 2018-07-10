#ifndef PIXMAPEFFECT_H
#define PIXMAPEFFECT_H

#include <QPixmap>

class PixmapEffect
{
public:    
    static QPixmap tint(const QPixmap ressource, uint tint);
};

#endif // PIXMAPEFFECT_H
