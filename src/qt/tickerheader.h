#ifndef TICKERHEADER_H
#define TICKERHEADER_H

#include <QLabel>
#include <QObject>
#include <QPainter>
#include <QResizeEvent>
#include <QTimer>
#include <QWidget>

class TickerHeader : public QWidget
{
    Q_OBJECT

public:
    explicit TickerHeader(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent *event);

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    QColor bg_color;

    QTimer *ticker;

    QFrame *bg;

    QLabel *textCarrier;

    QFont fontToMeasure;

    QList<int> labelWidth;

    QPixmap base_pixmap;
    QPixmap pixmap;

    int main_offset_x;
    int move_step;
    int old_width_of_banner;
    int current_width_of_banner;

    bool initiated;

signals:

public slots:

    void onEnterTicker();
    void setTickerData(QList<QString> stringList);
};

#endif // TICKERHEADER_H
