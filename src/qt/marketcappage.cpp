#include "marketcappage.h"
#include "ui_marketcappage.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTableWidget>
#include <QUrl>

#include <QLocale>

#include <qDebug>

class TabItem : public QTableWidgetItem {
public:

    enum sort_type{
        ALPHABET,
        INT,
        DOUBLE
    };

    TabItem(QString txt = "0", QVariant _property_ = 0, sort_type type = INT) :
        QTableWidgetItem(txt),
        type(type){
        this->setData(Qt::UserRole, _property_);
    }

    sort_type type;

    bool operator <(const QTableWidgetItem &other) const
    {
        //qDebug() << "sorting" << data(Qt::UserRole).toDouble() << other.data(Qt::UserRole).toDouble();
        if(type == INT){
            return data(Qt::UserRole).toInt() < other.data(Qt::UserRole).toInt();
        } else if(type == DOUBLE){
            return data(Qt::UserRole).toDouble() < other.data(Qt::UserRole).toDouble();
        } else if(type == ALPHABET){
            return text().toLower() < other.text().toLower();
        } else{
            return this;
        }
    }
};

MarketCapPage::MarketCapPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MarketCapPage)
{
    ui->setupUi(this);


    QLocale loc= QLocale::system();
    //printLocale("System OK", my_loc);
    //qDebug() << QLocale::languageToString(my_loc.language());
    //QLocale::countryToString(locale.country()) <<
    group_separator = loc.groupSeparator();
    decimal_point = loc.decimalPoint();

    sec_since_last_update = -10000000;
    limit = 20;
    currency = "USD";

    ui->comboBox->addItem(" coinmarketcap.com  ", 1);
    ui->comboBox->addItem(" Zillion Grid  ", 2);

    //Disabling Zillion Grid for now:
    QModelIndex index = ui->comboBox->model()->index(1, 0);
    // This is the effective 'disable' flag
    QVariant v(0);
    ui->comboBox->model()->setData(index, v, Qt::UserRole - 1);

    ui->comboBox->adjustSize();

    ui->comboBox_2->addItem(" USD  ", "USD");
    ui->comboBox_2->addItem(" EUR  ", "EUR");
    ui->comboBox_2->addItem(" CHF  ", "CHF");
    ui->comboBox_2->adjustSize();

    ui->comboBox_3->addItem(" 20  ", 20);
    ui->comboBox_3->addItem(" 50  ", 50);
    ui->comboBox_3->addItem(" 100  ", 100);
    ui->comboBox_3->addItem(" 200  ", 200);
    ui->comboBox_3->addItem(" ALL  ", 0);
    ui->comboBox_3->adjustSize();

    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCurrency(int)));
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLimit(int)));

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    managerGlobalData = new QNetworkAccessManager(this);
    connect(managerGlobalData, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinishedGlobalData(QNetworkReply*)));


    //model = new QStandardItemModel(100,5,this);

    ui->tableWidget->setColumnCount(6);

    updateCurrencySymbolAndHeader();

    //ui->tableWidget->setHorizontalHeaderLabels(headerLabels);

    //ui->tableWidget->setModel(model);

    ui->tableWidget->setAlternatingRowColors(true);
    //ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);//Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->tableView->horizontalHeader()->resizeSection(2,150);

    ui->tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);//MultiSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    //ui->tableWidget->setRowCount(500);
    //ui->tableWidget->setColumnCount(5);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onEnterFrame()));
    timer->start(30000);

    timerGlobalData = new QTimer(this);
    connect(timerGlobalData, SIGNAL(timeout()), this, SLOT(onEnterGlobalData()));
    timerGlobalData->start(30000);

    QTimer *ticker = new QTimer(this);
    connect(ticker, SIGNAL(timeout()), this, SLOT(onEnterTicker()));
    ticker->start(1000);

    getMarketValues();
    getGlobalData();
}

MarketCapPage::~MarketCapPage()
{
    delete ui;
}

void MarketCapPage::getMarketValues()
{
    qDebug() << "getMarketValues";
    manager->get(QNetworkRequest(QUrl("https://api.coinmarketcap.com/v1/ticker/?limit="+QString::number(limit)+"&convert="+currency)));
}

void MarketCapPage::getGlobalData()
{
    qDebug() << "getGlobalData";

    managerGlobalData->get(QNetworkRequest(QUrl("https://api.coinmarketcap.com/v1/global/?convert="+currency)));
}

void MarketCapPage::updateCurrencySymbolAndHeader()
{
    currency_symbol = "$";
    if(currency == "EUR"){
        currency_symbol = "€";
    }
    if(currency == "CHF"){
        currency_symbol = "Fr.";
    }

    QStringList headerLabels;
    headerLabels << "Name" << QString("Market Cap ( "+currency_symbol+" )") << QString("Price ( "+currency_symbol+" )") << QString("Volume (24h) ( "+currency_symbol+" )") << "Circulating Supply" << "Change (24h)";

    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);
}

