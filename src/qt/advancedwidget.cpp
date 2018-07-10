#include "advancedwidget.h"
#include "advancedwidget.h"

#include <QPainter>

#include <QDebug>

#include <QPixmap>

AdvancedWidget::AdvancedWidget(QWidget *parent) :
    QWidget(parent)
{
    /*for(int i = 0; i<100; i++){
        qDebug() << "button advanced init" << qrand()%10;
    }*/

    //da8d21
    is_fading_on_mouseover = false;
    bg_color = 0xffffff;

    //    iconLabel = new QLabel(this);
    //    iconLabel->setPixmap( QPixmap(":/icons/deco_recent"));//gui_header_logo"));
    //    iconLabel->setContentsMargins(0,0,0,0);
    //    iconLabel->setAttribute( Qt::WA_TransparentForMouseEvents );
    //    iconLabel->adjustSize();

    //    textLabel = new QLabel(textString,this);
    //    textLabel->setFixedHeight(50);
    //    textLabel->move(60,0);
    //    //border:1px solid;
    //    textLabel->setContentsMargins(0,0,0,0);
    //    textLabel->setAttribute( Qt::WA_TransparentForMouseEvents );

    //    textLabel->setStyleSheet(".QLabel{color:#FFFFFF; font: bold 22px \"Open Sans\";}");
    //    textLabel->setAlignment(Qt::AlignVCenter);
    //    textLabel->adjustSize();


    opacity = new QGraphicsOpacityEffect(this);
    opacity->setOpacity(1);
    //setGraphicsEffect(opacity);

    //Shadows

    shadow_width = 10;
    shadow_opacity = 0.3;

    QMatrix mat;
    mat.rotate(90);

    /*for(int i = 0; i<4;i++){
        pix_shadow_corner[i] = new QPixmap(10,10);
        pix_shadow_corner[i]->fill(Qt::transparent);
        pix_shadow_edge[i] = new QPixmap(10,10);
        pix_shadow_edge[i]->fill(Qt::transparent);
    }*/

    for(int i = 0; i<4;i++){
        pix_shadow_corner[i] = QPixmap(10,10);
        pix_shadow_corner[i].fill(Qt::transparent);
        pix_shadow_edge[i] = QPixmap(10,10);
        pix_shadow_edge[i].fill(Qt::transparent);
    }


    QPainter *painter = new QPainter();
    painter->begin(&pix_shadow_corner[0]);
    painter->setOpacity(shadow_opacity);
    painter->drawPixmap(0, 0, QPixmap(":/gui/shadow_corner"));
    painter->end();
    painter->begin(&pix_shadow_edge[0]);
    painter->setOpacity(shadow_opacity);
    painter->drawPixmap(0, 0, QPixmap(":/gui/shadow_edge"));
    painter->end();


    //= QPixmap(":/icons/shadow_corner");
    pix_shadow_corner[1] = pix_shadow_corner[0].transformed(mat);
    pix_shadow_corner[2] = pix_shadow_corner[1].transformed(mat);
    pix_shadow_corner[3] = pix_shadow_corner[2].transformed(mat);

    //pix_shadow_edge[0] = QPixmap(":/icons/shadow_edge");
    pix_shadow_edge[1] = pix_shadow_edge[0].transformed(mat);
    pix_shadow_edge[2] = pix_shadow_edge[1].transformed(mat);
    pix_shadow_edge[3] = pix_shadow_edge[2].transformed(mat);

    for(int i = 0; i<4;i++){
        shadow_corner[i] = new QLabel(parent);
        shadow_corner[i]->setPixmap(pix_shadow_corner[i]);
        //QImage img = QImage(":/icons/");
        //shadow_corner->setPixmap(QPixmap::fromImage(QImage(":/icons/bitcoin")));//shadow_top_right")));
        shadow_corner[i]->setScaledContents(true);
        shadow_corner[i]->setFixedSize(shadow_width,shadow_width);
        shadow_corner[i]->setAttribute( Qt::WA_TransparentForMouseEvents );
        shadow_corner[i]->adjustSize();
        shadow_corner[i]->lower();

        shadow_edge[i] = new QLabel(parent);
        shadow_edge[i]->setPixmap(pix_shadow_edge[i]);
        shadow_edge[i]->setScaledContents(true);
        shadow_edge[i]->setFixedSize(shadow_width,shadow_width);
        shadow_edge[i]->setAttribute( Qt::WA_TransparentForMouseEvents );
        shadow_edge[i]->adjustSize();
        shadow_edge[i]->lower();

        shadow_corner_opacity[i] = new QGraphicsOpacityEffect(shadow_corner[i]);
        shadow_corner_opacity[i]->setOpacity(1);
        shadow_corner[i]->setGraphicsEffect(shadow_corner_opacity[i]);

        shadow_edge_opacity[i] = new QGraphicsOpacityEffect(shadow_edge[i]);
        shadow_edge_opacity[i]->setOpacity(1);
        shadow_edge[i]->setGraphicsEffect(shadow_edge_opacity[i]);

    }

}

