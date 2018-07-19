#include "miningpage.h"
#include "ui_miningpage.h"
#include "main.h"
#include "util.h"
#include "bitcoinrpc.h"
#include "walletmodel.h"

#include <boost/thread.hpp>
#include <stdio.h>

#include <QDebug>

extern json_spirit::Value GetNetworkHashPS(int lookup, int height);

MiningPage::MiningPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MiningPage),
    hasMiningprivkey(false)
{
    ui->setupUi(this);

    int nThreads = boost::thread::hardware_concurrency();

    int nUseThreads = GetArg("-genproclimit", -1);
    if (nUseThreads < 0)
        nUseThreads = nThreads;

    std::string PrivAddress = GetArg("-miningprivkey", "");
    if (!PrivAddress.empty())
    {
        CBitcoinSecret Secret;
        Secret.SetString(PrivAddress);
        if (Secret.IsValid())
        {
            CBitcoinAddress Address;
            Address.Set(Secret.GetKey().GetPubKey().GetID());
            ui->labelAddress->setText(QString("All mined coins will go to to %1").arg(Address.ToString().c_str()));
            hasMiningprivkey = true;
        }
    }

    ui->sliderCores->setMinimum(0);
    ui->sliderCores->setMaximum(nThreads);
    ui->sliderCores->setValue(nUseThreads);
    ui->labelNCores->setText(QString("%1").arg(nUseThreads));

    connect(ui->sliderCores, SIGNAL(valueChanged(int)), this, SLOT(changeNumberOfCores(int)));
    connect(ui->pushSwitchMining, SIGNAL(clicked()), this, SLOT(switchMining()));

    updateUI();
    startTimer(1000);

    int algo_version = 1;

    int height = nBestHeight+1;
    if(height>=360){
        algo_version = 2;
    }

    BGLINE = new QLabel(this);
    BGLINE->setPixmap( QPixmap(":/gui/hash_order_bg"));
    BGLINE->setContentsMargins(0,0,0,0);
    BGLINE->move(120,69);
    BGLINE->adjustSize();


    ButtonSPREAD = new QPushButton(this);

    if(algo_version == 2){
        ButtonSPREAD->setText("SPREAD");
        ButtonSPREAD->setGeometry(20,50,100,40);
    } else{
        ButtonSPREAD->setText("SPREADX11");
        ButtonSPREAD->setGeometry(20,50,130,40);
    }
    ButtonSPREAD->setStyleSheet("color:#FFFFFFFF;"
                                "background-color: #ff3F3F3F;"
                                "border-style: solid;"
                                "border-width: 1px;"
                                "border-color: #ff000000;"
                                "font: bold 16px \"Open Sans Semibold\";");



    ButtonBLAKE = new QPushButton(this);
    ButtonBLAKE->setText("BLAKE");
    ButtonBLAKE->setStyleSheet("color:#FF000000;"
                               "background-color: #ffDDDDDD;"
                               "border-style: solid;"
                               "border-width: 1px;"
                               "border-color: #ff000000;"
                               "font: bold 16px \"Open Sans Semibold\";");

    ButtonJH = new QPushButton(this);
    ButtonJH->setText("JH");
    ButtonJH->setStyleSheet("color:#FF000000;"
                            "background-color: #ffFFFFFF;"
                            "border-style: solid;"
                            "border-width: 1px;"
                            "border-color: #ff000000;"
                            "font: bold 16px \"Open Sans Semibold\";");

    ButtonKECCAK = new QPushButton(this);
    ButtonKECCAK->setText("KECCAK");
    ButtonKECCAK->setStyleSheet("color:#FF000000;"
                                "background-color: #ffFFFFFF;"
                                "border-style: solid;"
                                "border-width: 1px;"
                                "border-color: #ff000000;"
                                "font: bold 16px \"Open Sans Semibold\";");

    ButtonSHAVITE = new QPushButton(this);
    ButtonSHAVITE->setText("SHAVITE");
    ButtonSHAVITE->setStyleSheet("color:#FF000000;"
                                 "background-color: #ffFFFFFF;"
                                 "border-style: solid;"
                                 "border-width: 1px;"
                                 "border-color: #ff000000;"
                                 "font: bold 16px \"Open Sans Semibold\";");

    ButtonECHO = new QPushButton(this);
    ButtonECHO->setText("ECHO");
    ButtonECHO->setStyleSheet("color:#FF000000;"
                              "background-color: #ffDDDDDD;"
                              "border-style: solid;"
                              "border-width: 1px;"
                              "border-color: #ff000000;"
                              "font: bold 16px \"Open Sans Semibold\";");



    ButtonBLAKE->setGeometry(20+100+20,50,90,40);

    ButtonJH->setGeometry(20+100+20+90+20,50,50,40);
    ButtonKECCAK->setGeometry(20+100+20+90+20+50+20,50,100,40);
    ButtonSHAVITE->setGeometry(20+100+20+90+20+50+20+100+20,50,110,40);

    ButtonECHO->setGeometry(20+100+20+90+20+50+20+100+20+110+20,50,70,40);


    if(algo_version == 1){
        BGLINE->setVisible(false);

        ButtonBLAKE->setVisible(false);
        ButtonJH->setVisible(false);
        ButtonKECCAK->setVisible(false);
        ButtonSHAVITE->setVisible(false);
        ButtonECHO->setVisible(false);
    } else{
        BGLINE->setVisible(true);
        ButtonBLAKE->setVisible(true);
        ButtonJH->setVisible(true);
        ButtonKECCAK->setVisible(true);
        ButtonSHAVITE->setVisible(true);
        ButtonECHO->setVisible(true);
    }

}