void MarketCapPage::onEnterFrame()
{
    getMarketValues();
}

void MarketCapPage::onEnterTicker()
{
    if(sec_since_last_update < 0){
        ui->label->setText("Connecting...");
    } else{
        ui->label->setText("Last updated "+QString::number(sec_since_last_update)+" sec ago...");
    }
    sec_since_last_update++;
}

void MarketCapPage::onEnterGlobalData()
{
    qDebug() << "onEnterGlobalData";
    getGlobalData();
}

void MarketCapPage::updateCurrency(int new_currency)
{
    currency = ui->comboBox_2->currentData().toString();

    getMarketValues();
    getGlobalData();
}

void MarketCapPage::updateLimit(int new_limit)
{
    //remove currently selected row, because index will now certainly change.
    current_selected_row = -1;

    limit = ui->comboBox_3->currentData().toInt();
    getMarketValues();
    getGlobalData();
    //qDebug() << "new limit" << new_limit << ui->comboBox_3->currentData().toInt();
}

QString MarketCapPage::formatNumber(QString number)
{
    QString str = "";
    QStringList letter;

    int index = number.indexOf(decimal_point);
    QString number_left = number.left(index);
    QString number_right = "";
    if(index != -1){
        number_right = number.right(number.length()-index);
    }
    /*qDebug() << "index of" << index;
    qDebug() << "number" << number;
    qDebug() << "number left" << number_left;
    qDebug() << "number right" << number_right;
*/
    for(int i = number_left.length()-1; i>=0;i--){
        letter.append(QString(number[i]));
    }

    int until_next_separator = 3;

    for(int i = 0; i<letter.length();i++){
        str.prepend(letter[i]);
        until_next_separator--;
        if(until_next_separator == 0){
            if(i != letter.length()-1){
                str.prepend(group_separator);
            }

            until_next_separator = 3;
        }
    }

    str.append(number_right);

    return str;

}

