#include "guiheader.h"

#include <QFormLayout>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <qsystemtrayicon.h>

#include "main.h"
#include "bitcoinrpc.h"
#include "util.h"

#include "clientmodel.h"
#include "walletmodel.h"
#include "bitcoinunits.h"

#include "optionsmodel.h"
#include "transactiontablemodel.h"
#include "transactionfilterproxy.h"
#include "guiutil.h"
#include "guiconstants.h"

#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QStyleOptionTabV3>

int screenId = 0;

extern json_spirit::Value GetNetworkHashPS(int lookup, int height);
extern json_spirit::Value ValueFromAmount(int64 amount);
extern int64 GetTotalSupply();

QFrameAdvanced::QFrameAdvanced(QWidget *parent)
{
    //setWindowFlags(Qt::ToolTip);

    shadow_width = 10;
    shadow_opacity = 0.3;

    QMatrix mat;
    mat.rotate(90);

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
        shadow_corner[i]->adjustSize();

        shadow_edge[i] = new QLabel(parent);
        shadow_edge[i]->setPixmap(pix_shadow_edge[i]);
        shadow_edge[i]->setScaledContents(true);
        shadow_edge[i]->setFixedSize(shadow_width,shadow_width);
        shadow_edge[i]->adjustSize();
    }

    //shadow_corner->move(-70,0);
    //adjustSize();
    //updateGeometry();
}
void QFrameAdvanced::resizeEvent(QResizeEvent *event)
{

    QPoint globalTL = this->mapToParent(this->rect().topLeft());
    //QPoint globalTR = this->mapToParent(this->rect().topRight());
    //QPoint globalBR = this->mapToParent(this->rect().bottomRight());
    //QPoint globalBL = this->mapToParent(this->rect().bottomLeft());
    qDebug() << this->geometry() << globalTL;

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

}

/*QWidgetAdvanced::QWidgetAdvanced(QWidget *parent)
{
    setContentsMargins(0,0,0,0);
    adjustSize();
    setAttribute(Qt::WA_TranslucentBackground);
}

void QWidgetAdvanced::resizeEvent(QResizeEvent *event)
{
    qDebug() << this->geometry();
}

void QWidgetAdvanced::paintEvent(QPaintEvent *event)
{
    //QWidget::paintEvent(event);
    QPainter p(this);
    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
*/

ClickableLabel::ClickableLabel(QWidget * parent ) :
    QLabel(parent)
{
}

ClickableLabel::~ClickableLabel()
{
}

void ClickableLabel::mousePressEvent ( QMouseEvent * event )
{
    emit clicked();
}

GuiHeader::~GuiHeader(){
}

