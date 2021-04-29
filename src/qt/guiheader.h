#ifndef GUIHEADER_H
#define GUIHEADER_H

#include <QWidget>
#include <QLabel>
#include <QFormLayout>
#include <QSignalMapper>
#include <QSystemTrayIcon>
#include <QPushButton>

class ClientModel;
class WalletModel;
class TransactionFilterProxy;

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

class ClickableLabel;

extern int screenId;


class QFrameAdvanced : public QFrame
{

    Q_OBJECT

public:
    explicit QFrameAdvanced(QWidget * parent = 0 );

    void resizeEvent(QResizeEvent *event);
    //void paintEvent(QPaintEvent *event);
    int shadow_width;
    qreal shadow_opacity;

    QPixmap pix_shadow_corner[4];
    QPixmap pix_shadow_edge[4];
    QLabel *shadow_corner[4];
    QLabel *shadow_edge[4];
signals:
    //void clicked();

protected:
    //void mousePressEvent ( QMouseEvent * event ) ;
};


/*class QWidgetAdvanced : public QWidget
{

    Q_OBJECT

public:
    explicit QWidgetAdvanced(QWidget * parent = 0 );

    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
signals:
    //void clicked();

protected:
    //void mousePressEvent ( QMouseEvent * event ) ;
};*/



class ClickableLabel : public QLabel
{

    Q_OBJECT

public:
    explicit ClickableLabel(QWidget * parent = 0 );
    ~ClickableLabel();

signals:
    void clicked();

protected:
    void mousePressEvent ( QMouseEvent * event ) ;
};


class GuiHeader : public QWidget
{
    Q_OBJECT

public:

    explicit GuiHeader(QWidget *parent = 0);
    ~GuiHeader();

    void resizeEvent(QResizeEvent *e);

    void setClientModel(ClientModel *clientModel);
    void setWalletModel(WalletModel *walletModel);



    //QWidget *backGround;
    //QWidget *bottomLine;

    QFrame *walletOverview;
    QLabel *walletTitle;

    QFrame *recentOverview;
    QWidget *recentOverviewBox;

    QFrame *networkOverview;
    QLabel *networkTitle;

    QFrame *marketOverview;
    QLabel *marketTitle;

    QFrame *gridOverview;
    QLabel *gridTitle;

    QWidget *recentOverviewCol[3];
    QVBoxLayout *recentColLayout[3];
    ClickableLabel *recentLabel[3][3];

    QTimer *timer;
    QTimer *timer2;

    void resize(int);
    void showOutOfSyncWarning(bool fShow);

    void updateNetworkOverview();

    TransactionFilterProxy *filter;
    QSignalMapper *mapper[3][3];

public slots:
    void setBalance(qint64 balance, qint64 unconfirmedBalance, qint64 immatureBalance);
    void setMarketValues(QString total_marketcap, QString total_volume_24h, int bear_bull_percentage);
    void handleTransactionClicked(int indexa);

    void updateCSS();

    void sendSimpleGridData(QStringList stringList);

signals:
    void transactionClicked2(const QModelIndex &index);
    void transactionClicked(int index);

private:
    ClickableLabel *spreadCoinLogo;

    ClientModel *clientModel;
    WalletModel *walletModel;
    qint64 currentBalance;
    qint64 currentUnconfirmedBalance;
    qint64 currentImmatureBalance;

    QLabel *versionLabel;

    QLabel *decoWallet;
    QLabel *decoRecent;
    QLabel *decoNetwork;
    QLabel *decoMarket;
    QLabel *decoGrid;

    //QWidgetAdvanced *walletOverviewBox;

    QLabel *balanceTitle;
    QLabel *unconfirmedTitle;
    QLabel *immatureTitle;

    QLabel *labelBalance;
    QLabel *labelUnconfirmedBalance;
    QLabel *labelImmatureBalance;

    QLabel *blockCountTitle;
    QLabel *moneySupplyTitle;
    QLabel *hashRateTitle;

    QLabel *blockCount;
    QLabel *moneySupply;
    QLabel *hashRate;

    QLabel *totalMarketCapTitle;
    QLabel *totalVolume24hTitle;
    QLabel *bearBullTitle;

    QLabel *totalMarketCap;
    QLabel *totalVolume24h;
    QLabel *bearBull;

    QLabel *classTypeTitle;
    QLabel *serversCountTitle;
    QLabel *ramTotalTitle;
    QLabel *totalStorageTitle;

    QLabel *gridLabelA0;
    QLabel *gridLabelB0;
    QLabel *gridLabelC0;

    QLabel *gridLabelA1;
    QLabel *gridLabelB1;
    QLabel *gridLabelC1;

    QLabel *gridLabelA2;
    QLabel *gridLabelB2;
    QLabel *gridLabelC2;

    QLabel *gridLabelA3;
    QLabel *gridLabelB3;
    QLabel *gridLabelC3;


    int unit;

private slots:
    void updateDisplayUnit();

};

#endif // GUIHEADER_H
