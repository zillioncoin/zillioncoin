#ifndef MARKETCAPPAGE_H
#define MARKETCAPPAGE_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWidget>

#include <QTimer>

namespace Ui {
class MarketCapPage;
}

class MarketCapPage : public QWidget
{
    Q_OBJECT

public:
    explicit MarketCapPage(QWidget *parent = 0);
    ~MarketCapPage();

    QNetworkAccessManager *manager;
    QNetworkAccessManager *managerGlobalData;

    QTimer *timer;
    QTimer *timerGlobalData;

    void getMarketValues();
    void getGlobalData();

    int sec_since_last_update;
    int limit;
    QString currency;
    QString currency_symbol;

    QChar group_separator;
    QChar decimal_point;

    void  updateCurrencySymbolAndHeader();

    int current_selected_row;

private:
    Ui::MarketCapPage *ui;

signals:
    void sendMarketValues(QString total_marketcap, QString total_volume_24h, int bear_bull_percentage);
    void sendTickerData(QList<QString> stringList);

public slots:
    void replyFinished(QNetworkReply*reply);
    void replyFinishedGlobalData(QNetworkReply*reply);

    void onEnterFrame();
    void onEnterTicker();
    void onEnterGlobalData();

    void updateCurrency(int new_currency);
    void updateLimit(int new_limit);

    QString formatNumber(QString number);
};

#endif // MARKETCAPPAGE_H