GuiHeader::GuiHeader(QWidget *parent) :
    QWidget(parent),
    clientModel(0),
    walletModel(0),
    currentBalance(-1),
    currentUnconfirmedBalance(-1),
    currentImmatureBalance(-1)
{

    //QFont largeFont; largeFont.setPixelSize(20);
    QFont boldFont; boldFont.setBold(true);
    //QFont font;   font.setFamily(":/font/montserrat-bold");       font.setPixelSize(12);       font.setStyleStrategy(QFont::PreferAntialias);       font.setBold(true);
    //QFont fontV;   fontV.setFamily(":/font/montserrat-bold");       fontV.setPixelSize(9);       fontV.setStyleStrategy(QFont::PreferAntialias);       fontV.setBold(false);

    QFont font;   font.setPixelSize(12);       font.setStyleStrategy(QFont::PreferAntialias);       font.setBold(true);
    QFont fontV;     fontV.setPixelSize(9);       fontV.setStyleStrategy(QFont::PreferAntialias);       fontV.setBold(false);


    setFixedSize(1000,165);
    setContentsMargins(0,0,0,0);

    //backGround = new QWidget(this);
    //backGround->setFixedSize(1000,120);
    //backGround->setStyleSheet("background-color: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #6c3d94, stop: 1 #a13469)");
    //backGround->setStyleSheet("background-color: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #7fffffff, stop: 1 #ffff0000)");

    /*bottomLine = new QWidget(this);
    bottomLine->setFixedSize(1000,5);
    bottomLine->move(0,115);
    bottomLine->setStyleSheet("background-color: #3c3c3b");*/

    spreadCoinLogo = new ClickableLabel(this);
    spreadCoinLogo->setPixmap( QPixmap(":/icons/zillioncoin_logo_125"));//gui_header_logo"));
    spreadCoinLogo->setContentsMargins(0,0,0,0);
    spreadCoinLogo->move(30,20);
    spreadCoinLogo->adjustSize();

    /*QGraphicsDropShadowEffect * dse = new QGraphicsDropShadowEffect();
    dse->setBlurRadius(20);
    dse->setXOffset(10);
    dse->setYOffset(10);
    dse->setColor(QColor(0, 0, 0));
    spreadCoinLogo->setGraphicsEffect(dse);*/

    versionLabel = new QLabel(this);
    versionLabel->setFont(fontV);
    versionLabel->move(0,90);
    versionLabel->setFixedWidth(95);
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setStyleSheet("color: #000000");
    versionLabel->setVisible(false);
    //versionLabel->setText("v 0.9.15.5");

    /*QWidget *separatorLine = new QWidget(this);
    separatorLine->setFixedSize(2,115);
    separatorLine->move(93,0);
    separatorLine->setStyleSheet("background-color: #3c3c3b");*/

    walletOverview = new QFrameAdvanced(this);
    walletOverview->setStyleSheet(".QFrameAdvanced{background-color: #cdffffff;}");
    //walletOverview->setStyleSheet("QFrame {color: #000000;} .QLabel { border: none; background: none; }");


    QWidget *walletOverviewBox = new QWidget(walletOverview);
    //walletOverviewBox->setStyleSheet(".QWidgetAdvanced{background-color: #cdffffff;}");

    //walletOverviewBox->lower();

    decoWallet = new QLabel(walletOverview);
    decoWallet->setPixmap( QPixmap(":/icons/deco_wallet"));//gui_header_logo"));
    decoWallet->setContentsMargins(0,0,0,0);
    decoWallet->adjustSize();

    //walletOverviewBox->setGraphicsEffect(dse);

    balanceTitle = new QLabel();
    //balanceTitle->setFont(boldFont);
    balanceTitle->setText("Balance:");
    balanceTitle->setCursor(Qt::IBeamCursor);
    balanceTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);

    unconfirmedTitle = new QLabel();
    //unconfirmedTitle->setFont(boldFont);
    unconfirmedTitle->setText("Unconfirmed:");
    unconfirmedTitle->setCursor(Qt::IBeamCursor);
    unconfirmedTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);

    immatureTitle = new QLabel();
    //immatureTitle->setFont(boldFont);
    immatureTitle->setText("Immature:");
    immatureTitle->setCursor(Qt::IBeamCursor);
    immatureTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);


    labelBalance = new QLabel();
    //labelBalance->setFont(boldFont);
    labelBalance->setText("---");
    labelBalance->setCursor(Qt::IBeamCursor);
    labelBalance->setTextInteractionFlags(Qt::TextSelectableByMouse);

    labelUnconfirmedBalance = new QLabel();
    //labelUnconfirmedBalance->setFont(boldFont);
    labelUnconfirmedBalance->setText("---");
    labelUnconfirmedBalance->setCursor(Qt::IBeamCursor);
    labelUnconfirmedBalance->setTextInteractionFlags(Qt::TextSelectableByMouse);

    labelImmatureBalance = new QLabel();
    //labelImmatureBalance->setFont(boldFont);
    labelImmatureBalance->setText("---");
    labelImmatureBalance->setCursor(Qt::IBeamCursor);
    labelImmatureBalance->setTextInteractionFlags(Qt::TextSelectableByMouse);


    walletTitle = new QLabel(walletOverview);
    walletTitle->setStyleSheet("QLabel { color: #000000; font: bold 18px \"Open Sans\";}");
    //walletTitle->setFont(font);
    walletTitle->setAlignment(Qt::AlignLeft);
    walletTitle->setText("WALLET <font color=#CC0000>(out of sync)</font>");
    walletTitle->setFixedWidth(200);
    walletTitle->move(15,10);

    QFormLayout *formLayout = new QFormLayout();

    //formLayout->addWidget(walletTitle);
    formLayout->addRow(balanceTitle, labelBalance);
    formLayout->addRow(unconfirmedTitle, labelUnconfirmedBalance);
    formLayout->addRow(immatureTitle, labelImmatureBalance);

    formLayout->setContentsMargins(15,42,0,10);
    formLayout->setVerticalSpacing(8);
    formLayout->setHorizontalSpacing(10);
    walletOverviewBox->setLayout(formLayout);

    QHBoxLayout  *boxLayout = new QHBoxLayout();
    boxLayout->addWidget(walletOverviewBox);
    boxLayout->setContentsMargins(0,0,0,0);
    walletOverview->setLayout(boxLayout);

    walletOverview->setMinimumWidth(280);
    walletOverview->setMaximumWidth(280);








    recentOverview = new QFrameAdvanced(this);
    recentOverview->setStyleSheet(".QFrameAdvanced{background-color: #cdffffff;}");
    //recentOverview->setStyleSheet("QFrame {color: #000000;} .QLabel { border: none; background: none; }");

    recentOverviewBox = new QWidget(recentOverview);
    //recentOverviewBox->setStyleSheet(".QWidget {background-color: #cdffffff;}");

    decoRecent = new QLabel(recentOverview);
    decoRecent->setPixmap( QPixmap(":/icons/deco_recent"));//gui_header_logo"));
    decoRecent->setContentsMargins(0,0,0,0);
    decoRecent->setAttribute( Qt::WA_TransparentForMouseEvents );
    decoRecent->adjustSize();


    QLabel *recentTitle = new QLabel(recentOverview);
    recentTitle->setStyleSheet("QLabel { color: #000000; font: bold 18px \"Open Sans\";}");
    recentTitle->setAlignment(Qt::AlignLeft);
    recentTitle->setText("RECENT EVENTS");
    recentTitle->setFixedWidth(150);
    recentTitle->move(15,10);

    for(int a=0;a<3;a++){
        recentLabel[a][0] = new ClickableLabel(recentOverview);
        //recentLabel[a][0]->setFont(boldFont);
        recentLabel[a][1] = new ClickableLabel(recentOverview);
        recentLabel[a][2] = new ClickableLabel(recentOverview);
    }

    for(int b=0;b<3;b++){
        recentOverviewCol[b] = new QWidget(recentOverview);
        recentOverviewCol[b]->setStyleSheet("border: none; background: none;");
        recentColLayout[b] = new QVBoxLayout();

        recentColLayout[b]->addWidget(recentLabel[0][b]);
        recentColLayout[b]->addWidget(recentLabel[1][b]);
        recentColLayout[b]->addWidget(recentLabel[2][b]);

        recentColLayout[b]->setContentsMargins(0,0,0,0);
        recentColLayout[b]->setSpacing(0);
        //recentColLayout[b]->activate();
        recentOverviewCol[b]->setLayout(recentColLayout[b]);
    }

    QHBoxLayout  *allRows = new QHBoxLayout();
    allRows->addWidget(recentOverviewCol[0]);
    allRows->addWidget(recentOverviewCol[1]);
    allRows->addWidget(recentOverviewCol[2]);
    allRows->setContentsMargins(15,38,30,12);//10,10-3,10,10);
    allRows->setSpacing(10);
    //allRows->activate();
    recentOverviewBox->setLayout(allRows);
    //recentOverviewBox->adjustSize();
    //recentOverviewBox->updateGeometry();

    QHBoxLayout  *boxLayout2 = new QHBoxLayout();
    boxLayout2->addWidget(recentOverviewBox);
    boxLayout2->setContentsMargins(0,0,0,0);
    //boxLayout2->activate();
    recentOverview->setLayout(boxLayout2);

    recentOverview->setMinimumWidth(300);
    recentOverview->setMaximumWidth(800);

    //recentOverview->adjustSize();
    //recentOverview->updateGeometry();


    networkOverview = new QFrameAdvanced(this);
    networkOverview->setStyleSheet(".QFrameAdvanced{background-color: #cdffffff;}");
    //networkOverview->setStyleSheet("QFrame {color: #000000;} .QLabel { border: none; background: none; }");


    QWidget *networkOverviewBox = new QWidget(networkOverview);
    //networkOverviewBox->setStyleSheet(".QWidget {background-color: #cdffffff;}");

    decoNetwork = new QLabel(networkOverview);
    decoNetwork->setPixmap( QPixmap(":/icons/deco_network"));//gui_header_logo"));
    decoNetwork->setContentsMargins(0,0,0,0);
    decoNetwork->adjustSize();

    blockCountTitle = new QLabel("Blockcount:");
    blockCountTitle->setCursor(Qt::IBeamCursor);
    blockCountTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);

    moneySupplyTitle = new QLabel("Moneysupply:");
    moneySupplyTitle->setCursor(Qt::IBeamCursor);
    moneySupplyTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);

    hashRateTitle = new QLabel("Hashrate:");
    hashRateTitle->setCursor(Qt::IBeamCursor);
    hashRateTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);

    blockCount = new QLabel();
    //blockCount->setFont(boldFont);
    blockCount->setText("---");
    blockCount->setCursor(Qt::IBeamCursor);
    blockCount->setTextInteractionFlags(Qt::TextSelectableByMouse);

    moneySupply = new QLabel();
    //moneySupply->setFont(boldFont);
    moneySupply->setText("---");
    moneySupply->setCursor(Qt::IBeamCursor);
    moneySupply->setTextInteractionFlags(Qt::TextSelectableByMouse);

    hashRate = new QLabel();
    //hashRate->setFont(boldFont);
    hashRate->setText("---");
    hashRate->setCursor(Qt::IBeamCursor);
    hashRate->setTextInteractionFlags(Qt::TextSelectableByMouse);


    networkTitle = new QLabel(networkOverview);
    networkTitle->setStyleSheet("QLabel { color: #000000; font: bold 18px \"Open Sans\";}");
    networkTitle->setAlignment(Qt::AlignLeft);
    networkTitle->setText("NETWORK <font color=#CC0000>(out of sync)</font>");
    networkTitle->setFixedWidth(200);
    networkTitle->move(15,10);

    QFormLayout *formLayout3 = new QFormLayout();

    formLayout3->addRow(blockCountTitle, blockCount);
    formLayout3->addRow(moneySupplyTitle, moneySupply);
    formLayout3->addRow(hashRateTitle, hashRate);

    formLayout3->setContentsMargins(15,42,0,10);
    formLayout3->setVerticalSpacing(8);
    formLayout3->setHorizontalSpacing(10);
    networkOverviewBox->setLayout(formLayout3);


    QHBoxLayout  *boxLayout3 = new QHBoxLayout();
    boxLayout3->addWidget(networkOverviewBox);
    boxLayout3->setContentsMargins(0,0,0,0);
    networkOverview->setLayout(boxLayout3);

    networkOverview->setMinimumWidth(230);
    networkOverview->setMaximumWidth(250);






    marketOverview = new QFrameAdvanced(this);
    marketOverview->setStyleSheet(".QFrameAdvanced{background-color: #cdffffff;}");
    //marketOverview->setStyleSheet("QFrame {color: #000000;} .QLabel { border: none; background: none; }");


    QWidget *marketOverviewBox = new QWidget(marketOverview);
    //marketOverviewBox->setStyleSheet(".QWidgetAdvanced{background-color: #cdffffff;}");

    //marketOverviewBox->lower();

    decoMarket = new QLabel(marketOverview);
    /*decoMarket->setPixmap( QPixmap(":/icons/deco_wallet"));//gui_header_logo"));
    decoMarket->setContentsMargins(0,0,0,0);
    decoMarket->adjustSize();*/

    //marketOverviewBox->setGraphicsEffect(dse);

    totalMarketCapTitle = new QLabel();
    //balanceTitle->setFont(boldFont);
    totalMarketCapTitle->setText("Total MarketCap:");
    totalMarketCapTitle->setCursor(Qt::IBeamCursor);
    totalMarketCapTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);

    totalVolume24hTitle = new QLabel();
    //unconfirmedTitle->setFont(boldFont);
    totalVolume24hTitle->setText("Today's Volume:");
    totalVolume24hTitle->setCursor(Qt::IBeamCursor);
    totalVolume24hTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);

    bearBullTitle = new QLabel();
    //immatureTitle->setFont(boldFont);
    bearBullTitle->setText("Bull/Bear:");
    bearBullTitle->setCursor(Qt::IBeamCursor);
    bearBullTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);


    totalMarketCap = new QLabel();
    //labelBalance->setFont(boldFont);
    totalMarketCap->setText("---");
    totalMarketCap->setCursor(Qt::IBeamCursor);
    totalMarketCap->setTextInteractionFlags(Qt::TextSelectableByMouse);

    totalVolume24h = new QLabel();
    //labelUnconfirmedBalance->setFont(boldFont);
    totalVolume24h->setText("---");
    totalVolume24h->setCursor(Qt::IBeamCursor);
    totalVolume24h->setTextInteractionFlags(Qt::TextSelectableByMouse);

    bearBull = new QLabel();
    //labelImmatureBalance->setFont(boldFont);

    QPixmap bullPix = QPixmap(100,15);
    bullPix.fill(0xFF00AF00);
    bearBull->setPixmap(bullPix);

    //bearBull->setCursor(Qt::IBeamCursor);
    //bearBull->setTextInteractionFlags(Qt::TextSelectableByMouse);


    marketTitle = new QLabel(marketOverview);
    marketTitle->setStyleSheet("QLabel { color: #000000; font: bold 18px \"Open Sans\";}");
    //marketTitle->setFont(font);
    marketTitle->setAlignment(Qt::AlignLeft);
    marketTitle->setText("MARKET");
    marketTitle->setFixedWidth(200);
    marketTitle->move(15,10);

    QFormLayout *formLayout4 = new QFormLayout();

    //formLayout4->addWidget(marketTitle);
    formLayout4->addRow(totalMarketCapTitle, totalMarketCap);
    formLayout4->addRow(totalVolume24hTitle, totalVolume24h);
    formLayout4->addRow(bearBullTitle, bearBull);

    formLayout4->setContentsMargins(15,42,0,10);
    formLayout4->setVerticalSpacing(8);
    formLayout4->setHorizontalSpacing(10);
    marketOverviewBox->setLayout(formLayout4);

    QHBoxLayout  *boxLayout4 = new QHBoxLayout();
    boxLayout4->addWidget(marketOverviewBox);
    boxLayout4->setContentsMargins(0,0,0,0);
    marketOverview->setLayout(boxLayout4);

    marketOverview->setMinimumWidth(240);
    marketOverview->setMaximumWidth(240);









    QHBoxLayout  *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(walletOverview);
    mainLayout->addWidget(recentOverview);
    mainLayout->addWidget(networkOverview);//,0,Qt::AlignRight);
    mainLayout->addWidget(marketOverview);//,0,Qt::AlignRight);
    mainLayout->setContentsMargins(195,20,0,20);
    mainLayout->setSpacing(20);
    mainLayout->setAlignment(Qt::AlignLeft);
    setLayout(mainLayout);

    qDebug() << height();

    for(int a=0;a<3;a++){
        for(int b=0;b<3;b++){
            mapper[a][b] = new QSignalMapper( this );
            mapper[a][b]->setMapping(recentLabel[a][b],a);
            connect( recentLabel[a][b], SIGNAL(clicked()), mapper[a][b], SLOT(map()) );
            connect(mapper[a][b], SIGNAL(mapped(int)), SLOT(handleTransactionClicked(int)));
        }
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateDisplayUnit()));
    timer->start(500);

    timer2 = new QTimer(this);
    connect(timer2, SIGNAL(timeout()), this, SLOT(updateCSS()));
    timer2->setSingleShot(true);
    timer2->start(100);

}