AdvancedWidget::~AdvancedWidget()
{
    for(int i = 0;i<4;i++){
        //delete pix_shadow_corner[i];
        //delete pix_shadow_edge[i];
        //delete shadow_corner[i];
        //delete shadow_edge[i];
    }
}

void AdvancedWidget::resizeEvent(QResizeEvent *event)
{
}

void AdvancedWidget::fadeTo(qreal opacity, int duration, int delay){

    target_opacity = opacity;
    duration_opacity = duration;

    delayTimer_fadeTo = new QTimer(this);
    connect(delayTimer_fadeTo,SIGNAL(timeout()), this, SLOT(delayFinished_fadeTo()));

    delayTimer_fadeTo->setSingleShot(true);
    delayTimer_fadeTo->start(delay);
}

void AdvancedWidget::moveTo(int x, int y, int duration, int delay, QEasingCurve easing_curve){

    target_move_x = x;
    target_move_y = y;
    duration_move = duration;

    target_move_easing_curve = easing_curve;

    delayTimer_moveTo = new QTimer(this);
    connect(delayTimer_moveTo,SIGNAL(timeout()), this, SLOT(delayFinished_moveTo()));

    delayTimer_moveTo->setSingleShot(true);
    delayTimer_moveTo->start(delay);
}

void AdvancedWidget::setColor(int _bg_color_)
{
    bg_color = _bg_color_;
}

void AdvancedWidget::setChecked(bool _is_fading_on_mouseover_)
{
    checked = _is_fading_on_mouseover_;
    //is_fading_on_mouseover = !_is_fading_on_mouseover_;
    if(checked){
        fadeTo(1,100,0);
    } else{
        fadeTo(0,100,0);
    }
}

void AdvancedWidget::delayFinished_moveTo()
{
    //qDebug() << "delay finished";
    animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(duration_move);
    animation->setStartValue(QPoint(this->x(), this->y()));
    //if(way == "in"){
    animation->setEndValue(QPoint(target_move_x,target_move_y));
    /*} else{
        animation->setEndValue(QPoint(10,125+MENU_OFFSET-50));
    }*/
    animation->setEasingCurve(target_move_easing_curve);//InCubic);
    animation->start();

    connect(animation, SIGNAL(finished()), this, SLOT(animationHasFinished()));
    connect(animation, SIGNAL(valueChanged(QVariant)),this,SLOT(updateShadows()));
}

void AdvancedWidget::delayFinished_fadeTo()
{
    //qDebug() << "delay finished";
    /*QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(250);
    animation->setStartValue(QPoint(this->x(), this->y()));
    //if(way == "in"){
    animation->setEndValue(QPoint(move_target_x,move_target_y));
    /*} else{
        animation->setEndValue(QPoint(10,125+MENU_OFFSET-50));
    }*/
    /* animation->setEasingCurve(QEasingCurve::OutCubic);//InCubic);
    animation->start();*/

    alphaFade = new QPropertyAnimation(opacity, "opacity");
    alphaFade->setDuration(duration_opacity);
    alphaFade->setStartValue(opacity->opacity());
    alphaFade->setEndValue(target_opacity);
    alphaFade->start();

    connect(alphaFade, SIGNAL(finished()), this, SLOT(animationHasFinished()));
    connect(alphaFade, SIGNAL(valueChanged(QVariant)),this,SLOT(updateShadows()));
    connect(alphaFade, SIGNAL(valueChanged(QVariant)),this,SLOT(update()));
}

