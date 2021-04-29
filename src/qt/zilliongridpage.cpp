#include "zilliongridpage.h"
#include "ui_zilliongridpage.h"

#include <QDebug>


ZillionGridPage::ZillionGridPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ZillionGridPage)
{
    ui->setupUi(this);

    QLocale loc= QLocale::system();
    group_separator = loc.groupSeparator();
    decimal_point = loc.decimalPoint();

    ui->tableWidget->setColumnCount(4);


    //ui->tableWidget->setHorizontalHeaderLabels(headerLabels);

    //ui->tableWidget->setModel(model);

    ui->tableWidget->setAlternatingRowColors(true);

    QStringList headerLabels;
    headerLabels << "Class Type" << "A" << "B" << "C";

    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);
    //ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);//Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->tableView->horizontalHeader()->resizeSection(2,150);

    ui->tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);//MultiSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->verticalHeader()->hide();

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(7);

    QFont bold;
    bold.setBold(true);

    QStringList titles;
    titles << "Servers Count" << "Online Servers" << "Offline Servers" << "RAM Total" << "Total Storage" << "Available Storage" << "Utilized Storage";

    QTableWidgetItem *item;

    for(int i=0; i<titles.count();i++){
        item = ui->tableWidget->item(i, 0);
        if(!item) {
            item = new QTableWidgetItem;
            ui->tableWidget->setItem(i, 0, item);
        }
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        item->setText(titles[i]);
    }

    for(int i=0; i<7;i++){
        item = ui->tableWidget->item(i, 1);
        if(!item) {
            item = new QTableWidgetItem;
            ui->tableWidget->setItem(i, 1, item);
        }
        item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        item->setText("0");

        item = ui->tableWidget->item(i, 2);
        if(!item) {
            item = new QTableWidgetItem;
            ui->tableWidget->setItem(i, 2, item);
        }
        item->setFont(bold);
        if(i == 0){
            item->setTextColor("#0000FF");
        }
        if(i == 1){
            item->setTextColor("#007F00");
        }
        if(i == 2){
            item->setTextColor("#FF0000");
        }
        item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        item->setText("0");

        item = ui->tableWidget->item(i, 3);
        if(!item) {
            item = new QTableWidgetItem;
            ui->tableWidget->setItem(i, 3, item);
        }
        item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        item->setText("0");

    }


    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    managerServerGridDetails = new QNetworkAccessManager(this);
    connect(managerServerGridDetails, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinishedServerGridDetails(QNetworkReply*)));

    //getServerStatus();

    timerServerGridDetails = new QTimer(this);
    connect(timerServerGridDetails, SIGNAL(timeout()), this, SLOT(onEnterServerGridDetails()));
    timerServerGridDetails->start(30000);

    getServerGridDetails();
}

ZillionGridPage::~ZillionGridPage()
{
    delete ui;
}

void ZillionGridPage::getServerStatus()
{
    //qDebug() << "getServerStatus";
    //manager->get(QNetworkRequest(QUrl("http://106.51.3.189:8082/ServerStatusnew")));

}

void ZillionGridPage::getServerGridDetails()
{
    qDebug() << "getServerGridDetails---------------------------------------------";
    managerServerGridDetails->get(QNetworkRequest(QUrl("http://106.51.3.189:8082/Get_ServerGridDetails_Hub")));
}