void GuiHeader::updateCSS(){

    QString css1 = "QLabel{font: normal 12px \"Open Sans\";} QToolTip{font: normal 12px \"Open Sans\"; border-style: outset;border-width: 1px;}";
    QString css2 = "QLabel{font: bold 12px \"Open Sans\";} QToolTip{font: normal 12px \"Open Sans\";  border-style: outset;border-width: 1px;}";

    balanceTitle->setStyleSheet(css1);
    unconfirmedTitle->setStyleSheet(css1);
    immatureTitle->setStyleSheet(css1);

    labelBalance->setStyleSheet(css2);
    labelImmatureBalance->setStyleSheet(css2);
    labelUnconfirmedBalance->setStyleSheet(css2);

    for(int a=0;a<3;a++){
        recentLabel[a][0]->setStyleSheet(css2);
        recentLabel[a][1]->setStyleSheet(css1);
        recentLabel[a][2]->setStyleSheet(css1);
    }

    blockCountTitle->setStyleSheet(css1);
    moneySupplyTitle->setStyleSheet(css1);
    hashRateTitle->setStyleSheet(css1);

    blockCount->setStyleSheet(css2);
    moneySupply->setStyleSheet(css2);
    hashRate->setStyleSheet(css2);

    totalMarketCapTitle->setStyleSheet(css1);
    totalVolume24hTitle->setStyleSheet(css1);
    bearBullTitle->setStyleSheet(css1);

    totalMarketCap->setStyleSheet(css2);
    totalVolume24h->setStyleSheet(css2);
    bearBull->setStyleSheet(css2);

    /*qDebug() << recentOverview->rect() << recentOverviewBox->rect();
    qDebug() << recentOverview->geometry() << recentOverviewBox->geometry();
    qDebug() << recentOverview->sizeHint() << recentOverviewBox->sizeHint();
    */

    decoWallet->move(walletOverview->width()-96-10, walletOverview->height()-96-10);
    decoRecent->move(recentOverview->width()-96-10, recentOverview->height()-96-10);
    decoNetwork->move(networkOverview->width()-96-10, networkOverview->height()-96-10);
    decoMarket->move(marketOverview->width()-96-10, marketOverview->height()-96-10);
}