MiningPage::~MiningPage()
{
    delete ui;
}

void MiningPage::setModel(WalletModel *model)
{
    this->model = model;
}

void MiningPage::updateUI()
{
    int nThreads = boost::thread::hardware_concurrency();

    int nUseThreads = GetArg("-genproclimit", -1);
    if (nUseThreads < 0)
        nUseThreads = nThreads;


    ui->labelNCores->setText(QString("%1").arg(nUseThreads));
    ui->pushSwitchMining->setText(GetBoolArg("-gen", false)? tr("Stop mining") : tr("Start mining"));
    ui->pushSwitchMining->setIcon(GetBoolArg("-gen", false)? QIcon(QPixmap(":/icons/player_stop_button")) : QIcon(QPixmap(":/icons/player_play_button")));
}

void MiningPage::restartMining(bool fGenerate)
{
    int nThreads = ui->sliderCores->value();

    mapArgs["-genproclimit"] = QString("%1").arg(nThreads).toUtf8().data();

    // unlock wallet before mining
    if (fGenerate && !hasMiningprivkey && !unlockContext.get())
    {
        this->unlockContext.reset(new WalletModel::UnlockContext(model->requestUnlock()));
        if (!unlockContext->isValid())
        {
            unlockContext.reset(NULL);
            return;
        }
    }

    json_spirit::Array Args;
    Args.push_back(fGenerate);
    Args.push_back(nThreads);
    setgenerate(Args, false);

    // lock wallet after mining
    if (!fGenerate && !hasMiningprivkey)
        unlockContext.reset(NULL);

    updateUI();
}

void MiningPage::changeNumberOfCores(int i)
{
    restartMining(GetBoolArg("-gen"));
}

void MiningPage::switchMining()
{
    static int switcher = -1;
    if(switcher == -1){
        ui->pushSwitchMining->setIcon(QIcon(QPixmap(":/icons/player_stop_button")));
    } else{
        ui->pushSwitchMining->setIcon(QIcon(QPixmap(":/icons/player_play_button")));
    }
    switcher *= -1;
    restartMining(!GetBoolArg("-gen"));
}

