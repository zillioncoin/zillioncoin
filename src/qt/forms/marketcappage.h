#ifndef MARKETCAPPAGE_H
#define MARKETCAPPAGE_H

#include <QWidget>

namespace Ui {
class MarketCapPage;
}

class MarketCapPage : public QWidget
{
    Q_OBJECT

public:
    explicit MarketCapPage(QWidget *parent = 0);
    ~MarketCapPage();

private:
    Ui::MarketCapPage *ui;
};

#endif // MARKETCAPPAGE_H