void GuiHeader::resizeEvent(QResizeEvent *e)
{
    updateCSS();
}

void GuiHeader::resize(int width)
{
    this->setFixedWidth(1500);//width);
    //backGround->setFixedWidth(width);
    //bottomLine->setFixedWidth(width);
}


void GuiHeader::setBalance(qint64 balance, qint64 unconfirmedBalance, qint64 immatureBalance)
{
    currentBalance = balance;
    currentUnconfirmedBalance = unconfirmedBalance;
    currentImmatureBalance = immatureBalance;

    labelBalance->setText(BitcoinUnits::formatWithUnit(unit, balance));
    labelUnconfirmedBalance->setText(BitcoinUnits::formatWithUnit(unit, unconfirmedBalance));
    labelImmatureBalance->setText(BitcoinUnits::formatWithUnit(unit, immatureBalance));
}

void GuiHeader::setMarketValues(QString total_marketcap, QString total_volume_24h, int bear_bull_percentage)
{
    if(total_marketcap != "" && total_volume_24h != ""){
        totalMarketCap->setText(total_marketcap);
        totalVolume24h->setText(total_volume_24h);
    } else{
        QPixmap bullPix = QPixmap(100,15);
        bullPix.fill(0xFFAF0000);

        QPixmap bullPixRed = QPixmap((qreal)bear_bull_percentage/(qreal)100*100,15);
        bullPixRed.fill(0xFF00AF00);

        QPainter *p = new QPainter(&bullPix);

        QPen pen;

        p->drawPixmap(0,0,bullPixRed);

        pen.setStyle(Qt::DotLine);
        pen.setColor(0xFF000000);

        p->setOpacity(0.5);

        p->setPen(pen);

        p->drawLine(25,0,25,15);

        pen.setStyle(Qt::SolidLine);
        p->setPen(pen);
        p->drawLine(50,0,50,15);

        pen.setStyle(Qt::DotLine);
        p->setPen(pen);
        p->drawLine(75,0,75,15);

        p->end();

        bearBull->setPixmap(bullPix);
        bearBull->adjustSize();
    }
}

