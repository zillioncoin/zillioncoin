#ifndef ZILLIONGRIDPAGE_H
#define ZILLIONGRIDPAGE_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QWidget>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

#include <QTableWidgetItem>

namespace Ui {
class ZillionGridPage;
}

class ZillionGridPage : public QWidget
{
    Q_OBJECT

public:
    explicit ZillionGridPage(QWidget *parent = 0);
    ~ZillionGridPage();

    QNetworkAccessManager *manager;
    QNetworkAccessManager *managerServerGridDetails;

    QTimer *timer;
    QTimer *timerServerGridDetails;

    void getServerStatus();
    void getServerGridDetails();

    QChar group_separator;
    QChar decimal_point;

    QString formatNumber(QString number);

signals:
    void sendSimpleGridData(QStringList stringList);
private:
    Ui::ZillionGridPage *ui;

public slots:
    void replyFinished(QNetworkReply*reply);
    void replyFinishedServerGridDetails(QNetworkReply*reply);

    void onEnterFrame();
    void onEnterServerGridDetails();
};

#endif // ZILLIONGRIDPAGE_H
