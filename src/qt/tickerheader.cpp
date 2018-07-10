#include "tickerheader.h"

#include <QDebug>

TickerHeader::TickerHeader(QWidget *parent) : QWidget(parent)
{

    main_offset_x = 0;
    move_step = 1;
    current_width_of_banner = 0;
    initiated = false;

    //bg_color = QColor(0xFF,0xFF,0xFF,0xFF);
    bg_color = QColor(0,0,0,0xFF);

    //setFixedSize(1000,20);
    setContentsMargins(0,0,0,0);

    //setStyleSheet("QWidget {background-color: #FFFFFF; color: #000000; font: bold 18px \"Open Sans\";}");

    setStyleSheet("background-color: #FFFFFF; color: #000000; font: bold 12px \"Open Sans\";");

    //adjustSize();

    bg = new QFrame(this);
    bg->resize(1000,20);
    bg->setStyleSheet("background-color: #FFFFFF;");

    textCarrier = new QLabel(this);

    /*int offset_x = 0;
    test = new QLabel(textCarrier);
    test->setText("BTC 001");
    test->move(offset_x, 2);
    test->adjustSize();

    offset_x = 200;
    test2 = new QLabel(textCarrier);
    test2->setText("lkjhlkjh");//li[0]+" "+li[1]);
    test2->move(offset_x, 2);
    test2->adjustSize();
*/

    fontToMeasure.setFamily("Open Sans");
    fontToMeasure.setBold(true);
    fontToMeasure.setPixelSize(12);


    ticker = new QTimer(this);
    connect(ticker, SIGNAL(timeout()), this, SLOT(onEnterTicker()));
    ticker->start(18);




    base_pixmap = QPixmap(8000,20);
    base_pixmap.fill(bg_color);//transparent);


    pixmap = QPixmap(4500,20);
    pixmap.fill(bg_color);

    //QPainter p(&base_pixmap);
    //    QPainter p(&pixmap);

    //    p.setRenderHint(QPainter::HighQualityAntialiasing);
    //    p.setRenderHint(QPainter::TextAntialiasing);

    //    /*QBrush brush;
    //    brush.setStyle(Qt::SolidPattern);
    //    brush.setColor(0xFF6a3a66);
    //    p.setBrush(brush);

    //    p.fillRect(QRect(0,0,4000,20),brush);*/
    //    //QPen col;
    //    //col.setColor(QColor(0,0,0,255));
    //    //p.setPen(col);
    //    p.setFont(fontToMeasure);
    //    p.drawText(QRect(0,0,400,20), "v0.0.2, ob's du's glaub",QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
    //    p.end();





    QPainter p2(&base_pixmap);

    p2.setRenderHint(QPainter::HighQualityAntialiasing);
    p2.drawPixmap(0,0,8000,20,pixmap);
    p2.end();

    textCarrier->setPixmap(base_pixmap);

}

void TickerHeader::resizeEvent(QResizeEvent *event)
{

    bg->resize(this->width(),20);
    textCarrier->resize(this->width(),20);
}

void TickerHeader::enterEvent(QEvent *event)
{
    qDebug() << "enterEvent";
    move_step = 0;
}

void TickerHeader::leaveEvent(QEvent *event)
{
    qDebug() << "leaveEvent";
    move_step = 1;
}

void TickerHeader::onEnterTicker()
{
    //qDebug() << "wid:" << this->width() << "main_offset_x" << main_offset_x << isVisible();
    if(initiated){
        //textCarrier->move(textCarrier->x()-1, textCarrier->y());

        base_pixmap.scroll(-move_step,0,base_pixmap.rect());

        main_offset_x -= move_step;

        /*QPainter p3(&base_pixmap);

    p3.setRenderHint(QPainter::HighQualityAntialiasing);
    //p3.drawPixmap(0,0,4000,20,pixmap);
    p3.end();*/

        textCarrier->setPixmap(base_pixmap);
    }
}

void TickerHeader::setTickerData(QList<QString> stringList)
{
    //qDebug() << "wid:" << this->width() << "main_offset_x" << main_offset_x << isVisible();
    //textCarrier->resize(this->width(),20);

    /*QLabel ass(textCarrier);
    ass.setText("lkjhlkjh");
    ass.adjustSize();

    test->move(test->x()-10,test->y());*/


    //qDebug() << stringList;

    //qDebug() << stringList.length();

    QFontMetrics metric(fontToMeasure);

    /*

        test2 = new QLabel(textCarrier);
        test2->setText("lkjhlkjh");//li[0]+" "+li[1]);
        test2->move(offset_x, 2);
        test2->resize(100,20);*/


    int offset_x = 0;

    QPainter p(&pixmap);

    //pixmap.fill(Qt::white);
    pixmap.fill(bg_color);

    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    p.setFont(fontToMeasure);



    QPen pen;


    //for(int i = 0;i<stringList.length();i++){
    for(int i = 0;i<20;i++){
        QStringList li = stringList[i].split("---");
        QString stringText = li[0]+" : "+li[1]+" "+li[3];
        int i_width = metric.width(stringText)+40;
        //qDebug() << i << metric.width(stringList[i]) << li << metric.width(li[0]+" "+li[1]);

        if(li[2] == "red"){
            pen.setColor(0xCF0000);
        } else{
            pen.setColor(0x008F00);
        }

        pen.setColor(0xFFFFFF);/**/

        p.setPen(pen);
        p.drawText(QRect(offset_x,0,i_width,20), stringText,QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
        if(li[2] == "red"){
            p.drawPixmap(offset_x+i_width-32,6,9,9,QPixmap(":/gui/ticker_red_down") );
        } else{
            p.drawPixmap(offset_x+i_width-32,6,9,9,QPixmap(":/gui/ticker_green_up") );
        }

        offset_x += i_width;
    }

    old_width_of_banner = current_width_of_banner;
    current_width_of_banner = offset_x;

    p.end();

    qDebug() << "current_width_of_banner" << current_width_of_banner;

    QPainter p2(&base_pixmap);

    int width = this->width();
    int new_offset_x = main_offset_x;
    if(old_width_of_banner != 0){
        new_offset_x = main_offset_x;
        while(new_offset_x < width){
            new_offset_x += old_width_of_banner;
        }

        main_offset_x = new_offset_x;
    }

    p2.setRenderHint(QPainter::HighQualityAntialiasing);
    for(int i=0;i<8000;i+=current_width_of_banner){
        p2.drawPixmap(new_offset_x+i,0,4500,20,pixmap);
    }
    p2.end();

    textCarrier->setPixmap(base_pixmap);

    initiated = true;
}