void GuiHeader::setClientModel(ClientModel *model)
{
    this->clientModel = model;

    if(model)
    {
        versionLabel->setText(model->formatFullVersion().split("-")[0]);
    }
}

void GuiHeader::setWalletModel(WalletModel *model)
{
    this->walletModel = model;

    if(model && model->getOptionsModel())
    {
        filter = new TransactionFilterProxy();
        filter->setSourceModel(model->getTransactionTableModel());
        filter->setLimit(3);//NUM_ITEMS);
        filter->setDynamicSortFilter(true);
        filter->setSortRole(Qt::EditRole);
        filter->sort(TransactionTableModel::Status, Qt::DescendingOrder);

        // Keep up to date with wallets
        setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance());
        connect(model, SIGNAL(balanceChanged(qint64, qint64, qint64)), this, SLOT(setBalance(qint64, qint64, qint64)));
        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));

    }

    // update the display unit, to not use the default ("BTC")
    updateDisplayUnit();
}

void GuiHeader::updateDisplayUnit()
{
    if(walletModel && walletModel->getOptionsModel())
    {
        unit = walletModel->getOptionsModel()->getDisplayUnit();
        if(currentBalance != -1)
            setBalance(currentBalance, currentUnconfirmedBalance, currentImmatureBalance);

        QDateTime now = QDateTime::currentDateTime();

        for(int i=0; i<filter->rowCount();i++){

            QString amountText = BitcoinUnits::formatWithUnit(unit, filter->data(filter->index(i, 0), TransactionTableModel::AmountRole).toLongLong());
            bool confirmed = filter->data(filter->index(i, 0), TransactionTableModel::ConfirmedRole).toBool();
            if(!confirmed)
            {
                amountText = QString("[") + amountText + QString("]");
            }

            recentLabel[i][0]->setText(amountText);
            if(filter->data(filter->index(i, 0), TransactionTableModel::AmountRole).toLongLong() < 0){
                recentLabel[i][1]->setText("Sent");
            } else{
                recentLabel[i][1]->setText("Received");
            }

            QString agoString = "";
            qint64 ago = filter->data(filter->index(i, 0), TransactionTableModel::DateRole).toDateTime().secsTo(now);
            if(ago <60){
                agoString = QString::number(ago)+" sec ago";
            }
            if(ago >=60 && ago <3600){
                agoString = QString::number(ago/60)+" min ago";
            }
            if(ago >=3600 && ago <86400){
                agoString = QString::number(ago/3600)+" h ago";
            }
            if(ago >=86400 && ago <31536000){
                agoString = QString::number(ago/86400)+" d ago";
            }
            if(ago >=31536000){
                agoString = QString::number(ago/86400)+" y ago";
            }

            recentLabel[i][2]->setText(agoString);

            recentLabel[i][0]->setToolTip(filter->data(filter->index(i, 0), TransactionTableModel::ToAddress).toString());
            recentLabel[i][1]->setToolTip(filter->data(filter->index(i, 0), TransactionTableModel::ToAddress).toString());
            recentLabel[i][2]->setToolTip(filter->data(filter->index(i, 0), TransactionTableModel::ToAddress).toString());
        }

    }
}