void AdvancedWidget::animationHasFinished()
{
    //qDebug() << "animation has finished";
    //updateShadows();

}

void AdvancedWidget::updateShadows()
{
    QPoint globalTL = this->mapToParent(this->rect().topLeft());
    //QPoint globalTR = this->mapToParent(this->rect().topRight());
    //QPoint globalBR = this->mapToParent(this->rect().bottomRight());
    //QPoint globalBL = this->mapToParent(this->rect().bottomLeft());
    //qDebug() << this->geometry() << globalTL << this->x() << this->y() << "shadow" << opacity->opacity();

    shadow_corner[0]->move(globalTL.x()-shadow_width, globalTL.y()-shadow_width);
    shadow_corner[1]->move(globalTL.x()+this->width(), globalTL.y()-shadow_width);
    shadow_corner[2]->move(globalTL.x()+this->width(), globalTL.y()+this->height());
    shadow_corner[3]->move(globalTL.x()-shadow_width, globalTL.y()+this->height());

    shadow_edge[0]->move(globalTL.x(), globalTL.y()-shadow_width);
    shadow_edge[0]->setFixedSize(this->width(),shadow_width);
    shadow_edge[1]->move(globalTL.x()+this->width(), globalTL.y());
    shadow_edge[1]->setFixedSize(shadow_width,this->height());
    shadow_edge[2]->move(globalTL.x(), globalTL.y()+this->height());
    shadow_edge[2]->setFixedSize(this->width(),shadow_width);
    shadow_edge[3]->move(globalTL.x()-shadow_width, globalTL.y());
    shadow_edge[3]->setFixedSize(shadow_width,this->height());

    for(int i = 0;i<4;i++){
        shadow_corner_opacity[i]->setOpacity(opacity->opacity());
        shadow_edge_opacity[i]->setOpacity(opacity->opacity());
    }
}

void AdvancedWidget::paintEvent(QPaintEvent *event)
{
    //qDebug() << "paintE";

    QPen pen;
    pen.setColor(0x98958f);

    QPainter painter(this);

    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    //painter.setRenderHint(QPainter::TextAntialiasing);

    painter.setPen(pen);

    QRect rect;

    rect = QRect(0,0,this->width(),this->height());

    /*if(state == "leave"){
        painter.fillRect(rect,0xdad7d1);//e5e2dc);//dad7d1
        painter.fillRect(rect.adjusted(130,5,-5,-5),0xEFEFEF);
    }
    if(state == "enter"){*/

    painter.setOpacity(opacity->opacity());
    painter.fillRect(rect,bg_color);
    //painter.fillRect(rect.adjusted(130,5,-5,-5),0xFFFFFF);
    //}

    //painter.drawRect(rect.adjusted(130,5,-5,-5));
    //painter.drawRect(rect);

    updateShadows();
}

void AdvancedWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug() << "property(index)" << property("index");
    emit released(property("index").toInt(), property("index2").toInt());
}

void AdvancedWidget::enterEvent(QEvent *)
{
    //qDebug() << "enterEvent";
    setCursor(Qt::PointingHandCursor);
    if(!checked){//is_fading_on_mouseover){
        fadeTo(1,100,0);
    }
    emit rollover(property("index").toInt(), property("index2").toInt());
}
void AdvancedWidget::leaveEvent(QEvent *)
{
    //qDebug() << "leaveEvent";
    setCursor(Qt::ArrowCursor);
    if(!checked){//
        fadeTo(0,100,0);
    } /*else{
        fadeTo(0.75,100,0);
    }*/
    emit rollout(property("index").toInt(), property("index2").toInt());
}