QString ZillionGridPage::formatNumber(QString number)
{
    QString str = "";
    QStringList letter;

    int index = number.indexOf(decimal_point);
    QString number_left = number.left(index);
    QString number_right = "";
    if(index != -1){
        number_right = number.right(number.length()-index);
    }

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

void ZillionGridPage::replyFinished(QNetworkReply *reply)
{

}

void ZillionGridPage::replyFinishedServerGridDetails(QNetworkReply *reply)
{

    qDebug() << "getServerStatus RESULT";

    QString jsonString = QString::fromUtf8(reply->readAll()).simplified();

    //qDebug() << jsonString;

    QJsonDocument jsonResponse = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonObject jsonObj = jsonResponse.object();

    /*double total_market_cap = 0;
    double total_24h_volume = 0;

    double _market_cap_d;
    double _total_volume_d;*/

    qDebug() << "jsonObj.count()" << jsonObj["StatusMessage"].toString();

    qDebug() << "jsonObj.count()" << jsonObj["mapvals"].toArray()[0].toObject()["Continent"];


    qDebug() << "Message" << jsonObj["Message"].toArray().count();

    int server_total = 0;
    int server_online = 0;
    int ram_total = 0;
    int HD_total = 0;
    int HD_available = 0;
    for(int i = 0;i<jsonObj["Message"].toArray().count();i++){
        QJsonObject temp = jsonObj["Message"].toArray()[i].toObject();
        qDebug() << i << temp.keys();
        QStringList keys = temp.keys();
        //qDebug() << keys;
        qDebug() << keys[0];

        qDebug() << temp[keys[0]].toArray().count();
        for(int i2 = 0;i2<temp[keys[0]].toArray().count();i2++){
            QJsonObject temp2 = temp[keys[0]].toArray()[i2].toObject();
            QStringList keys2 = temp2.keys();
            qDebug() << "        " << keys2;

            for(int i3 = 0; i3<temp2[keys2[0]].toArray().count();i3++){

                QJsonObject temp3 = temp2[keys2[0]].toArray()[i3].toObject();
                //qDebug() << "              " <<  temp3["Online"].toInt();
                //qDebug() << "              " <<  temp3["RAM"].toString().replace(" MB","");
                ram_total += temp3["RAM"].toString().replace(" MB","").toDouble();
                QUrl url(temp3["HD"].toString());
                QString check_HD_str = url.fromPercentEncoding(temp3["HD"].toString().toUtf8());

                qDebug() << check_HD_str;

                int index_of_i = 0;
                int index_of_tail = 0;

                for(;;){
                    index_of_i = check_HD_str.indexOf("Total+Size:", index_of_i);
                    qDebug() << " occurence" << index_of_i;
                    if(index_of_i == -1){
                        break;
                    } else{
                        //check for "GB" tail
                        index_of_tail = check_HD_str.indexOf("GB", index_of_i);
                        if(index_of_tail != -1){
                            qDebug() << "HD found of total size:" << check_HD_str.mid(index_of_i,index_of_tail-index_of_i).replace("Total+Size:","");
                            HD_total += check_HD_str.mid(index_of_i,index_of_tail-index_of_i).replace("Total+Size:","").toInt();
                            qDebug() << " ---> " << check_HD_str.mid(index_of_i,index_of_tail-index_of_i).replace("Total+Size:","");
                        }
                    }
                    index_of_i++;
                }

                index_of_i = 0;
                index_of_tail = 0;

                for(;;){
                    index_of_i = check_HD_str.indexOf("Available+Size:", index_of_i);
                    qDebug() << " occurence" << index_of_i;
                    if(index_of_i == -1){
                        break;
                    } else{
                        //check for "GB" tail
                        index_of_tail = check_HD_str.indexOf("+GB", index_of_i);
                        if(index_of_tail != -1){
                            qDebug() << "HD found of available size:" << check_HD_str.mid(index_of_i,index_of_tail-index_of_i).replace("Available+Size:","");
                            HD_available += check_HD_str.mid(index_of_i,index_of_tail-index_of_i).replace("Available+Size:","").toInt();
                            qDebug() << " ---> " << check_HD_str.mid(index_of_i,index_of_tail-index_of_i).replace("Available+Size:","");
                        }
                    }
                    index_of_i++;
                }

                server_total ++;
                server_online += temp3["Online"].toInt();
            }


        }

    }

    qDebug() << server_online << server_total << ram_total << HD_total << HD_available;

    QTableWidgetItem *item;
    item = ui->tableWidget->item(0, 2);
    item->setText(QString::number(server_total));
    item = ui->tableWidget->item(1, 2);
    item->setText(QString::number(server_online));
    item = ui->tableWidget->item(2, 2);
    item->setText(QString::number(server_total - server_online));
    item = ui->tableWidget->item(3, 2);
    item->setText(formatNumber(QString::number(ram_total))+" MB");
    item = ui->tableWidget->item(4, 2);
    item->setText(formatNumber(QString::number(HD_total))+" GB");
    item = ui->tableWidget->item(5, 2);
    item->setText(formatNumber(QString::number(HD_available))+" GB");

    /*for(int i = 0;i<jsonObj.count();i++){
        jsonObj[i].toObject()["market_cap"].toDouble();
        _total_volume_d = jsonObj[i].toObject()["total_volume"].toDouble();

        total_market_cap+= _market_cap_d;
        total_24h_volume+= _total_volume_d;
    }*/

    QStringList stringList;
    stringList << QString::number(server_total) <<
                  (formatNumber(QString::number(ram_total))+" MB") <<
                  (formatNumber(QString::number(HD_total))+" GB");
    emit sendSimpleGridData(stringList);

}

void ZillionGridPage::onEnterFrame()
{

}

void ZillionGridPage::onEnterServerGridDetails()
{
    getServerGridDetails();
}