void MarketCapPage::replyFinished(QNetworkReply *reply)
{
    /*QByteArray data = reply->readAll();

    qDebug() << "orka" << data.size();
    QString str_raw = data.toStdString().c_str();
    qDebug() << str_raw;

    */
    QString jsonString = QString::fromUtf8(reply->readAll()).simplified();



    QJsonDocument jsonResponse = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonArray jsonObj = jsonResponse.array();//.object();


    int sort_section = ui->tableWidget->horizontalHeader()->sortIndicatorSection();
    Qt::SortOrder sort_order = ui->tableWidget->horizontalHeader()->sortIndicatorOrder();
    qDebug() << "sort_section" << sort_section;
    qDebug() << "sort_order" << sort_order;


    current_selected_row = ui->tableWidget->currentRow();
    qDebug() << "current item" << ui->tableWidget->currentRow();//ui->tableWidget->currentItem()->row();

    ui->tableWidget->setSortingEnabled(true);

    ui->tableWidget->sortByColumn(1,Qt::DescendingOrder);
    ui->tableWidget->clearSelection();
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(jsonObj.count());
    QFont f;
    f.setFamily("Open Sans");
    f.setPixelSize(13);
    QFont f2;
    f2.setFamily("Open Sans");
    f2.setBold(true);
    f2.setPixelSize(13);

    ui->tableWidget->setFont(f);

    QTableWidgetItem *item;
    TabItem *titem;
    QString str_percent_change_24h;

    double _market_cap_d;
    double _24h_volume_d;
    double _available_supply_d;
    double _percent_change_24h_d;

    int bull_count = 0;

    QList<QString> stringListForTicker;

    QString stringItem;

    for(int i = 0;i<jsonObj.count();i++){
        //qDebug() << jsonObj[i].toObject()["id"].toString();
        //qDebug() << jsonObj[i].toObject()["symbol"].toString();
        //qDebug() << jsonObj[i].toObject()["total_supply"].toString().toDouble();

        stringItem = "";

        stringItem.append(jsonObj[i].toObject()["symbol"].toString());

        item = new QTableWidgetItem(jsonObj[i].toObject()["name"].toString());
        item->setData(Qt::UserRole, i);
        item->setIcon(QIcon(QPixmap(":/coins/empty")));


        if(jsonObj[i].toObject()["id"].toString() == "bitcoin"){
            item->setIcon(QIcon(QPixmap(":/coins/bitcoin")));
        }
        if(jsonObj[i].toObject()["id"].toString() == "ethereum"){
            item->setIcon(QIcon(QPixmap(":/coins/ethereum")));
        }
        if(jsonObj[i].toObject()["id"].toString() == "dash"){
            item->setIcon(QIcon(QPixmap(":/coins/dash")));
        }
        if(jsonObj[i].toObject()["id"].toString() == "litecoin"){
            item->setIcon(QIcon(QPixmap(":/coins/litecoin")));
        }
        if(jsonObj[i].toObject()["id"].toString() == "monero"){
            item->setIcon(QIcon(QPixmap(":/coins/monero")));
        }
        if(jsonObj[i].toObject()["id"].toString() == "monacoin"){
            item->setIcon(QIcon(QPixmap(":/coins/monacoin")));
        }
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        ui->tableWidget->setItem(i,0,item);

        //MarketCap

        _market_cap_d = jsonObj[i].toObject()["market_cap_"+currency.toLower()].toString().toDouble();

        /*item = new QTableWidgetItem(formatNumber(QString::number(_market_cap_d,'f',0)));
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidget->setItem(i,1,item);*/

        titem = new TabItem(
                    formatNumber(QString::number(_market_cap_d,'f',0)),
                QVariant(_market_cap_d),
                TabItem::DOUBLE);
        titem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidget->setItem(i,1,titem);

        //Price

        /*item = new QTableWidgetItem(formatNumber(jsonObj[i].toObject()["price_"+currency.toLower()].toString()));
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        item->setFont(f2);
        ui->tableWidget->setItem(i,2,item);
        */


        stringItem.append("---"+formatNumber(jsonObj[i].toObject()["price_"+currency.toLower()].toString()));

        titem = new TabItem(
                    formatNumber(jsonObj[i].toObject()["price_"+currency.toLower()].toString()),
                QVariant(jsonObj[i].toObject()["price_"+currency.toLower()].toString().toDouble()),
                TabItem::DOUBLE);
        titem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        titem->setFont(f2);
        ui->tableWidget->setItem(i,2,titem);
        //qDebug() << jsonObj[i].toObject()["price_"+currency.toLower()].toString().toDouble();
        /*ui->tableWidget->setItem(i,2,new TabItem(
                                     formatNumber(jsonObj[i].toObject()["price_"+currency.toLower()].toString()),
                                 QVariant(jsonObj[i].toObject()["price_"+currency.toLower()].toString().toDouble()),
                                 TabItem::DOUBLE));*/

        //ui->tableWidget->setItem(i,0,new TabItem("item"+QString::number(i),QVariant(i),TabItem::INT));
        //ui->tableWidget->setItem(i,1,new TabItem(GetRandomString(),-1,TabItem::ALPHABET));
        //ui->tableWidget->setItem(i,2,new TabItem(QString::number(r),QVariant(r),TabItem::DOUBLE));

        //24h volume

        _24h_volume_d = jsonObj[i].toObject()["24h_volume_"+currency.toLower()].toString().toDouble();

        /*item = new QTableWidgetItem(formatNumber(QString::number(_24h_volume_d,'f',0)));
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidget->setItem(i,3,item);*/

        titem = new TabItem(
                    formatNumber(QString::number(_24h_volume_d,'f',0)),
                QVariant(_24h_volume_d),
                TabItem::DOUBLE);
        titem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidget->setItem(i,3,titem);

        //available_supply

        _available_supply_d =jsonObj[i].toObject()["available_supply"].toString().toDouble();

        /*item = new QTableWidgetItem(formatNumber(QString::number(_available_supply_d,'f',0))+" "+jsonObj[i].toObject()["symbol"].toString());
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidget->setItem(i,4,item);*/

        titem = new TabItem(
                    formatNumber(QString::number(_available_supply_d,'f',0))+" "+jsonObj[i].toObject()["symbol"].toString(),
                QVariant(_available_supply_d),
                TabItem::DOUBLE);
        titem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidget->setItem(i,4,titem);

        //Percentage change 24h

        str_percent_change_24h = jsonObj[i].toObject()["percent_change_24h"].toString()+" %";//+ QString::number(i);
        _percent_change_24h_d = jsonObj[i].toObject()["percent_change_24h"].toString().toDouble();

        /*item = new QTableWidgetItem(str_percent_change_24h);
        //item->setData(Qt::EditRole, i);
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        if(str_percent_change_24h.count("-",Qt::CaseInsensitive) >0){
            item->setTextColor(0xCF0000);
        } else{
            bull_count++;
            item->setTextColor(0x008F00);
        }
        ui->tableWidget->setItem(i,5,item);*/

        titem = new TabItem(
                    str_percent_change_24h,
                QVariant(_percent_change_24h_d),
                TabItem::DOUBLE);
        titem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        if(str_percent_change_24h.count("-",Qt::CaseInsensitive) >0){
            titem->setTextColor(0xCF0000);
            stringItem.append("---red");
        } else{
            bull_count++;
            titem->setTextColor(0x008F00);
            stringItem.append("---green");
        }
        ui->tableWidget->setItem(i,5,titem);



        stringItem.append("---"+currency_symbol);

        stringListForTicker.append(stringItem);
    }

    sec_since_last_update = 0;



    emit sendTickerData(stringListForTicker);

    ui->tableWidget->sortByColumn(sort_section,sort_order);

    if(current_selected_row != -1){
        ui->tableWidget->setCurrentIndex(ui->tableWidget->model()->index(current_selected_row,0));
        //ui->tableWidget->setCurrentCell(ui->tableWidget->currentRow(),0);//ui->tableWidget->currentColumn());
    }

    emit sendMarketValues("","",(qreal)bull_count/(qreal)jsonObj.count()*100);

    /*VanityGenNThreads = jsonObj["state"].toObject()["threads"].toString().toInt();
    ui->horizontalSlider->setSliderPosition(VanityGenNThreads);
    ui->horizontalSlider->setValue(VanityGenNThreads);

    ui->checkBoxMatchCase->setChecked((jsonObj["state"].toObject()["matchCase"].toString() == "true") ? true : false);
    VanityGenMatchCase = (ui->checkBoxMatchCase->checkState() == 2) ? 1 : 0;

    ui->checkBoxShowPrivKeys->setChecked((jsonObj["state"].toObject()["showPrivKeys"].toString() == "true") ? true : false);

    ui->checkBoxAutoImport->setChecked((jsonObj["state"].toObject()["autoImport"].toString() == "true") ? true : false);

    QJsonArray workList = jsonObj["workList"].toArray();

    for(int i = workList.count()-1; i>=0; i--){
        VanityGenWorkList.prepend(VanGenStruct());
        VanityGenWorkList[0].pattern = workList[i].toObject()["pattern"].toString();
        VanityGenWorkList[0].privkey = workList[i].toObject()["privkey"].toString();
        VanityGenWorkList[0].pubkey = workList[i].toObject()["pubkey"].toString();
        VanityGenWorkList[0].difficulty = "";
        VanityGenWorkList[0].pubkey != "" ? VanityGenWorkList[0].state = 2 : VanityGenWorkList[0].state = 0;
        VanityGenWorkList[0].notification = 0;
    }

    rebuildTableView();

    if(jsonObj["state"].toObject()["running"].toString() == "true" && VanityGenNThreads > 0){
        startThread();
    }*/

    /*
    qDebug() << str_raw.count("<body>",Qt::CaseInsensitive);

    uint body_pos = str_raw.lastIndexOf("<body>",-1,Qt::CaseInsensitive);
    qDebug() << body_pos;
    str_raw = str_raw.mid(body_pos,-1);
    str_raw.chop(7); //remove </html>

    str_raw.replace("&nbsp;","", Qt::CaseInsensitive);
    str_raw.replace("&laquo;","", Qt::CaseInsensitive);
    str_raw.replace("&raquo;","", Qt::CaseInsensitive);
    str_raw.replace("&copy;","", Qt::CaseInsensitive);


    ui->plainTextEdit->setPlainText(str_raw);//QString(data.toStdString().c_str()));
    //ui->plainTextEdit->appendHtml(str_raw);//QString(data.toStdString().c_str()));


    QXmlStreamReader xml;
    xml.addData(str_raw);
    while (!xml.atEnd()) {
        if(xml.readNext() != 1){
            qDebug() << xml.tokenString() << xml.name();// << xml.readElementText();
            qDebug() << "            " << xml.attributes().toList().count();
            //if(xml.name() == "div"){
            //qDebug() << "IS DIV----";
            //}
        }

    }
    if (xml.hasError()) {
        qDebug() << "ERROR" << xml.errorString();
    }*/

}

void MarketCapPage::replyFinishedGlobalData(QNetworkReply *reply)
{


    QString jsonString = QString::fromUtf8(reply->readAll()).simplified();

    //qDebug() << jsonString;

    QJsonDocument jsonResponse = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonObject jsonObj = jsonResponse.object();

    double total_market_cap = 0;
    double total_24h_volume = 0;

    total_market_cap = jsonObj["total_market_cap_"+currency.toLower()].toDouble();
    total_24h_volume = jsonObj["total_24h_volume_"+currency.toLower()].toDouble();

    updateCurrencySymbolAndHeader();

    qDebug() << "currency_sybol" << currency_symbol;

    emit sendMarketValues(currency_symbol+formatNumber(QString::number(total_market_cap,'f',0)), currency_symbol+formatNumber(QString::number(total_24h_volume,'f',0)),-1);
    //qDebug() << ;
    //qDebug() << jsonObj["total_market_cap_"+currency].toDouble();


}


