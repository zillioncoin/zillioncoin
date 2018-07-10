#include "marketcappage.h"
#include "ui_marketcappage.h"

MarketCapPage::MarketCapPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MarketCapPage)
{
    ui->setupUi(this);
}

MarketCapPage::~MarketCapPage()
{
    delete ui;
}