static QString formatTimeInterval(CBigNum t)
{
    enum  EUnit { YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, NUM_UNITS };

    const int SecondsPerUnit[NUM_UNITS] =
    {
        31556952, // average number of seconds in gregorian year
        31556952/12, // average number of seconds in gregorian month
        24*60*60, // number of seconds in a day
        60*60, // number of seconds in an hour
        60, // number of seconds in a minute
        1
    };

    const char* UnitNames[NUM_UNITS] =
    {
        "year",
        "month",
        "day",
        "hour",
        "minute",
        "second"
    };

    if (t > 0xFFFFFFFF)
    {
        t /= SecondsPerUnit[YEAR];
        return QString("%1 years").arg(t.ToString(10).c_str());
    }
    else
    {
        unsigned int t32 = t.getuint();

        int Values[NUM_UNITS];
        for (int i = 0; i < NUM_UNITS; i++)
        {
            Values[i] = t32/SecondsPerUnit[i];
            t32 %= SecondsPerUnit[i];
        }

        int FirstNonZero = 0;
        while (FirstNonZero < NUM_UNITS && Values[FirstNonZero] == 0)
            FirstNonZero++;

        QString TimeStr;
        for (int i = FirstNonZero; i < std::min(FirstNonZero + 3, (int)NUM_UNITS); i++)
        {
            int Value = Values[i];
            TimeStr += QString("%1 %2%3 ").arg(Value).arg(UnitNames[i]).arg((Value == 1)? "" : "s"); // FIXME: this is English specific
        }
        return TimeStr;
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

void MiningPage::timerEvent(QTimerEvent *)
{
    //qDebug() << "MAX_BLOCK_SIZE_FOR_HEIGHT" << MAX_BLOCK_SIZE_FOR_HEIGHT(nBestHeight+1);

    int64 NetworkHashrate = GetNetworkHashPS(120, -1).get_int64();
    int64 Hashrate = GetBoolArg("-gen")? gethashespersec(json_spirit::Array(), false).get_int64() : 0;

    QString NextBlockTime;
    if (Hashrate == 0)
        NextBlockTime = QChar(L'∞');
    else
    {
        CBigNum Target;
        Target.SetCompact(pindexBest->nBits);
        CBigNum ExpectedTime = (CBigNum(1) << 256)/(Target*Hashrate);
        NextBlockTime = formatTimeInterval(ExpectedTime);
    }

    ui->labelNethashrate->setText(formatHashrate(NetworkHashrate));
    ui->labelYourHashrate->setText(formatHashrate(Hashrate));
    ui->labelNextBlock->setText(NextBlockTime);

    int algo_version = 1;

    int height = nBestHeight+1;
    if(height>=360){
        algo_version = 2;
    }

    if(algo_version == 2){
        ui->label_blockheight->setText("Current Hash-Algo-Order (@ new blockheight: "+QString::number(height)+")");
        ButtonSPREAD->setText("SPREAD");
        ButtonSPREAD->setGeometry(20,50,100,40);
        BGLINE->setVisible(true);
        ButtonBLAKE->setVisible(true);
        ButtonJH->setVisible(true);
        ButtonKECCAK->setVisible(true);
        ButtonSHAVITE->setVisible(true);
        ButtonECHO->setVisible(true);
    } else{
        ui->label_blockheight->setText("Current Hash-Algo (@ new blockheight: "+QString::number(height)+")");
        ButtonSPREAD->setText("SPREADX11");
        ButtonSPREAD->setGeometry(20,50,130,40);
        BGLINE->setVisible(false);
        ButtonBLAKE->setVisible(false);
        ButtonJH->setVisible(false);
        ButtonKECCAK->setVisible(false);
        ButtonSHAVITE->setVisible(false);
        ButtonECHO->setVisible(false);
    }


    int jh_x, keccak_x, shavite_x;

    if (height%3 == 0)
    {
        jh_x = 250;
        keccak_x =320;
        shavite_x = 440;
    }
    if (height%3 == 1)
    {
        keccak_x =250;
        shavite_x = 370;
        jh_x = 500;
    }
    if (height%3 == 2)
    {
        shavite_x = 250;
        jh_x = 380;
        keccak_x =450;
    }

    animation = new QPropertyAnimation(ButtonJH, "pos");
    animation->setDuration(500);
    animation->setStartValue(QPoint(ButtonJH->x(), 50));
    animation->setEndValue(QPoint(jh_x, 50));
    animation->setEasingCurve(QEasingCurve::InCubic);
    animation->start();

    animation2 = new QPropertyAnimation(ButtonKECCAK, "pos");
    animation2->setDuration(500);
    animation2->setStartValue(QPoint(ButtonKECCAK->x(), 50));
    animation2->setEndValue(QPoint(keccak_x, 50));
    animation2->setEasingCurve(QEasingCurve::InCubic);
    animation2->start();

    animation3 = new QPropertyAnimation(ButtonSHAVITE, "pos");
    animation3->setDuration(500);
    animation3->setStartValue(QPoint(ButtonSHAVITE->x(), 50));
    animation3->setEndValue(QPoint(shavite_x, 50));
    animation3->setEasingCurve(QEasingCurve::InCubic);
    animation3->start();
}