void GuiHeader::showOutOfSyncWarning(bool fShow)
{
    if(fShow){
        walletTitle->setText("WALLET <font size=14px color=#CC0000>(out of sync)</font>");
        networkTitle->setText("NETWORK <font size=14px color=#CC0000>(out of sync)</font>");
    } else{
        walletTitle->setText("WALLET");
        networkTitle->setText("NETWORK");
    }
}

static QString formatHashrate(int64 n)
{
    if (n == 0)
        return "0 H/s";

    int i = (int)floor(log(n)/log(1000));
    float v = n*pow(1000.0f, -i);

    QString prefix = "";
    if (i >= 1 && i < 9)
        prefix = " kMGTPEZY"[i];

    return QString("%1 %2H/s").arg(v, 0, 'f', 2).arg(prefix);
}

void GuiHeader::updateNetworkOverview()
{
    blockCount->setText(QString::number(nBestHeight));

    int totalMoney = (double)GetTotalSupply()/(double)COIN;
    moneySupply->setText(QString::number( totalMoney)+" ZLN");

    int64 NetworkHashrate = GetNetworkHashPS(120, -1).get_int64();
    hashRate->setText(formatHashrate(NetworkHashrate));

}

void GuiHeader::handleTransactionClicked(int indexa)
{
    if(filter->rowCount() > indexa){
        QModelIndex id = filter->index(indexa,3, QModelIndex());
        emit transactionClicked2(filter->mapToSource(id));
    }
}

