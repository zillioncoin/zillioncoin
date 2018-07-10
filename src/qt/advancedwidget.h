#ifndef BUTTONADVANCED_H
#define BUTTONADVANCED_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QGraphicsOpacityEffect>

#include <QPropertyAnimation>

#include <QPixmap>

struct selected_menues{
    int menu;
    int sub_menu;
};

class AdvancedWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AdvancedWidget(QWidget *parent = nullptr);
    ~AdvancedWidget();

    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);

    virtual void mouseReleaseEvent(QMouseEvent *event);

    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);

    void fadeTo(qreal opacity, int duration, int delay);
    void moveTo(int x, int y, int duration, int delay = 0, QEasingCurve easing_curve=QEasingCurve::Linear);

    bool is_fading_on_mouseover;
    int bg_color;
    void setColor(int _bg_color_);

    QTimer *delayTimer_fadeTo;
    QTimer *delayTimer_moveTo;

    QGraphicsOpacityEffect *opacity;

    QPropertyAnimation *alphaFade;
    QPropertyAnimation *animation;

    int target_move_x;
    int target_move_y;
    int duration_move;

    QEasingCurve target_move_easing_curve;

    qreal target_opacity;
    int duration_opacity;

    //shadows
    int shadow_width;
    qreal shadow_opacity;

    QPixmap pix_shadow_corner[4];
    QPixmap pix_shadow_edge[4];
    QLabel *shadow_corner[4];
    QLabel *shadow_edge[4];

    QGraphicsOpacityEffect *shadow_corner_opacity[4];
    QGraphicsOpacityEffect *shadow_edge_opacity[4];

    void setChecked(bool _is_fading_on_mouseover_);
    bool checked = false;

signals:
    void released(int, int);
    void rollover(int,int);
    void rollout(int,int);
public slots:

    void delayFinished_fadeTo();
    void delayFinished_moveTo();

    void animationHasFinished();

    void updateShadows();
};

#endif // BUTTONADVANCED_H
